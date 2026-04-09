#include "SqPolygonShape.h"
#include "../../common/math/SqTransform.h"
#include "../../common/SqCore.h"
#include "../../common/SqConfig.h"
#include "../../common/math/SqMath.h"
#include "../utils/SqHull.h"
#include "SqCircleShape.h"
#include "SqCapsuleShape.h"
#include "../../collision/narrowphase/shape-cast.h"
#include <float.h>
#include "../../dynamics/SqWorld.h"

using namespace phxy;

SqPolygonShape::SqPolygonShape()
{
    type = sq_polygonShape;
    count = 0;
    radius = 0.0f;
    centroid = SqVec2(0.0f, 0.0f);
}

/**
 *
 * 计算多边形的质心坐标
 * 思路是将一个凸多边形切分成若干个三角形（三角剖分），然后计算这些三角形的质心与面积，最终做加权平均，得到整个多边形的质心（centroid）。
 */
static SqVec2 computePolygonCentroid(const SqVec2 *vertices, int count)
{
    SqVec2 center = {0.0f, 0.0f};
    float area = 0.0f;

    // Get a reference point for forming triangles.
    // Use the first vertex to reduce round-off errors.
    const SqVec2 &origin = vertices[0];

    const float inv3 = 1.0f / 3.0f;

    for (int i = 1; i < count - 1; ++i)
    {
        // Triangle edges
        SqVec2 e1 = SqVec2::Sub(vertices[i], origin);
        SqVec2 e2 = SqVec2::Sub(vertices[i + 1], origin);
        float a = 0.5f * SqVec2::Cross(e1, e2);

        // Area weighted centroid
        center = SqVec2::MulAdd(center, a * inv3, SqVec2::Add(e1, e2));
        area += a;
    }

    SQ_ASSERT(area > FLT_EPSILON);
    float invArea = 1.0f / area;
    center.x *= invArea;
    center.y *= invArea;

    // Restore offset
    center = SqVec2::Add(origin, center);

    return center;
}

void SqPolygonShape::setAsBox(float halfWidth, float halfHeight, SqVec2 center, SqRot rotation)
{
    SqTransform xf = {center, rotation};
    count = 4;
    vertices[0] = xf.transformPoint(SqVec2{-halfWidth, -halfHeight});
    vertices[1] = xf.transformPoint(SqVec2{halfWidth, -halfHeight});
    vertices[2] = xf.transformPoint(SqVec2{halfWidth, halfHeight});
    vertices[3] = xf.transformPoint(SqVec2{-halfWidth, halfHeight});

    // printf("fuck2 %f %f %f %f %f %f %f %f \n",
	// 	vertices[0].x, vertices[0].y,
	// 	vertices[1].x, vertices[1].y,
	// 	vertices[2].x, vertices[2].y,
	// 	vertices[3].x, vertices[3].y
	// );

    normals[0] = xf.transformVector(SqVec2{0.0f, -1.0f});
    normals[1] = xf.transformVector(SqVec2{1.0f, 0.0f});
    normals[2] = xf.transformVector(SqVec2{0.0f, 1.0f});
    normals[3] = xf.transformVector(SqVec2{-1.0f, 0.0f});
    radius = 0.0f;
    centroid = xf.p;
}

void SqPolygonShape::setPolygon(const SqHull &hull, float radius)
{
    SQ_ASSERT(SqValidateHull(&hull));
    count = hull.count;
    this->radius = radius;

    for (int i = 0; i < count; ++i)
    {
        vertices[i] = hull.points[i];
    }

    // 计算多边形边的法线
    for (int i = 0; i < count; ++i)
    {
        int i1 = i;
        int i2 = i + 1 < count ? i + 1 : 0;
        SqVec2 edge = SqVec2::Sub(vertices[i2], vertices[i1]);
        // printf("add polygon (%f %f) (%f %f) \n",vertices[i1].x,vertices[i1].y, vertices[i2].x,vertices[i2].y);
        SQ_ASSERT(SqVec2::Dot(edge, edge) > FLT_EPSILON * FLT_EPSILON);
        normals[i] = SqVec2::Normalize(SqVec2::CrossVS(edge, 1.0f));
    }

    centroid = computePolygonCentroid(vertices, count);
}

void SqPolygonShape::scale(float sx, float sy)
{
    // this->radius *= sx;
    // for (int i = 0; i < count; ++i)
    // {
    //     vertices[i].x *= sx;
    //     vertices[i].y *= sy;
    // }
    // centroid = computePolygonCentroid(vertices, count);
}

