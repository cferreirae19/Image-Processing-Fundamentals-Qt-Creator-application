#include "fpimage.h"
#include "ui_fpimage.h"

#include <QFileDialog>
#include <QPainter>
#include <cmath>
#include <algorithm>


//--------------------------------------------------
//-- Filtro de eventos para capturar mouse clicks --
//--------------------------------------------------
bool FPImage::eventFilter(QObject *Ob, QEvent *Ev)
{
    // Comprobamos que el evento capturado es un  mouseclick
    if(Ev->type()==QEvent::MouseButtonPress) {
        // Comprobamos que el click ocurrió sobre nuestro QLabel
        if(Ob==ui->Ecran) {
            // Hacemos un cast del evento para poder acceder a sus propiedades
            const QMouseEvent *me=static_cast<const QMouseEvent *>(Ev);
            // Nos interesan las coordenadas del click
            int y=me->y(), x=me->x();
            // Si estamos fuera de la imagen, nos vamos
            if(y>=H||x>=W) return true;
            // Hacemos algo con las coordenadas y el píxel
//            statusBar()->showMessage(QString::number(x)+":"+
//                                      QString::number(y)+" "+
//                                      QString::number(pixR[(y*S+3*x)])+":"+
//                                      QString::number(pixG[(y*S+3*x)])+":"+
//                                      QString::number(pixB[(y*S+3*x)]));
            //Llamamos a la función que calcula el mini histograma
            //MiniHistogram(x,y);
            //Llamamos a la función que dibuja las sombras del feature space
            FeatureSpaceShadows(x,y);
            // Llamamos a la función que calcula el color LHS a partir del RGB obtenido
            RGB2LHS(pixR[(y*S+3*x)],pixG[(y*S+3*x)],pixB[(y*S+3*x)],L,Hab,Sab);
            // Hacemos algo con las coordenadas y el píxel
            statusBar()->showMessage(QString::number(x)+":"+
                                     QString::number(y)+" "+
                                     QString::number(L)+":"+
                                     QString::number(Hab)+":"+
                                     QString::number(Sab)+" "+
                                     "Blob="+QString::number(labelling[y*W+x])+" "+
                                     "Área="+QString::number(Area[labelling[y*W+x]-1])+" "+
                                     "Perim="+QString::number(Perims[labelling[y*W+x]-1])+ " "+
                                     "Comp="+QString::number(Comps[labelling[y*W+x]-1]));
            // Devolvemos un "true" que significa que hemos gestionado el evento
            return true;
        } else return false;  // No era para nosotros, lo dejamos en paz
    } else return false;
}

//-------------------------------------------------
//-- Constructor: Conexiones e inicializaciones ---
//-------------------------------------------------
FPImage::FPImage(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FPImage)
{

    ui->setupUi(this);

    // CONEXIONES de nuestros objetos (botones, etc...) a nuestros slots
    connect(ui->BLoad,SIGNAL(clicked()),this,SLOT(Load()));
    connect(ui->BDoIt,SIGNAL(clicked()),this,SLOT(DoIt()));
    connect(ui->BGrayScale,SIGNAL(clicked()),this,SLOT(GrayScale()));
    connect(ui->BFlowerOutline,SIGNAL(clicked()),this,SLOT(FlowerOutline()));


    // "Instalamos" un "filtro de eventos" en nuestro QLabel Ecran
    // para capturar clicks de ratón sobre la imagen
    ui->Ecran->installEventFilter(this);


    // INICIALIZACIONES
    c = 450;
    b = 0;
    n = 0;
    W=H=0;      // Empezamos sin imagen cargada
    Path="..";  // Carpeta inicial
    PixCopia = nullptr; // los punters no apunta a nada inicialmente
    mask = nullptr;
    norma = 0;
    L_avrg = 72;    // Valores iniciales de media y desviación típica para el tono, la saturación y el "brillo" de la piel
    H_avrg = 164;
    S_avrg = 17;
    L_stdev = 25;
    H_stdev = 30;
    S_stdev = 9;
    blurApplied = 0;    // Al iniciar, nunca se ha aplicado el blur (flag a 0)
    ker = 0;            // Matriz de convolución estandar

    // Inicializamos a negro los lienzos (QPixmap) y los asociamos a las "pantallas" gráficas (QLabel)
    //   Le damos tamaño
    Dib1=QPixmap(256,100);
    //   Lo pintamos de negro
    Dib1.fill(Qt::white);
    //   Lo asignamos a un QLabel
    ui->EcranHistoR->setPixmap(Dib1);

    //   Idem
    Dib2=QPixmap(256,100);
    Dib2.fill(Qt::white);
    ui->EcranHistoG->setPixmap(Dib2);

    // De ídem
    Dib3=QPixmap(256,100);
    Dib3.fill(Qt::white);
    ui->EcranHistoB->setPixmap(Dib3);

    Dib1.fill(Qt::white);
    Dib2.fill(Qt::white);
    Dib3.fill(Qt::white);

    /*
    // Ejemplo de cómo dibujar usando funciones de alto nivel (QPainter)
    //   Declaramos un pintor (QPainter) y lo asociamos a un lienzo (QPixmap)
    QPainter p(&Dib3);
    //   Escogemos un lápiz (QPen; también hay pinceles, QBrush, para los rellenos)
    p.setPen(QPen(Qt::black));
    //   Trazamos un par de líneas, por ejemplo
    p.drawLine(0,0,255,99);
    p.drawLine(0,99,255,0);
    */
}

//-------------------------------------------------
//------ Destructor: Limpieza antes de salir ------
//-------------------------------------------------
FPImage::~FPImage()
{
    delete ui;
    delete [] PixCopia;
    delete [] pixBlur;
    delete [] mask;
}

