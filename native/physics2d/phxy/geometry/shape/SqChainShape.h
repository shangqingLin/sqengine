#pragma once
#include "../../common/math/SqVec2.h"
#include "../../common/collection/SqArray.h"
#include "SqShape.h"
#include "SqEdgeShape.h"
#include "SqChainEdgeShape.h"

namespace phxy
{
    class SqChainShape : public SqShape
    {
    protected:
        void RayCastShapeImpl(SqCastOutput &output, const SqShapeCastInput &input, const SqTransform &transform) const;
        virtual void RayCastImpl(SqCastOutput &output, const SqRayCastInput &input, const SqTransform &transform) const;
        virtual void CollideMoverImpl(SqPlaneResult &result, SqCapsuleShape *mover) const;

    public:
        int count = 0;
        SqArray<SqChainEdgeShape> segments;
        SqChainShape();
        virtual void setFriction(float v);
        virtual void setRestitution(float v);
        virtual void setFilter(const SqShapeFilter &f);
        virtual SqAABB computeShapeAABB(const SqTransform &transform) const;
        virtual SqMassData computeShapeMass();
        virtual SqShapeExtent computeShapeExtent(const SqVec2 &localCenter);
        virtual void computeDistance(const SqVec2 &point, const SqTransform &xf, float &outDistance, SqVec2 &outNormal) const;
        virtual SqVec2 getCentroid();
        virtual void scale(float sx,float sy);
        virtual void setTangentSpeed(float f);
        void setChain(SqVec2 *, int, bool);
    };

}