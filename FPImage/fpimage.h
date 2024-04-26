#ifndef FPIMAGE_H
#define FPIMAGE_H


#include <QMainWindow>

namespace Ui {
class FPImage;
}

class FPImage : public QMainWindow
{
    Q_OBJECT

public:
    explicit FPImage(QWidget *parent = 0);
    ~FPImage();

private:
    Ui::FPImage *ui;

    QString Path;               // Para recordar la carpeta al cargar imágenes
    QImage Image;               // Imagen Qt
    uchar *pixR, *pixG, *pixB;  // Punteros a los tres canales R, G y B
    uchar *pixBlur;             // Puntero a la imagen con blur
    uchar *mask;                 // Puntero a la máscara binaria
    int *labelling;              // Puntero a la matriz de labelling
    int W, H;                   // Tamaño de la imagen actual
    int S, Padding;                // Step en imágenes no continuas
    float c, n, b;                 // Pendiente y punto de corte de la recta que gobierna contraste y brillo
    uchar L, Hab, Sab;

    int norma;

    int blurApplied;

    int SE_side;

    uchar LUT[256];
    uchar LUT_HistoR[256];
    uchar LUT_HistoG[256];
    uchar LUT_HistoB[256];
    uchar LUT_HistoAcR[256];
    uchar LUT_HistoAcG[256];
    uchar LUT_HistoAcB[256];

    int histoR[256];            // Histogramas para las 3 componentes
    int histoG[256];
    int histoB[256];

    int histoAcR[256];            // Histogramas acumulados para las 3 componentes
    int histoAcG[256];
    int histoAcB[256];

    int histoMiniR[256];         // Mini histogramas para las 3 componentes
    int histoMiniG[256];
    int histoMiniB[256];

    uchar *PixCopia;

    QPixmap Dib1, Dib2, Dib3;   // Tres lienzos en los que dibujar
    QPixmap DibE;

    void ShowIt(void);          // Muestra la imagen actual

    bool eventFilter(QObject *Ob, QEvent *Ev);  // Un "filtro de eventos"

    void ContrastChanger(); // Cambio de contraste y brillo

    float L_avrg, H_avrg, S_avrg;
    float L_stdev, H_stdev, S_stdev;

    int ker;

    int NPix, Perim;   // Número de píxeles que forman parte de un blob y de su perímetro
    int Area[1000], Perims[1000]; // Area en píxeles de cada blob y longitud de sus perímetros
    float Comps[1000];              // Compacidades de cada uno de los blobs
    int label;
    int Ymin, Ymax, Xmin, Xmax;
    int SumaY, SumaX;
    float AspectRatio[1000];

private slots:
    void Load(void);    // Slot para el botón de carga de imagen
    void DoIt(void);    // Slot para el botón de hacer algo con la imagen
    void GrayScale (void);  // Slot para cambiar la imagen a escala de grises
    void FlowerOutline(void);   // Slot para resaltar solo la flor
    void on_BLeavesOutline_clicked();    // Slot para resaltar todo menos la flor
    void on_BSepia_clicked();   // Slot para pasar a modo sepia
    void on_BrightnessSlider_valueChanged(int value);   // Slot para cambiar el brillo
    void Restore(void); // Restaurar la imagen
    void on_BSave_clicked();
    void on_ContrastSlider_valueChanged(int value);
    void Edges(int);
    void Histogram();   // Calcular histograma de las 3 componentes
    void MiniHistogram(int x, int y);   // Calcula mini histograma, en un área 25x25 centrado en el pixel de coordenadas [x,y]
    void TempHistogram();   // Calcula mini histograma de cada pixel de la imagen y opera según el parecido con el parecido con el mini histograma del punto que hayamos seleccionado
    void on_EdgeSlider_valueChanged(int value);
    void on_N2_clicked();
    void on_Ninf_clicked();
    void on_N1_clicked();
    void on_SAM_clicked();
    void on_BLinearStretch_clicked();
    void on_BEq_clicked();
    void on_BSkinDetector_clicked();
    void on_BRp_valueChanged(int value);
    void on_BRvar_valueChanged(int value);
    void on_BGp_valueChanged(int value);
    void on_BGvar_valueChanged(int value);
    void on_BBp_valueChanged(int value);
    void on_BBvar_valueChanged(int value);
    void FeatureSpaceShadows(int x, int y);
    void RGB2LHS(uchar r, uchar g, uchar b, uchar &L, uchar &H, uchar &S);
    void Filtering(int k);   // Aplica un blur a la imagen (a partir de un filtrado de media)
    void on_spinBox_valueChanged(int arg1);
    void on_BDilatation_clicked();
    void on_BErosion_clicked();
    void ShowMask();
    void on_SE_Slider_valueChanged(int value);
    void PutLabel(int pos);
    void on_BLabelling_clicked();
};

#endif // FPIMAGE_H