void SqPolygonShape::makeFromCapsule(const SqVec2 &p1, const SqVec2 &p2, float radius)
{

    // 计算胶囊体中心 从center1到center2的线性插值到中心点
    vertices[0] = p1;
    vertices[1] = p2;
    centroid = SqVec2::Lerp(p1, p2, 0.5f);

    SqVec2 d = SqVec2::Sub(p2, p1);
    SQ_ASSERT(SqVec2::LengthSquared(d) > FLT_EPSILON);

    // 计算胶囊体的轴
    SqVec2 axis = SqVec2::Normalize(d);

    // 垂直于轴的右边法线
    SqVec2 normal = SqVec2::RightPerp(axis);
    normals[0] = normal;

    // 垂直于轴的左边法线
    normals[1] = SqVec2::Neg(normal);
    count = 2;
    this->radius = radius;
}

SqAABB SqPolygonShape::computeShapeAABB(const SqTransform &xf) const
{

    // printf("fuck22222 %f %f %f %f %f %f %f %f \n",
	// 	vertices[0].x, vertices[0].y,
	// 	vertices[1].x, vertices[1].y,
	// 	vertices[2].x, vertices[2].y,
	// 	vertices[3].x, vertices[3].y
	// );

    SQ_ASSERT(count > 0);
    SqVec2 lower = SqTransform::transformPoint(xf, vertices[0]);
    SqVec2 upper = lower;

    for (int i = 1; i < count; ++i)
    {
        SqVec2 v = SqTransform::transformPoint(xf, vertices[i]);
        lower = SqVec2::Min(lower, v);
        upper = SqVec2::Max(upper, v);
    }

    SqVec2 r = {radius, radius};
    lower = SqVec2::Sub(lower, r);
    upper = SqVec2::Add(upper, r);

    SqAABB aabb = {lower, upper};

    // const float speculativeDistance = SqConfig::getInstance()->getSpeculativeDistance();
    // aabb.lowerBound.x -= speculativeDistance;
    // aabb.lowerBound.y -= speculativeDistance;
    // aabb.upperBound.x += speculativeDistance;
    // aabb.upperBound.y += speculativeDistance;

    return aabb;
}

SqMassData SqPolygonShape::computeShapeMass()
{
    // Polygon mass, centroid, and inertia.
    // Let rho be the polygon density in mass per unit area.
    // Then:
    // mass = rho * int(dA)
    // centroid.x = (1/mass) * rho * int(x * dA)
    // centroid.y = (1/mass) * rho * int(y * dA)
    // I = rho * int((x*x + y*y) * dA)
    //
    // We can compute these integrals by summing all the integrals
    // for each triangle of the polygon. To evaluate the integral
    // for a single triangle, we make a change of variables to
    // the (u,v) coordinates of the triangle:
    // x = x0 + e1x * u + e2x * v
    // y = y0 + e1y * u + e2y * v
    // where 0 <= u && 0 <= v && u + v <= 1.
    //
    // We integrate u from [0,1-v] and then v from [0,1].
    // We also need to use the Jacobian of the transformation:
    // D = cross(e1, e2)
    //
    // Simplification: triangle centroid = (1/3) * (p1 + p2 + p3)
    //
    // The rest of the derivation is handled by computer algebra.

    SQ_ASSERT(count > 0);

    if (count == 1)
    {
        SqCircleShape circle;
        circle.center = vertices[0];
        circle.radius = radius;
        circle.setDensity(density);
        return circle.computeShapeMass();
    }

    if (count == 2)
    {
        SqCapsuleShape capsule;
        capsule.center1 = vertices[0];
        capsule.center2 = vertices[1];
        capsule.radius = radius;
        capsule.setDensity(density);
        return capsule.computeShapeMass();
    }

    SqVec2 vertices[SQ_MAX_POLYGON_VERTICES];
    if (radius > 0.0f)
    {
        // Approximate mass of rounded polygons by pushing out the vertices.
        float sqrt2 = 1.412f;
        for (int i = 0; i < count; ++i)
        {
            int j = i == 0 ? count - 1 : i - 1;
            const SqVec2 &n1 = normals[j];
            const SqVec2 &n2 = normals[i];

            SqVec2 mid = SqVec2::Normalize(SqVec2::Add(n1, n2));
            vertices[i] = SqVec2::MulAdd(this->vertices[i], sqrt2 * radius, mid);
        }
    }
    else
    {
        for (int i = 0; i < count; ++i)
        {
            vertices[i] = this->vertices[i];
        }
    }

    SqVec2 center;
    float area = 0.0f;
    float rotationalInertia = 0.0f;

    // Get a reference point for forming triangles.
    // Use the first vertex to reduce round-off errors.
    SqVec2 r = vertices[0];

    const float inv3 = 1.0f / 3.0f;

    for (int i = 1; i < count - 1; ++i)
    {
        // Triangle edges
        SqVec2 e1 = SqVec2::Sub(vertices[i], r);
        SqVec2 e2 = SqVec2::Sub(vertices[i + 1], r);

        float D = SqVec2::Cross(e1, e2);

        float triangleArea = 0.5f * D;
        area += triangleArea;

        // Area weighted centroid, r at origin
        center = SqVec2::MulAdd(center, triangleArea * inv3, SqVec2::Add(e1, e2));

        float ex1 = e1.x, ey1 = e1.y;
        float ex2 = e2.x, ey2 = e2.y;

        float intx2 = ex1 * ex1 + ex2 * ex1 + ex2 * ex2;
        float inty2 = ey1 * ey1 + ey2 * ey1 + ey2 * ey2;

        rotationalInertia += (0.25f * inv3 * D) * (intx2 + inty2);
    }

    SqMassData massData;

    // Total mass
    massData.mass = density * area;

    // Center of mass, shift back from origin at r
    SQ_ASSERT(area > FLT_EPSILON);
    float invArea = 1.0f / area;
    center.x *= invArea;
    center.y *= invArea;
    massData.center = SqVec2::Add(r, center);

    // Inertia tensor relative to the local origin (point s).
    massData.rotationalInertia = density * rotationalInertia;

    // Shift to center of mass then to original body origin.
    massData.rotationalInertia += massData.mass * (SqVec2::Dot(massData.center, massData.center) - SqVec2::Dot(center, center));

    return massData;
}

