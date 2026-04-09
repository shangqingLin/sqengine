#include "SqChainShape.h"
#include "../../common/SqCore.h"
#include <stdio.h>

using namespace phxy;

SqChainShape::SqChainShape()
{
    type = sq_chainShape;
}

/**
 * 我这里的底层碰撞算法规定，必须是按照顺时针的顺序来指定坐标点
 * 所以chainShape中的坐标点必须按照顺时针顺序来输入
 */
void SqChainShape::setChain(SqVec2 *points, int count, bool loop)
{
    SQ_ASSERT(count >= 2);

    if (loop)
    {
        this->count = count;
        segments.resize(this->count);

        int prevIndex = count - 1;
        int i = 0;
        for (i = 0; i < count - 2; ++i)
        {
            SqChainEdgeShape *chainSegment = segments.Add();
            chainSegment->bodyIndex = bodyIndex;
            chainSegment->setEdge(points[i], points[i + 1], points[prevIndex], points[i + 2]);
            prevIndex = i;
        }

        {
            SqChainEdgeShape *chainSegment = segments.Add();
            chainSegment->bodyIndex = bodyIndex;
            chainSegment->setEdge(points[count - 2], points[count - 1], points[count - 3], points[0]);
            ++i;
        }

        {
            SqChainEdgeShape *chainSegment = segments.Add();
            chainSegment->bodyIndex = bodyIndex;
            chainSegment->setEdge(points[count - 1], points[0], points[count - 2], points[1]);
        }
    }
    else
    {
        this->count = count - 1;
        segments.resize(this->count);
        SqVec2 ghost1, ghost2;

        for (int i = 0; i < this->count; ++i)
        {
            SqChainEdgeShape *chainSegment = segments.Add();
            chainSegment->bodyIndex = bodyIndex;

            ghost1 = i == 0 ? points[i] : points[i - 1];                   // 前一个
            ghost2 = i + 2 <= this->count ? points[i + 2] : points[i + 1]; // 后一个
                                                                           // chainSegment->setEdge(points[i], points[i + 1], points[i], points[i + 1]);

            // printf("chian shape point %d p1 %f %f p2 %f %f \n", i, points[i].x, points[i].y, points[i + 1].x,points[i + 1].y);

            chainSegment->setEdge(points[i], points[i + 1], ghost1, ghost2);
        }
    }
}

void SqChainShape::scale(float sx,float sy)
{
    for (int i = 0; i < this->count; ++i)
    {
        segments.get(i)->scale(sx,sy);
    }
}

void SqChainShape::setFriction(float v)
{
    friction = v;
    for (int i = 0; i < this->count; ++i)
    {
        segments.get(i)->setFriction(v);
    }
}

void SqChainShape::setRestitution(float v)
{
    restitution = v;
    for (int i = 0; i < this->count; ++i)
    {
        segments.get(i)->setRestitution(v);
    }
}

void SqChainShape::setFilter(const SqShapeFilter &f)
{
    // printf("SqChainShape::setFilter %llu %llu edgeNum %d \n", f.categoryBits, f.maskBits,this->count);
    if (filter.categoryBits == f.categoryBits && filter.maskBits == f.maskBits)
        return;
    filter = f;
    for (int i = 0; i < this->count; ++i)
    {
        segments.get(i)->setFilter(f);
    }
}

void SqChainShape::setTangentSpeed(float f)
{
    if (f == tangentSpeed)
        return;
    SqShape::setTangentSpeed(f);
    for (int i = 0; i < this->count; ++i)
    {
        segments.get(i)->setTangentSpeed(f);
    }
}

SqAABB SqChainShape::computeShapeAABB(const SqTransform &transform) const
{
    return SqAABB();
}

SqMassData SqChainShape::computeShapeMass()
{
    return SqMassData();
}

SqShapeExtent SqChainShape::computeShapeExtent(const SqVec2 &localCenter)
{
    return SqShapeExtent();
}

SqVec2 SqChainShape::getCentroid()
{
    return SqVec2();
}

void SqChainShape::RayCastShapeImpl(SqCastOutput &output, const SqShapeCastInput &input, const SqTransform &transform) const {}
void SqChainShape::RayCastImpl(SqCastOutput &output, const SqRayCastInput &input, const SqTransform &transform) const {}
void SqChainShape::CollideMoverImpl(SqPlaneResult &result, SqCapsuleShape *mover) const {}
void SqChainShape::computeDistance(const SqVec2 &point, const SqTransform &xf, float &outDistance, SqVec2 &outNormal) const {}