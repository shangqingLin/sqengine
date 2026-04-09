#pragma once
#include "SqShape.h"

namespace phxy
{

    class SqChainEdgeShape : public SqShape
    {
    protected:
        void RayCastShapeImpl(SqCastOutput &output, const SqShapeCastInput &input, const SqTransform &transform) const;
        virtual void RayCastImpl(SqCastOutput &output, const SqRayCastInput &input, const SqTransform &transform) const;
        virtual void CollideMoverImpl(SqPlaneResult &result, SqCapsuleShape *mover) const;

    public:
        friend class SqChainShape;
        SqVec2 point1;
        SqVec2 point2;

        /**
         * https://box2d.org/posts/2020/06/ghost-collisions/
         * ghost1为point1的前一个点
         * ghost2为poin2的后一个点
         */
        SqVec2 ghost1;
        SqVec2 ghost2;
        SqChainEdgeShape();

        virtual SqAABB computeShapeAABB(const SqTransform &transform) const;
        virtual SqMassData computeShapeMass();
        virtual SqShapeExtent computeShapeExtent(const SqVec2 &localCenter);
        virtual void computeDistance(const SqVec2 &point, const SqTransform &xf, float &outDistance, SqVec2 &outNormal) const;
        virtual SqVec2 getCentroid();
        virtual void scale(float sx,float sy);
        void setEdge(const SqVec2 &point1, const SqVec2 &point2, const SqVec2 &ghost1, const SqVec2 &ghost2);
    };
}