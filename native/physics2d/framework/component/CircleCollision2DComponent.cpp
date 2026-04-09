#include "CircleCollision2DComponent.h"
#include "../PhysicsSystem.h"

using namespace physics2d;

CircleCollision2DComponent::CircleCollision2DComponent() : Collision2DComponent()
{
}

void CircleCollision2DComponent::setOffset(float x, float y)
{

    if (shape)
    {
        phxy::SqCircleShape *circleShape = static_cast<phxy::SqCircleShape *>(shape);
        circleShape->setCircle(phxy::SqVec2(x, y), circleShape->radius);
    }
    else
    {
        Collision2DComponent::setOffset(x, y);
    }
}

void CircleCollision2DComponent::setRadius(float radius)
{

    phxy::SqCircleShapeDef eDef;
    eDef.type = phxy::SqShapeType::sq_circleShape;
    copyBaseDef(eDef);

    float centerX = 0, centerY = 0;
    if (offset.has_value())
    {
        centerX = offset.value().x;
        centerY = offset.value().y;
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
    eDef.center.x = centerX * sx;
    eDef.center.y = centerY * sy;
    eDef.radius = radius * (sx < sy ? sx : sy);

    // printf("create circle %d bodyId %d scale %f %f radius %f %f \n", node->nativeId,bodyId, sx, sy,radius,eDef.radius);

    shape = PhysicsSystem::getInstance()->getWorld()->createShape(bodyId, eDef);
}

float CircleCollision2DComponent::getRadius()
{
    return shape ? ((phxy::SqCircleShape *)shape)->radius : 0.f;
}