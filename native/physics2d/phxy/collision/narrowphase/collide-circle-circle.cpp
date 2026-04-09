#include "collide-circle-circle.h"
#include "../../common/SqConfig.h"

using namespace phxy;

// point = qA * localAnchorA + pA
// localAnchorB = qBc * (point - pB)
// anchorB = point - pB = qA * localAnchorA + pA - pB
//         = anchorA + (pA - pB)
SqManifold phxy::sqCollideCircles(const SqCircleShape *circleA, SqTransform &xfA, const SqCircleShape *circleB, SqTransform &xfB)
{
    SqManifold manifold;
    const float speculativeDistance = SqConfig::getInstance()->getSpeculativeDistance();
    SqTransform xf = SqTransform::InvMulTransforms(xfA, xfB);

    SqVec2 pointA = circleA->center;
    SqVec2 pointB = xf.transformPoint(circleB->center);

    float distance;
    SqVec2 normal = SqVec2::GetLengthAndNormalize(&distance, SqVec2::Sub(pointB, pointA));

    float radiusA = circleA->radius;
    float radiusB = circleB->radius;

    float separation = distance - radiusA - radiusB;
    if (separation > speculativeDistance)
    {
        return manifold;
    }

    SqVec2 cA = SqVec2::MulAdd(pointA, radiusA, normal);
    SqVec2 cB = SqVec2::MulAdd(pointB, -radiusB, normal);
    SqVec2 contactPointA = SqVec2::Lerp(cA, cB, 0.5f);

    manifold.normal = xfA.transformVector(normal);
    SqManifoldPoint *mp = manifold.points + 0;
    mp->anchorA = xfA.transformVector(contactPointA);
    mp->anchorB = SqVec2::Add(mp->anchorA, SqVec2::Sub(xfA.p, xfB.p));
    mp->point = SqVec2::Add(mp->anchorA, xfA.p);
    mp->separation = separation;
    mp->id = 0;
    manifold.pointCount = 1;
    return manifold;
}