//-------------------------------------------------
//----------- Carga una imagen de disco -----------
//-------------------------------------------------
void FPImage::Load(void)
{
    // Permite al usuario escoger un fichero de imagen
    QString file=QFileDialog::getOpenFileName(this,tr("Abrir imagen"),Path,tr("Image Files (*.png *.jpg *.bmp)"));
    // Si no escogió nada, nos vamos
    if(file.isEmpty()) return;

    // Creamos un QFileInfo para manipular cómodamente el nombre del fichero a efectos informativos
    // Por ejemplo deshacernos del path para que el nombre no ocupe demasiado
    QFileInfo finfo(file);
    // Memorizamos la carpeta usando la variable global Path, para la próxima vez
    Path=finfo.path();
    // Ponemos el nombre del fichero en el recuadro de texto
    ui->EFile->setText(finfo.fileName());
    // Decoración: Añadimos el nombre del fichero al título de la ventana
    setWindowTitle("FPImage v0.1b - "+finfo.fileName());

    // Cargamos la imagen a nuestra variable "Image" usando la función apropiada de la clase QImage
    Image.load(file);
    // Convertimos a RGB (eliminamos el canal A)
    Image=Image.convertToFormat(QImage::Format_RGB888);

    // Almacenamos las dimensiones de la imagen
    W=Image.width();
    H=Image.height();

    // Ponemos nuestros punteros apuntando a cada canal del primer píxel
    pixB=(pixG=(pixR=Image.bits())+1)+1;

    // Ojo! La imagen puede llevar "relleno" ("zero padding"):

    // Longitud en bytes de cada línea incluyendo el padding
    S=Image.bytesPerLine();
    // Padding (número de zeros añadidos al final de cada línea)
    Padding=S-3*W;

    // Mostramos algo de texto informativo
    ui->ERes->appendPlainText("Loaded "+finfo.fileName());
    ui->ERes->appendPlainText("Size "+QString::number(W)+"x"+QString::number(H));
    ui->ERes->appendPlainText("Padded length "+QString::number(S));
    ui->ERes->appendPlainText("Pad "+QString::number(Padding));
    ui->ERes->appendPlainText("");

    // Ponemos algo en la barra de estado durante 2 segundos
    statusBar()->showMessage("Loaded.",2000);

    // Ajustamos el tamaño de la "pantalla" al de la imagen
    ui->Ecran->setFixedWidth(W);
    ui->Ecran->setFixedHeight(H);

    // Copia de seguridad de la imagen original
    if (PixCopia) delete[]PixCopia;
    PixCopia = new uchar[S*H];
    memcpy(PixCopia,pixR,S*H);

    // Reset de los controles del slider de brillo y del slider de contraste
    ui->BrightnessSlider->setValue(0);
    ui->ContrastSlider->setValue(450);
    ui->EdgeSlider->setValue(0);

    // Reset de los controles del slider de medias y desviaciones típicas de L, H y S
    ui->BRp->setValue(72);
    ui->BRvar->setValue(25);
    ui->BGp->setValue(164);
    ui->BGvar->setValue(30);
    ui->BBp->setValue(17);
    ui->BBvar->setValue(9);

    // // Al cargar una nueva foto, nunca se ha aplicado el blur (reseteamos flag a 0)
    blurApplied = 0;
    // Reservamos espacio para la nueva imagen borrosa
    pixBlur = new uchar[S*H];

    // Reservamos espacio para la máscara binaria y borramos la existente si ya ha sido declarada (es decir, de loads de imágenes anteriores)
    if(mask) delete [] mask;
    mask = new uchar[W*H];

    // Reservamos espacio para la matriz de labelling y borramos la existente si ya ha sido declarada (es decir, de loads de imágenes anteriores)
    if(labelling) delete [] labelling;
    labelling = new int[W*H];

    // Volcamos la imagen a pantalla
    ShowIt();

    // Calculamos y mostramos histograma
    //Histogram();
}

//--------------------------------------------
//------------- Guarda la imagen -------------
//--------------------------------------------
void FPImage::on_BSave_clicked()
{
    QString fileName = "";
    // Selecciona el nombre del archivo en que se guardará la imagen
    fileName = QFileDialog::getSaveFileName(this,"Guardar imagen",Path,tr("Image Files (*.png *.jpg *.bmp)"));
    // Si no hay nombre de archivo
    if(fileName.length()==0) return;
    // Guarda imagen
    Image.save(fileName);
}

//-------------------------------------------------
//------------- Jugamos con la imagen -------------
//-------------------------------------------------
void FPImage::DoIt(void)
{
    // Nos aseguramos de que hay una imagen cargada
    if(!H) return;

    // Ejemplo de procesamiento A BAJO NIVEL
    //   Recorremos toda la imagen manipulando los píxeles uno a uno
    //   Atención a los límites y a los saltos de 3 en 3 (3 canales)
    for(int y=0,i=0;y<H;y++,i+=Padding) for(int x=0;x<W;x++,i+=3) {
            uchar swap=pixR[i]; pixR[i]=pixB[i]; pixB[i]=swap;
            pixG[i]=255-pixG[i];
    }

    // Sacamos algo de texto informativo
    ui->ERes->appendPlainText("Did it");

    // Ponemos algo en la barra de estado durante 2 segundos
    statusBar()->showMessage("Did it.",2000);

    // Volcamos la imagen a pantalla
    // OJO: Si os olvidáis de esto, la imagen en pantalla no refleja los cambios y
    // pensaréis que no habéis hecho nada, pero Image e Ima (que son la misma) sí
    // que han cambiado aunqu eno lo veáis
    ShowIt();
}

//----------------------------------------------------------------------------
//---- Convertimos todo lo que no sea flor en la imagen a escala de grises ---
//----------------------------------------------------------------------------
void FPImage::FlowerOutline(void)
{
    // Nos aseguramos de que hay una imagen cargada
    if(!H) return;

    // Ejemplo de procesamiento A BAJO NIVEL
    //   Recorremos toda la imagen manipulando los píxeles uno a uno
    //   Atención a los límites y a los saltos de 3 en 3 (3 canales)
    for(int y=0,i=0;y<H;y++,i+=Padding) for(int x=0;x<W;x++,i+=3) {
        if(pixR[i]>pixG[i] && pixR[i]>pixB[i]) {
            // Si la componente predominante es el rojo, no cambiamos nada en ese pixel
        }
        else {
            // Ponemos el pixel en escala de grises
            pixR[i]=(pixG[i]=(pixB[i] = 0.2125*pixR[i] + 0.7154*pixG[i] + 0.0721*pixB[i]));
        }
    }

    // Sacamos algo de texto informativo
    ui->ERes->appendPlainText("Outline");

    // Ponemos algo en la barra de estado durante 2 segundos
    statusBar()->showMessage("Outline.",2000);

    // Volcamos la imagen a pantalla
    // OJO: Si os olvidáis de esto, la imagen en pantalla no refleja los cambios y
    // pensaréis que no habéis hecho nada, pero Image e Ima (que son la misma) sí
    // que han cambiado aunqu eno lo veáis
    ShowIt();
}

//---------------------------------------------------------------------------
//---- Convertimos todo lo que sea flor en la imagen a escala de grises ---
//---------------------------------------------------------------------------
void FPImage::on_BLeavesOutline_clicked()
{
    // Nos aseguramos de que hay una imagen cargada
    if(!H) return;

    // Ejemplo de procesamiento A BAJO NIVEL
    //   Recorremos toda la imagen manipulando los píxeles uno a uno
    //   Atención a los límites y a los saltos de 3 en 3 (3 canales)
    for(int y=0,i=0;y<H;y++,i+=Padding) for(int x=0;x<W;x++,i+=3) {
        if(pixG[i]>pixR[i] && pixG[i]>pixB[i]) {
            // Si la componente predominante es el verde, no cambiamos nada en ese pixel
        }
        else {
            // Ponemos el pixel en escala de grises
            pixR[i]=(pixG[i]=(pixB[i] = 0.2125*pixR[i] + 0.7154*pixG[i] + 0.0721*pixB[i]));
        }
    }

    // Sacamos algo de texto informativo
    ui->ERes->appendPlainText("Outline");

    // Ponemos algo en la barra de estado durante 2 segundos
    statusBar()->showMessage("Outline.",2000);

    // Volcamos la imagen a pantalla
    // OJO: Si os olvidáis de esto, la imagen en pantalla no refleja los cambios y
    // pensaréis que no habéis hecho nada, pero Image e Ima (que son la misma) sí
    // que han cambiado aunqu eno lo veáis
    ShowIt();
}

