#pragma once

#include <array>
#include <Eigen/Dense>

#include "common/workpiece_params.h"

namespace reconstruction
{

    // ===== Algorithm Parameters =====
    struct SteelAngleParams
    {
        PassThroughFilter ptParams{{-500.0, 460.0}, {0.0, 0.0}, {650.0, 1200.0}};
        double downSample = 0.75;
        RadiusOutlierFitter outlierParams{2.5, 3};
        RANSACPlanes planeFitParams{0, 0, 1.5, 200};
        double holeArea = 50.0 * 50.0;

        YOLOParams yoloParams{0.55f, 0.65f, 100, 0, 0};
        std::string enginePath = "/resource/engine/yolov11_Segment.engine";

        double radiusNeighbor = 10.0;
        RANSACLines lineFitParams{0, 0, 2.0, 200};

        double corrAngleTh = 15.0;
        double corrdisTh = 10.0;

        template <class Archive>
        void serialize(Archive &archive)
        {
            archive(CEREAL_NVP(ptParams),
                    CEREAL_NVP(downSample),
                    CEREAL_NVP(outlierParams),
                    CEREAL_NVP(planeFitParams),
                    CEREAL_NVP(holeArea),
                    CEREAL_NVP(yoloParams),
                    CEREAL_NVP(enginePath),
                    CEREAL_NVP(radiusNeighbor),
                    CEREAL_NVP(lineFitParams),
                    CEREAL_NVP(corrAngleTh),
                    CEREAL_NVP(corrdisTh));
        }
    };

    // ===== Model Parameters =====
    // Parameters of the angle-steel model in the 3D image
    struct steelAngleParams3D
    {
        std::array<Eigen::Vector3d, 2> ridge; // Corner-guided sweep line AB
        Eigen::Vector3d leg1Vec;              // Direction vector of leg 1 of the angle steel
        Eigen::Vector3d leg2Vec;              // Direction vector of leg 2 of the angle steel
        double width = 0.0;                   // Leg width of the angle steel
        double thickness = 5.0;               // Leg thickness of the angle steel
    };
}
