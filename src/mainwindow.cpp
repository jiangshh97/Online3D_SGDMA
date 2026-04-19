#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    cloudIn.reset(new pcl::PointCloud<pcl::PointXYZ>);
    module = QString::fromStdString(Config::get_instance()->reconstructParams.name);

    /***************************************** UI Display Initialization ****************************************/
    // Log and status bar display
    LogWrite::get_instance()->setWidgets(ui->statusbar, ui->textEdit_Console);
    // Point cloud preview window
    pointcloudViewer = new PCLViewerWidget(this);
    ui->gridLayout_cloud->addWidget(pointcloudViewer);
    connect(pointcloudViewer, &PCLViewerWidget::sendPLCViewerMessage, this, &MainWindow::displayModuleMessage);
    // Skeleton image preview window
    imageViewer = new ImageViewerWidget(this);
    ui->gridLayout_image->addWidget(imageViewer);
    connect(imageViewer, &ImageViewerWidget::sendImageViewerMessage, this, &MainWindow::displayModuleMessage);
    // Model preview window
    modelViewer = new OCCTViewerWidget(this);
    ui->gridLayout_model->addWidget(modelViewer);
    connect(modelViewer, &OCCTViewerWidget::sendOCCTViewerMessage, this, &MainWindow::displayModuleMessage);

    /*************************************** Model Reconstruction Module Initialization ***************************************/
    modelRecon = new ReconstructionProcess(&Config::get_instance()->reconstructParams);
    // Connect point cloud visualization
    connect(modelRecon->reconstruction_, &ReconstructionAlgorithm::sendCloudFitterResult, this, [=](bool succeed)
            {
        if(succeed)
            pointcloudViewer->pointcloudShow(modelRecon->pointcloud.cloud_worpiece); });
    // Connect image visualization
    connect(modelRecon, &ReconstructionProcess::sendSkeletonResult, imageViewer, &ImageViewerWidget::imageShow);
    // Connect model visualization
    connect(modelRecon, &ReconstructionProcess::sendReconstructionResult, this, [=](bool succeed)
            {
        if(succeed)
            modelViewer->modelShow(modelRecon->model.shape); });
    // Connect log messages from the model reconstruction module
    connect(modelRecon, &ReconstructionProcess::sendReconstructionMessage, this, &MainWindow::displayModuleMessage);
}

MainWindow::~MainWindow()
{
    delete modelRecon;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    delete imageViewer;
    delete pointcloudViewer;
    delete modelViewer;
    delete ui;
    event->accept();
}

void MainWindow::displayModuleMessage(QString module, QString msg, int code)
{
    if (code == -1)
        LogWrite::get_instance()->printLog(module, msg);
    else
        LogWrite::get_instance()->printLog(module, msg, code);
}

void MainWindow::on_action_Importpcdfile_triggered()
{
    // Set default folder path
    QDir appDir = QCoreApplication::applicationDirPath();
    appDir.cdUp();
    QString directory = appDir.path();
    // File dialog
    QString filename = QFileDialog::getOpenFileName(this, u8"Open Point Cloud File", directory, "Point Cloud (*.pcd *.ply)");
    // Check if the user selected a file
    if (!filename.isEmpty())
    {
        QFileInfo fileInfo(filename);
        QString extension = fileInfo.suffix();
        if (extension.compare("pcd", Qt::CaseInsensitive) == 0)
        { // Check file type
            pcl::PointCloud<pcl::PointXYZ>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZ>);
            if (pcl::io::loadPCDFile<pcl::PointXYZ>(filename.toStdString(), *cloud) == -1)
            {
                QString msg = u8"Failed to open point cloud file!";
                displayModuleMessage(module, msg, 0);
                return;
            }
            else
            {
                *cloudIn = *cloud;
                pointcloudViewer->pointcloudShow(cloudIn);

                QString msg = u8"Point cloud file opened successfully!";
                displayModuleMessage(module, msg, -1);
            }
        }
        else if (extension.compare("ply", Qt::CaseInsensitive) == 0)
        {
            pcl::PointCloud<pcl::PointXYZ>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZ>);
            if (pcl::io::loadPLYFile<pcl::PointXYZ>(filename.toStdString(), *cloud) == -1)
            {
                QString msg = u8"Failed to open point cloud file!";
                displayModuleMessage(module, msg, 0);
                return;
            }
            else
            {
                *cloudIn = *cloud;
                pointcloudViewer->pointcloudShow(cloudIn);
                QString msg = u8"Point cloud file opened successfully!";
                displayModuleMessage(module, msg, -1);
            }
        }
        else
        {
            QString msg = u8"User selected an unknown file type";
            displayModuleMessage(module, msg, 0);
        }
    }
    else
    {
        QString msg = u8"No point cloud file selected";
        displayModuleMessage(module, msg, 0);
    }
}

void MainWindow::on_action_Reconstruction_triggered()
{
    if (cloudIn->size() > 0)
    {
        int workpieceType = static_cast<int>(reconstruction::workpieceType::SteelAngle);
        modelRecon->modelingActive(workpieceType, cloudIn);
        QString msg = u8"Starting model reconstruction";
        displayModuleMessage(module, msg, -1);
    }
    else
    {
        QString module = QString::fromStdString(Config::get_instance()->reconstructParams.name);
        QString msg = u8"Input point cloud is empty";
        displayModuleMessage(module, msg, 0);
    }
}

void MainWindow::on_action_Exportstlfile_triggered()
{
    if (modelRecon && !modelRecon->model.shape.IsNull())
    {
        QDir appDir = QCoreApplication::applicationDirPath();
        appDir.cdUp();
        std::string fileName = appDir.path().toStdString() + Config::get_instance()->reconstructParams.filePath + "model.stl";
        std::cout << fileName <<std::endl;
        reconstruction::modelExplorer_stlWriter(modelRecon->model.shape, fileName);
        QString msg = u8"Model file exported!";
        displayModuleMessage(module, msg, -1);
    }
    else
    {
        QString module = QString::fromStdString(Config::get_instance()->reconstructParams.name);
        QString msg = u8"No model file generated";
        displayModuleMessage(module, msg, 0);
    }
}