//--------------------------------------
//---- Convertimos todo a modo sepia ---
//--------------------------------------
void FPImage::on_BSepia_clicked()
{
    // Variables locales auxiliares
    uchar pixR_aux, pixG_aux, pixB_aux;

    // Nos aseguramos de que hay una imagen cargada
    if(!H) return;

    // Ejemplo de procesamiento A BAJO NIVEL
    //   Recorremos toda la imagen manipulando los píxeles uno a uno
    //   Atención a los límites y a los saltos de 3 en 3 (3 canales)
    for(int y=0,i=0;y<H;y++,i+=Padding) for(int x=0;x<W;x++,i+=3) {
        // Fórmula para tono sepia
        pixR_aux = (pixR[i]*.393) + (pixG[i]*.769) + (pixB[i]*.189);
        pixG_aux = (pixR[i]*.349) + (pixG[i]*.686) + (pixB[i]*.168);
        pixB_aux = (pixR[i]*.272) + (pixG[i]*.534) + (pixB[i]*.131);

        // Actualizamos los valores de las 3 componentes
        pixR[i] = pixR_aux;
        pixG[i] = pixG_aux;
        pixB[i] = pixB_aux;
    }

    // Sacamos algo de texto informativo
    ui->ERes->appendPlainText("Sepia tone image");

    // Ponemos algo en la barra de estado durante 2 segundos
    statusBar()->showMessage("Sepia tone image.",2000);

    // Volcamos la imagen a pantalla
    // OJO: Si os olvidáis de esto, la imagen en pantalla no refleja los cambios y
    // pensaréis que no habéis hecho nada, pero Image e Ima (que son la misma) sí
    // que han cambiado aunqu eno lo veáis
    ShowIt();
}

//--------------------------
//---- Cambiar el brillo ---
//--------------------------
void FPImage::on_BrightnessSlider_valueChanged(int value)
{
    // Nos aseguramos de que hay una imagen cargada
    if(!H) return;

    // Aumento (disminución) de brillo = Aumento (disminución) del punto de corte 'n' = Aumento (disminución) del slider de brillo
    b = value;

    // Llamada a la función ContrastChanger()
    ContrastChanger();

    // Volcamos la imagen a pantalla
    // OJO: Si os olvidáis de esto, la imagen en pantalla no refleja los cambios y
    // pensaréis que no habéis hecho nada, pero Image e Ima (que son la misma) sí
    // que han cambiado aún que no lo veáis
    ShowIt();
}

//-----------------------------
//---- Cambiar el contraste ---
//-----------------------------
void FPImage::on_ContrastSlider_valueChanged(int value)
{
    // Nos aseguramos de que hay una imagen cargada
    if(!H) return;

    // Pendiente y corte con el eje de la recta que gobierna el contraste (y=c*x+n)
    c = tan((3.14*value/180)/10.0f);
    n = 127-c*127;

    // Llamada a la función ContrastChanger()
    ContrastChanger();

    // Volcamos la imagen a pantalla
    // OJO: Si os olvidáis de esto, la imagen en pantalla no refleja los cambios y
    // pensaréis que no habéis hecho nada, pero Image e Ima (que son la misma) sí
    // que han cambiado aún que no lo veáis
    ShowIt();
}

//---------------------------------------------
//----- Resalta bordes llamando a Edges() -----
//---------------------------------------------
void FPImage::on_EdgeSlider_valueChanged(int value)
{
    // Nos aseguramos de que hay una imagen cargada
    if(!H) return;

    // Marcamos los bordes
    Edges(value);
}

//-------------------------------------------------
//---- Convertimos la imagen a escala de grises ---
//-------------------------------------------------
void FPImage::GrayScale(void)
{
    // Nos aseguramos de que hay una imagen cargada
    if(!H) return;

    // Ejemplo de procesamiento A BAJO NIVEL
    //   Recorremos toda la imagen manipulando los píxeles uno a uno
    //   Atención a los límites y a los saltos de 3 en 3 (3 canales)
    for(int y=0,i=0;y<H;y++,i+=Padding) for(int x=0;x<W;x++,i+=3) {
            pixR[i]=(pixG[i]=(pixB[i] = 0.2125*pixR[i] + 0.7154*pixG[i] + 0.0721*pixB[i]));
    }

    // Sacamos algo de texto informativo
    ui->ERes->appendPlainText("Converted to grayscale");

    // Ponemos algo en la barra de estado durante 2 segundos
    statusBar()->showMessage("Converted to grayscale.",2000);

    // Volcamos la imagen a pantalla
    // OJO: Si os olvidáis de esto, la imagen en pantalla no refleja los cambios y
    // pensaréis que no habéis hecho nada, pero Image e Ima (que son la misma) sí
    // que han cambiado aunqu eno lo veáis
    ShowIt();
}

//-------------------------------------------------
//-------------- Mostramos la imagen --------------
//-------------------------------------------------
inline void FPImage::ShowIt(void)
{
    // Creamos un lienzo (QPixmap) a partir de la QImage
    // y lo asignamos a la QLabel central
    ui->Ecran->setPixmap(QPixmap::fromImage(Image));
}

//-------------------------------------------------
//-------------- Restaurar la imagen --------------
//-------------------------------------------------
void FPImage::Restore(void)
{
    //Restaurar la imagen original almacenada, de la que habíamos hecho una copia de
    //seguridad al cargarla (PixCopia)
    memcpy(pixR,PixCopia,S*H);
}

//-----------------------------
//---- Cambiar el contraste ---
//-----------------------------
void FPImage::ContrastChanger() {

    // Restauramos la imagen
    Restore();

    // Cálculo de la LUT
    for (int i=0; i<255; i++) {
        LUT[i] = c*i+n+b>255?255:c*i+n+b<0?0:c*i+n+b;
    }

    // Ejemplo de procesamiento A BAJO NIVEL
    //   Recorremos toda la imagen manipulando los píxeles uno a uno
    //   Atención a los límites y a los saltos de 3 en 3 (3 canales)
    for(int y=0,i=0;y<H;y++,i+=Padding) for(int x=0;x<W;x++,i+=3) {

        pixR[i] = LUT[pixR[i]];
        pixG[i] = LUT[pixG[i]];
        pixB[i] = LUT[pixB[i]];

    }

}

