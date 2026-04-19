#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <iostream>

/********************** System Configuration and Algorithms ***********************/
#include "config/config.h"
#include "logging/logwrite.h"
#include "reconstructionProcess/reconstructionprocess.h"

/********************** Main System UI Display ***********************/
#include "ui/imageviewerwidget.h"
#include "ui/pclviewerwidget.h"
#include "ui/occtviewerwidget.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event);

private:
    void displayModuleMessage(QString module, QString msg, int code);

private slots:

    void on_action_Importpcdfile_triggered();

    void on_action_Reconstruction_triggered();

    void on_action_Exportstlfile_triggered();

private:
    Ui::MainWindow *ui;

    ImageViewerWidget *imageViewer = nullptr;    // Image preview window
    PCLViewerWidget *pointcloudViewer = nullptr; // Point cloud preview window
    OCCTViewerWidget *modelViewer = nullptr;     // Model preview window

    QString module;                              // Module name
    pcl::PointCloud<pcl::PointXYZ>::Ptr cloudIn; // Input point cloud
    ReconstructionProcess *modelRecon = nullptr; // Model reconstruction
};
#endif // MAINWINDOW_H
