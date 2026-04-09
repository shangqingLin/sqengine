#include "Capsule.h"
#include "../../curve/bezier2D.h"
#include "../misc/line2D.h"
#include "../../../math/math.h"
#include "../../../sqstd/StackTempArenaAllocator.h"
using namespace d2;

#define KAPPA90 0.5522847493

Capsule::Capsule(float c1x, float c1y, float c2x, float c2y, float radius)
    : radius(radius),
      ShapePrimitive(ShapeType::CAPUSLE)
{
    set(c1x, c2y, c2x, c2y, radius);
}

Capsule::Capsule() : ShapePrimitive(ShapeType::CAPUSLE)
{
    radius = 0.f;
}

void Capsule::set(float c1x, float c1y, float c2x, float c2y, float radius)
{
    this->radius = radius;
    
    // 计算胶囊体的朝向，要保证胶囊体上下是半圆,向外凸
    axisUp.set(c2x - c1x, c2y - c1y);
    float angle = Math::getAngle(axisUp.x, axisUp.y);

    if (angle > 180 || angle == 0)
    {
        upperCenterX = c1x;
        upperCenterY = c1y;
        lowerCenterX = c2x;
        lowerCenterY = c2y;
        axisLeftNormal.x = axisUp.y;
        axisLeftNormal.y = -axisUp.x;
        axisUp.x = -axisUp.x;
        axisUp.y = -axisUp.y;
    }
    else if (angle > 0 & angle <= 180)
    {
        upperCenterX = c2x;
        upperCenterY = c2y;
        lowerCenterX = c1x;
        lowerCenterY = c1y;
        axisLeftNormal.x = -axisUp.y;
        axisLeftNormal.y = axisUp.x;
    }

    axisLeftNormal.normalize();
    axisUp.normalize();
}

void Capsule::build(sqstd::Array<Vec2> &points)
{

    Vec2 startPoint;
    Vec2 endPoint;

    float radiusK = radius * KAPPA90;

    // 按照顺时针生成的点

    // 左上角
    startPoint.set(upperCenterX + axisLeftNormal.x * radius, upperCenterY + axisLeftNormal.y * radius);
    points.push(startPoint);
    endPoint.set(upperCenterX + axisUp.x * radius, upperCenterY + axisUp.y * radius);
    casteljauBerzier2D(
        startPoint.x, startPoint.y,
        startPoint.x + axisUp.x * radiusK, startPoint.y + axisUp.y * radiusK,
        endPoint.x + axisLeftNormal.x * radiusK, endPoint.y + axisLeftNormal.y * radiusK,
        endPoint.x, endPoint.y,
        0, points);

    startPoint = *points.get(points.getCount() - 1);
    endPoint.set(upperCenterX - axisLeftNormal.x * radius, upperCenterY - axisLeftNormal.y * radius);

    // 右上角
    casteljauBerzier2D(
        startPoint.x, startPoint.y,
        startPoint.x - axisLeftNormal.x * radiusK, startPoint.y - axisLeftNormal.y * radiusK,
        endPoint.x + axisUp.x * radiusK, endPoint.y + axisUp.y * radiusK,
        endPoint.x, endPoint.y,
        0, points);

    startPoint.set(lowerCenterX - axisLeftNormal.x * radius, lowerCenterY - axisLeftNormal.y * radius);
    points.push(startPoint);

    endPoint.set(lowerCenterX - axisUp.x * radius, lowerCenterY - axisUp.y * radius);

    // 右下角
    casteljauBerzier2D(
        startPoint.x, startPoint.y,
        startPoint.x - axisUp.x * radiusK, startPoint.y - axisUp.y * radiusK,
        endPoint.x - axisLeftNormal.x * radiusK, endPoint.y - axisLeftNormal.y * radiusK,
        endPoint.x, endPoint.y,
        0, points);

    startPoint = *points.get(points.getCount() - 1);
    endPoint.set(lowerCenterX + axisLeftNormal.x * radius, lowerCenterY + axisLeftNormal.y * radius);

    // 左下角
    casteljauBerzier2D(
        startPoint.x, startPoint.y,
        startPoint.x + axisLeftNormal.x * radiusK, startPoint.y + axisLeftNormal.y * radiusK,
        endPoint.x - axisUp.x * radiusK, endPoint.y - axisUp.y * radiusK,
        endPoint.x, endPoint.y,
        0, points);
}

void Capsule::fill(sqstd::Array<Vec2> &points, sqstd::Array<unsigned short> &triangles)
{
    int vertexOffset = points.getCount();
    build(points);
    for (int start = vertexOffset + 2, end = points.getCount(); start < end; start++)
    {
        triangles.push(vertexOffset);
        triangles.push(start);
        triangles.push(start - 1);
    }
}

void Capsule::stroke(sqstd::Array<Vec2> &points, sqstd::Array<unsigned short> &triangle, float strokeWidth)
{
    sqstd::Array<Vec2> circlePoints(sqstd::StackTempArenaAllocator::getInstance());
    build(circlePoints);
    d2::buildLine(circlePoints, points, triangle, strokeWidth, true);
}

bool Capsule::strokeContains(float x, float y, float strokeWidth)
{
    return false;
}

bool Capsule::contains(float x, float y)
{
    return false;
}

void Capsule::queryFill(float x, float y, d2::QueryShapeResult &)
{
}

void Capsule::queryStroke(float x, float y, d2::QueryShapeResult &, float)
{
}

void Capsule::getBounds(Bound2 &out)
{
}