//--------------------------
//----- Resalta bordes -----
//--------------------------
void FPImage::Edges(int threshold)
{
    // Restauramos la imagen
    Restore();

    // Ejemplo de procesamiento A BAJO NIVEL
    //   Recorremos toda la imagen manipulando los píxeles uno a uno
    //   Atención a los límites y a los saltos de 3 en 3 (3 canales)
    for(int y=0,i=0;y<H;y++,i+=Padding) for(int x=0;x<W;x++,i+=3) {
        // Declaración de distancias
        float distE;
        float distS;

        float cosE;
        float cosS;

        // Hallamos la distancia de cada pixel con su vecindario 4N o NN (4 distancias, una para cada vecino) -> En realidad solo con sus vecinos "futuros" (distE y distS)
        if (norma == 2){ // Usando norma 2 (distancia euclídea)
            distE = sqrt( (pixR[i]-pixR[i+3])*(pixR[i]-pixR[i+3]) + (pixG[i]-pixG[i+3])*(pixG[i]-pixG[i+3]) + (pixB[i]-pixB[i+3])*(pixB[i]-pixB[i+3]) );
            distS = sqrt( (pixR[i]-pixR[i+S])*(pixR[i]-pixR[i+S]) + (pixG[i]-pixG[i+S])*(pixG[i]-pixG[i+S]) + (pixB[i]-pixB[i+S])*(pixB[i]-pixB[i+S]) );
            // Si las distancias superan un cierto umbral, resaltamos el borde y dejamos el resto de píxeles en blanco
            if (distE>threshold || distS>threshold) {
                pixR[i]=(pixG[i]=(pixB[i] = 0));    // Borde a negro
            }
            else {
                pixR[i]=(pixG[i]=(pixB[i] = 255));    // Pixel en blanco
            }
        }
        if (norma == 1) {    // Usando norma 1 (distancia Manhattan o city block
            distE = abs(pixR[i]-pixR[i+3]) + abs(pixG[i]-pixG[i+3]) + abs(pixB[i]-pixB[i+3]);
            distS = abs(pixR[i]-pixR[i+S]) + abs(pixG[i]-pixG[i+S]) + abs(pixB[i]-pixB[i+S]);
            // Si las distancias superan un cierto umbral, resaltamos el borde y dejamos el resto de píxeles en blanco
            if (distE>threshold || distS>threshold) {
                pixR[i]=(pixG[i]=(pixB[i] = 0));    // Borde a negro
            }
            else {
                pixR[i]=(pixG[i]=(pixB[i] = 255));    // Pixel en blanco
            }
        }
        if (norma == 0) {  // Usando norma infinito
            distE = std::max(std::max(abs(pixR[i]-pixR[i+3]),abs(pixG[i]-pixG[i+3])),abs(pixB[i]-pixB[i+3]));
            distS = std::max(std::max(abs(pixR[i]-pixR[i+S]),abs(pixG[i]-pixG[i+S])),abs(pixB[i]-pixB[i+S]));
            // Si las distancias superan un cierto umbral, resaltamos el borde y dejamos el resto de píxeles en blanco
            if (distE>threshold || distS>threshold) {
                pixR[i]=(pixG[i]=(pixB[i] = 0));    // Borde a negro
            }
            else {
                pixR[i]=(pixG[i]=(pixB[i] = 255));    // Pixel en blanco
            }
        }

        // Usando métrica SAM, comparamos el ángulo que forman el vector que une el pixel E/S con el (0,0,0) con el vector que une el pixel actual con (0,0,0)
        if (norma==3) {
            // Coseno del ángulo que forman 2 vectores = Producto escalar de los 2 vectores / (Norma del primer vector * Norma del segundo vector)
            cosE = (pixR[i]*pixR[i+3] + pixG[i]*pixG[i+3] + pixB[i]*pixB[i+3]) / (sqrt(pixR[i]*pixR[i] + pixG[i]*pixG[i] + pixB[i]*pixB[i]) * sqrt(pixR[i+3]*pixR[i+3] + pixG[i+3]*pixG[i+3] + pixB[i+3]*pixB[i+3]));
            cosS = (pixR[i]*pixR[i+S] + pixG[i]*pixG[i+S] + pixB[i]*pixB[i+S]) / (sqrt(pixR[i]*pixR[i] + pixG[i]*pixG[i] + pixB[i]*pixB[i]) * sqrt(pixR[i+S]*pixR[i+S] + pixG[i+S]*pixG[i+S] + pixB[i+S]*pixB[i+S]));
            //TESTING
//            if (i>=100 && i <= 200) {
//                qDebug()<<cosE<<cosS;
//            }
            // Si el ángulo está por debajo de un cierto umbral (es decir, si los 2 vectores están en rectas que no son mínimamente coincidentes), resaltamos el borde en negro y dejamos el resto de píxeles en blanco
            if ((acos(cosE)*3.14/180)<((threshold-65)/65.0) || (acos(cosS)*3.14/180)<((threshold-65)/65.0)) {
                pixR[i]=(pixG[i]=(pixB[i] = 0));    // Borde a negro
            }
            else {
                pixR[i]=(pixG[i]=(pixB[i] = 255));    // Pixel en blanco
            }
        }
    }

    // Volcamos la imagen a pantalla
    // OJO: Si os olvidáis de esto, la imagen en pantalla no refleja los cambios y
    // pensaréis que no habéis hecho nada, pero Image e Ima (que son la misma) sí
    // que han cambiado aunqu eno lo veáis
    ShowIt();

}


//----------------------------------
//----- Realiza stretch lineal -----
//----------------------------------
void FPImage::on_BLinearStretch_clicked()
{
    int iniLevelR = 0;
    int endLevelR = 255;
    int iniLevelG = 0;
    int endLevelG = 255;
    int iniLevelB = 0;
    int endLevelB = 255;
    int iniRStop = 0;
    int iniGStop = 0;
    int iniBStop = 0;
    int endRStop = 0;
    int endGStop = 0;
    int endBStop = 0;

    // Calculamos índices iniciales para el stretch
    for(int i=0; i<256; i++) {
        if (histoR[i]>0 && iniRStop==0) {
            iniLevelR = i;
            iniRStop = 1;
        }
        if (histoG[i]>0 && iniGStop==0) {
            iniLevelG = i;
            iniGStop = 1;
        }
        if (histoB[i]>0 && iniBStop==0) {
            iniLevelB = i;
            iniBStop = 1;
        }
    }

    // Calculamos índices finales para el stretch
    for(int i=0; i<256; i++) {
        if (histoR[255-i]>0 && endRStop==0) {
            endLevelR = 255-i;
            endRStop = 1;
        }
    }
    for(int i=0; i<256; i++) {
        if (histoG[255-i]>0 && endGStop==0) {
            endLevelG = 255-i;
            endGStop = 1;
        }
    }
    for(int i=0; i<256; i++) {
        if (histoB[255-i]>0 && endBStop==0) {
            endLevelB = 255-i;
            endBStop = 1;
        }
    }

    // TESTING
    //qDebug()<<iniLevelR<<endLevelR;
    //qDebug()<<iniLevelG<<endLevelG;
    //qDebug()<<iniLevelB<<endLevelB;

    // Cálculo de la LUT
    for (int i=0; i<255; i++) {
        LUT_HistoR[i] = ((i-iniLevelR)*255)/(endLevelR-iniLevelR)>255?255:((i-iniLevelR)*255)/(endLevelR-iniLevelR)<0?0:((i-iniLevelR)*255)/(endLevelR-iniLevelR);
        LUT_HistoG[i] = ((i-iniLevelG)*255)/(endLevelG-iniLevelG)>255?255:((i-iniLevelG)*255)/(endLevelG-iniLevelG)<0?0:((i-iniLevelG)*255)/(endLevelG-iniLevelG);
        LUT_HistoB[i] = ((i-iniLevelB)*255)/(endLevelB-iniLevelB)>255?255:((i-iniLevelG)*255)/(endLevelG-iniLevelG)<0?0:((i-iniLevelG)*255)/(endLevelG-iniLevelG);
    }

    // Recorre toda la imagen cambiando los niveles
    for(int y=0,i=0;y<H;y++,i+=Padding) for(int x=0;x<W;x++,i+=3) {
        // Reglas de 3 y medio para los 3 canales
        pixR[i] = LUT_HistoR[pixR[i]];
        pixG[i] = LUT_HistoG[pixG[i]];
        pixB[i] = LUT_HistoB[pixB[i]];
    }

    // Volcamos la imagen a pantalla
    // OJO: Si os olvidáis de esto, la imagen en pantalla no refleja los cambios y
    // pensaréis que no habéis hecho nada, pero Image e Ima (que son la misma) sí
    // que han cambiado aunqu eno lo veáis
    ShowIt();

    // Calculamos y mostramos histograma
    Histogram();

}


