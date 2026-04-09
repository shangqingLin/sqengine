#pragma once
#include "SqShape.h"
#include "../../common/math/SqVec2.h"

namespace phxy
{
    /**
     * 胶囊体
     */
    class SqCapsuleShape : public SqShape
    {

    protected:
        virtual void RayCastShapeImpl(SqCastOutput &output, const SqShapeCastInput &input, const SqTransform &transform) const;
        virtual void RayCastImpl(SqCastOutput &output, const SqRayCastInput &input, const SqTransform &transform) const;
        virtual void CollideMoverImpl(SqPlaneResult &result, SqCapsuleShape *mover) const;

    public:
        /**
         * 上下两个半圆的圆心坐标
         * 其这两个圆心的距离决定中间矩形体的高度
         */
        SqVec2 center1;
        SqVec2 center2;

        // 半圆的半径。大小决定了胶囊体的宽度
        float radius = 0;

        SqCapsuleShape();

        virtual bool PointIn(const SqVec2 &point) const;
        virtual SqAABB computeShapeAABB(const SqTransform &transform) const;
        virtual SqMassData computeShapeMass();
        virtual SqShapeExtent computeShapeExtent(const SqVec2 &localCenter);
        virtual void computeDistance(const SqVec2 &point, const SqTransform &xf, float &outDistance, SqVec2 &outNormal) const;
        virtual SqVec2 getCentroid();
        virtual void scale(float sx,float sy);
        void setCapsule(const SqVec2 &center1, const SqVec2 &center2, float radius);
    };

}
