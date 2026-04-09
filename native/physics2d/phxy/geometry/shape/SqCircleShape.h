#pragma once
#include "SqShape.h"
#include "../../common/math/SqVec2.h"

namespace phxy
{
    class SqCircleShape : public SqShape
    {
    protected:
        void RayCastShapeImpl(SqCastOutput &output, const SqShapeCastInput &input, const SqTransform &transform) const;
        virtual void RayCastImpl(SqCastOutput &output, const SqRayCastInput &input, const SqTransform &transform) const;
        virtual void CollideMoverImpl(SqPlaneResult &result, SqCapsuleShape *mover) const;

    public:
        /**
         * 圆心位置。本地坐标
         */
        SqVec2 center;

        /**
         * 半径
         */
        float radius = 0.f;

        SqCircleShape();
        virtual bool PointIn(const SqVec2 &point) const;
        virtual SqAABB computeShapeAABB(const SqTransform &transform) const;
        virtual SqMassData computeShapeMass();
        virtual SqShapeExtent computeShapeExtent(const SqVec2 &localCenter);
        virtual void computeDistance(const SqVec2 &point, const SqTransform &xf, float &outDistance, SqVec2 &outNormal) const;
        virtual SqVec2 getCentroid();
        virtual void scale(float sx,float sy);
        void setCircle(const SqVec2 &center, float radius);
    };

}