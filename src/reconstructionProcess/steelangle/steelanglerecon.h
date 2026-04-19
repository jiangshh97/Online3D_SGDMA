#pragma once

#include <QtConcurrent>
#include "omp.h"
#include "abstractreconalgorithm.h"
#include "steelangle_params.h"
#include "common/pointcloud_common.h"
#include "common/image_common.h"
#include "common/model_common.h"
#include "yoloseginference.h"

struct SteelAngleReconParams : public ReconAlgParams
{
    const reconstruction::SteelAngleParams *cfg = nullptr;
};

class SteelAngleRecon : public AbstractReconAlgorithm
{
    Q_OBJECT
public:
    explicit SteelAngleRecon(QObject *parent = nullptr);

    bool setParams(std::shared_ptr<const ReconAlgParams> params) override;

protected:
    // Point cloud preprocessing
    bool preprocessing() override;
    // Point cloud decomposition
    bool decomposition() override;
    // Feature-parameter extraction
    bool featureExtraction() override;
    // Parametric modeling
    bool parametricModeling() override;

private:
    // Feature-parameter extraction
    void featureExtraction_Steelangle(const std::vector<reconstruction::OBB3D> &steelangle_obb, std::vector<reconstruction::steelAngleParams3D> &steelangle_params);

    // Parameter regularization
    void featureRegularization(std::vector<reconstruction::steelAngleParams3D> &steelangle_params);

    // Data conversion
    void featureToModelParams(const std::vector<reconstruction::steelAngleParams3D> &steelangle_params, std::vector<reconstruction::ModelParameters> &modelParams);

private:
    // Instantiate the YOLO OBB module
    std::string enginePath_;
    std::unique_ptr<YoLoSegInference> yolo_seg = nullptr;

    // Algorithm configuration parameters
    const reconstruction::SteelAngleParams *params_ = nullptr;

    // Main plane equation of the point cloud
    pcl::ModelCoefficients::Ptr mainPlaneEqu_;
};