//-------------------------------
//----- Ecualización lineal -----
//-------------------------------
void FPImage::on_BEq_clicked()
{
    // Cálculo de la LUT
    for (int i=0; i<256; i++) {
        LUT_HistoAcR[i] = (histoAcR[i]*255) / histoAcR[255];
        LUT_HistoAcG[i] = (histoAcG[i]*255) / histoAcG[255];
        LUT_HistoAcB[i] = (histoAcB[i]*255) / histoAcB[255];
    }

    // Recorre toda la imagen cambiando los niveles
    for(int y=0,i=0;y<H;y++,i+=Padding) for(int x=0;x<W;x++,i+=3) {
        // Reglas de 3 y medio para los 3 canales
        pixR[i] = LUT_HistoAcR[pixR[i]];
        pixG[i] = LUT_HistoAcG[pixG[i]];
        pixB[i] = LUT_HistoAcB[pixB[i]];
    }

    // Volcamos la imagen a pantalla
    // OJO: Si os olvidáis de esto, la imagen en pantalla no refleja los cambios y
    // pensaréis que no habéis hecho nada, pero Image e Ima (que son la misma) sí
    // que han cambiado aunqu eno lo veáis
    ShowIt();

    // Calculamos y mostramos histograma
    Histogram();

}


//-------------------------------
//----- Calcula histogramas -----
//-------------------------------
void FPImage::Histogram()
{
    Dib1.fill(Qt::white);
    Dib2.fill(Qt::white);
    Dib3.fill(Qt::white);

    memset(histoR,0,256*sizeof(int));
    memset(histoG,0,256*sizeof(int));
    memset(histoB,0,256*sizeof(int));

    memset(histoAcR,0,256*sizeof(int));
    memset(histoAcG,0,256*sizeof(int));
    memset(histoAcB,0,256*sizeof(int));

    // Ejemplo de cómo dibujar usando funciones de alto nivel (QPainter)
    // Declaramos un pintor (QPainter) y lo asociamos a un lienzo (QPixmap)
    QPainter p1(&Dib1);
    QPainter p2(&Dib2);
    QPainter p3(&Dib3);

    // Escogemos un lápiz (QPen; también hay pinceles, QBrush, para los rellenos)
    p1.setPen(QPen(Qt::red));
    p2.setPen(QPen(Qt::green));
    p3.setPen(QPen(Qt::blue));

    int maxR = 0;
    int maxG = 0;
    int maxB = 0;

    // Recorre toda la imagen
    for(int y=0,i=0;y<H;y++,i+=Padding) for(int x=0;x<W;x++,i+=3) {
            // Vamos actualizando cada histograma
            histoR[pixR[i]]++;
            histoG[pixG[i]]++;
            histoB[pixB[i]]++;
    }

    // Histogramas acumulados de las 3 componentes
    histoAcR[0] = histoR[0];
    histoAcG[0] = histoG[0];
    histoAcB[0] = histoB[0];
    for (int i=1; i<256; i++) {
        histoAcR[i] = histoAcR[i-1] + histoR[i];
        histoAcG[i] = histoAcG[i-1] + histoG[i];
        histoAcB[i] = histoAcB[i-1] + histoB[i];
    }

    // Mostramos los niveles del histograma por pantalla
    for(int i=0; i<256; i++) {

        // Sacamos algo de texto informativo
        //ui->ERes->appendPlainText("Nivel " + QString::number(i) + ": histoR = " + QString::number(histoR[i]) + "; histoG = " + QString::number(histoG[i]) + "; histoB = " + QString::number(histoB[i]));

        // Hallamos el valor máximo
        if (histoR[i] > maxR) {
            maxR = histoR[i];
        }
        if (histoG[i] > maxG) {
            maxG = histoG[i];
        }
        if (histoB[i] > maxB) {
            maxB = histoB[i];
        }
    }

    for(int i=0; i<256; i++) {
        //Reescalamos los histogramas
        histoR[i] = histoR[i]*100/maxR;
        histoG[i] = histoG[i]*100/maxG;
        histoB[i] = histoB[i]*100/maxB;

        //   Trazamos un par de líneas, por ejemplo
        p1.drawLine(i,100,i,(100-histoR[i]));
        p2.drawLine(i,100,i,(100-histoG[i]));
        p3.drawLine(i,100,i,(100-histoB[i]));
    }

    ui->EcranHistoR->setPixmap(Dib1);
    ui->EcranHistoG->setPixmap(Dib2);
    ui->EcranHistoB->setPixmap(Dib3);
}


//-----------------------------------
//----- Calcula mini histograma -----
//-----------------------------------
void FPImage::MiniHistogram(int xp, int yp)
{
    memset(histoMiniR,0,256*sizeof(int));
    memset(histoMiniG,0,256*sizeof(int));
    memset(histoMiniB,0,256*sizeof(int));

    for(int y=(yp-12);y<(yp+13);y++) for(int x=(xp-12);x<(xp+13);x++) {
            // Vamos actualizando cada histograma
            histoMiniR[pixR[y*S+3*x]]++;
            histoMiniG[pixG[y*S+3*x]]++;
            histoMiniB[pixB[y*S+3*x]]++;
    }

    Dib1.fill(Qt::white);
    Dib2.fill(Qt::white);
    Dib3.fill(Qt::white);

    // Ejemplo de cómo dibujar usando funciones de alto nivel (QPainter)
    // Declaramos un pintor (QPainter) y lo asociamos a un lienzo (QPixmap)
    QPainter p1(&Dib1);
    QPainter p2(&Dib2);
    QPainter p3(&Dib3);

    // Escogemos un lápiz (QPen; también hay pinceles, QBrush, para los rellenos)
    p1.setPen(QPen(Qt::red));
    p2.setPen(QPen(Qt::green));
    p3.setPen(QPen(Qt::blue));

    int maxR = 0;
    int maxG = 0;
    int maxB = 0;

    // Mostramos los niveles del histograma por pantalla
    for(int i=0; i<256; i++) {

        // Sacamos algo de texto informativo
        //ui->ERes->appendPlainText("Nivel " + QString::number(i) + ": histoR = " + QString::number(histoR[i]) + "; histoG = " + QString::number(histoG[i]) + "; histoB = " + QString::number(histoB[i]));

        // Hallamos el valor máximo
        if (histoMiniR[i] > maxR) {
            maxR = histoMiniR[i];
        }
        if (histoMiniG[i] > maxG) {
            maxG = histoMiniG[i];
        }
        if (histoMiniB[i] > maxB) {
            maxB = histoMiniB[i];
        }
    }

    for(int i=0; i<256; i++) {
        //Reescalamos los histogramas
        histoMiniR[i] = histoMiniR[i]*100/maxR;
        histoMiniG[i] = histoMiniG[i]*100/maxG;
        histoMiniB[i] = histoMiniB[i]*100/maxB;

        //   Trazamos un par de líneas, por ejemplo
        p1.drawLine(i,100,i,(100-histoMiniR[i]));
        p2.drawLine(i,100,i,(100-histoMiniG[i]));
        p3.drawLine(i,100,i,(100-histoMiniB[i]));
    }

//    for (int i=0; i<256; i++) {
//        // Sacamos algo de texto informativo
//        ui->ERes->appendPlainText("Nivel " + QString::number(i) + ": histoMiniR = " + QString::number(histoMiniR[i]) + "; histoMiniG = " + QString::number(histoMiniG[i]) + "; histoMiniB = " + QString::number(histoMiniB[i]));
//    }

    ui->EcranHistoR->setPixmap(Dib1);
    ui->EcranHistoG->setPixmap(Dib2);
    ui->EcranHistoB->setPixmap(Dib3);
}


