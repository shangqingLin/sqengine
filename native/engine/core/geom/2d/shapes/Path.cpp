#include "Path.h"
#include "../../curve/bezier2D.h"
#include "./Ellipse.h"
#include "./Rectangle.h"
#include "./Capsule.h"
#include "./Circle.h"

using namespace d2;

ShapePrimitive *Path::getShapeFromCache(ShapeType type)
{
    for (int i = 0; i < cacheShape.getCount(); ++i)
    {
        if (cacheShape[i]->shapeType == type)
        {
            ShapePrimitive *shape = cacheShape[i];
            cacheShape.removeSwap(i);
            return shape;
        }
    }
    return nullptr;
}
void Path::addPoint(float &x, float &y)
{
    _currentPolygonShape->points.push(Vec2(x, y));
}

void Path::moveTo(float x, float y)
{
    dirty = true;
    d2::Polygon *shape = (d2::Polygon *)getShapeFromCache(ShapeType::POLYGON);
    if (!shape)
    {
        shape = new d2::Polygon();
    }
    _currentPolygonShape = shape;
    shapePrimitives.push(_currentPolygonShape);
    addPoint(x, y);
}

void Path::lineTo(float x, float y)
{
    addPoint(x, y);
}

void Path::updateLinePoint(int pointIndex, float x, float y)
{
    d2::Polygon *polygon = dynamic_cast<d2::Polygon *>(shapePrimitives[0]);
    polygon->points[pointIndex].x = x;
    polygon->points[pointIndex].y = y;

    // printf("updateLinePoint %d %f %f \n",pointIndex,x,y);
}

void Path::insertAfterLinePoint(int insertAfterpointIndex, float x, float y)
{
    d2::Polygon *polygon = dynamic_cast<d2::Polygon *>(shapePrimitives[0]);
    polygon->points.insert(insertAfterpointIndex, Vec2(x, y));
}

void Path::deleteLinePoint(int pointIndex)
{
    d2::Polygon *polygon = dynamic_cast<d2::Polygon *>(shapePrimitives[0]);
    polygon->points.removeAt(pointIndex);
}

void Path::endShape(bool closed)
{
    if (_currentPolygonShape)
    {
        _currentPolygonShape->closed = closed;
        _currentPolygonShape = NULL;
    }
}

void Path::close()
{
    endShape(true);
}

void Path::bezierCurveTo(float ctX1, float ctY1, float ctX2, float ctY2, float endX, float endY)
{
    Vec2 &last = *_currentPolygonShape->points.getLast();
    casteljauBerzier2D(last.x, last.y, ctX1, ctY1, ctX2, ctY2, endX, endY, 0, _currentPolygonShape->points);
}

void Path::drawEllipse(float cx, float cy, float rx, float ry)
{
    endShape();
    dirty = true;

    d2::Ellipse *shape = (d2::Ellipse *)getShapeFromCache(ShapeType::ELLIPSE);
    if (!shape)
    {
        shape = new d2::Ellipse();
    }
    shape->set(cx, cy, rx, ry);
    shapePrimitives.push(shape);
}

void Path::drawCircle(float cx, float cy, float r)
{
    endShape();
    dirty = true;
    d2::Circle *shape = (d2::Circle *)getShapeFromCache(ShapeType::CIRCLEC);
    if (!shape)
    {
        shape = new d2::Circle();
    }
    shape->set(cx, cy, r);
    shapePrimitives.push(shape);
}

void Path::drawRect(float x, float y, float w, float h)
{
    endShape();
    dirty = true;

    d2::Rectangle *shape = (d2::Rectangle *)getShapeFromCache(ShapeType::RETANGLE);
    if (!shape)
    {
        shape = new d2::Rectangle();
    }
    shape->set(x, y, w, h);
    shapePrimitives.push(shape);
}

void Path::drawCapsule(float c1x, float c1y, float c2x, float c2y, float radius)
{
    endShape();
    dirty = true;
    d2::Capsule *shape = (d2::Capsule *)getShapeFromCache(ShapeType::CAPUSLE);
    if (!shape)
    {
        shape = new d2::Capsule();
    }
    shape->set(c1x, c1y, c2x, c2y, radius);
    shapePrimitives.push(shape);
}

void Path::build(sqstd::Array<Vec2> &points, sqstd::Array<unsigned short> &triangles, float strokeWidth)
{
    for (int i = 0; i < shapePrimitives.getCount(); ++i)
    {
        if (action == GraphicsPathAction::FILL)
        {
            (*shapePrimitives.get(i))->fill(points, triangles);
        }
        else
        {
            (*shapePrimitives.get(i))->stroke(points, triangles, strokeWidth);
        }
    }
}

void Path::fill()
{
    action = GraphicsPathAction::FILL;
}

void Path::stroke()
{
    action = GraphicsPathAction::STROKE;
}

void Path::fill(FillStyle &fillStyle)
{
    this->fillStyle = fillStyle;
    fill();
}

void Path::stroke(StrokeStyle &strokeStyle)
{
    this->strokeStyle = strokeStyle;
    stroke();
}

void Path::clear()
{
    for (int i = 0; i < shapePrimitives.getCount(); ++i)
    {
        ShapePrimitive *shape = *shapePrimitives.get(i);
        shape->reset();
        cacheShape.push(shape);
    }
    shapePrimitives.clear();
    action = GraphicsPathAction::NONE;
    fillStyle.reset();
    strokeStyle.reset();
}

void Path::getBound(Bound2 &out)
{
    out.set(0, 0, 0, 0);
    Bound2 b;
    for (int i = 0; i < shapePrimitives.getCount(); ++i)
    {
        shapePrimitives[i]->getBounds(b);
        out.addBound(b);
    }
}

bool Path::containsPoint(float x, float y)
{
    for (int i = 0; i < shapePrimitives.getCount(); ++i)
    {
        if (action == GraphicsPathAction::FILL)
        {
            if (shapePrimitives[i]->contains(x, y))
            {
                return true;
            }
        }
        else
        {
            if (shapePrimitives[i]->strokeContains(x, y, 2))
            {
                return true;
            }
        }
    }
    return false;
}

void Path::queryGemoetry(float x, float y, d2::QueryShapeResult &result)
{
    for (int i = 0; i < shapePrimitives.getCount(); ++i)
    {
        if (action == GraphicsPathAction::FILL)
        {
            shapePrimitives[i]->queryFill(x, y, result);
            if (result.gemoetryIndex != -1)
            {
                result.gemoetryIndex = i;
                return;
            }
        }
        else
        {
            shapePrimitives[i]->queryStroke(x, y, result, 2);
            if (result.linePoint1Index != -1)
            {
                result.gemoetryIndex = i;
                return;
            }
        }
    }
}

Path::~Path()
{
    for (int i = 0; i < shapePrimitives.getCount(); ++i)
    {
        delete *shapePrimitives.get(i);
    }
    shapePrimitives.clear();

    for (int i = 0; i < cacheShape.getCount(); ++i)
    {
        delete *cacheShape.get(i);
    }
    cacheShape.clear();
}