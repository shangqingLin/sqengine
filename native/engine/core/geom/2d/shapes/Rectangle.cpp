#include "Rectangle.h"
#include "../misc/line2D.h"
#include <stdio.h>
#include "../../../sqstd/StackTempArenaAllocator.h"
using namespace d2;

Rectangle::Rectangle(float x, float y, float w, float h)
    : x(x), y(y), w(w), h(h), ShapePrimitive(ShapeType::RETANGLE)
{
}

Rectangle::Rectangle() : ShapePrimitive(ShapeType::RETANGLE)
{
    set(0.f, 0.f, 0.f, 0.f);
}

void Rectangle::set(float x, float y, float w, float h)
{
    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;
}

void Rectangle::build(sqstd::Array<Vec2> &points)
{

    Vec2 v;
    v.x = x;
    v.y = y;
    points.push(v);

    v.x = x + w;
    v.y = y;
    points.push(v);

    v.x = x + w;
    v.y = y + h;
    points.push(v);

    v.x = x;
    v.y = y + h;
    points.push(v);
}

void Rectangle::stroke(sqstd::Array<Vec2> &points, sqstd::Array<unsigned short> &triangles, float strokeWidth)
{
    sqstd::Array<Vec2> vertexPoints(sqstd::StackTempArenaAllocator::getInstance());
    build(vertexPoints);
    d2::buildLine(vertexPoints, points, triangles, strokeWidth, true);
}

void Rectangle::fill(sqstd::Array<Vec2> &points, sqstd::Array<unsigned short> &triangles)
{
    int vertexOffset = points.getCount();
    triangles.push(vertexOffset);
    triangles.push(vertexOffset + 1);
    triangles.push(vertexOffset + 2);

    triangles.push(vertexOffset);
    triangles.push(vertexOffset + 2);
    triangles.push(vertexOffset + 3);

    build(points);
}

void Rectangle::getBounds(Bound2 &out)
{
    out.set(x, y, x + w, y + h);
}

bool Rectangle::contains(float x, float y)
{
    if (w <= 0 || h <= 0)
    {
        return false;
    }

    if (x >= this->x && x < this->x + w)
    {
        if (y >= this->y && y < this->y + h)
        {
            return true;
        }
    }

    return false;
}

bool Rectangle::strokeContains(float x, float y, float strokeWidth)
{
    if (w <= 0 || h <= 0)
        return false;

    float _x = this->x;
    float _y = this->y;
    float half = strokeWidth / 2;
    float outerLeft = _x - half;
    float outerRight = _x + w + half;
    float outerTop = _y - half;
    float outerBottom = _y + h + half;
    float innerLeft = _x + half;
    float innerRight = _x + w - half;
    float innerTop = _y + half;
    float innerBottom = _y + h - half;

    return (x >= outerLeft && x <= outerRight && y >= outerTop && y <= outerBottom) && !(x > innerLeft && x < innerRight && y > innerTop && y < innerBottom);
}

void Rectangle::queryFill(float x, float y, d2::QueryShapeResult &result)
{
    result.gemoetryIndex = -1;
    if (contains(x, y))
    {
        result.gemoetryIndex = 0;
    }
}

void Rectangle::queryStroke(float x, float y, d2::QueryShapeResult &result, float strokeWidth)
{
    float halfStrokeWidth = strokeWidth * 0.5;
    halfStrokeWidth = halfStrokeWidth * halfStrokeWidth;
    int size = points.getCount();
    Vec2 point(x, y);
    sqstd::Array<Vec2> points(sqstd::StackTempArenaAllocator::getInstance());
    build(points);

    for (int i = 0; i < size; ++i)
    {
        Vec2 &point1 = points[i];
        Vec2 &point2 = points[(i + 1) % size];
        float dist = squaredDistanceToLineSegment(point, point1, point2);
        if (dist <= halfStrokeWidth)
        {
            result.linePoint1Index = i;
            result.linePoint2Index = (i + 1) % size;
            break;
        }
    }
}