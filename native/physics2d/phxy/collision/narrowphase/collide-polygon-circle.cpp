#include "collide-polygon-circle.h"
#include "../../common/SqConfig.h"
#include <float.h>
using namespace phxy;

SqManifold phxy::sqCollidePolygonAndCircle(const SqPolygonShape *polygonA, SqTransform &xfA, const SqCircleShape *circleB, SqTransform &xfB)
{
    SqManifold manifold;
    const float speculativeDistance = SqConfig::getInstance()->getSpeculativeDistance();

    SqTransform xf = SqTransform::InvMulTransforms(xfA, xfB);

    // Compute circle position in the frame of the polygon.
    SqVec2 center = xf.transformPoint(circleB->center);
    float radiusA = polygonA->radius;
    float radiusB = circleB->radius;
    float radius = radiusA + radiusB;

    // Find the min separating edge.
    int normalIndex = 0;
    float separation = -FLT_MAX;
    int vertexCount = polygonA->count;
    const SqVec2 *vertices = polygonA->vertices;
    const SqVec2 *normals = polygonA->normals;

    for (int i = 0; i < vertexCount; ++i)
    {
        float s = SqVec2::Dot(normals[i], SqVec2::Sub(center, vertices[i]));
        if (s > separation)
        {
            separation = s;
            normalIndex = i;
        }
    }

    if (separation > radius + speculativeDistance)
    {
        return manifold;
    }

    // Vertices of the reference edge.
    int vertIndex1 = normalIndex;
    int vertIndex2 = vertIndex1 + 1 < vertexCount ? vertIndex1 + 1 : 0;
    const SqVec2& v1 = vertices[vertIndex1];
    const SqVec2& v2 = vertices[vertIndex2];

    // 计算重心坐标 u1和u2
    // u1 = (center - v1) . (v2 - v1)
    float u1 = SqVec2::Dot(SqVec2::Sub(center, v1), SqVec2::Sub(v2, v1));

    // u2 = (center - v2) . (v1 - v2)
    float u2 = SqVec2::Dot(SqVec2::Sub(center, v2), SqVec2::Sub(v1, v2));

    if (u1 < 0.0f && separation > FLT_EPSILON)
    {
        // Circle center is closest to v1 and safely outside the polygon
        SqVec2 normal = SqVec2::Normalize(SqVec2::Sub(center, v1));
        separation = SqVec2::Dot(SqVec2::Sub(center, v1), normal);
        if (separation > radius + speculativeDistance)
        {
            return manifold;
        }

        SqVec2 cA = SqVec2::MulAdd(v1, radiusA, normal);
        SqVec2 cB = SqVec2::MulSub(center, radiusB, normal);
        SqVec2 contactPointA = SqVec2::Lerp(cA, cB, 0.5f);

        manifold.normal = xfA.transformVector(normal);
        SqManifoldPoint *mp = manifold.points + 0;
        mp->anchorA = xfA.transformVector(contactPointA);
        mp->anchorB = SqVec2::Add(mp->anchorA, SqVec2::Sub(xfA.p, xfB.p));
        mp->point = SqVec2::Add(xfA.p, mp->anchorA);
        mp->separation = SqVec2::Dot(SqVec2::Sub(cB, cA), normal);
        mp->id = 0;
        manifold.pointCount = 1;
    }
    else if (u2 < 0.0f && separation > FLT_EPSILON)
    {
        // Circle center is closest to v2 and safely outside the polygon
        SqVec2 normal = SqVec2::Normalize(SqVec2::Sub(center, v2));
        separation = SqVec2::Dot(SqVec2::Sub(center, v2), normal);
        if (separation > radius + speculativeDistance)
        {
            return manifold;
        }

        SqVec2 cA = SqVec2::MulAdd(v2, radiusA, normal);
        SqVec2 cB = SqVec2::MulSub(center, radiusB, normal);
        SqVec2 contactPointA = SqVec2::Lerp(cA, cB, 0.5f);

        manifold.normal = xfA.transformVector(normal);
        SqManifoldPoint *mp = manifold.points + 0;
        mp->anchorA = xfA.transformVector(contactPointA);
        mp->anchorB = SqVec2::Add(mp->anchorA, SqVec2::Sub(xfA.p, xfB.p));
        mp->point = SqVec2::Add(xfA.p, mp->anchorA);
        mp->separation = SqVec2::Dot(SqVec2::Sub(cB, cA), normal);
        mp->id = 0;
        manifold.pointCount = 1;
    }
    else
    {
        // Circle center is between v1 and v2. Center may be inside polygon
        const SqVec2& normal = normals[normalIndex];
        manifold.normal = xfA.transformVector(normal);

        // cA is the projection of the circle center onto to the reference edge
        SqVec2 cA = SqVec2::MulAdd(center, radiusA - SqVec2::Dot(SqVec2::Sub(center, v1), normal), normal);

        // cB is the deepest point on the circle with respect to the reference edge
        SqVec2 cB = SqVec2::MulSub(center, radiusB, normal);

        SqVec2 contactPointA = SqVec2::Lerp(cA, cB, 0.5f);

        // The contact point is the midpoint in world space
        SqManifoldPoint *mp = manifold.points + 0;
        mp->anchorA = xfA.transformVector(contactPointA);
        mp->anchorB = SqVec2::Add(mp->anchorA, SqVec2::Sub(xfA.p, xfB.p));
        mp->point = SqVec2::Add(xfA.p, mp->anchorA);
        mp->separation = separation - radius;
        mp->id = 0;
        manifold.pointCount = 1;
    }

    return manifold;
}