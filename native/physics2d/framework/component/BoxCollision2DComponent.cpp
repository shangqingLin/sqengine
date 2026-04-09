#include "BoxCollision2DComponent.h"
#include "../PhysicsSystem.h"
#include "../../phxy/sq-phxy.h"

using namespace physics2d;

BoxCollision2DComponent::BoxCollision2DComponent() : Collision2DComponent()
{
}

BoxCollision2DComponent::~BoxCollision2DComponent()
{
}

void BoxCollision2DComponent::setOffset(float x, float y)
{
    if (shape)
    {
        phxy::SqPolygonShape *polygon = static_cast<phxy::SqPolygonShape *>(shape);
        polygon->setAsBox(halfWidth, halfHeight, phxy::SqVec2(x, y), phxy::SqRot());
    }
    else
    {
        Collision2DComponent::setOffset(x, y);
    }
}

void BoxCollision2DComponent::create(float width, float height)
{

    phxy::SqBoxShapeDef eDef;
    if (shape != nullptr)
    {
        eDef.type = phxy::SqShapeType::sq_boxShape;
        eDef.material.friction = shape->getFriction();
        eDef.material.restitution = shape->getRestitution();
        eDef.material.tangentSpeed = shape->getTangentSpeed();
        eDef.density = shape->getDensity();
        eDef.filter = shape->getFilter();
        eDef.customColor = shape->getCustomColor();
        eDef.isSensor = shape->isSensor();
        eDef.sensorAABB = shape->isSensorAABB();
        PhysicsSystem::getInstance()->getWorld()->getPyxhWorld()->destroyShape(shape);
    }
    else
    {
        eDef.type = phxy::SqShapeType::sq_boxShape;
        copyBaseDef(eDef);
    }

    if (offset.has_value())
    {
        eDef.center.x = offset.value().x;
        eDef.center.y = offset.value().y;
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

    // printf("create box %d scale %f %f w %f h %f \n", node->nativeId, sx, sy,width,height);

    halfWidth = width;
    halfHeight = height;
    eDef.halfWidth = width * sx;
    eDef.halfHeight = height * sy;
    shape = PhysicsSystem::getInstance()->getWorld()->createShape(bodyId, eDef);
}
