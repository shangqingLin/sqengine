#pragma once
#include "../../common/math/SqVec2.h"
#include "SqShape.h"

namespace phxy
{

    class SqEdgeShape : public SqShape
    {
    protected:
        virtual void RayCastShapeImpl(SqCastOutput &output, const SqShapeCastInput &input, const SqTransform &transform) const;
        virtual void RayCastImpl(SqCastOutput &output, const SqRayCastInput &input, const SqTransform &transform) const;
        virtual void CollideMoverImpl(SqPlaneResult &result, SqCapsuleShape *mover) const;

    public:
        friend class SqChainEdgeShape;
        SqVec2 point1;
        SqVec2 point2;
        bool oneSided;
        SqEdgeShape();
        virtual SqAABB computeShapeAABB(const SqTransform &transform) const;
        virtual SqMassData computeShapeMass();
        virtual SqShapeExtent computeShapeExtent(const SqVec2 &localCenter);
        virtual void computeDistance(const SqVec2 &point, const SqTransform &xf, float &outDistance, SqVec2 &outNormal) const;
        virtual SqVec2 getCentroid();
        virtual void scale(float sx,float sy);
        void setEdge(const SqVec2 &point1, const SqVec2 &point2);
    };

}