//--------------------------------------------------------------------------
//----- Calcula mini histograma de cada pixel y opera según resultados -----
//--------------------------------------------------------------------------
void FPImage::TempHistogram()
{
    // Recorre la parcela 25x25 de la imagen centrada en el pixel [x,y] (parámetros que le pasamos a la función)
    for(int yp=12;yp<(H-13);yp++) for(int xp=12;xp<(W-13);xp++) {
        for(int y=(yp-12);y<(yp+13);y++) for(int x=(xp-12);x<(xp+13);x++) {
                // Vamos actualizando cada histograma
                histoMiniR[pixR[y*S+3*x]]++;
                histoMiniG[pixG[y*S+3*x]]++;
                histoMiniB[pixB[y*S+3*x]]++;
        }



    }
}


//---------------------------------------------
//----- Detecta si hay piel en cada pixel -----
//---------------------------------------------
void FPImage::on_BSkinDetector_clicked()
{
    // Comprobamos si es necesario hacer el blur o no
    if (!blurApplied) { // Si es la primera vez que se llama a la función (es decir, si la llamada a esta función se ha realizado desde el botón y no desde los sliders)
        for (int n=0; n<16; n++) {
            // En ese caso, aplicamos blur
            Filtering(5);
        }
        // Ponemos la flag a 1
        blurApplied = 1;
        // La próxima vez que se llame a esta función (es decir, desde algún slider y no desde el botón) no se realiza blur (solo se detecta piel)
    }

    // Recorre toda la imagen
    for(int y=0,i=0;y<H;y++,i+=Padding) for(int x=0;x<W;x++,i+=3) {
        // Convertimos cada valor RGB a LHabSab
        RGB2LHS(pixBlur[i],pixBlur[i+1],pixBlur[i+2],L,Hab,Sab);
        // Comparamos el valor de cada componente de cada pixel para ver si está dentro de nuestra "caja de zapatos"
        if ( ( ((L-L_avrg)*(L-L_avrg)/(L_stdev*L_stdev)) + ((Hab-H_avrg)*(Hab-H_avrg)/(H_stdev*H_stdev)) + ((Sab-S_avrg)*(Sab-S_avrg)/(S_stdev*S_stdev)) ) > 1 ) {
            // Si no es piel -> Pixel a negro
            pixR[i] = (pixG[i] = (pixB[i] = 0));
            mask[i/3] = 0;
        }
        else {
            // Si es piel -> Restauramos pixel original (desde pixCopia) a la imagen de trabajo (pixR, pixG y pixB)
            pixR[i] = PixCopia[i];
            pixG[i] = PixCopia[i+1];
            pixB[i] = PixCopia[i+2];
            mask[i/3] = 1;
        }
    }

    ShowIt();

}


//------------------------------------------------------------
//----- Dibuja las sombras de la forma del feature space -----
//------------------------------------------------------------
void FPImage::FeatureSpaceShadows(int x, int y)
{
    memset(histoR,0,256*sizeof(int));
    memset(histoG,0,256*sizeof(int));
    memset(histoB,0,256*sizeof(int));

    // Ejemplo de cómo dibujar usando funciones de alto nivel (QPainter)
    // Declaramos un pintor (QPainter) y lo asociamos a un lienzo (QPixmap)
    QPainter p1(&Dib1);
    QPainter p2(&Dib2);
    QPainter p3(&Dib3);

    // Escogemos un lápiz (QPen; también hay pinceles, QBrush, para los rellenos)
    p1.setPen(QPen(Qt::black));
    p2.setPen(QPen(Qt::black));
    p3.setPen(QPen(Qt::black));

    RGB2LHS(pixR[y*S+3*x],pixG[y*S+3*x],pixB[y*S+3*x],L,Hab,Sab);

    // Pintar punto en el plano L-Hab (L horiz. y Hab vert.)
    p1.drawPoint((L/2.55),(Hab/2.55));
    // Pintar punto en el plano Hab-Sab (Hab horiz. y Sab vert.)
    p2.drawPoint((Hab/2.55),(Sab/2.55));
    // Pintar punto en el plano Sab-L (Sab horiz. y L vert.)
    p3.drawPoint((Sab/2.55),(L/2.55));

    ui->EcranHistoR->setPixmap(Dib1);
    ui->EcranHistoG->setPixmap(Dib2);
    ui->EcranHistoB->setPixmap(Dib3);
}


//-------------------------------------------------------
//----- Conversión de un espacio de color RGB a LHS -----
//-------------------------------------------------------
void FPImage::RGB2LHS(uchar r, uchar g, uchar bb, uchar &L, uchar &H, uchar &S)
{

    // Conversión RGB a XYZ
    float X = 0.412453*r/255 + 0.357580*g/255 + 0.180423*bb/255;
    float Y = 0.212671*r/255 + 0.715160*g/255 + 0.072169*bb/255;
    float Z = 0.019334*r/255 + 0.119193*g/255 + 0.950227*bb/255;

    // Cálculo de L
    if (Y > 0.008856) {
        L = 116*cbrt(Y) - 16;
    }
    else {
        L = 903.3*Y;
    }

    // Cálculo de Xw y Zw
    float Xw = 0.950456;
    float Zw = 1.088754;

    // Cálculos de parámetros según la función de argumento alpha
    float fXXW = (X/Xw)>0.008856?cbrt(X/Xw):(7.787*(X/Xw)+16/116.0);
    float fZZW = (Z/Zw)>0.008856?cbrt(Z/Zw):(7.787*(Z/Zw)+16/116.0);
    float fY = Y>0.008856?cbrt(Y):(7.787*Y+16/116.0);

    // Cálculo de a
    float A = (1000*(fXXW-fY)+1)/255;

    // Cálculo de b
    float B = (400*(fY-fZZW)+1)/255;

    // Conversión ab a HS
    H = (atan2(A,B)+3.14159)*255/(2*3.14159);
    S = sqrt(A*A+B*B)*255/1.41421;
}


