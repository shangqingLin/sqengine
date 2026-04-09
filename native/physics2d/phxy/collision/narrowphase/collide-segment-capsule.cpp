#include "collide-segment-capsule.h"
#include "collide-capsule-capsule.h"
using namespace phxy;

SqManifold phxy::sqCollideSegmentAndCapsule( const SqEdgeShape* segmentA, SqTransform xfA, const SqCapsuleShape* capsuleB, SqTransform xfB )
{
	SqCapsuleShape capsuleA;
	capsuleA.setCapsule( segmentA->point1, segmentA->point2, 0.0f );
	return sqCollideCapsules( &capsuleA, xfA, capsuleB, xfB );
}