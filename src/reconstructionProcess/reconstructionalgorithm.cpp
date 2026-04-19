#include "reconstructionalgorithm.h"

using namespace reconstruction;

ReconstructionAlgorithm::ReconstructionAlgorithm(reconstructionParams *params, cloudData *cloud, modelData *model)
    : params_(params),
      cloud_(cloud),
      model_(model)
{
    // Get the save path
    QDir appDir = QCoreApplication::applicationDirPath();
    appDir.cdUp();
    filePath = appDir.path() + QString::fromStdString(params_->filePath);
    QDir mDir(filePath);
    if (!mDir.exists())
        mDir.mkpath(filePath);
    cachePath = appDir.path() + QString::fromStdString(params_->cachePath);
    mDir = QDir(cachePath);
    if (!mDir.exists())
        mDir.mkpath(cachePath);
}

void ReconstructionAlgorithm::creatFactory()
{
    reconFactory = std::make_unique<ReconAlgFactory>();
    paramsFactory = std::make_unique<ReconAlgParamsFactory>(params_);
}

void ReconstructionAlgorithm::reconstruction()
{
    // Create the target directory
    if (params_->dbgEnable)
    {
        QString currentDateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd_HH-mm-ss");
        cacheFilePath = cachePath + currentDateTime + "/";

        // Create the path object
        QDir dir(cacheFilePath);
        if (!dir.exists())
        { // Create the folder if it does not exist
            if (!dir.mkdir(cacheFilePath))
            {
                QString msg = u8"Failed to create the process folder!";
                emit sendReconstructionAlgorithmMessage(msg, 0); // Emit the runtime message for this class
                params_->dbgEnable = false;
            }
        }
    }

    // Create the corresponding algorithm instance
    const int currentType = cloud_->type;
    if (!reconAlgorithm || currentType != type_last)
    {
        // Create the corresponding algorithm and parameter set
        auto newReconAlgorithm = reconFactory->createAlgorithm(currentType);
        auto newAlgoParams = paramsFactory->createParams(currentType);
        // Check for null
        if (!newReconAlgorithm || !newAlgoParams)
        {
            QString msg = u8"Failed to create the algorithm";
            emit sendReconstructionAlgorithmMessage(msg, 0);
            return;
        }
        // Replace the member only after successful creation
        reconAlgorithm = std::move(newReconAlgorithm);
        algoParams = std::move(newAlgoParams);

        // Configure algorithm parameters
        bindAlgoSignals();
        if (!reconAlgorithm->setParams(algoParams))
        {
            QString msg = u8"Failed to configure algorithm parameters";
            emit sendReconstructionAlgorithmMessage(msg, 0);
            return;
        }
        // Update the workpiece type
        type_last = currentType;
    }

    // Update the input data
    reconAlgorithm->setInputCloudCopy(cloud_->cloud_in);
    reconAlgorithm->setDebug(params_->dbgEnable, cacheFilePath.toStdString());

    // Execute reconstruction
    bool ok = reconAlgorithm->run();
    if (!ok)
    {
        QString msg = u8"Algorithm execution error";
        emit sendReconstructionAlgorithmMessage(msg, 0);
        return;
    }
    else
    {
        // Model
        model_->shape = reconAlgorithm->modelData();
        emit sendReconstructionResult(true);

        QString msg = u8"Model reconstruction completed";
        emit sendReconstructionAlgorithmMessage(msg, -1);
    }
}

void ReconstructionAlgorithm::bindAlgoSignals()
{
    if (!reconAlgorithm)
        return;

    // Avoid duplicate connections: use UniqueConnection
    QObject::connect(reconAlgorithm.get(), &AbstractReconAlgorithm::sendCloudFitterResult, this, [=](bool succeed)
                     { emit sendCloudFitterResult(succeed); }, Qt::UniqueConnection);

    QObject::connect(reconAlgorithm.get(), &AbstractReconAlgorithm::sendSkeletonResult, this, [=](const cv::Mat &picture)
                     { emit sendSkeletonResult(picture); }, Qt::UniqueConnection);

    QObject::connect(reconAlgorithm.get(), &AbstractReconAlgorithm::sendReconstructMessage, this, [=](QString msg, int code)
                     { emit sendReconstructionAlgorithmMessage(msg, code); }, Qt::UniqueConnection);
}


