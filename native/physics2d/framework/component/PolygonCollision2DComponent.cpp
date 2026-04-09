#include "PolygonCollision2DComponent.h"
#include "../PhysicsSystem.h"
using namespace physics2d;

PolygonCollision2DComponent::PolygonCollision2DComponent() : Collision2DComponent()
{
}

/**
 * points必须是顺时针排序
 */
void PolygonCollision2DComponent::setPoints(float *points, int numPoint)
{

    phxy::SqPolygonShapeDef eDef;
    eDef.type = phxy::SqShapeType::sq_polygonShape;
    copyBaseDef(eDef);

    if (offset.has_value())
    {
        Vec2 &pos = offset.value();
        if (pos.x != 0 || pos.y != 0)
        {
            int index = 0;
            for (int i = 0; i < numPoint; ++i)
            {
                points[index++] += pos.x;
                points[index++] += pos.y;
            }
        }
        offset.reset();
    }
    eDef.hull.count = numPoint;

    phxy::SqBody *body = PhysicsSystem::getInstance()->getWorld()->getBody(bodyId);
    const Mat3 &worldMat = node->getComponent<Transform2DComponent>()->getWorldTransform();
    float nodeScaleX, nodeScaleY;
    worldMat.getScale(nodeScaleX, nodeScaleY);
    float sx, sy;
    body->getScale(sx, sy);
    if (sx != nodeScaleX || sy != nodeScaleY)
    {
        body->setScale(nodeScaleX, nodeScaleY);
        sx = nodeScaleX;
        sy = nodeScaleY;
    }
    
    int index = 0;
    for (int i = 0; i < numPoint * 2; i += 2)
    {
        eDef.hull.points[index].x = points[i] * sx;
        eDef.hull.points[index].y = points[i + 1] * sy;
        ++index;
    }

    shape = PhysicsSystem::getInstance()->getWorld()->createShape(bodyId, eDef);
}