#pragma once
#include "collision.h"
#include "../../common/SqTypeDefine.h"

namespace phxy
{

    struct SqShapeCastPairInput
    {
        SqShapeProxy proxyA;    ///< The proxy for shape A
        SqShapeProxy proxyB;    ///< The proxy for shape B
        SqTransform transformA; ///< The world transform for shape A
        SqTransform transformB; ///< The world transform for shape B
        SqVec2 translationB;    ///< The translation of shape B
        float maxFraction;      ///< The fraction of the translation to consider, typically 1
        bool canEncroach;       ///< Allows shapes with a radius to move slightly closer if already touching
    };

    void sqShapeCast(SqCastOutput &, const SqShapeCastPairInput *input);
}