SqShapeExtent SqPolygonShape::computeShapeExtent(const SqVec2 &localCenter)
{
    float minExtent = SqConfig::getInstance()->getHugValue();
    float maxExtentSqr = 0.0f;
    for (int i = 0; i < count; ++i)
    {
        const SqVec2 &v = vertices[i];
        float planeOffset = SqVec2::Dot(normals[i], SqVec2::Sub(v, centroid));
        minExtent = min(minExtent, planeOffset);

        float distanceSqr = SqVec2::LengthSquared(SqVec2::Sub(v, localCenter));
        maxExtentSqr = max(maxExtentSqr, distanceSqr);
    }

    SqShapeExtent extent;
    extent.minExtent = minExtent + radius;
    extent.maxExtent = sqrtf(maxExtentSqr) + radius;
    return extent;
}

SqVec2 SqPolygonShape::getCentroid()
{
    return centroid;
}

void SqPolygonShape::RayCastShapeImpl(SqCastOutput &output, const SqShapeCastInput &input, const SqTransform &transform) const
{
    SqShapeCastPairInput pairInput;
    pairInput.proxyA = sqMakeProxy(vertices, count, radius);
    pairInput.proxyB = input.proxy;
    pairInput.translationB = input.translation;
    pairInput.maxFraction = input.maxFraction;
    pairInput.canEncroach = input.canEncroach;
    sqShapeCast(output, &pairInput);
}

