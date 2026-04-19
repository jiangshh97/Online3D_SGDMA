#pragma once

#include <QObject>
#include "reconstructioncommon.h"
#include "abstractreconfactory.h"

class ReconstructionAlgorithm : public QObject
{
    Q_OBJECT
public:
    ReconstructionAlgorithm(reconstructionParams *params = nullptr, reconstruction::cloudData *cloud = nullptr, reconstruction::modelData *model = nullptr);

public slots:
    void reconstruction();

public slots:
    void creatFactory();

private:
    void bindAlgoSignals();

private:
    std::unique_ptr<ReconAlgFactory> reconFactory = nullptr;
    std::unique_ptr<ReconAlgParamsFactory> paramsFactory = nullptr;
    std::unique_ptr<AbstractReconAlgorithm> reconAlgorithm = nullptr;
    std::shared_ptr<ReconAlgParams> algoParams = nullptr;
    int type_last = -1;

signals:
    void sendCloudFitterResult(bool succeed);                       // Point cloud preprocessing
    void sendSkeletonResult(const cv::Mat &picture);                // Skeleton extraction result
    void sendReconstructionResult(bool succeed);                    // Reconstruction result
    void sendReconstructionAlgorithmMessage(QString msg, int code); // Emit the runtime message for this class

private:
    reconstructionParams *params_ = nullptr;     // Configuration parameters
    reconstruction::cloudData *cloud_ = nullptr; // Point-cloud data
    reconstruction::modelData *model_ = nullptr; // Model data
    QString filePath, cachePath, cacheFilePath;  // Save paths
};


