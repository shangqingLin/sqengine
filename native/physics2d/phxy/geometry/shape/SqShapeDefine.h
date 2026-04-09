#pragma once
#include "../../common/constants-define.h"
#include "../utils/SqHull.h"
#include "../../common/math/SqVec2.h"
#include "../../common/math/SqRot.h"
#include <stdint.h>
#include "../../collision/broadphase/SqBroadPhaseFilter.h"

namespace phxy
{

    enum SqShapeType
    {
        sq_circleShape,

        sq_capsuleShape,

        sq_segmentShape,

        sq_polygonShape,

        sq_chainSegmentShape,

        sq_shapeTypeCount,

        // 辅助类型，不是实际的形状类型
        sq_boxShape,   // 实际是sq_polygonShape
        sq_chainShape, // 实际是sq_chainSegmentShape
    };

  

    typedef struct SqShapeExtent
    {
        float minExtent;
        float maxExtent;
    } SqShapeExtent;

    struct SqSurfaceMaterial
    {
        /// The Coulomb (dry) friction coefficient, usually in the range [0,1].
        float friction{0.6f};

        ///
        /// https://en.wikipedia.org/wiki/Coefficient_of_restitution
        float restitution{0.f};

        /// The rolling resistance usually in the range [0,1].
        float rollingResistance{0.f};

        /// The tangent speed for conveyor belts
        float tangentSpeed{0.f};

        uint32_t customColor;
    };

    struct SqShapeDef
    {
        /**
         * 必须指定
         */
        SqShapeType type;

        void *userData{nullptr};

        SqSurfaceMaterial material;

        float density{1.0f};

        SqShapeFilter filter;

        bool isSensor{false};

        bool sensorAABB{false};

        uint32_t customColor = 0;
    };

    struct SqChainDef : public SqShapeDef
    {
        SqVec2 *points;
        int count{0};
        bool isLoop{false};
    };

    struct SqPolygonShapeDef : public SqShapeDef
    {
        SqHull hull;
        float radius{0.f};
    };

    struct SqBoxShapeDef : public SqShapeDef
    {
        float halfWidth;
        float halfHeight;
        SqVec2 center;
        SqRot rotation;
    };

    struct SqCapusleShapeDef : public SqShapeDef
    {
        SqVec2 center1;
        SqVec2 center2;
        float radius;
    };

    struct SqCircleShapeDef : public SqShapeDef
    {
        SqVec2 center;
        float radius;
    };

    struct SqEdgeShapeDef : public SqShapeDef
    {
        SqVec2 point1;
        SqVec2 point2;
    };
}