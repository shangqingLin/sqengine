#include "SqChainEdgeShape.h"
#include "SqEdgeShape.h"
#include "../../common/math/SqMath.h"
#include "../../common/SqConfig.h"
#include "../../collision/narrowphase/shape-cast.h"
#include <stdio.h>

using namespace phxy;

SqChainEdgeShape::SqChainEdgeShape()
{
    type = SqShapeType::sq_chainSegmentShape;
}

void SqChainEdgeShape::setEdge(const SqVec2 &point1, const SqVec2 &point2, const SqVec2 &ghost1, const SqVec2 &ghost2)
{
    this->point1 = point1;
    this->point2 = point2;
    this->ghost1 = ghost1;
    this->ghost2 = ghost2;
}

void SqChainEdgeShape::scale(float sx,float sy)
{
    this->point1.x *= sx;
    this->point1.y *= sy;

    this->point2.x *= sx;
    this->point2.y *= sy;
    
    this->ghost1.x *= sx;
    this->ghost1.y *= sy;
    
    this->ghost2.x *= sx;
    this->ghost2.y *= sy;
}

SqAABB SqChainEdgeShape::computeShapeAABB(const SqTransform &transform) const
{
    SqVec2 v1 = SqTransform::transformPoint(transform, point1);
    SqVec2 v2 = SqTransform::transformPoint(transform, point2);
    SqVec2 lower = SqVec2::Min(v1, v2);
    SqVec2 upper = SqVec2::Max(v1, v2);
    SqAABB aabb = {lower, upper};
    const float speculativeDistance = SqConfig::getInstance()->getSpeculativeDistance();
    aabb.lowerBound.x -= speculativeDistance;
    aabb.lowerBound.y -= speculativeDistance;
    aabb.upperBound.x += speculativeDistance;
    aabb.upperBound.y += speculativeDistance;
    return aabb;
}

SqMassData SqChainEdgeShape::computeShapeMass()
{
    // 对于Chain线段，没有质量没有质心，因为Chain Shape一般都是用于静态 碰撞体使用
    return SqMassData();
}

SqShapeExtent SqChainEdgeShape::computeShapeExtent(const SqVec2 &localCenter)
{
    SqShapeExtent extent;
    extent.minExtent = 0.0f;
    SqVec2 c1 = SqVec2::Sub(point1, localCenter);
    SqVec2 c2 = SqVec2::Sub(point2, localCenter);
    extent.maxExtent = sqrtf(max(SqVec2::LengthSquared(c1), SqVec2::LengthSquared(c2)));
    return extent;
}

SqVec2 SqChainEdgeShape::getCentroid()
{
    return SqVec2::Lerp(point1, point2, 0.5f);
}

void SqChainEdgeShape::RayCastShapeImpl(SqCastOutput &output, const SqShapeCastInput &input, const SqTransform &transform) const
{

    // Check for back side collision
    SqVec2 approximateCentroid = input.proxy.points[0];
    for (int i = 1; i < input.proxy.count; ++i)
    {
        approximateCentroid = SqVec2::Add(approximateCentroid, input.proxy.points[i]);
    }

    approximateCentroid = SqVec2::MulSV(1.0f / input.proxy.count, approximateCentroid);

    SqVec2 edge = SqVec2::Sub(point2, point1);
    SqVec2 r = SqVec2::Sub(approximateCentroid, point1);

    if (SqVec2::Cross(r, edge) < 0.0f)
    {
        // Shape cast starts behind
        return;
    }

    SqShapeCastPairInput pairInput;
    SqVec2 points[2] = {point1, point2};
    pairInput.proxyA = sqMakeProxy(points, 2, 0.0f);
    pairInput.proxyB = input.proxy;
    pairInput.translationB = input.translation;
    pairInput.maxFraction = input.maxFraction;
    pairInput.canEncroach = input.canEncroach;
    sqShapeCast(output, &pairInput);
}

void SqChainEdgeShape::RayCastImpl(SqCastOutput &output, const SqRayCastInput &input, const SqTransform &transform) const
{

    // SqVec2 point11 = SqTransform::transformPoint(transform, point1);
    // SqVec2 point22 = SqTransform::transformPoint(transform, point2);
    // printf("edge point1 %f %f point2 %f %f ? %f \n", point1.x, point1.y, point2.x, point2.y, input.maxFraction);

    SqEdgeShape edgeShape;
    edgeShape.setEdge(point1, point2);
    edgeShape.RayCastImpl(output, input, transform);
}

void SqChainEdgeShape::CollideMoverImpl(SqPlaneResult &result, SqCapsuleShape *mover) const
{
    SqEdgeShape edgeShape;
    edgeShape.setEdge(point1, point2);
    edgeShape.CollideMoverImpl(result, mover);
}

void SqChainEdgeShape::computeDistance(const SqVec2 &point, const SqTransform &xf, float &outDistance, SqVec2 &outNormal) const
{
    SqEdgeShape edge;
    edge.setEdge(point1, point2);
    edge.computeDistance(point, xf, outDistance, outNormal);
}