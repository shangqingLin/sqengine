#include "collide-chainsegment-capsule.h"
#include "../../geometry/shape/SqPolygonShape.h"
#include "collide-chainsegment-polygon.h"

using namespace phxy;

SqManifold phxy::sqCollideChainSegmentAndCapsule(const SqChainEdgeShape *segmentA, SqTransform &xfA, const SqCapsuleShape *capsuleB,
										   SqTransform &xfB, SqSimplexCache *cache)
{

	SqPolygonShape polygonShape;
	polygonShape.makeFromCapsule(capsuleB->center1, capsuleB->center2, capsuleB->radius);
	return sqCollideChainSegmentAndPolygon(segmentA, xfA, &polygonShape, xfB, cache);
}