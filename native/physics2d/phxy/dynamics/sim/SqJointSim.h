#pragma once
#include "../joint/SqJoint.h"
#include "../SqSoftConstraint.h"
namespace phxy
{
    struct SqJointSim
    {
        int jointId{SQ_NULL_INDEX};
        int bodyAIndex{SQ_NULL_INDEX};
        int bodyBIndex{SQ_NULL_INDEX};
        SqTransform localFrameA;
        SqTransform localFrameB;
        bool enableSoftness{true};
        float constraintHertz{60.f};
        float constraintDampingRatio{0.f};
        float forceThreshold{0.f};
        float torqueThreshold{0.f};
        SqSoftConstraint constraintSoftness;
        SqJointSim();
        void reset();
    };

}