#pragma once

// common
#include <QObject>
#include <QCoreApplication>
#include <QDir>
#include <QDateTime>
#include <QElapsedTimer>
#include <vector>
#include <array>
#include "common/model_common.h"
#include "common/image_common.h"

namespace reconstruction
{
    // Bounding-box information
    struct OBB3D
    {
        Eigen::Vector3d center;                     // Center point
        std::array<Eigen::Vector3d, 3> orientation; // Major axis, minor axis, and third axis
        std::array<double, 3> dimension;            // Bounding-box dimensions
    };

}

struct ReconAlgParams
{
    virtual ~ReconAlgParams() = default;
};

class AbstractReconAlgorithm : public QObject
{
    Q_OBJECT
public:
    explicit AbstractReconAlgorithm(QObject *parent = nullptr) : QObject(parent)
    {
        cloud_.reset(new pcl::PointCloud<pcl::PointXYZ>);
        cloud_fitter_.reset(new pcl::PointCloud<pcl::PointXYZ>);
    }

    virtual ~AbstractReconAlgorithm() = default;

    // ---------- Configure Algorithm Parameters ----------
    virtual bool setParams(std::shared_ptr<const ReconAlgParams> params) = 0;

    // ---------- Configure Debug Parameters ----------
    void setDebug(bool dbgEnable, std::string filePath)
    {
        debug_ = dbgEnable;
        filePath_ = filePath;
    }

    // ---------- Inputs ----------
    void setInputCloudCopy(pcl::PointCloud<pcl::PointXYZ>::ConstPtr cloud)
    {
        *cloud_ = *cloud;
    }

    // ---------- Execution ----------
    bool run()
    {
        // Clear cached data
        resetInterData();
        // Validate the inputs
        if (!cloud_)
            return false;
        // Step 1: Preprocessing
        if (!preprocessing())
            return false;

        // Step 2: Workpiece point-cloud decomposition
        if (!decomposition())
            return false;

        // Step 3: Feature-parameter extraction
        if (!featureExtraction())
            return false;

        // Step 5: Parametric modeling
        if (!parametricModeling())
            return false;
        // Output
        return true;
    }

    // ---------- Outputs ----------
    TopoDS_Shape modelData() const
    {
        BRepBuilderAPI_Copy copier(shape_);
        return copier.Shape();
    }

    void cloudData(pcl::PointCloud<pcl::PointXYZ>::Ptr &cloud) const
    {
        *cloud = *cloud_fitter_;
    }

private:
    // ===== Clear Data =====
    void resetInterData()
    {
        obbsGroups_.clear();
        modelParams_.clear();
        cloud_fitter_->clear();
        shape_.Nullify();
    }

protected:
    // ===== Unified Data =====
    // Debug parameters
    bool debug_ = false;
    std::string filePath_;
    // Input point cloud
    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_;
    // Preprocessed point cloud (intermediate data)
    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_fitter_;
    // Point-cloud decomposition results (intermediate data)
    std::map<int, std::vector<reconstruction::OBB3D>> obbsGroups_;
    // Modeling parameters (intermediate data)
    std::vector<reconstruction::ModelParameters> modelParams_;
    // Output model
    TopoDS_Shape shape_;

    // ===== Unified Processing Pipeline =====
    // Point cloud preprocessing
    virtual bool preprocessing() = 0;
    // Point cloud decomposition
    virtual bool decomposition() = 0;
    // Feature-parameter extraction
    virtual bool featureExtraction() = 0;
    // Parametric modeling
    virtual bool parametricModeling() = 0;

signals:
    // Emit point-cloud preprocessing results
    void sendCloudFitterResult(bool succeed);
    // Skeleton extraction result
    void sendSkeletonResult(const cv::Mat &picture);
    // Emit the runtime message for this class
    void sendReconstructMessage(QString msg, int code);
};


