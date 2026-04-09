#include "Polygon.h"
#include "../misc/line2D.h"
#include "../misc/earcut2D.h"
#include <stdio.h>
#include "../../../base/config.h"

using namespace d2;

Polygon::Polygon() : ShapePrimitive(ShapeType::POLYGON)
{
}

void Polygon::reset()
{
    ShapePrimitive::reset();
    closed = false;
}

void Polygon::fill(sqstd::Array<Vec2> &vPoints, sqstd::Array<unsigned short> &triangles)
{

    int vertexOffset = vPoints.getCount();

    for (int i = 0; i < points.getCount(); ++i)
    {
        vPoints.push(points[i]);
    }

    earcut(points, triangles, vertexOffset, false);
    SQ_ASSERT(triangles.getCount());
}

void Polygon::stroke(sqstd::Array<Vec2> &resultPoints, sqstd::Array<unsigned short> &triangles, float strokeWidth)
{
    d2::buildLine(points, resultPoints, triangles, strokeWidth, closed);
}

void Polygon::getBounds(Bound2 &out)
{
    for (int i = 0; i < points.getCount(); ++i)
    {
        Vec2 &point = points[i];
        out.addFrame(point.x, point.y);
    }
}

bool Polygon::contains(float x, float y)
{
    bool inside = false;

    // use some raycasting to test hits
    // https://github.com/substack/point-in-polygon/blob/master/index.js
    int count = points.getCount();
    for (int i = 0, j = count - 1; i < count; j = i++)
    {
        Vec2 &p1 = points[i];
        float xi = p1.x;
        float yi = p1.y;

        Vec2 &p2 = points[j];
        float xj = p2.x;
        float yj = p2.y;

        bool intersect = ((yi > y) != (yj > y)) && (x < ((xj - xi) * ((y - yi) / (yj - yi))) + xi);
        if (intersect)
        {
            inside = !inside;
        }
    }
    return inside;
}

bool Polygon::strokeContains(float x, float y, float strokeWidth)
{
    QueryShapeResult result;
    queryStroke(x, y, result, strokeWidth);
    return result.linePoint1Index != -1;
}

void Polygon::queryFill(float x, float y, d2::QueryShapeResult &result)
{
    result.gemoetryIndex = -1;
    if (contains(x, y))
    {
        result.gemoetryIndex = 0;
    }
}

void Polygon::queryStroke(float x, float y, d2::QueryShapeResult &result, float strokeWidth)
{
    // 线段宽度的一半
    float halfStrokeWidth = strokeWidth * 0.5;
    halfStrokeWidth = halfStrokeWidth * halfStrokeWidth;
    int size = points.getCount();
    Vec2 point(x, y);
    for (int i = 0; i < size; ++i)
    {
        Vec2 &point1 = points[i];
        Vec2 &point2 = points[(i + 1) % size];

        // 计算点到直线的距离，如果距离小于等于线段的宽度，那么表示点在线段中
        float dist = squaredDistanceToLineSegment(point, point1, point2);
        if (dist <= halfStrokeWidth)
        {
            result.linePoint1Index = i;
            result.linePoint2Index = (i + 1) % size;
            break;
        }
    }
}
