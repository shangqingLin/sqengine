#pragma once
#include "../../common/SqTypeDefine.h"
#include "SqShape.h"

namespace phxy
{
    class SqPolygonShape : public SqShape
    {
    protected:
        void RayCastShapeImpl(SqCastOutput &output, const SqShapeCastInput &input, const SqTransform &transform) const;
        virtual void RayCastImpl(SqCastOutput &output, const SqRayCastInput &input, const SqTransform &transform) const;
        virtual void CollideMoverImpl(SqPlaneResult &result, SqCapsuleShape *mover) const;

    public:
        /**
         * 存储多边形的顶点
         */
        SqVec2 vertices[SQ_MAX_POLYGON_VERTICES];

        /**
         * 多边形边的法线，方向是指向外面的
         */
        SqVec2 normals[SQ_MAX_POLYGON_VERTICES];

        /**
         * 质心坐标
         */
        SqVec2 centroid;

        float radius = 0.f;

        /**
         * 记录当前共有多少个顶点
         */
        int count = 0;

        SqPolygonShape();

        virtual bool PointIn(const SqVec2 &point) const;
        virtual SqAABB computeShapeAABB(const SqTransform &transform) const;
        virtual SqMassData computeShapeMass();
        virtual SqShapeExtent computeShapeExtent(const SqVec2 &localCenter);
        virtual void computeDistance(const SqVec2 &point, const SqTransform &xf, float &outDistance, SqVec2 &outNormal) const;
        virtual SqVec2 getCentroid();
        virtual void scale(float sx,float sy);
        void setAsBox(float halfWidth, float halfHeight, SqVec2 center, SqRot rotation);
        void setPolygon(const SqHull &hull, float radius);
        void makeFromCapsule(const SqVec2 &p1, const SqVec2 &p2, float radius);
    };
}