#include "collision.h"
#include "../../common/math/SqMath.h"
#include "../../geometry/shape/SqCapsuleShape.h"
#include "../../geometry/shape/SqCircleShape.h"
#include "../../geometry/shape/SqEdgeShape.h"
#include "../../geometry/shape/SqPolygonShape.h"
#include "../../geometry/shape/SqChainEdgeShape.h"

#include "../../common/SqCore.h"

using namespace phxy;

phxy::SqShapeProxy phxy::sqMakeProxy(const SqVec2 *points, int count, float radius)
{
    count = min(count, SQ_MAX_POLYGON_VERTICES);
    phxy::SqShapeProxy proxy;
    for (int i = 0; i < count; ++i)
    {
        proxy.points[i] = points[i];
    }
    proxy.count = count;
    proxy.radius = radius;
    return proxy;
}

phxy::SqShapeProxy phxy::sqMakeProxy(const SqShape *shape)
{
    switch (shape->type)
    {
    case sq_capsuleShape:
    {
        SqCapsuleShape *capsule = (SqCapsuleShape *)shape;
        return sqMakeProxy(&capsule->center1, 2, capsule->radius);
    }
    case sq_circleShape:
    {
        SqCircleShape *circle = (SqCircleShape *)shape;
        return sqMakeProxy(&circle->center, 1, circle->radius);
    }
    case sq_polygonShape:
    {
        SqPolygonShape *polygon = (SqPolygonShape *)shape;
        return sqMakeProxy(polygon->vertices, polygon->count, polygon->radius);
    }
    case sq_segmentShape:
    {
        SqEdgeShape *segment = (SqEdgeShape *)shape;
        phxy::SqShapeProxy proxy;
        proxy.count = 2;
        proxy.radius = 0.0f;
        proxy.points[0] = segment->point1;
        proxy.points[1] = segment->point2;
        return proxy;
    }
    case sq_chainSegmentShape:
    {
        SqChainEdgeShape *chainSegment = (SqChainEdgeShape *)shape;
        phxy::SqShapeProxy proxy;
        proxy.count = 2;
        proxy.radius = 0.0f;
        proxy.points[0] = chainSegment->point1;
        proxy.points[1] = chainSegment->point2;
        return proxy;
    }
    default:
    {
        SQ_ASSERT(false);
        SqShapeProxy empty;
        return empty;
    }
    }
}

bool phxy::sqIsValidRay(const SqRayCastInput *input)
{
    bool isValid = SqVec2::isValid(input->origin) && SqVec2::isValid(input->translation) &&
                   isValidFloat(input->maxFraction) && 0.0f <= input->maxFraction && input->maxFraction < SQ_HUGE;
    return isValid;
}