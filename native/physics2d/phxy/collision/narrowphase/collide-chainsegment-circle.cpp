#include "collide-chainsegment-circle.h"
#include "../../common/SqConfig.h"

using namespace phxy;

SqManifold phxy::sqCollideChainSegmentAndCircle(const SqChainEdgeShape *segmentA, SqTransform &xfA, const SqCircleShape *circleB, SqTransform &xfB)
{
	SqManifold manifold;
	const float speculativeDistance = SqConfig::getInstance()->getSpeculativeDistance();

	SqTransform xf = SqTransform::InvMulTransforms(xfA, xfB);

	// Compute circle in frame of segment
	SqVec2 pB = xf.transformPoint(circleB->center);

	const SqVec2& p1 = segmentA->point1;
	const SqVec2& p2 = segmentA->point2;
	
	SqVec2 e = SqVec2::Sub(p2, p1);

	// Normal points to the right
	float offset = SqVec2::Dot(SqVec2::RightPerp(e), SqVec2::Sub(pB, p1));
	if (offset < 0.0f)
	{
		// collision is one-sided
		return manifold;
	}

	// Barycentric coordinates
	float u = SqVec2::Dot(e, SqVec2::Sub(p2, pB));
	float v = SqVec2::Dot(e, SqVec2::Sub(pB, p1));

	SqVec2 pA;

	if (v <= 0.0f)
	{
		// Behind point1?
		// Is pB in the Voronoi region of the previous edge?
		SqVec2 prevEdge = SqVec2::Sub(p1, segmentA->ghost1);
		float uPrev = SqVec2::Dot(prevEdge, SqVec2::Sub(pB, p1));
		if (uPrev <= 0.0f)
		{
			return manifold;
		}

		pA = p1;
	}
	else if (u <= 0.0f)
	{
		// Ahead of point2?
		SqVec2 nextEdge = SqVec2::Sub(segmentA->ghost2, p2);
		float vNext = SqVec2::Dot(nextEdge, SqVec2::Sub(pB, p2));

		// Is pB in the Voronoi region of the next edge?
		if (vNext > 0.0f)
		{
			return manifold;
		}

		pA = p2;
	}
	else
	{
		float ee = SqVec2::Dot(e, e);
		pA = SqVec2{u * p1.x + v * p2.x, u * p1.y + v * p2.y};
		pA = ee > 0.0f ? SqVec2::MulSV(1.0f / ee, pA) : p1;
	}

	float distance;
	SqVec2 normal = SqVec2::GetLengthAndNormalize(&distance, SqVec2::Sub(pB, pA));

	float radius = circleB->radius;
	float separation = distance - radius;
	if (separation > speculativeDistance)
	{
		return manifold;
	}

	SqVec2 cA = pA;
	SqVec2 cB = SqVec2::MulAdd(pB, -radius, normal);
	SqVec2 contactPointA = SqVec2::Lerp(cA, cB, 0.5f);

	manifold.normal = xfA.transformVector(normal);

	SqManifoldPoint *mp = manifold.points + 0;
	mp->anchorA = xfA.transformVector(contactPointA);
	mp->anchorB = SqVec2::Add(mp->anchorA, SqVec2::Sub(xfA.p, xfB.p));
	mp->point = SqVec2::Add(xfA.p, mp->anchorA);
	mp->separation = separation;
	mp->id = 0;
	manifold.pointCount = 1;
	return manifold;
}
