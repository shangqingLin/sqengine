#include "Circle.h"
#include "../../curve/bezier2D.h"
#include <cmath>
#include "../misc/line2D.h"
#include "../../../sqstd/StackTempArenaAllocator.h"

using namespace d2;

#define KAPPA90 0.5522847493

Circle::Circle(float x, float y, float radius)
    : x(x), y(y), radius(radius),
      ShapePrimitive(ShapeType::CIRCLEC)
{
}

Circle::Circle() : ShapePrimitive(ShapeType::CIRCLEC)
{
    set(0.f, 0.f, 0.f);
}

void Circle::set(float x, float y, float radius)
{
    this->x = x;
    this->y = y;
    this->radius = radius;
}

void Circle::build(sqstd::Array<Vec2> &points)
{
    points.push(Vec2(x - radius, y));

    Vec2 &last = *points.getLast();

    // 圆分为4份绘制

    // 左上角
    casteljauBerzier2D(
        last.x, last.y,
        x - radius, y + radius * KAPPA90,
        x - radius * KAPPA90, y + radius,
        x, y + radius,
        0, points);

    Vec2 &last1 = *points.getLast();
    // 右上角
    casteljauBerzier2D(
        last1.x, last1.y,
        x + radius * KAPPA90, y + radius,
        x + radius, y + radius * KAPPA90,
        x + radius, y,
        0, points);

    Vec2 &last2 = *points.getLast();
    // 右下角
    casteljauBerzier2D(
        last2.x, last2.y,
        x + radius, y - radius * KAPPA90,
        x + radius * KAPPA90, y - radius,
        x, y - radius,
        0, points);

    Vec2 &last3 = *points.getLast();
    // 左下角
    casteljauBerzier2D(
        last3.x, last3.y,
        x - radius * KAPPA90, y - radius,
        x - radius, y - radius * KAPPA90,
        x - radius, y,
        0, points);
}

void Circle::fill(sqstd::Array<Vec2> &points, sqstd::Array<unsigned short> &triangles)
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

void Circle::stroke(sqstd::Array<Vec2> &points, sqstd::Array<unsigned short> &triangle, float strokeWidth)
{
    sqstd::Array<Vec2> circlePoints(sqstd::StackTempArenaAllocator::getInstance());
    build(circlePoints);
    d2::buildLine(circlePoints, points, triangle, strokeWidth, true);
}

void Circle::getBounds(Bound2 &out)
{
    out.minX = x - radius;
    out.minY = y - radius;
    out.maxX = x + radius;
    out.maxY = y + radius;
}

bool Circle::contains(float x, float y)
{
    if (radius <= 0)
        return false;

    float r2 = radius * radius;
    float dx = (this->x - x);
    float dy = (this->y - y);

    dx *= dx;
    dy *= dy;

    return (dx + dy <= r2);
}

bool Circle::strokeContains(float x, float y, float strokeWidth)
{
    if (radius == 0)
        return false;

    float dx = (this->x - x);
    float dy = (this->y - y);
    float r = radius;
    float w2 = strokeWidth / 2;
    float distance = sqrt((dx * dx) + (dy * dy));

    return (distance < r + w2 && distance > r - w2);
}

void Circle::queryFill(float x, float y, d2::QueryShapeResult &result)
{
    result.gemoetryIndex = -1;
    if (contains(x, y))
    {
        result.gemoetryIndex = 0;
    }
}

void Circle::queryStroke(float x, float y, d2::QueryShapeResult &result, float strokeWidth)
{
    result.gemoetryIndex = -1;
    if (strokeContains(x, y, strokeWidth))
    {
        result.gemoetryIndex = 0;
    }
}