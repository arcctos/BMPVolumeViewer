/********************************************************************************
** Form generated from reading UI file 'BMPVolumeViewer.ui'
**
** Created by: Qt User Interface Compiler version 5.12.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BMPVOLUMEVIEWER_H
#define UI_BMPVOLUMEVIEWER_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <qvtkopenglnativewidget.h>

QT_BEGIN_NAMESPACE

class Ui_BMPVolumeViewer
{
public:
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QGroupBox *groupBox;
    QGridLayout *gridLayout;
    QLabel *label;
    QLineEdit *filePrefixEdit;
    QPushButton *browseButton;
    QLabel *label_2;
    QLineEdit *filePatternEdit;
    QPushButton *loadButton;
    QHBoxLayout *horizontalLayout_2;
    QGroupBox *volumeGroupBox;
    QVBoxLayout *verticalLayout_2;
    QVTKOpenGLNativeWidget *volumeWidget;
    QVBoxLayout *verticalLayout_3;
    QGroupBox *xSliceGroupBox;
    QVBoxLayout *verticalLayout_4;
    QVTKOpenGLNativeWidget *xSliceWidget;
    QHBoxLayout *horizontalLayout_3;
    QLabel *xSliceLabel;
    QSlider *xSliceSlider;
    QGroupBox *ySliceGroupBox;
    QVBoxLayout *verticalLayout_5;
    QVTKOpenGLNativeWidget *ySliceWidget;
    QHBoxLayout *horizontalLayout_4;
    QLabel *ySliceLabel;
    QSlider *ySliceSlider;
    QGroupBox *controlGroupBox;
    QHBoxLayout *horizontalLayout;
    QLabel *opacityLabel;
    QSlider *opacitySlider;
    QLabel *colorLabel;
    QSlider *colorSlider;
    QPushButton *resetViewButton;

    void setupUi(QMainWindow *BMPVolumeViewer)
    {
        if (BMPVolumeViewer->objectName().isEmpty())
            BMPVolumeViewer->setObjectName(QString::fromUtf8("BMPVolumeViewer"));
        BMPVolumeViewer->resize(1200, 1200);
        centralWidget = new QWidget(BMPVolumeViewer);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        groupBox = new QGroupBox(centralWidget);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        gridLayout = new QGridLayout(groupBox);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 0, 0, 1, 1);

        filePrefixEdit = new QLineEdit(groupBox);
        filePrefixEdit->setObjectName(QString::fromUtf8("filePrefixEdit"));

        gridLayout->addWidget(filePrefixEdit, 0, 1, 1, 1);

        browseButton = new QPushButton(groupBox);
        browseButton->setObjectName(QString::fromUtf8("browseButton"));

        gridLayout->addWidget(browseButton, 0, 2, 1, 1);

        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout->addWidget(label_2, 1, 0, 1, 1);

        filePatternEdit = new QLineEdit(groupBox);
        filePatternEdit->setObjectName(QString::fromUtf8("filePatternEdit"));

        gridLayout->addWidget(filePatternEdit, 1, 1, 1, 2);

        loadButton = new QPushButton(groupBox);
        loadButton->setObjectName(QString::fromUtf8("loadButton"));

        gridLayout->addWidget(loadButton, 2, 0, 1, 3);


        verticalLayout->addWidget(groupBox);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        volumeGroupBox = new QGroupBox(centralWidget);
        volumeGroupBox->setObjectName(QString::fromUtf8("volumeGroupBox"));
        verticalLayout_2 = new QVBoxLayout(volumeGroupBox);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        volumeWidget = new QVTKOpenGLNativeWidget(volumeGroupBox);
        volumeWidget->setObjectName(QString::fromUtf8("volumeWidget"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(volumeWidget->sizePolicy().hasHeightForWidth());
        volumeWidget->setSizePolicy(sizePolicy);
        volumeWidget->setMinimumSize(QSize(600, 600));

        verticalLayout_2->addWidget(volumeWidget);


        horizontalLayout_2->addWidget(volumeGroupBox);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        xSliceGroupBox = new QGroupBox(centralWidget);
        xSliceGroupBox->setObjectName(QString::fromUtf8("xSliceGroupBox"));
        verticalLayout_4 = new QVBoxLayout(xSliceGroupBox);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        xSliceWidget = new QVTKOpenGLNativeWidget(xSliceGroupBox);
        xSliceWidget->setObjectName(QString::fromUtf8("xSliceWidget"));
        xSliceWidget->setMinimumSize(QSize(300, 300));
        sizePolicy.setHeightForWidth(xSliceWidget->sizePolicy().hasHeightForWidth());
        xSliceWidget->setSizePolicy(sizePolicy);

        verticalLayout_4->addWidget(xSliceWidget);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        xSliceLabel = new QLabel(xSliceGroupBox);
        xSliceLabel->setObjectName(QString::fromUtf8("xSliceLabel"));

        horizontalLayout_3->addWidget(xSliceLabel);

        xSliceSlider = new QSlider(xSliceGroupBox);
        xSliceSlider->setObjectName(QString::fromUtf8("xSliceSlider"));
        xSliceSlider->setOrientation(Qt::Horizontal);
        xSliceSlider->setMinimum(0);
        xSliceSlider->setMaximum(100);
        xSliceSlider->setValue(50);

        horizontalLayout_3->addWidget(xSliceSlider);


        verticalLayout_4->addLayout(horizontalLayout_3);


        verticalLayout_3->addWidget(xSliceGroupBox);

        ySliceGroupBox = new QGroupBox(centralWidget);
        ySliceGroupBox->setObjectName(QString::fromUtf8("ySliceGroupBox"));
        verticalLayout_5 = new QVBoxLayout(ySliceGroupBox);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        ySliceWidget = new QVTKOpenGLNativeWidget(ySliceGroupBox);
        ySliceWidget->setObjectName(QString::fromUtf8("ySliceWidget"));
        ySliceWidget->setMinimumSize(QSize(300, 300));
        sizePolicy.setHeightForWidth(ySliceWidget->sizePolicy().hasHeightForWidth());
        ySliceWidget->setSizePolicy(sizePolicy);

        verticalLayout_5->addWidget(ySliceWidget);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        ySliceLabel = new QLabel(ySliceGroupBox);
        ySliceLabel->setObjectName(QString::fromUtf8("ySliceLabel"));

        horizontalLayout_4->addWidget(ySliceLabel);

        ySliceSlider = new QSlider(ySliceGroupBox);
        ySliceSlider->setObjectName(QString::fromUtf8("ySliceSlider"));
        ySliceSlider->setOrientation(Qt::Horizontal);
        ySliceSlider->setMinimum(0);
        ySliceSlider->setMaximum(100);
        ySliceSlider->setValue(50);

        horizontalLayout_4->addWidget(ySliceSlider);


        verticalLayout_5->addLayout(horizontalLayout_4);


        verticalLayout_3->addWidget(ySliceGroupBox);

        verticalLayout_3->setStretch(0, 1);
        verticalLayout_3->setStretch(1, 1);

        horizontalLayout_2->addLayout(verticalLayout_3);

        horizontalLayout_2->setStretch(0, 6);
        horizontalLayout_2->setStretch(1, 4);

        verticalLayout->addLayout(horizontalLayout_2);

        controlGroupBox = new QGroupBox(centralWidget);
        controlGroupBox->setObjectName(QString::fromUtf8("controlGroupBox"));
        horizontalLayout = new QHBoxLayout(controlGroupBox);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        opacityLabel = new QLabel(controlGroupBox);
        opacityLabel->setObjectName(QString::fromUtf8("opacityLabel"));

        horizontalLayout->addWidget(opacityLabel);

        opacitySlider = new QSlider(controlGroupBox);
        opacitySlider->setObjectName(QString::fromUtf8("opacitySlider"));
        opacitySlider->setOrientation(Qt::Horizontal);
        opacitySlider->setMinimum(0);
        opacitySlider->setMaximum(100);
        opacitySlider->setValue(50);

        horizontalLayout->addWidget(opacitySlider);

        colorLabel = new QLabel(controlGroupBox);
        colorLabel->setObjectName(QString::fromUtf8("colorLabel"));

        horizontalLayout->addWidget(colorLabel);

        colorSlider = new QSlider(controlGroupBox);
        colorSlider->setObjectName(QString::fromUtf8("colorSlider"));
        colorSlider->setOrientation(Qt::Horizontal);
        colorSlider->setMinimum(0);
        colorSlider->setMaximum(100);
        colorSlider->setValue(50);

        horizontalLayout->addWidget(colorSlider);

        resetViewButton = new QPushButton(controlGroupBox);
        resetViewButton->setObjectName(QString::fromUtf8("resetViewButton"));

        horizontalLayout->addWidget(resetViewButton);


        verticalLayout->addWidget(controlGroupBox);

        BMPVolumeViewer->setCentralWidget(centralWidget);

        retranslateUi(BMPVolumeViewer);

        QMetaObject::connectSlotsByName(BMPVolumeViewer);
    } // setupUi

    void retranslateUi(QMainWindow *BMPVolumeViewer)
    {
        BMPVolumeViewer->setWindowTitle(QApplication::translate("BMPVolumeViewer", "VTK BMP\345\272\217\345\210\227\344\270\211\347\273\264\344\275\223\346\270\262\346\237\223\344\270\216\345\210\207\347\211\207\346\237\245\347\234\213", nullptr));
        groupBox->setTitle(QApplication::translate("BMPVolumeViewer", "\346\226\207\344\273\266\350\256\276\347\275\256", nullptr));
        label->setText(QApplication::translate("BMPVolumeViewer", "\346\226\207\344\273\266\345\211\215\347\274\200:", nullptr));
        browseButton->setText(QApplication::translate("BMPVolumeViewer", "\346\265\217\350\247\210...", nullptr));
        label_2->setText(QApplication::translate("BMPVolumeViewer", "\346\226\207\344\273\266\345\220\215\346\240\274\345\274\217:", nullptr));
        filePatternEdit->setText(QApplication::translate("BMPVolumeViewer", "%s%d.00um.bmp", nullptr));
        loadButton->setText(QApplication::translate("BMPVolumeViewer", "\345\212\240\350\275\275\345\233\276\345\203\217\345\272\217\345\210\227", nullptr));
        volumeGroupBox->setTitle(QApplication::translate("BMPVolumeViewer", "\344\270\211\347\273\264\344\275\223\346\270\262\346\237\223", nullptr));
        xSliceGroupBox->setTitle(QApplication::translate("BMPVolumeViewer", "X\346\226\271\345\220\221\345\210\207\347\211\207 (\345\271\263\350\241\214\344\272\216Z\350\275\264)", nullptr));
        xSliceLabel->setText(QApplication::translate("BMPVolumeViewer", "X\345\210\207\347\211\207\344\275\215\347\275\256: 0", nullptr));
        ySliceGroupBox->setTitle(QApplication::translate("BMPVolumeViewer", "Y\346\226\271\345\220\221\345\210\207\347\211\207 (\345\271\263\350\241\214\344\272\216Z\350\275\264)", nullptr));
        ySliceLabel->setText(QApplication::translate("BMPVolumeViewer", "Y\345\210\207\347\211\207\344\275\215\347\275\256: 0", nullptr));
        controlGroupBox->setTitle(QApplication::translate("BMPVolumeViewer", "\346\270\262\346\237\223\346\216\247\345\210\266", nullptr));
        opacityLabel->setText(QApplication::translate("BMPVolumeViewer", "\344\270\215\351\200\217\346\230\216\345\272\246:", nullptr));
        colorLabel->setText(QApplication::translate("BMPVolumeViewer", "\351\242\234\350\211\262\346\230\240\345\260\204:", nullptr));
        resetViewButton->setText(QApplication::translate("BMPVolumeViewer", "\351\207\215\347\275\256\350\247\206\345\233\276", nullptr));
    } // retranslateUi

};

namespace Ui {
    class BMPVolumeViewer: public Ui_BMPVolumeViewer {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BMPVOLUMEVIEWER_H
