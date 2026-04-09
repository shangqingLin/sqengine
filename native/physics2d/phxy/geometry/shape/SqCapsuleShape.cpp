#include "SqCapsuleShape.h"
#include "SqCircleShape.h"
#include "SqPolygonShape.h"
#include "../../common/math/SqMath.h"
#include "../../common/SqConfig.h"
#include "../../collision/narrowphase/shape-cast.h"

#include <float.h>

using namespace phxy;

SqCapsuleShape::SqCapsuleShape()
{
	type = sq_capsuleShape;
}

void SqCapsuleShape::setCapsule(const SqVec2 &center1, const SqVec2 &center2, float radius)
{
	this->center1 = center1;
	this->center2 = center2;
	this->radius = radius;
}

void SqCapsuleShape::scale(float sx,float sy)
{
	this->center1.x *= sx;
	this->center1.y *= sy;
	this->center2.x *= sx;
	this->center2.y *= sx < sy ? sx : sy;
}

SqAABB SqCapsuleShape::computeShapeAABB(const SqTransform &transform) const
{
	SqVec2 v1 = SqTransform::transformPoint(transform, center1);
	SqVec2 v2 = SqTransform::transformPoint(transform, center2);
	SqVec2 r = {radius, radius};
	SqVec2 lower = SqVec2::Sub(SqVec2::Min(v1, v2), r);
	SqVec2 upper = SqVec2::Add(SqVec2::Max(v1, v2), r);
	SqAABB aabb = {lower, upper};
	const float speculativeDistance = SqConfig::getInstance()->getSpeculativeDistance();
	aabb.lowerBound.x -= speculativeDistance;
	aabb.lowerBound.y -= speculativeDistance;
	aabb.upperBound.x += speculativeDistance;
	aabb.upperBound.y += speculativeDistance;
	return aabb;
}

SqMassData SqCapsuleShape::computeShapeMass()
{
	float rr = radius * radius;
	float length = SqVec2::Length(SqVec2::Sub(center1, center2));
	float ll = length * length;

	// 密度乘以圆面积得出胶囊体的上半圆和下半圆的质量
	float circleMass = density * (SQ_PI * radius * radius);

	// 密度乘以面积得出胶囊体的矩形部分的质量
	float boxMass = density * (2.0f * radius * length);

	SqMassData massData;

	// 得出质量
	massData.mass = circleMass + boxMass;

	// 胶囊体质心坐标直接取两个半圆心的连线的中心
	massData.center.x = 0.5f * (center1.x + center2.x);
	massData.center.y = 0.5f * (center1.y + center2.y);

	// two offset half circles, both halves add up to full circle and each half is offset by half length
	// semi-circle centroid = 4 r / 3 pi
	// Need to apply parallel-axis theorem twice:
	// 1. shift semi-circle centroid to origin
	// 2. shift semi-circle to box end
	// m * ((h + lc)^2 - lc^2) = m * (h^2 + 2 * h * lc)
	// See: https://en.wikipedia.org/wiki/Parallel_axis_theorem
	// I verified this formula by computing the convex hull of a 128 vertex capsule

	// half circle centroid
	float lc = 4.0f * radius / (3.0f * SQ_PI);

	// half length of rectangular portion of capsule
	float h = 0.5f * length;

	float circleInertia = circleMass * (0.5f * rr + h * h + 2.0f * h * lc);
	float boxInertia = boxMass * (4.0f * rr + ll) / 12.0f;
	massData.rotationalInertia = circleInertia + boxInertia;

	// inertia about the local origin
	massData.rotationalInertia += massData.mass * SqVec2::Dot(massData.center, massData.center);

	return massData;
}

SqShapeExtent SqCapsuleShape::computeShapeExtent(const SqVec2 &localCenter)
{
	SqShapeExtent extent;
	extent.minExtent = radius;
	SqVec2 c1 = SqVec2::Sub(center1, localCenter);
	SqVec2 c2 = SqVec2::Sub(center2, localCenter);
	extent.maxExtent = sqrtf(max(SqVec2::LengthSquared(c1), SqVec2::LengthSquared(c2))) + radius;
	return extent;
}

SqVec2 SqCapsuleShape::getCentroid()
{
	return SqVec2::Lerp(center1, center2, 0.5f);
}

void SqCapsuleShape::RayCastShapeImpl(SqCastOutput &output, const SqShapeCastInput &input, const SqTransform &transform) const
{
	SqShapeCastPairInput pairInput;
	SqVec2 points[2] = {center1, center2};
	pairInput.proxyA = sqMakeProxy(points, 2, radius);
	pairInput.proxyB = input.proxy;
	pairInput.translationB = input.translation;
	pairInput.maxFraction = input.maxFraction;
	pairInput.canEncroach = input.canEncroach;
	sqShapeCast(output, &pairInput);
}

