#include "collide-segment-polygon.h"
#include "collide-polygon-polygon.h"

using namespace phxy;

SqManifold phxy::sqCollideSegmentAndPolygon(const SqEdgeShape *segmentA, SqTransform &xfA, const SqPolygonShape *polygonB, SqTransform &xfB)
{
	SqPolygonShape polygonA;
	polygonA.makeFromCapsule(segmentA->point1, segmentA->point2, 0.0f);
	return sqCollidePolygons(&polygonA, xfA, polygonB, xfB);
}