//-----------------------------------
//----- Filtrado para la imagen -----
//-----------------------------------
void FPImage::Filtering(int k)
{

    // Kernel del filtro
    float normz = 0;
    float *kernel = new float[k*k];

    // Si seleccionamos el filtro de medias (spinBox = 0)
    if (ker == 0) {
        for (int u=0, c=0; u<k; u++) for (int v=0; v<k; v++, c++) {
            normz += (kernel[c] = 1);    // Vamos añadiendo 1 elemento a normz (al final tendremos el número de elementos del vecindario, es decir, k x k) al mismo tiempo que establecemos el valor de cada elemento del kernel a 1
        }
    }

    // ***COMENTAR/DESCOMENTAR***
    // Si seleccionamos el filtro de menos media o sharpening (spinBox = 1)
    if (ker == 1) {
        for (int u=0, c=0; u<k; u++) for (int v=0; v<k; v++, c++) {
            kernel[c] = -1;    // Vamos añadiendo 1 elemento a normz (al final tendremos el número de elementos del vecindario, es decir, k x k) al mismo tiempo que establecemos el valor de cada elemento del kernel a 1
        }
        kernel[k*k/2] = k*k;
        normz = 1;
    }

    // ***COMENTAR/DESCOMENTAR***
    // Si seleccionamos el filtro gaussiano (spinBox = 2)
    if (ker == 2) {
        kernel[24] = (kernel[20] = (kernel[4] = (kernel[0] = 1)));
        kernel[23] = (kernel[21] = (kernel[19] = (kernel[15] = (kernel[9] = (kernel[5] = (kernel[3] = (kernel[1] = 4)))))));
        kernel[22] = (kernel[14] = (kernel[10] = (kernel[2] = 7)));
        kernel[18] = (kernel[16] = (kernel[8] = (kernel[6] = 20)));
        kernel[17] = (kernel[13] = (kernel[11] = (kernel[7] = 33)));
        kernel[12] = 55;
        normz = 4*(1+7+20+33) + 8*4 + 55;
    }

    // ***COMENTAR/DESCOMENTAR***
    // Si seleccionamos el filtro unitario (spinBox = 3)
    if (ker == 3) {
        for (int u=0, c=0; u<k; u++) for (int v=0; v<k; v++, c++) {
            if (c==(k*k/2)) {
                kernel[c] = 1;   // Vamos añadiendo 1 elemento a normz (al final tendremos el número de elementos del vecindario, es decir, k x k) al mismo tiempo que establecemos el valor de cada elemento del kernel a 1
            }
            else {
                kernel[c] = 0;
            }
        }
        normz = 1;
    }

    // Recorre toda la imagen
    for (int y=k/2;y<(H-k/2);y++) for(int x=k/2;x<(W-k/2);x++) {
        // Declaramos valores de suma para las 3 componentes
        float sumaR = 0;
        float sumaG = 0;
        float sumaB = 0;
        // Recorremos vecindario k x k centrado en nuestro pixel
        for (int yp=(y-k/2),c=0;yp<=(y+k/2);yp++) for(int xp=(x-k/2);xp<=(x+k/2);xp++,c++) {
                // Vamos sumando el valor de cada uno de los píxeles vecinos
                sumaR += pixR[yp*S+3*xp]*kernel[c];
                sumaG += pixG[yp*S+3*xp]*kernel[c];
                sumaB += pixB[yp*S+3*xp]*kernel[c];
        }
        // Dividimos por el total de píxeles del vecindario (i.e., normalizamos sumaR, sumaG y sumaB)
        sumaR /= normz;
        sumaG /= normz;
        sumaB /= normz;
        // Metemos el valor obtenido al pixel central
        pixBlur[y*S+3*x] = sumaR;
        pixBlur[y*S+3*x+1] = sumaG;
        pixBlur[y*S+3*x+2] = sumaB;
    }

    // Copia y limpieza
    delete [] kernel;
    memcpy(pixR,pixBlur,S*H);
}


//------------------------------------------------------------
//----- Operación de dilatación sobre la máscara binaria -----
//------------------------------------------------------------
void FPImage::on_BDilatation_clicked()
{
    // Creación del SE
    SE_side = ui->SE_Slider->value();
    uchar *SE = new uchar[SE_side*SE_side];

    // Inicialización del SE a un círculo
    if(ui->RBCirculo->isChecked()) {
        int r = SE_side/2;
        int r2 = r*r;
        for(int c=0,y=-r; y<=r; ++y) for(int x=-r; x<=r; ++x, ++c) {
            SE[c] = y*y+x*x<=r2?1:0;
        }
    }
    // Inicialización del SE a un cuadrado
    else if(ui->RBCuadrado->isChecked()) {
        int SE_side2 = SE_side*SE_side;
        memset(SE,1,SE_side2);
    }
    // Inicialización del SE a una línea vertical
    else if(ui->RBLinea->isChecked()) {
        for(int c=0,x=(SE_side/2-3); x<=(SE_side/2+3); ++x) for(int y=0; y<=SE_side; ++y, ++c) {
            SE[c] = 1;
        }
    }
    // Inicialización del SE a una elipse
    else if(ui->RBElipse->isChecked()) {
        // ...
    }

    // Declaramos copia de la máscara binaria
    uchar *OldMask = new uchar[W*H];
    memcpy(OldMask,mask,W*H);

    // Recorremos
    for(int y=SE_side/2; y<H-SE_side/2; ++y) for(int x=SE_side/2; x<W-SE_side/2; ++x) {
        uchar &center = mask[y*W+x];
        for (int c=0, yy=y-SE_side/2; !center && yy<=y+SE_side/2; ++yy) for (int xx=x-SE_side/2; !center && xx<=x+SE_side/2; ++xx, ++c) {
            if(SE[c] && OldMask[yy*W+xx]) {
                mask[y*W+x] = 1;
            }
        }
    }

    //Limpiamos
    delete [] OldMask;
    delete [] SE;

    // Mostramos máscara
    ShowMask();
}


//---------------------------------------------------------
//----- Operación de erosión sobre la máscara binaria -----
//---------------------------------------------------------
void FPImage::on_BErosion_clicked()
{
    // Creación del SE
    SE_side = ui->SE_Slider->value();
    uchar *SE = new uchar[SE_side*SE_side];
    for (int i=0; i<= SE_side*SE_side; i++) {
        SE[i] = 0;
    }

    // Inicialización del SE a un círculo
    if(ui->RBCirculo->isChecked()) {
        int r = SE_side/2;
        int r2 = r*r;
        for(int c=0,y=-r; y<=r; ++y) for(int x=-r; x<=r; ++x, ++c) {
            SE[c] = y*y+x*x<=r2?1:0;
        }
    }
    // Inicialización del SE a un cuadrado
    else if(ui->RBCuadrado->isChecked()) {
        int SE_side2 = SE_side*SE_side;
        memset(SE,1,SE_side2);
    }
    // Inicialización del SE a una línea vertical
    else if(ui->RBLinea->isChecked()) {
        for(int c=0,x=(SE_side/2-3); x<=(SE_side/2+3); ++x) for(int y=0; y<=SE_side; ++y, ++c) {
            SE[c] = 1;
        }
    }
    // Inicialización del SE a una elipse
    else if(ui->RBElipse->isChecked()) {
        // ...
    }

    // Declaramos copia de la máscara binaria
    uchar *OldMask = new uchar[W*H];
    memcpy(OldMask,mask,W*H);

    // Recorremos
    for(int y=SE_side/2; y<H-SE_side/2; ++y) for(int x=SE_side/2; x<W-SE_side/2; ++x) {
        uchar &center = mask[y*W+x];
        for (int c=0, yy=y-SE_side/2; center && yy<=y+SE_side/2; ++yy) for (int xx=x-SE_side/2; center && xx<=x+SE_side/2; ++xx, ++c) {
            if(SE[c] && !OldMask[yy*W+xx]) {
                center = 0;
            }
        }
    }

    //Limpiamos
    delete [] OldMask;
    delete [] SE;

    // Mostramos máscara
    ShowMask();
}