void SqCapsuleShape::RayCastImpl(SqCastOutput &output, const SqRayCastInput &input, const SqTransform &transform) const
{
	SQ_ASSERT(sqIsValidRay(&input));

	const SqVec2 &v1 = center1;
	const SqVec2 &v2 = center2;

	SqVec2 e = SqVec2::Sub(v2, v1);

	float capsuleLength;
	SqVec2 a = SqVec2::GetLengthAndNormalize(&capsuleLength, e);

	if (capsuleLength < FLT_EPSILON)
	{
		// Capsule is really a circle
		SqCircleShape circle;
		circle.setCircle(v1, this->radius);
		circle.RayCast(output, input, transform);
		return;
	}

	const SqVec2 &p1 = input.origin;
	const SqVec2 &d = input.translation;

	// Ray from capsule start to ray start
	SqVec2 q = SqVec2::Sub(p1, v1);
	float qa = SqVec2::Dot(q, a);

	// Vector to ray start that is perpendicular to capsule axis
	SqVec2 qp = SqVec2::MulAdd(q, -qa, a);

	// Does the ray start within the infinite length capsule?
	if (SqVec2::Dot(qp, qp) < this->radius * this->radius)
	{
		if (qa < 0.0f)
		{
			// start point behind capsule segment
			SqCircleShape circle;
			circle.setCircle(v1, this->radius);
			circle.RayCast(output, input, transform);
			return;
		}

		if (qa > capsuleLength)
		{
			// start point ahead of capsule segment
			SqCircleShape circle;
			circle.setCircle(v1, this->radius);
			circle.RayCast(output, input, transform);
			return;
		}

		// ray starts inside capsule -> no hit
		output.point = input.origin;
		output.hit = true;
		return;
	}

	// Perpendicular to capsule axis, pointing right
	SqVec2 n = {a.y, -a.x};

	float rayLength;
	SqVec2 u = SqVec2::GetLengthAndNormalize(&rayLength, d);

	// Intersect ray with infinite length capsule
	// v1 + radius * n + s1 * a = p1 + s2 * u
	// v1 - radius * n + s1 * a = p1 + s2 * u

	// s1 * a - s2 * u = b
	// b = q - radius * ap
	// or
	// b = q + radius * ap

	// Cramer's rule [a -u]
	float den = -a.x * u.y + u.x * a.y;
	if (-FLT_EPSILON < den && den < FLT_EPSILON)
	{
		// Ray is parallel to capsule and outside infinite length capsule
		return;
	}

	SqVec2 b1 = SqVec2::MulSub(q, radius, n);
	SqVec2 b2 = SqVec2::MulAdd(q, radius, n);

	float invDen = 1.0f / den;

	// Cramer's rule [a b1]
	float s21 = (a.x * b1.y - b1.x * a.y) * invDen;

	// Cramer's rule [a b2]
	float s22 = (a.x * b2.y - b2.x * a.y) * invDen;

	float s2;
	SqVec2 b;
	if (s21 < s22)
	{
		s2 = s21;
		b = b1;
	}
	else
	{
		s2 = s22;
		b = b2;
		n = SqVec2::Neg(n);
	}

	if (s2 < 0.0f || input.maxFraction * rayLength < s2)
	{
		return;
	}

	// Cramer's rule [b -u]
	float s1 = (-b.x * u.y + u.x * b.y) * invDen;

	if (s1 < 0.0f)
	{
		// ray passes behind capsule segment
		SqCircleShape circle;
		circle.setCircle(v1, this->radius);
		circle.RayCast(output, input, transform);
	}
	else if (capsuleLength < s1)
	{
		// ray passes ahead of capsule segment
		SqCircleShape circle;
		circle.setCircle(v1, this->radius);
		circle.RayCast(output, input, transform);
	}
	else
	{
		// ray hits capsule side
		output.fraction = s2 / rayLength;
		output.point = SqVec2::Add(SqVec2::Lerp(v1, v2, s1 / capsuleLength), SqVec2::MulSV(this->radius, n));
		output.normal = n;
		output.hit = true;
	}
}

void SqCapsuleShape::CollideMoverImpl(SqPlaneResult &result, SqCapsuleShape *mover) const
{
	SqDistanceInput distanceInput;
	SqVec2 points[2] = {center1, center2};
	distanceInput.proxyA = sqMakeProxy(points, 2, 0.0f);

	points[0] = mover->center1;
	points[1] = mover->center2;
	distanceInput.proxyB = sqMakeProxy(points, 2, mover->radius);
	distanceInput.useRadii = false;
	distanceInput.transformLocaBVertex = false;

	float totalRadius = mover->radius + this->radius;

	SqSimplexCache cache;
	SqDistanceOutput distanceOutput = sqShapeDistance(&distanceInput, &cache, NULL, 0);

	if (distanceOutput.distance <= totalRadius)
	{
		SqPlane plane = {distanceOutput.normal, totalRadius - distanceOutput.distance};
		{
			result.plane = plane;
			result.point = distanceOutput.pointA;
			result.hit = true;
		}
	}
}

bool SqCapsuleShape::PointIn(const SqVec2 &point) const
{
	float rr = radius * radius;
	const SqVec2 &p1 = center1;
	const SqVec2 &p2 = center2;

	SqVec2 d = SqVec2::Sub(p2, p1);
	float dd = SqVec2::Dot(d, d);
	if (dd == 0.0f)
	{
		// Capsule is really a circle
		return SqVec2::DistanceSquared(point, p1) <= rr;
	}

	// Get closest point on capsule segment
	// c = p1 + t * d
	// dot(point - c, d) = 0
	// dot(point - p1 - t * d, d) = 0
	// t = dot(point - p1, d) / dot(d, d)
	float t = SqVec2::Dot(SqVec2::Sub(point, p1), d) / dd;
	t = clamp(t, 0.0f, 1.0f);
	SqVec2 c = SqVec2::MulAdd(p1, t, d);

	// Is query point within radius around closest point?
	return SqVec2::DistanceSquared(point, c) <= rr;
}

void SqCapsuleShape::computeDistance(const SqVec2 &point, const SqTransform &xf, float &outDistance, SqVec2 &outNormal) const
{
	SqPolygonShape polygon;
	polygon.makeFromCapsule(center1, center2, radius);
	polygon.computeDistance(point, xf, outDistance, outNormal);
}