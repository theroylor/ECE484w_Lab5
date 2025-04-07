/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QWidget *horizontalLayoutWidget_2;
    QHBoxLayout *horizontalLayout_4;
    QVBoxLayout *verticalLayout_2;
    QLabel *label_brightness;
    QPushButton *pushButton_brightness;
    QSlider *verticalSlider_brightness;
    QVBoxLayout *verticalLayout_6;
    QLabel *label_contrast;
    QPushButton *pushButton_contrast;
    QSlider *verticalSlider_contrast;
    QWidget *gridLayoutWidget;
    QGridLayout *gridLayout;
    QLabel *label_input;
    QPushButton *pushButton_Load;
    QPushButton *pushButton_Send_Base;
    QCheckBox *checkBox_overlay_toggle;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(799, 608);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        horizontalLayoutWidget_2 = new QWidget(centralwidget);
        horizontalLayoutWidget_2->setObjectName(QString::fromUtf8("horizontalLayoutWidget_2"));
        horizontalLayoutWidget_2->setGeometry(QRect(10, 40, 191, 521));
        horizontalLayout_4 = new QHBoxLayout(horizontalLayoutWidget_2);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        horizontalLayout_4->setContentsMargins(0, 0, 0, 0);
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        label_brightness = new QLabel(horizontalLayoutWidget_2);
        label_brightness->setObjectName(QString::fromUtf8("label_brightness"));

        verticalLayout_2->addWidget(label_brightness, 0, Qt::AlignHCenter);

        pushButton_brightness = new QPushButton(horizontalLayoutWidget_2);
        pushButton_brightness->setObjectName(QString::fromUtf8("pushButton_brightness"));

        verticalLayout_2->addWidget(pushButton_brightness);

        verticalSlider_brightness = new QSlider(horizontalLayoutWidget_2);
        verticalSlider_brightness->setObjectName(QString::fromUtf8("verticalSlider_brightness"));
        verticalSlider_brightness->setMinimum(0);
        verticalSlider_brightness->setMaximum(99);
        verticalSlider_brightness->setOrientation(Qt::Vertical);

        verticalLayout_2->addWidget(verticalSlider_brightness, 0, Qt::AlignHCenter);


        horizontalLayout_4->addLayout(verticalLayout_2);

        verticalLayout_6 = new QVBoxLayout();
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        label_contrast = new QLabel(horizontalLayoutWidget_2);
        label_contrast->setObjectName(QString::fromUtf8("label_contrast"));

        verticalLayout_6->addWidget(label_contrast, 0, Qt::AlignHCenter);

        pushButton_contrast = new QPushButton(horizontalLayoutWidget_2);
        pushButton_contrast->setObjectName(QString::fromUtf8("pushButton_contrast"));

        verticalLayout_6->addWidget(pushButton_contrast);

        verticalSlider_contrast = new QSlider(horizontalLayoutWidget_2);
        verticalSlider_contrast->setObjectName(QString::fromUtf8("verticalSlider_contrast"));
        verticalSlider_contrast->setMinimum(0);
        verticalSlider_contrast->setMaximum(99);
        verticalSlider_contrast->setValue(99);
        verticalSlider_contrast->setSliderPosition(99);
        verticalSlider_contrast->setOrientation(Qt::Vertical);

        verticalLayout_6->addWidget(verticalSlider_contrast, 0, Qt::AlignHCenter);


        horizontalLayout_4->addLayout(verticalLayout_6);

        gridLayoutWidget = new QWidget(centralwidget);
        gridLayoutWidget->setObjectName(QString::fromUtf8("gridLayoutWidget"));
        gridLayoutWidget->setGeometry(QRect(210, 40, 563, 521));
        gridLayout = new QGridLayout(gridLayoutWidget);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        label_input = new QLabel(gridLayoutWidget);
        label_input->setObjectName(QString::fromUtf8("label_input"));
        label_input->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label_input, 1, 0, 1, 1);

        pushButton_Load = new QPushButton(gridLayoutWidget);
        pushButton_Load->setObjectName(QString::fromUtf8("pushButton_Load"));

        gridLayout->addWidget(pushButton_Load, 0, 0, 1, 1);

        pushButton_Send_Base = new QPushButton(centralwidget);
        pushButton_Send_Base->setObjectName(QString::fromUtf8("pushButton_Send_Base"));
        pushButton_Send_Base->setGeometry(QRect(10, 10, 191, 24));
        checkBox_overlay_toggle = new QCheckBox(centralwidget);
        checkBox_overlay_toggle->setObjectName(QString::fromUtf8("checkBox_overlay_toggle"));
        checkBox_overlay_toggle->setGeometry(QRect(410, 10, 141, 19));
        checkBox_overlay_toggle->setChecked(false);
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 799, 20));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        label_brightness->setText(QCoreApplication::translate("MainWindow", "Brightness: +0", nullptr));
        pushButton_brightness->setText(QCoreApplication::translate("MainWindow", "Reset", nullptr));
        label_contrast->setText(QCoreApplication::translate("MainWindow", "Contrast: 99", nullptr));
        pushButton_contrast->setText(QCoreApplication::translate("MainWindow", "Reset", nullptr));
        label_input->setText(QCoreApplication::translate("MainWindow", "No Image Loaded", nullptr));
        pushButton_Load->setText(QCoreApplication::translate("MainWindow", "Load Input Image", nullptr));
        pushButton_Send_Base->setText(QCoreApplication::translate("MainWindow", "Send Input Image to FPGA", nullptr));
        checkBox_overlay_toggle->setText(QCoreApplication::translate("MainWindow", "Enable overlay on DE10", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
