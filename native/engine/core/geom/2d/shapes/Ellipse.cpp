#include "Ellipse.h"
#include "../../curve/bezier2D.h"
#include "../misc/line2D.h"
#include <stdio.h>
#include "../../../sqstd/StackTempArenaAllocator.h"

using namespace d2;

#define KAPPA90 0.5522847493

Ellipse::Ellipse(float x, float y, float radiusWidth, float radiusHeight)
    : ShapePrimitive(ShapeType::ELLIPSE),
      x(x), y(y), radiusWidth(radiusWidth), radiusHeight(radiusHeight)
{
}

Ellipse::Ellipse() : ShapePrimitive(ShapeType::ELLIPSE)
{
    x = 0;
    y = 0;
    radiusWidth = 0;
    radiusHeight = 0;
}

void Ellipse::set(float x, float y, float radiusX, float radiusY)
{
    this->x = x;
    this->y = y;
    this->radiusWidth = radiusX;
    this->radiusHeight = radiusY;
}

void set(float x, float y, float radiusX, float radiusY);

void Ellipse::build(sqstd::Array<Vec2> &points)
{
    points.push(Vec2(x - radiusWidth, y));

    Vec2 &last = *points.getLast();

    // printf("==============fff %d %f %f \n",points.size(),last.x,last.y);

    casteljauBerzier2D(
        last.x, last.y,
        x - radiusWidth, y + radiusHeight * KAPPA90,
        x - radiusWidth * KAPPA90, y + radiusHeight,
        x, y + radiusHeight,
        0, points);

    Vec2 &last1 = *points.getLast();
    casteljauBerzier2D(
        last1.x, last1.y,
        x + radiusWidth * KAPPA90, y + radiusHeight,
        x + radiusWidth, y + radiusHeight * KAPPA90,
        x + radiusWidth, y,
        0, points);

    Vec2 &last2 = *points.getLast();
    casteljauBerzier2D(
        last2.x, last2.y,
        x + radiusWidth, y - radiusHeight * KAPPA90,
        x + radiusWidth * KAPPA90, y - radiusHeight,
        x, y - radiusHeight,
        0, points);

    Vec2 &last3 = *points.getLast();
    casteljauBerzier2D(
        last3.x, last3.y,
        x - radiusWidth * KAPPA90, y - radiusHeight,
        x - radiusWidth, y - radiusHeight * KAPPA90,
        x - radiusWidth, y,
        0, points);
}

void Ellipse::fill(sqstd::Array<Vec2> &points, sqstd::Array<unsigned short> &triangles)
{
    int vertexOffset = points.getCount();
    build(points);
    for (int start = vertexOffset + 2, end = vertexOffset + points.getCount(); start < end; start++)
    {
        triangles.push(vertexOffset);
        triangles.push(start);
        triangles.push(start - 1);
    }
}

void Ellipse::stroke(sqstd::Array<Vec2> &points, sqstd::Array<unsigned short> &triangle, float strokeWidth)
{
    sqstd::Array<Vec2> vertexPoints(sqstd::StackTempArenaAllocator::getInstance());
    build(vertexPoints);
    d2::buildLine(vertexPoints, points, triangle, strokeWidth, true);
}

void Ellipse::getBounds(Bound2 &out)
{
    out.minX = x - radiusWidth;
    out.minY = y - radiusHeight;
    out.maxX = x + radiusWidth;
    out.maxY = y + radiusHeight;
}

bool Ellipse::contains(float x, float y)
{
    if (radiusWidth <= 0 || radiusHeight <= 0)
    {
        return false;
    }

    // normalize the coords to an ellipse with center 0,0
    float normx = ((x - this->x) / radiusWidth);
    float normy = ((y - this->y) / radiusHeight);

    normx *= normx;
    normy *= normy;
    return (normx + normy <= 1);
}

bool Ellipse::strokeContains(float x, float y, float strokeWidth)
{

    if (radiusWidth <= 0 || radiusHeight <= 0)
    {
        return false;
    }

    float halfStrokeWidth = strokeWidth / 2;
    float innerA = radiusWidth - halfStrokeWidth;
    float innerB = radiusHeight - halfStrokeWidth;
    float outerA = radiusWidth + halfStrokeWidth;
    float outerB = radiusHeight + halfStrokeWidth;

    float normalizedX = x - this->x;
    float normalizedY = y - this->y;

    float innerEllipse = ((normalizedX * normalizedX) / (innerA * innerA)) + ((normalizedY * normalizedY) / (innerB * innerB));
    float outerEllipse = ((normalizedX * normalizedX) / (outerA * outerA)) + ((normalizedY * normalizedY) / (outerB * outerB));

    return innerEllipse > 1 && outerEllipse <= 1;
}

void Ellipse::queryFill(float x, float y, d2::QueryShapeResult &result)
{
    result.gemoetryIndex = -1;
    if (contains(x, y))
    {
        result.gemoetryIndex = 0;
    }
}

void Ellipse::queryStroke(float x, float y, d2::QueryShapeResult &result, float strokeWidth)
{
    result.gemoetryIndex = -1;
    if (strokeContains(x, y, strokeWidth))
    {
        result.gemoetryIndex = 0;
    }
}