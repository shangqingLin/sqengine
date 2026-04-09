#include "CapsuleCollision2DComponent.h"
#include "../PhysicsSystem.h"

using namespace physics2d;

CapsuleCollision2DComponent::CapsuleCollision2DComponent() : Collision2DComponent()
{
}

void CapsuleCollision2DComponent::setOffset(float x, float y)
{
    if (shape)
    {
        phxy::SqCapsuleShape *capuse = static_cast<phxy::SqCapsuleShape *>(shape);
        capuse->center1.x = center1.x + x;
        capuse->center1.y = center1.y + y;
        capuse->center2.x = center2.x + x;
        capuse->center2.y = center2.y + y;
    }
    else
    {
        Collision2DComponent::setOffset(x, y);
    }
}

void CapsuleCollision2DComponent::create(float p1x, float p1y, float p2x, float p2y, float radius)
{
    phxy::SqCapusleShapeDef eDef;
    eDef.type = phxy::SqShapeType::sq_capsuleShape;
    copyBaseDef(eDef);

    center1.x = p1x;
    center1.y = p1y;
    center2.x = p2x;
    center2.y = p2y;

    if (offset.has_value())
    {
        Vec2 &pos = offset.value();
        if (pos.x != 0)
        {
            p1x += pos.x;
            p2x += pos.x;
        }

        if (pos.y != 0)
        {
            p1y += pos.y;
            p2y += pos.y;
        }
        offset.reset();
    }

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
    
    eDef.center1.x = p1x * sx;
    eDef.center1.y = p1y * sy;
    eDef.center2.x = p2x * sx;
    eDef.center2.y = p2y * sy;
    eDef.radius = radius * (sx < sy ? sx : sy);
    shape = PhysicsSystem::getInstance()->getWorld()->createShape(bodyId, eDef);
}

CapsuleCollision2DComponent::~CapsuleCollision2DComponent()
{
}
