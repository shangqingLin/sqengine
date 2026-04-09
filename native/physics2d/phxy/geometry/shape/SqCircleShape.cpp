#include "SqCircleShape.h"
#include "SqCapsuleShape.h"
#include "../../common/SqConfig.h"
#include "../../common/math/SqMath.h"
#include "../../collision/narrowphase/shape-cast.h"

using namespace phxy;

SqCircleShape::SqCircleShape()
{
    type = sq_circleShape;
    center = SqVec2(0.0f, 0.0f);
    radius = 0.0f;
}

void SqCircleShape::setCircle(const SqVec2 &center, float radius)
{
    this->center = center;
    this->radius = radius;
}

void SqCircleShape::scale(float sx, float sy)
{
    this->center.x *= sx;
    this->center.y *= sy;
    this->radius *= sx < sy ? sx : sy;
    // printf(" SqCircleShape::scale %p %f %f radius %f \n", this, sx, sy,  this->radius);
}

SqAABB SqCircleShape::computeShapeAABB(const SqTransform &xf) const
{
    SqVec2 p = SqTransform::transformPoint(xf, center);
    float r = radius;

    // printf(" SqCircleShape::computeShapeAABB %p %f \n", this, this->radius);

    SqAABB aabb = {{p.x - r, p.y - r}, {p.x + r, p.y + r}};
    const float speculativeDistance = SqConfig::getInstance()->getSpeculativeDistance();
    aabb.lowerBound.x -= speculativeDistance;
    aabb.lowerBound.y -= speculativeDistance;
    aabb.upperBound.x += speculativeDistance;
    aabb.upperBound.y += speculativeDistance;
    return aabb;
}

SqMassData SqCircleShape::computeShapeMass()
{
    float rr = radius * radius;

    SqMassData massData;
    massData.mass = density * SQ_PI * rr;
    massData.center = center;

    // inertia about the local origin
    massData.rotationalInertia = massData.mass * (0.5f * rr + SqVec2::Dot(center, center));
    return massData;
}

SqShapeExtent SqCircleShape::computeShapeExtent(const SqVec2 &localCenter)
{
    SqShapeExtent extent;
    extent.minExtent = radius;
    extent.maxExtent = SqVec2::Length(SqVec2::Sub(center, localCenter)) + radius;
    return extent;
}

SqVec2 SqCircleShape::getCentroid()
{
    return center;
}

void SqCircleShape::RayCastShapeImpl(SqCastOutput &output, const SqShapeCastInput &input, const SqTransform &transform) const
{
    SqShapeCastPairInput pairInput;
    pairInput.proxyA = sqMakeProxy(&center, 1, radius);
    pairInput.proxyB = input.proxy;
    pairInput.translationB = input.translation;
    pairInput.maxFraction = input.maxFraction;
    pairInput.canEncroach = input.canEncroach;
    sqShapeCast(output, &pairInput);
}

void SqCircleShape::RayCastImpl(SqCastOutput &output, const SqRayCastInput &input, const SqTransform &transform) const
{
    SQ_ASSERT(sqIsValidRay(&input));

    const SqVec2 &p = center;

    // Shift ray so circle center is the origin
    SqVec2 s = SqVec2::Sub(input.origin, p);

    float rr = this->radius * this->radius;

    float length;
    SqVec2 d = SqVec2::GetLengthAndNormalize(&length, input.translation);
    if (length == 0.0f)
    {
        // zero length ray

        if (SqVec2::LengthSquared(s) < rr)
        {
            // initial overlap
            output.point = input.origin;
            output.hit = true;
        }
        return;
    }

    // Find closest point on ray to origin

    // solve: dot(s + t * d, d) = 0
    float t = -SqVec2::Dot(s, d);

    // c is the closest point on the line to the origin
    SqVec2 c = SqVec2::MulAdd(s, t, d);

    float cc = SqVec2::Dot(c, c);

    if (cc > rr)
    {
        // closest point is outside the circle
        return;
    }

    // Pythagoras
    float h = sqrtf(rr - cc);

    float fraction = t - h;

    if (fraction < 0.0f || input.maxFraction * length < fraction)
    {
        // intersection is point outside the range of the ray segment

        if (SqVec2::LengthSquared(s) < rr)
        {
            // initial overlap
            output.point = input.origin;
            output.hit = true;
        }
        return;
    }

    // hit point relative to center
    SqVec2 hitPoint = SqVec2::MulAdd(s, fraction, d);

    output.fraction = fraction / length;
    output.normal = SqVec2::Normalize(hitPoint);
    output.point = SqVec2::MulAdd(p, this->radius, output.normal);
    output.hit = true;
}

void SqCircleShape::CollideMoverImpl(SqPlaneResult &result, SqCapsuleShape *mover) const
{
    SqDistanceInput distanceInput;
    distanceInput.proxyA = sqMakeProxy(&center, 1, 0.0f);

    SqVec2 points[2] = {mover->center1, mover->center2};
    distanceInput.proxyB = sqMakeProxy(points, 2, mover->radius);
    distanceInput.useRadii = false;
    distanceInput.transformLocaBVertex = false;

    float totalRadius = mover->radius + this->radius;

    SqSimplexCache cache = {0};
    SqDistanceOutput distanceOutput = sqShapeDistance(&distanceInput, &cache, NULL, 0);

    if (distanceOutput.distance <= totalRadius)
    {
        SqPlane plane = {distanceOutput.normal, totalRadius - distanceOutput.distance};
        result.plane = plane;
        result.point = distanceOutput.pointA;
        result.hit = true;
    }
}

bool SqCircleShape::PointIn(const SqVec2 &point) const
{
    return SqVec2::DistanceSquared(point, center) <= radius * radius;
}

void SqCircleShape::computeDistance(const SqVec2 &point, const SqTransform &xf, float &outDistance, SqVec2 &outNormal) const
{
    SqVec2 center = xf.p + SqRot::transformVector(xf.q, center);
    SqVec2 d = point - center;
    float d1 = SqVec2::Length(d);
    outDistance = d1 - radius;
    outNormal = 1.0f / d1 * d;
}