void SqPolygonShape::RayCastImpl(SqCastOutput &output, const SqRayCastInput &input, const SqTransform &transform) const
{
    SQ_ASSERT(sqIsValidRay(&input));

    if (this->radius == 0.0f)
    {
        // Shift all math to first vertex since the polygon may be far
        // from the origin.
        const SqVec2& base = vertices[0];

        SqVec2 p1 = SqVec2::Sub(input.origin, base);
        SqVec2 d = input.translation;

        float lower = 0.0f, upper = input.maxFraction;

        int index = -1;
        
        output.fraction = 0;

        for (int edgeIndex = 0; edgeIndex < this->count; ++edgeIndex)
        {
            // p = p1 + a * d
            // dot(normal, p - v) = 0
            // dot(normal, p1 - v) + a * dot(normal, d) = 0
            SqVec2 vertex = SqVec2::Sub(this->vertices[edgeIndex], base);
            float numerator = SqVec2::Dot(this->normals[edgeIndex], SqVec2::Sub(vertex, p1));
            float denominator = SqVec2::Dot(this->normals[edgeIndex], d);

            if (denominator == 0.0f)
            {
                // Parallel and runs outside edge
                if (numerator < 0.0f)
                {
                    return;
                }
            }
            else
            {
                // Note: we want this predicate without division:
                // lower < numerator / denominator, where denominator < 0
                // Since denominator < 0, we have to flip the inequality:
                // lower < numerator / denominator <==> denominator * lower > numerator.
                if (denominator < 0.0f && numerator < lower * denominator)
                {
                    // Increase lower.
                    // The segment enters this half-space.
                    lower = numerator / denominator;
                    index = edgeIndex;
                }
                else if (denominator > 0.0f && numerator < upper * denominator)
                {
                    // Decrease upper.
                    // The segment exits this half-space.
                    upper = numerator / denominator;
                }
            }

            if (upper < lower)
            {
                // Ray misses
                return;
            }
        }

        SQ_ASSERT(0.0f <= lower && lower <= input.maxFraction);

        if (index >= 0)
        {
            output.fraction = lower;
            output.normal = this->normals[index];
            output.point = SqVec2::MulAdd(input.origin, lower, d);
            output.hit = true;
        }
        else
        {
            // initial overlap
            output.point = input.origin;
            output.hit = true;
        }
        return;
    }

    SqShapeCastPairInput castInput;
    castInput.proxyA = sqMakeProxy(vertices, count, radius);
    castInput.proxyB = sqMakeProxy(&input.origin, 1, 0.0f);
    castInput.translationB = input.translation;
    castInput.maxFraction = input.maxFraction;
    castInput.canEncroach = false;
    sqShapeCast(output, &castInput);
}

void SqPolygonShape::CollideMoverImpl(SqPlaneResult &result, SqCapsuleShape *mover) const
{
    SqDistanceInput distanceInput;
    distanceInput.proxyA = sqMakeProxy(vertices, count, radius);

    SqVec2 points[2] = {mover->center1, mover->center2};
    distanceInput.proxyB = sqMakeProxy(points, 2, mover->radius);
    distanceInput.useRadii = false;
    distanceInput.transformLocaBVertex = false;

    float totalRadius = mover->radius + this->radius;

    SqSimplexCache cache;
    SqDistanceOutput distanceOutput = sqShapeDistance(&distanceInput, &cache, NULL, 0);

    if (distanceOutput.distance <= totalRadius)
    {
        SqPlane plane = {distanceOutput.normal, totalRadius - distanceOutput.distance};
        result.plane = plane;
        result.point = distanceOutput.pointA;
        result.hit = true;
    }
}

bool SqPolygonShape::PointIn(const SqVec2 &point) const
{
    SqDistanceInput input;
    input.proxyA = sqMakeProxy(vertices, count, 0.0f);
    input.proxyB = sqMakeProxy(&point, 1, 0.0f);
    input.useRadii = false;
    input.transformLocaBVertex = false;

    SqSimplexCache cache;
    SqDistanceOutput output = sqShapeDistance(&input, &cache, NULL, 0);

    return output.distance <= radius;
}

void SqPolygonShape::computeDistance(const SqVec2 &point, const SqTransform &xf, float &outDistance, SqVec2 &outNormal) const
{
    SqVec2 pLocal = SqRot::invRotateVector(xf.q, point - xf.p);
    float maxDistance = -FLT_MAX;
    SqVec2 normalForMaxDistance = pLocal;

    for (int i = 0; i < this->count; ++i)
    {
        float dot = SqVec2::Dot(normals[i], pLocal - vertices[i]);
        if (dot > maxDistance)
        {
            maxDistance = dot;
            normalForMaxDistance = normals[i];
        }
    }

    if (maxDistance > 0)
    {
        SqVec2 minDistance = normalForMaxDistance;
        float minDistance2 = maxDistance * maxDistance;
        for (int i = 0; i < this->count; ++i)
        {
            SqVec2 distance = pLocal - vertices[i];
            float distance2 = SqVec2::LengthSquared(distance);
            if (minDistance2 > distance2)
            {
                minDistance = distance;
                minDistance2 = distance2;
            }
        }

        outDistance = sqrt(minDistance2);
        outNormal = SqRot::transformVector(xf.q, minDistance);
        outNormal = SqVec2::Normalize(outNormal);
    }
    else
    {
        outDistance = maxDistance;
        outNormal = SqRot::transformVector(xf.q, normalForMaxDistance);
    }
}
