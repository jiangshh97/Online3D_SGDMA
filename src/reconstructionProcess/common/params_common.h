#ifndef PARAMS_COMMON_H
#define PARAMS_COMMON_H

namespace reconstruction
{

    // Pass-through filter parameters
    struct PassThroughFilter
    {
        std::vector<double> valueX{0.0, 0.0}; // X-axis filtering range [min, max]
        std::vector<double> valueY{0.0, 0.0}; // Y-axis filtering range [min, max]
        std::vector<double> valueZ{0.0, 0.0}; // Z-axis filtering range [min, max]

        template <class Archive>
        void serialize(Archive &archive)
        {
            archive(CEREAL_NVP(valueX),
                    CEREAL_NVP(valueY),
                    CEREAL_NVP(valueZ));
        }
    };

    // Statistical outlier removal parameters
    struct StatisticalFitter
    {
        int meank = 50;       // Number of nearest neighbors
        double stdMult = 1.0; // Standard deviation multiplier threshold

        template <class Archive>
        void serialize(Archive &archive)
        {
            archive(CEREAL_NVP(meank),
                    CEREAL_NVP(stdMult));
        }
    };

    // RANSAC-based plane fitting parameters
    struct RANSACPlanes
    {
        int maxPlaneNum = 10;           // Maximum number of planes to detect
        int minInlierNum = 3;           // Minimum number of inliers for a valid plane
        double distanceThreshold = 1.0; // Distance threshold for inlier classification
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

    // RANSAC-based line fitting parameters
    struct RANSACLines
    {
        int maxLineNum = 10;            // Maximum number of lines to detect
        int minInlierNum = 2;           // Minimum number of inliers for a valid line
        double distanceThreshold = 1.0; // Distance threshold for inlier classification
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

}

#endif // PARAMS_COMMON_H