//-----------------------------------------------
//----- Visualización de la máscara binaria -----
//-----------------------------------------------
void FPImage::ShowMask()
{
    Restore();
    // Recorre toda la imagen
    for(int y=0,i=0;y<H;y++,i+=Padding) for(int x=0;x<W;x++,i+=3) {
        // Si la máscara es 0, pintamos a negro
        if (!mask[i/3]) {
           pixR[i] = (pixG[i] = (pixB[i] = 0));
        }
    }

    // Mostramos la máscara resultante
    ShowIt();
}


//---------------------
//----- Labelling -----
//---------------------
void FPImage::on_BLabelling_clicked()
{
    memset(labelling,0,W*H*sizeof(int));
    // Establece a -1 (en la matriz labelling) aquellas posiciones en las que la máscara vale 1
    for (int i=0; i<=W*H; i++) {
        labelling[i] -= mask[i];
    }

    // Inicialización de label
    label = 0;

    // Inicialización de Area y Perims;
    for (int i=0; i<=1000; i++) {
        Area[i] = 0;
        Perims[i] = 0;
        AspectRatio[i] = 0;
    }

    // Painter
    QPainter pE(&Image);
    pE.setPen(QPen(Qt::yellow));

    Restore();

    // Recorre la imagen
    for (int y=0; y<H; y++) for (int x=0; x<W; x++) {
        // Si el píxel ha sido marcado para etiquetado
        if (labelling[y*W+x]<0) {
            // Inicialización de NPix y Perim
            NPix = 0;
            Perim = 0;
            // Inicialización de SumaY y SumaX
            SumaY = 0;
            SumaX = 0;
            // Inicialización de Ymin, Ymax, Xmin y Xmax
            Ymax = (Ymin = y);
            Xmax = (Xmin = x);
            // Actualizamos valor de label y etiquetamos
            label++;
            PutLabel(y*W+x);
            // Actualizamos las matrices de áreas y perímetros de blobs
            Area[label-1] = NPix;
            Perims[label-1] = Perim;
            // Hallamos la compacidad del blob y la añadimos a la matriz de compacidades
            float numComp = 4*3.14*Area[label-1];
            float denomComp = Perims[label-1]*Perims[label-1];
            Comps[label-1] = numComp/denomComp;
            // Hallamos el aspect ratio y lo añadimos a la matriz de aspect ratios
            float numAR = Ymax-Ymin;
            float denomAR = Xmax-Xmin;
            AspectRatio[label-1] = numAR/denomAR;
            // Comprobabr si el blob es significativo
            if (AspectRatio[label-1]>1 && Comps[label-1]>0.4 && Area[label-1]>450) {
                // Dibujamos rectángulo
                pE.setPen(QPen(Qt::yellow));
                pE.drawRect(Xmin,Ymin,denomAR,numAR);
                // Dibujamos el centroide
                pE.setPen(QPen(Qt::blue));
                pE.drawRect((SumaX/NPix-1),(SumaY/NPix-1),3,3);
                // Dibujamos el centro geométrico de la bounding box
                pE.setPen(QPen(Qt::red));
                pE.drawRect(Xmin+(denomAR/2)-1,Ymin+(numAR/2)-1,3,3);
            }
        }
    }

//    // Recorre la imagen
//    for (int y=0; y<H; y++) for (int x=0; x<W; x++) {
//        // Pinta cada blob de un color diferente (hace uso de números primos)
//        pixR[y*S+3*x] = labelling[y*W+x]*23;
//        pixG[y*S+3*x] = labelling[y*W+x]*13;
//        pixB[y*S+3*x] = labelling[y*W+x]*73;
//    }

    ShowIt();

}


//---------------------
//----- Función recursiva para etiquetado -----
//---------------------
void FPImage::PutLabel(int pos)
{
    // Etiquetamos
    labelling[pos] = label;

    // Añadimos 1 al número de píxeles del blob
    NPix++;

    // Actualizamos SumaY y SumaX
    SumaY += pos/W;
    SumaX += pos%W;

    // Añadimos 1 a Perim en caso de que el píxel actual sea parte del perímetro (i.e., si alguno de sus vecinos es 0)
    if (labelling[pos-1]==0 || labelling[pos+1]==0 || labelling[pos-W]==0 || labelling[pos+W]==0) {
        Perim++;
    }

    // Actualizamos Ymin, Ymax, Xmin y Xmax en caso necesario
    if (pos%W<Xmin) {
        Xmin = pos%W;
    }
    if (pos%W>Xmax) {
        Xmax = pos%W;
    }
    if (pos/W<Ymin) {
        Ymin = pos/W;
    }
    if (pos/W>Ymax) {
        Ymax = pos/W;
    }

    // Comprobamos los vecinos (si no se salen de los bordes) y etiquetamos (mediante llamadas recursivas) en caso necesario
    if (pos>=0 && labelling[pos-1]<0) { // Vecino de la izquierda
        PutLabel(pos-1);
    }
    if (pos>=0 && labelling[pos-W]<0) {  // Vecino de arriba
        PutLabel(pos-W);
    }
    if (pos<=W*H && labelling[pos+1]<0) { // Vecino de la derecha
        PutLabel(pos+1);
    }
    if (pos<=W*H && labelling[pos+W]<0) {  // Vecino de abajo
        PutLabel(pos+W);
    }
}


void FPImage::on_N2_clicked()
{
    norma = 2;
}

void FPImage::on_N1_clicked()
{
    norma = 1;
}

void FPImage::on_Ninf_clicked()
{
    norma = 0;
}


void FPImage::on_SAM_clicked()
{
    norma = 3;
}




void FPImage::on_BRp_valueChanged(int value)
{
    L_avrg = value;
    on_BSkinDetector_clicked();
    ui->ERes->appendPlainText("Lavrg "+QString::number(L_avrg));
}


void FPImage::on_BRvar_valueChanged(int value)
{
    L_stdev = value;
    on_BSkinDetector_clicked();
    ui->ERes->appendPlainText("Lstdev "+QString::number(L_stdev));
}


void FPImage::on_BGp_valueChanged(int value)
{
    H_avrg = value;
    on_BSkinDetector_clicked();
    ui->ERes->appendPlainText("Havrg "+QString::number(H_avrg));
}


void FPImage::on_BGvar_valueChanged(int value)
{
    H_stdev = value;
    on_BSkinDetector_clicked();
    ui->ERes->appendPlainText("Hstdev "+QString::number(H_stdev));
}


void FPImage::on_BBp_valueChanged(int value)
{
    S_avrg = value;
    on_BSkinDetector_clicked();
    ui->ERes->appendPlainText("Savrg "+QString::number(S_avrg));
}


void FPImage::on_BBvar_valueChanged(int value)
{
    S_stdev = value;
    on_BSkinDetector_clicked();
    ui->ERes->appendPlainText("Sstdev "+QString::number(S_stdev));
}

void FPImage::on_spinBox_valueChanged(int arg1)
{
    ker = arg1;
}


void FPImage::on_SE_Slider_valueChanged(int value)
{
    //COMMENT FOLLOWING LINE
    //SE_side = value;
    ui->ERes->appendPlainText("SE_side "+QString::number(value));
}

