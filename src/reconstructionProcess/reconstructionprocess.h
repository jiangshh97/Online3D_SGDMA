#pragma once

#include <QObject>
#include <QThread>
#include "reconstructioncommon.h"
#include "reconstructionalgorithm.h"

class ReconstructionProcess : public QObject
{
    Q_OBJECT
    QThread reconstructionThread;

public:
    ReconstructionProcess(reconstructionParams *para = nullptr);

    ~ReconstructionProcess();

    void modelingActive(int type, pcl::PointCloud<pcl::PointXYZ>::ConstPtr cloud);

    reconstruction::cloudData pointcloud; // Point cloud

    reconstruction::modelData model; // Model

signals:
    void sendCloudFitterResult(bool succeed);                              // Point-cloud preprocessing result
    void sendSkeletonResult(const cv::Mat &picture);                       // Skeleton extraction result
    void sendReconstructionResult(bool succeed);                           // Reconstruction result
    void sendReconstructionMessage(QString module, QString msg, int code); // Forward runtime messages for this class (code == -1: info, code != -1: error code)

private:
    reconstructionParams defaults_; // Automatic storage duration
    reconstructionParams *params_;  // Parameter pointer
    ReconstructionAlgorithm *reconstruction_ = nullptr;

    friend class MainWindow;
};

