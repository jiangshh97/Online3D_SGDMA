#include "reconstructionprocess.h"

using namespace reconstruction;

ReconstructionProcess::ReconstructionProcess(reconstructionParams *params)
    : params_(params ? params : &defaults_)
{
    // Model-reconstruction class instantiation
    reconstruction_ = new ReconstructionAlgorithm(params_, &pointcloud, &model);
    reconstruction_->moveToThread(&reconstructionThread);
    connect(&reconstructionThread, &QThread::finished, reconstruction_, &QObject::deleteLater);
    // Create the algorithm factory
    connect(&reconstructionThread, &QThread::started, reconstruction_, &ReconstructionAlgorithm::creatFactory);
    // Receive point-cloud preprocessing results
    connect(reconstruction_, &ReconstructionAlgorithm::sendCloudFitterResult, this, [=](bool success)
            { emit sendCloudFitterResult(success); });
    // Receive skeleton extraction results
    connect(reconstruction_, &ReconstructionAlgorithm::sendSkeletonResult, this, [=](const cv::Mat &picture)
            { emit sendSkeletonResult(picture); });
    // Receive reconstruction result feedback
    connect(reconstruction_, &ReconstructionAlgorithm::sendReconstructionResult, this, [=](bool success)
            { emit sendReconstructionResult(success); });
    // Receive thread messages
    connect(reconstruction_, &ReconstructionAlgorithm::sendReconstructionAlgorithmMessage, this, [=](QString msg, int code)
            { emit sendReconstructionMessage(QString::fromStdString(params_->name), msg, code); });
    // Start the thread
    reconstructionThread.start();
}

ReconstructionProcess::~ReconstructionProcess()
{
    reconstructionThread.quit();
    reconstructionThread.wait();
}

void ReconstructionProcess::modelingActive(int type, pcl::PointCloud<pcl::PointXYZ>::ConstPtr cloud)
{
    pointcloud.type = type;
    *(pointcloud.cloud_in) = *cloud;
    QMetaObject::invokeMethod(reconstruction_, "reconstruction", Qt::QueuedConnection);
}


