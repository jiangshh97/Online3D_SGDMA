/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *action_Importpcdfile;
    QAction *action_Reconstruction;
    QAction *action_Exportstlfile;
    QWidget *centralwidget;
    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_3;
    QGridLayout *gridLayout_cloud;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout_5;
    QGridLayout *gridLayout_image;
    QVBoxLayout *verticalLayout_2;
    QGroupBox *groupBox_3;
    QGridLayout *gridLayout_8;
    QGridLayout *gridLayout_model;
    QGroupBox *groupBox_4;
    QGridLayout *gridLayout_2;
    QTextEdit *textEdit_Console;
    QMenuBar *menubar;
    QMenu *menuFile;
    QMenu *menuAlgorithom;
    QStatusBar *statusbar;
    QToolBar *toolBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1024, 768);
        MainWindow->setStyleSheet(QString::fromUtf8("background-color:#303030;"));
        action_Importpcdfile = new QAction(MainWindow);
        action_Importpcdfile->setObjectName(QString::fromUtf8("action_Importpcdfile"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/resource/icon/Import.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_Importpcdfile->setIcon(icon);
        action_Reconstruction = new QAction(MainWindow);
        action_Reconstruction->setObjectName(QString::fromUtf8("action_Reconstruction"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/resource/icon/Model.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_Reconstruction->setIcon(icon1);
        action_Exportstlfile = new QAction(MainWindow);
        action_Exportstlfile->setObjectName(QString::fromUtf8("action_Exportstlfile"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/resource/icon/Export.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_Exportstlfile->setIcon(icon2);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        gridLayout = new QGridLayout(centralwidget);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(2);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(1, 1, 1, 1);
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(10, 10, 10, 10);
        groupBox = new QGroupBox(centralwidget);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        QFont font;
        font.setPointSize(12);
        groupBox->setFont(font);
        groupBox->setStyleSheet(QString::fromUtf8("QGroupBox {\n"
"    border: 1px;\n"
"    border-radius: 0px;\n"
"    border-style: solid;\n"
"    border-color: white;\n"
"    margin-top: 2ex;\n"
"    padding-top: 2ex;\n"
"}\n"
"QGroupBox::title {\n"
"    subcontrol-origin: margin;\n"
"    subcontrol-position: top left;\n"
"    color: white;\n"
"    left: 5px;\n"
"    padding:0 1px;\n"
"}"));
        gridLayout_3 = new QGridLayout(groupBox);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        gridLayout_3->setContentsMargins(1, 1, 1, 1);
        gridLayout_cloud = new QGridLayout();
        gridLayout_cloud->setObjectName(QString::fromUtf8("gridLayout_cloud"));

        gridLayout_3->addLayout(gridLayout_cloud, 0, 0, 1, 1);


        verticalLayout->addWidget(groupBox);

        groupBox_2 = new QGroupBox(centralwidget);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        groupBox_2->setFont(font);
        groupBox_2->setStyleSheet(QString::fromUtf8("QGroupBox {\n"
"    border: 1px;\n"
"    border-radius: 0px;\n"
"    border-style: solid;\n"
"    border-color: white;\n"
"    margin-top: 2ex;\n"
"    padding-top: 2ex;\n"
"}\n"
"QGroupBox::title {\n"
"    subcontrol-origin: margin;\n"
"    subcontrol-position: top left;\n"
"    color: white;\n"
"    left: 5px;\n"
"    padding:0 1px;\n"
"}"));
        gridLayout_5 = new QGridLayout(groupBox_2);
        gridLayout_5->setObjectName(QString::fromUtf8("gridLayout_5"));
        gridLayout_5->setContentsMargins(1, 1, 1, 1);
        gridLayout_image = new QGridLayout();
        gridLayout_image->setObjectName(QString::fromUtf8("gridLayout_image"));

        gridLayout_5->addLayout(gridLayout_image, 0, 1, 1, 1);


        verticalLayout->addWidget(groupBox_2);


        horizontalLayout->addLayout(verticalLayout);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(10, 10, 10, 10);
        groupBox_3 = new QGroupBox(centralwidget);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        groupBox_3->setFont(font);
        groupBox_3->setStyleSheet(QString::fromUtf8("QGroupBox {\n"
"    border: 1px;\n"
"    border-radius: 0px;\n"
"    border-style: solid;\n"
"    border-color: white;\n"
"    margin-top: 2ex;\n"
"    padding-top: 2ex;\n"
"}\n"
"QGroupBox::title {\n"
"    subcontrol-origin: margin;\n"
"    subcontrol-position: top left;\n"
"    color: white;\n"
"    left: 5px;\n"
"    padding:0 1px;\n"
"}"));
        gridLayout_8 = new QGridLayout(groupBox_3);
        gridLayout_8->setObjectName(QString::fromUtf8("gridLayout_8"));
        gridLayout_8->setContentsMargins(1, 1, 1, 1);
        gridLayout_model = new QGridLayout();
        gridLayout_model->setObjectName(QString::fromUtf8("gridLayout_model"));

        gridLayout_8->addLayout(gridLayout_model, 0, 0, 1, 1);


        verticalLayout_2->addWidget(groupBox_3);

        groupBox_4 = new QGroupBox(centralwidget);
        groupBox_4->setObjectName(QString::fromUtf8("groupBox_4"));
        groupBox_4->setFont(font);
        groupBox_4->setStyleSheet(QString::fromUtf8("QGroupBox {\n"
"    border: 1px;\n"
"    border-radius: 0px;\n"
"    border-style: solid;\n"
"    border-color: white;\n"
"    margin-top: 2ex;\n"
"    padding-top: 2ex;\n"
"}\n"
"QGroupBox::title {\n"
"    subcontrol-origin: margin;\n"
"    subcontrol-position: top left;\n"
"    color: white;\n"
"    left: 5px;\n"
"    padding:0 1px;\n"
"}"));
        gridLayout_2 = new QGridLayout(groupBox_4);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        gridLayout_2->setContentsMargins(1, 1, 1, 1);
        textEdit_Console = new QTextEdit(groupBox_4);
        textEdit_Console->setObjectName(QString::fromUtf8("textEdit_Console"));
        textEdit_Console->setFont(font);
        textEdit_Console->setStyleSheet(QString::fromUtf8("QTextEdit {\n"
"	border-width: 1px;\n"
"	border-style: solid;\n"
"	border-color:transparent;\n"
"        color:white;\n"
"	selection-background-color:rgb(236,116,64);\n"
"}"));

        gridLayout_2->addWidget(textEdit_Console, 0, 0, 1, 1);


        verticalLayout_2->addWidget(groupBox_4);

        verticalLayout_2->setStretch(0, 1);
        verticalLayout_2->setStretch(1, 1);

        horizontalLayout->addLayout(verticalLayout_2);

        horizontalLayout->setStretch(0, 1);
        horizontalLayout->setStretch(1, 1);

        gridLayout->addLayout(horizontalLayout, 0, 0, 1, 1);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 1024, 29));
        menubar->setFont(font);
        menubar->setStyleSheet(QString::fromUtf8("QMenuBar {\n"
"    color:white;\n"
"    background-color:transparent;\n"
"}\n"
"\n"
"QMenuBar::item {\n"
"    padding: 4px 12px 4px 12px;\n"
"    background-color:transparent;\n"
"}"));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        menuFile->setStyleSheet(QString::fromUtf8("QMenu {\n"
"    border: 1px solid grey;\n"
"    background-color:transparent;\n"
"    color:white;\n"
"}\n"
"\n"
"QMenu::indicator {\n"
"    width: 0px;\n"
"}\n"
"\n"
"QMenu::item {\n"
"    padding: 4px 12px 4px 12px;\n"
"    background-color:transparent;\n"
"}\n"
"\n"
"QMenu::item:selected {\n"
"    background-color: lightblue;\n"
"    color: black;\n"
"}\n"
"\n"
"QMenu::item:checked {\n"
"    background-color: lightblue;\n"
"    color: black;\n"
"}\n"
"\n"
"QMenu::item:disabled {\n"
"    background-color: #404040;\n"
"    color: gray;\n"
"}"));
        menuAlgorithom = new QMenu(menubar);
        menuAlgorithom->setObjectName(QString::fromUtf8("menuAlgorithom"));
        menuAlgorithom->setStyleSheet(QString::fromUtf8("QMenu {\n"
"    border: 1px solid grey;\n"
"    background-color:transparent;\n"
"    color:white;\n"
"}\n"
"\n"
"QMenu::indicator {\n"
"    width: 0px;\n"
"}\n"
"\n"
"QMenu::item {\n"
"    padding: 4px 12px 4px 12px;\n"
"    background-color:transparent;\n"
"}\n"
"\n"
"QMenu::item:selected {\n"
"    background-color: lightblue;\n"
"    color: black;\n"
"}\n"
"\n"
"QMenu::item:checked {\n"
"    background-color: lightblue;\n"
"    color: black;\n"
"}\n"
"\n"
"QMenu::item:disabled {\n"
"    background-color: #404040;\n"
"    color: gray;\n"
"}"));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        statusbar->setFont(font);
        MainWindow->setStatusBar(statusbar);
        toolBar = new QToolBar(MainWindow);
        toolBar->setObjectName(QString::fromUtf8("toolBar"));
        toolBar->setFont(font);
        toolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
        MainWindow->addToolBar(Qt::TopToolBarArea, toolBar);

        menubar->addAction(menuFile->menuAction());
        menubar->addAction(menuAlgorithom->menuAction());
        menuFile->addAction(action_Importpcdfile);
        menuFile->addAction(action_Exportstlfile);
        menuAlgorithom->addAction(action_Reconstruction);
        toolBar->addAction(action_Importpcdfile);
        toolBar->addSeparator();
        toolBar->addAction(action_Reconstruction);
        toolBar->addSeparator();
        toolBar->addAction(action_Exportstlfile);
        toolBar->addSeparator();

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        action_Importpcdfile->setText(QCoreApplication::translate("MainWindow", "Import PCD", nullptr));
#if QT_CONFIG(tooltip)
        action_Importpcdfile->setToolTip(QCoreApplication::translate("MainWindow", "Import PCD", nullptr));
#endif // QT_CONFIG(tooltip)
        action_Reconstruction->setText(QCoreApplication::translate("MainWindow", "Reconstruct Model", nullptr));
#if QT_CONFIG(tooltip)
        action_Reconstruction->setToolTip(QCoreApplication::translate("MainWindow", "Reconstruct Model", nullptr));
#endif // QT_CONFIG(tooltip)
        action_Exportstlfile->setText(QCoreApplication::translate("MainWindow", "Export STL", nullptr));
#if QT_CONFIG(tooltip)
        action_Exportstlfile->setToolTip(QCoreApplication::translate("MainWindow", "Export STL", nullptr));
#endif // QT_CONFIG(tooltip)
        groupBox->setTitle(QCoreApplication::translate("MainWindow", "Point Cloud View", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("MainWindow", "Image View", nullptr));
        groupBox_3->setTitle(QCoreApplication::translate("MainWindow", "Model View", nullptr));
        groupBox_4->setTitle(QCoreApplication::translate("MainWindow", "Console Output", nullptr));
        menuFile->setTitle(QCoreApplication::translate("MainWindow", "File", nullptr));
        menuAlgorithom->setTitle(QCoreApplication::translate("MainWindow", "Algorithom", nullptr));
        toolBar->setWindowTitle(QCoreApplication::translate("MainWindow", "toolBar", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
