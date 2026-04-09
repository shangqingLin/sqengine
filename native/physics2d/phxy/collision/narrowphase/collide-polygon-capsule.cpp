#include "collide-polygon-capsule.h"
#include "collide-polygon-polygon.h"

using namespace phxy;

SqManifold phxy::sqCollidePolygonAndCapsule(const SqPolygonShape *polygonA, SqTransform &xfA, const SqCapsuleShape *capsuleB, SqTransform &xfB)
{
	SqPolygonShape polygonShape;
	polygonShape.makeFromCapsule(capsuleB->center1, capsuleB->center2, capsuleB->radius);
	return sqCollidePolygons(polygonA, xfA, &polygonShape, xfB);
}