#pragma once

// PCL
#include <pcl/point_types.h>
#include <pcl/point_cloud.h>
// OCCT
#include <TopoDS.hxx>
// Cereal
#include "cereal/archives/json.hpp"
#include "cereal/types/vector.hpp"
#include <cereal/types/utility.hpp>
// Utility wrapper functions
#include "steelangle/steelangle_params.h"

struct reconstructionParams
{
    std::string name = "Model Reconstruct";
    bool dbgEnable = false;                 // Whether debug output is enabled
    std::string filePath = "/data/model/";  // Data output path
    std::string cachePath = "/data/cache/"; // Cache output path

    reconstruction::SteelAngleParams steelangle; // Algorithm parameters for angle-steel workpieces
    /****** Parameters for other workpiece types *******/

    template <class Archive>
    void serialize(Archive &archive)
    {
        archive(CEREAL_NVP(name),
                CEREAL_NVP(dbgEnable),
                CEREAL_NVP(filePath),
                CEREAL_NVP(cachePath),
                CEREAL_NVP(steelangle));
    }
};

namespace reconstruction
{
    // Point-cloud parameters
    struct cloudData
    {
        int type;                                           // Workpiece type
        pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_in;       // Source point cloud
        pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_worpiece; // Workpiece point cloud
        pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_other;    // Hook point cloud

        cloudData()
        {
            cloud_in.reset(new pcl::PointCloud<pcl::PointXYZ>);
            cloud_worpiece.reset(new pcl::PointCloud<pcl::PointXYZ>);
            cloud_other.reset(new pcl::PointCloud<pcl::PointXYZ>);
        }
    };

    // Model parameters
    struct modelData
    {
        int type;                          // Workpiece type
        TopoDS_Shape shape;                // Model(OCCT)
        std::array<double, 3> dimensions;  // 3D dimensions (overall length, width, and height)
        std::array<double, 3> orientation; // 3D orientation
    };

}


