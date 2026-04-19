#pragma once

// Cereal
#include "cereal/archives/json.hpp"
#include "cereal/types/vector.hpp"
#include <cereal/types/utility.hpp>

namespace reconstruction
{

    // Pass-through filter parameters
    struct PassThroughFilter
    {
        std::vector<double> valueX{0.0, 0.0};
        std::vector<double> valueY{0.0, 0.0};
        std::vector<double> valueZ{0.0, 0.0};

        template <class Archive>
        void serialize(Archive &archive)
        {
            archive(CEREAL_NVP(valueX),
                    CEREAL_NVP(valueY),
                    CEREAL_NVP(valueZ));
        }
    };

    // Statistical filtering
    struct StatisticalFitter
    {
        int meank = 50;       // Number of neighboring points
        double stdMult = 1.0; // Standard deviation

        template <class Archive>
        void serialize(Archive &archive)
        {
            archive(CEREAL_NVP(meank),
                    CEREAL_NVP(stdMult));
        }
    };

    // Voxel filtering
    struct VoxelFitter
    {
        float leafX = 1.0; // Leaf size
        float leafY = 1.0; // Leaf size
        float leafZ = 1.0; // Leaf size

        template <class Archive>
        void serialize(Archive &archive)
        {
            archive(CEREAL_NVP(leafX),
                    CEREAL_NVP(leafY),
                    CEREAL_NVP(leafZ));
        }
    };

    // Radius filtering
    struct RadiusOutlierFitter
    {
        double radius = 1.0; // Neighborhood radius
        int minPts = 1;      // Minimum number of points

        template <class Archive>
        void serialize(Archive &archive)
        {
            archive(CEREAL_NVP(radius),
                    CEREAL_NVP(minPts));
        }
    };

    // Plane fitting parameters
    struct RANSACPlanes
    {
        int maxPlaneNum = 10;           // Maximum number of fitted planes
        int minInlierNum = 3;           // Minimum number of inliers for a plane
        double distanceThreshold = 1.0; // Distance threshold from an inlier to the model
        int maxIterations = 20;         // Maximum number of RANSAC iterations

        template <class Archive>
        void serialize(Archive &archive)
        {
            archive(CEREAL_NVP(maxPlaneNum),
                    CEREAL_NVP(minInlierNum),
                    CEREAL_NVP(distanceThreshold),
                    CEREAL_NVP(maxIterations));
        }
    };

    // Line fitting parameters
    struct RANSACLines
    {
        int maxLineNum = 10;            // Maximum number of fitted lines
        int minInlierNum = 2;           // Minimum number of inliers for a line
        double distanceThreshold = 1.0; // Distance threshold from an inlier to the model
        int maxIterations = 20;         // Maximum number of RANSAC iterations

        template <class Archive>
        void serialize(Archive &archive)
        {
            archive(CEREAL_NVP(maxLineNum),
                    CEREAL_NVP(minInlierNum),
                    CEREAL_NVP(distanceThreshold),
                    CEREAL_NVP(maxIterations));
        }
    };

    // Merge adjacent parallel lines
    struct ParallelLinesMerg
    {
        double angleThreshold;       // Minimum angle between line segments
        double widthMinBox;          // Minimum bounding-box width
        double widthMaxBox;          // Maximum bounding-box width
        double nonZeroRatio;         // Ratio of nonzero pixels
        double overlappingThreshold; // Overlapping area of the bounding boxes

        template <class Archive>
        void serialize(Archive &archive)
        {
            archive(CEREAL_NVP(angleThreshold),
                    CEREAL_NVP(widthMinBox),
                    CEREAL_NVP(widthMaxBox),
                    CEREAL_NVP(nonZeroRatio),
                    CEREAL_NVP(overlappingThreshol));
        }
    };

    // YOLO configuration parameters
    struct YOLOParams
    {
        float scoreThres = 0.55f; // Confidence threshold
        float iouThres = 0.65f;   // NMS threshold
        int topk = 10;            // Keep the top-k results
        int labelsNum = 1;        // Number of classes
        int postprocess = 1;      // Enable post-processing

        template <class Archive>
        void serialize(Archive &archive)
        {
            archive(CEREAL_NVP(scoreThres),
                    CEREAL_NVP(iouThres),
                    CEREAL_NVP(topk),
                    CEREAL_NVP(labelsNum),
                    CEREAL_NVP(postprocess));
        }
    };

}

