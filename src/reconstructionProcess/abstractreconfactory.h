#pragma once

#include "reconstructioncommon.h"
#include "steelangle/steelanglerecon.h"

namespace reconstruction
{
    enum class workpieceType : int
    {
        SteelAngle = 1,
        Other = 2
    };
}

class ReconAlgFactory
{
public:
    std::unique_ptr<AbstractReconAlgorithm> createAlgorithm(int type)
    {
        switch (type)
        {
        case reconstruction::workpieceType::SteelAngle:
            return std::make_unique<SteelAngleRecon>(nullptr);

        default:
            return nullptr;
        }
        return nullptr;
    }
};

class ReconAlgParamsFactory
{
public:
    explicit ReconAlgParamsFactory(reconstructionParams *params) : params_(params) {}

    std::shared_ptr<ReconAlgParams> createParams(int type)
    {
        switch (type)
        {
        case reconstruction::workpieceType::SteelAngle:
        {
            auto p = std::make_shared<SteelAngleReconParams>();
            p->cfg = &params_->steelangle;
            return p;
        }

        default:
            return nullptr;
        }
        return nullptr;
    }

private:
    reconstructionParams *params_ = nullptr;
};
