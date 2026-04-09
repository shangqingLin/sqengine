#include "ChainShape2DComponent.h"
#include "../PhysicsSystem.h"

using namespace physics2d;

ChainShape2DComponent::ChainShape2DComponent() : Collision2DComponent()
{
}

ChainShape2DComponent::~ChainShape2DComponent()
{
}

void ChainShape2DComponent::setOneSided(bool b)
{
    if (shape)
    {
        // B2ChainShape *sshape = dynamic_cast<B2ChainShape *>(shape);
        // sshape->setOneSided(b);
    }
}

/**
 * Points 必须是顺时针顺序指定的，否则会导致碰撞检测异常
 */
void ChainShape2DComponent::createChain(float *points, int num)
{
    phxy::SqChainDef eDef;
    eDef.type = phxy::SqShapeType::sq_chainShape;

    copyBaseDef(eDef);

    // printf("ChainShape2DComponent::createChain node %d num %d \n", node->nativeId, num);

    if (offset.has_value())
    {
        Vec2 &pos = offset.value();
        if (pos.x != 0 || pos.y != 0)
        {
            int index = 0;
            for (int i = 0; i < num; ++i)
            {
                points[index++] += pos.x;
                points[index++] += pos.y;
            }
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

    if (sx != 1.0 || sy != 1.0)
    {
        int index = 0;
        for (int i = 0; i < num; ++i)
        {
            points[index++] *= sx;
            points[index++] *= sy;
        }
    }

    eDef.count = num;
    eDef.points = (phxy::SqVec2 *)points;
    shape = PhysicsSystem::getInstance()->getWorld()->createShape(bodyId, eDef);
}

void ChainShape2DComponent::createChainEnableLink(float *points, int *ids, int numPoint)
{
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
    }
    // dynamic_cast<ChainShape2D *>(shape)->createChainEnableLink(getBody(), points, ids, numPoint);
}

void ChainShape2DComponent::insertBefore(int fromId, int id, float x, float y)
{
    // dynamic_cast<ChainShape2D *>(shape)->insertBefore(fromId, id, x, y);
}

void ChainShape2DComponent::insertAfter(int fromId, int id, float x, float y)
{
    // dynamic_cast<ChainShape2D *>(shape)->insertAfter(fromId, id, x, y);
}

void ChainShape2DComponent::modify(int id, float x, float y)
{
    // dynamic_cast<ChainShape2D *>(shape)->modify(id, x, y);
}

void ChainShape2DComponent::remove(int id)
{
    // dynamic_cast<ChainShape2D *>(shape)->remove(id);
}