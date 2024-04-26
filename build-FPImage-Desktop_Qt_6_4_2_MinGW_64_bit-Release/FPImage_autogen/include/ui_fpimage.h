/********************************************************************************
** Form generated from reading UI file 'fpimage.ui'
**
** Created by: Qt User Interface Compiler version 6.4.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FPIMAGE_H
#define UI_FPIMAGE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_FPImage
{
public:
    QWidget *centralWidget;
    QHBoxLayout *horizontalLayout_2;
    QVBoxLayout *verticalLayout;
    QPlainTextEdit *ERes;
    QLineEdit *EFile;
    QSlider *BrightnessSlider;
    QSlider *ContrastSlider;
    QSlider *EdgeSlider;
    QPushButton *BEq;
    QPushButton *BLinearStretch;
    QPushButton *BSepia;
    QPushButton *BGrayScale;
    QHBoxLayout *horizontalLayout_6;
    QPushButton *BFlowerOutline;
    QPushButton *BLeavesOutline;
    QPushButton *BDoIt;
    QHBoxLayout *horizontalLayout;
    QPushButton *BLoad;
    QPushButton *BSave;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QGridLayout *gridLayout;
    QLabel *Ecran;
    QVBoxLayout *verticalLayout_2;
    QLabel *EcranHistoR;
    QLabel *EcranHistoG;
    QLabel *EcranHistoB;
    QGroupBox *GNorma;
    QRadioButton *N1;
    QRadioButton *N2;
    QRadioButton *Ninf;
    QRadioButton *SAM;
    QSlider *BRp;
    QSlider *BRvar;
    QSlider *BGp;
    QSlider *BBp;
    QSlider *BBvar;
    QSlider *BGvar;
    QPushButton *BSkinDetector;
    QSpinBox *spinBox;
    QRadioButton *RBCirculo;
    QRadioButton *RBElipse;
    QRadioButton *RBLinea;
    QRadioButton *RBCuadrado;
    QPushButton *BDilatation;
    QPushButton *BErosion;
    QSlider *SE_Slider;
    QPushButton *BLabelling;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *FPImage)
    {
        if (FPImage->objectName().isEmpty())
            FPImage->setObjectName("FPImage");
        FPImage->resize(1260, 782);
        centralWidget = new QWidget(FPImage);
        centralWidget->setObjectName("centralWidget");
        horizontalLayout_2 = new QHBoxLayout(centralWidget);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
        ERes = new QPlainTextEdit(centralWidget);
        ERes->setObjectName("ERes");

        verticalLayout->addWidget(ERes);

        EFile = new QLineEdit(centralWidget);
        EFile->setObjectName("EFile");

        verticalLayout->addWidget(EFile);

        BrightnessSlider = new QSlider(centralWidget);
        BrightnessSlider->setObjectName("BrightnessSlider");
        BrightnessSlider->setMinimum(-255);
        BrightnessSlider->setMaximum(255);
        BrightnessSlider->setValue(0);
        BrightnessSlider->setSliderPosition(0);
        BrightnessSlider->setOrientation(Qt::Horizontal);
        BrightnessSlider->setTickPosition(QSlider::NoTicks);

        verticalLayout->addWidget(BrightnessSlider);

        ContrastSlider = new QSlider(centralWidget);
        ContrastSlider->setObjectName("ContrastSlider");
        ContrastSlider->setMinimum(0);
        ContrastSlider->setMaximum(890);
        ContrastSlider->setValue(450);
        ContrastSlider->setSliderPosition(450);
        ContrastSlider->setOrientation(Qt::Horizontal);

        verticalLayout->addWidget(ContrastSlider);

        EdgeSlider = new QSlider(centralWidget);
        EdgeSlider->setObjectName("EdgeSlider");
        EdgeSlider->setMaximum(130);
        EdgeSlider->setOrientation(Qt::Horizontal);

        verticalLayout->addWidget(EdgeSlider);

        BEq = new QPushButton(centralWidget);
        BEq->setObjectName("BEq");

        verticalLayout->addWidget(BEq);

        BLinearStretch = new QPushButton(centralWidget);
        BLinearStretch->setObjectName("BLinearStretch");

        verticalLayout->addWidget(BLinearStretch);

        BSepia = new QPushButton(centralWidget);
        BSepia->setObjectName("BSepia");

        verticalLayout->addWidget(BSepia);

        BGrayScale = new QPushButton(centralWidget);
        BGrayScale->setObjectName("BGrayScale");

        verticalLayout->addWidget(BGrayScale);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setSpacing(6);
        horizontalLayout_6->setObjectName("horizontalLayout_6");
        BFlowerOutline = new QPushButton(centralWidget);
        BFlowerOutline->setObjectName("BFlowerOutline");

        horizontalLayout_6->addWidget(BFlowerOutline);

        BLeavesOutline = new QPushButton(centralWidget);
        BLeavesOutline->setObjectName("BLeavesOutline");

        horizontalLayout_6->addWidget(BLeavesOutline);


        verticalLayout->addLayout(horizontalLayout_6);

        BDoIt = new QPushButton(centralWidget);
        BDoIt->setObjectName("BDoIt");

        verticalLayout->addWidget(BDoIt);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName("horizontalLayout");
        BLoad = new QPushButton(centralWidget);
        BLoad->setObjectName("BLoad");

        horizontalLayout->addWidget(BLoad);

        BSave = new QPushButton(centralWidget);
        BSave->setObjectName("BSave");

        horizontalLayout->addWidget(BSave);


        verticalLayout->addLayout(horizontalLayout);


        horizontalLayout_2->addLayout(verticalLayout);

        scrollArea = new QScrollArea(centralWidget);
        scrollArea->setObjectName("scrollArea");
        scrollArea->setFrameShape(QFrame::Panel);
        scrollArea->setLineWidth(2);
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName("scrollAreaWidgetContents");
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 706, 700));
        gridLayout = new QGridLayout(scrollAreaWidgetContents);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName("gridLayout");
        Ecran = new QLabel(scrollAreaWidgetContents);
        Ecran->setObjectName("Ecran");

        gridLayout->addWidget(Ecran, 0, 2, 1, 1);

        scrollArea->setWidget(scrollAreaWidgetContents);

        horizontalLayout_2->addWidget(scrollArea);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setObjectName("verticalLayout_2");
        EcranHistoR = new QLabel(centralWidget);
        EcranHistoR->setObjectName("EcranHistoR");
        EcranHistoR->setMinimumSize(QSize(260, 104));
        EcranHistoR->setMaximumSize(QSize(260, 104));
        EcranHistoR->setFrameShape(QFrame::Panel);
        EcranHistoR->setFrameShadow(QFrame::Sunken);
        EcranHistoR->setLineWidth(2);
        EcranHistoR->setMidLineWidth(0);

        verticalLayout_2->addWidget(EcranHistoR);

        EcranHistoG = new QLabel(centralWidget);
        EcranHistoG->setObjectName("EcranHistoG");
        EcranHistoG->setMinimumSize(QSize(260, 104));
        EcranHistoG->setMaximumSize(QSize(260, 104));
        EcranHistoG->setFrameShape(QFrame::Panel);
        EcranHistoG->setFrameShadow(QFrame::Sunken);
        EcranHistoG->setLineWidth(2);

        verticalLayout_2->addWidget(EcranHistoG);

        EcranHistoB = new QLabel(centralWidget);
        EcranHistoB->setObjectName("EcranHistoB");
        EcranHistoB->setMinimumSize(QSize(260, 104));
        EcranHistoB->setMaximumSize(QSize(260, 104));
        EcranHistoB->setFrameShape(QFrame::Panel);
        EcranHistoB->setFrameShadow(QFrame::Sunken);
        EcranHistoB->setLineWidth(2);

        verticalLayout_2->addWidget(EcranHistoB);

        GNorma = new QGroupBox(centralWidget);
        GNorma->setObjectName("GNorma");
        N1 = new QRadioButton(GNorma);
        N1->setObjectName("N1");
        N1->setGeometry(QRect(140, 20, 111, 22));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(N1->sizePolicy().hasHeightForWidth());
        N1->setSizePolicy(sizePolicy);
        N2 = new QRadioButton(GNorma);
        N2->setObjectName("N2");
        N2->setGeometry(QRect(10, 20, 111, 22));
        Ninf = new QRadioButton(GNorma);
        Ninf->setObjectName("Ninf");
        Ninf->setGeometry(QRect(10, 40, 111, 22));
        sizePolicy.setHeightForWidth(Ninf->sizePolicy().hasHeightForWidth());
        Ninf->setSizePolicy(sizePolicy);
        SAM = new QRadioButton(GNorma);
        SAM->setObjectName("SAM");
        SAM->setGeometry(QRect(140, 40, 111, 22));
        BRp = new QSlider(GNorma);
        BRp->setObjectName("BRp");
        BRp->setGeometry(QRect(10, 70, 241, 16));
        BRp->setMaximum(255);
        BRp->setValue(72);
        BRp->setOrientation(Qt::Horizontal);
        BRvar = new QSlider(GNorma);
        BRvar->setObjectName("BRvar");
        BRvar->setGeometry(QRect(10, 90, 241, 16));
        BRvar->setMaximum(50);
        BRvar->setValue(25);
        BRvar->setOrientation(Qt::Horizontal);
        BGp = new QSlider(GNorma);
        BGp->setObjectName("BGp");
        BGp->setGeometry(QRect(10, 110, 241, 16));
        BGp->setMaximum(255);
        BGp->setValue(164);
        BGp->setOrientation(Qt::Horizontal);
        BBp = new QSlider(GNorma);
        BBp->setObjectName("BBp");
        BBp->setGeometry(QRect(10, 150, 241, 16));
        BBp->setMaximum(255);
        BBp->setValue(17);
        BBp->setOrientation(Qt::Horizontal);
        BBvar = new QSlider(GNorma);
        BBvar->setObjectName("BBvar");
        BBvar->setGeometry(QRect(10, 170, 241, 16));
        BBvar->setMaximum(50);
        BBvar->setValue(9);
        BBvar->setOrientation(Qt::Horizontal);
        BGvar = new QSlider(GNorma);
        BGvar->setObjectName("BGvar");
        BGvar->setGeometry(QRect(10, 130, 241, 16));
        BGvar->setMaximum(50);
        BGvar->setValue(30);
        BGvar->setOrientation(Qt::Horizontal);
        BSkinDetector = new QPushButton(GNorma);
        BSkinDetector->setObjectName("BSkinDetector");
        BSkinDetector->setGeometry(QRect(5, 190, 121, 21));
        spinBox = new QSpinBox(GNorma);
        spinBox->setObjectName("spinBox");
        spinBox->setGeometry(QRect(140, 190, 42, 25));
        RBCirculo = new QRadioButton(GNorma);
        RBCirculo->setObjectName("RBCirculo");
        RBCirculo->setGeometry(QRect(10, 220, 111, 22));
        RBElipse = new QRadioButton(GNorma);
        RBElipse->setObjectName("RBElipse");
        RBElipse->setGeometry(QRect(140, 240, 111, 22));
        RBLinea = new QRadioButton(GNorma);
        RBLinea->setObjectName("RBLinea");
        RBLinea->setGeometry(QRect(10, 240, 111, 22));
        RBCuadrado = new QRadioButton(GNorma);
        RBCuadrado->setObjectName("RBCuadrado");
        RBCuadrado->setGeometry(QRect(140, 220, 111, 22));
        BDilatation = new QPushButton(GNorma);
        BDilatation->setObjectName("BDilatation");
        BDilatation->setGeometry(QRect(10, 270, 121, 24));
        BErosion = new QPushButton(GNorma);
        BErosion->setObjectName("BErosion");
        BErosion->setGeometry(QRect(140, 270, 111, 24));
        SE_Slider = new QSlider(GNorma);
        SE_Slider->setObjectName("SE_Slider");
        SE_Slider->setGeometry(QRect(10, 300, 241, 20));
        SE_Slider->setMinimum(3);
        SE_Slider->setValue(26);
        SE_Slider->setOrientation(Qt::Horizontal);
        BLabelling = new QPushButton(GNorma);
        BLabelling->setObjectName("BLabelling");
        BLabelling->setGeometry(QRect(10, 330, 121, 24));

        verticalLayout_2->addWidget(GNorma);


        horizontalLayout_2->addLayout(verticalLayout_2);

        horizontalLayout_2->setStretch(1, 5);
        FPImage->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(FPImage);
        menuBar->setObjectName("menuBar");
        menuBar->setGeometry(QRect(0, 0, 1260, 21));
        FPImage->setMenuBar(menuBar);
        mainToolBar = new QToolBar(FPImage);
        mainToolBar->setObjectName("mainToolBar");
        FPImage->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(FPImage);
        statusBar->setObjectName("statusBar");
        FPImage->setStatusBar(statusBar);

        retranslateUi(FPImage);

        QMetaObject::connectSlotsByName(FPImage);
    } // setupUi

    void retranslateUi(QMainWindow *FPImage)
    {
        FPImage->setWindowTitle(QCoreApplication::translate("FPImage", "FPImage", nullptr));
        BEq->setText(QCoreApplication::translate("FPImage", "Equalization", nullptr));
        BLinearStretch->setText(QCoreApplication::translate("FPImage", "LinearStretch", nullptr));
        BSepia->setText(QCoreApplication::translate("FPImage", "Sepia", nullptr));
        BGrayScale->setText(QCoreApplication::translate("FPImage", "GrayScale", nullptr));
        BFlowerOutline->setText(QCoreApplication::translate("FPImage", "FlowerOutline", nullptr));
        BLeavesOutline->setText(QCoreApplication::translate("FPImage", "LeavesOutline", nullptr));
        BDoIt->setText(QCoreApplication::translate("FPImage", "Do it", nullptr));
        BLoad->setText(QCoreApplication::translate("FPImage", "Load", nullptr));
        BSave->setText(QCoreApplication::translate("FPImage", "Save", nullptr));
        Ecran->setText(QString());
        EcranHistoR->setText(QString());
        EcranHistoG->setText(QString());
        EcranHistoB->setText(QString());
        GNorma->setTitle(QCoreApplication::translate("FPImage", "M\303\251tricas", nullptr));
        N1->setText(QCoreApplication::translate("FPImage", "Norma 1", nullptr));
        N2->setText(QCoreApplication::translate("FPImage", "Norma 2", nullptr));
        Ninf->setText(QCoreApplication::translate("FPImage", "Norma infinito", nullptr));
        SAM->setText(QCoreApplication::translate("FPImage", "SAM", nullptr));
        BSkinDetector->setText(QCoreApplication::translate("FPImage", "Skin Detector", nullptr));
        RBCirculo->setText(QCoreApplication::translate("FPImage", "C\303\255rculo", nullptr));
        RBElipse->setText(QCoreApplication::translate("FPImage", "Elipse", nullptr));
        RBLinea->setText(QCoreApplication::translate("FPImage", "Linea", nullptr));
        RBCuadrado->setText(QCoreApplication::translate("FPImage", "Cuadrado", nullptr));
        BDilatation->setText(QCoreApplication::translate("FPImage", "Dilatation", nullptr));
        BErosion->setText(QCoreApplication::translate("FPImage", "Erosion", nullptr));
        BLabelling->setText(QCoreApplication::translate("FPImage", "Labelling", nullptr));
    } // retranslateUi

};

namespace Ui {
    class FPImage: public Ui_FPImage {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FPIMAGE_H
