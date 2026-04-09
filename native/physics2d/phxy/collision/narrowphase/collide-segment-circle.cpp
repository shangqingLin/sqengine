#include "collide-segment-circle.h"
#include "collide-capsule-circle.h"
#include "../../geometry/shape/SqCapsuleShape.h"

using namespace phxy;

SqManifold phxy::sqCollideSegmentAndCircle(const SqEdgeShape *segmentA, SqTransform &xfA, const SqCircleShape *circleB, SqTransform &xfB)
{
	SqCapsuleShape capsuleA;
	capsuleA.setCapsule(segmentA->point1, segmentA->point2, 0.0f);
	return sqCollideCapsuleAndCircle(&capsuleA, xfA, circleB, xfB);
}