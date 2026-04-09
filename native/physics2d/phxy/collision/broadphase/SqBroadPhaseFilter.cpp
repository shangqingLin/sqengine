#include "SqBroadPhaseFilter.h"
#include "../../common/SqTypeDefine.h"
#include "../../dynamics/SqWorld.h"
#include "../../geometry/shape/SqShape.h"
#include "../../dynamics/SqBody.h"
// #include <engine/engine.h>
using namespace phxy;

bool phxy::SqShouldBodiesCollide(SqWorld *world, SqBody *bodyA, SqBody *bodyB)
{

    // 发生碰撞的两个刚体要求其中至少有一个为Dynamic的
    if (bodyA->getType() != sq_dynamicBody && bodyB->getType() != sq_dynamicBody)
    {
        return false;
    }

    int jointKey;
    SqBody *otherBody;
    if (bodyA->jointCount < bodyB->jointCount)
    {
        jointKey = bodyA->headJointKey;
        otherBody = bodyB;
    }
    else
    {
        jointKey = bodyB->headJointKey;
        otherBody = bodyA;
    }

    while (jointKey != SQ_NULL_INDEX)
    {
        int jointId = jointKey >> 1;
        int edgeIndex = jointKey & 1;
        int otherEdgeIndex = edgeIndex ^ 1;

        SqJoint *joint = world->getJoint(jointId);

        // 应用Joint上的collideConnected属性
        if (joint->collideConnected == false && joint->edges[otherEdgeIndex].bodyId == otherBody->bodyIndex)
        {
            return false;
        }

        jointKey = joint->edges[edgeIndex].nextKey;
    }

    return true;
}

bool phxy::SqShouldShapesCollide(const SqShapeFilter &filterA, const SqShapeFilter &filterB)
{
    // printf("SqShouldShapesCollide maskA %llu cA %llu  masB %llu cB %llu check %d \n",
    //      filterA.maskBits, filterA.categoryBits, filterB.maskBits, filterB.categoryBits,
    //      (filterA.maskBits & filterB.categoryBits) != 0 && (filterA.categoryBits & filterB.maskBits) != 0
    //     );
    return (filterA.maskBits & filterB.categoryBits) != 0 && (filterA.categoryBits & filterB.maskBits) != 0;
}

bool phxy::SqBroadShapeFilterCollide(SqWorld *world, SqShape *shapeA, SqShape *shapeB)
{
    if (shapeA == shapeB)
        return false;

    SqBody *bodyA = world->getBody(shapeA->getBody());
    SqBody *bodyB = world->getBody(shapeB->getBody());

    if (bodyA == bodyB)
    {
        return false;
    }

    // Node *nodeA = static_cast<Node *>(bodyA->getUserData());
    // Node *nodeB = static_cast<Node *>(bodyB->getUserData());
    // printf(" SqBroadShapeFilterCollide nodeA %d nodeB %d \n",nodeA->nativeId,nodeB->nativeId);

    // Sensors are handled elsewhere
    // if (shapeA->sensorIndex != SQ_NULL_INDEX || shapeB->sensorIndex != SQ_NULL_INDEX)
    // {
    //     return true;
    // }

    if (SqShouldShapesCollide(shapeA->getFilter(), shapeB->getFilter()) == false)
    {
        return false;
    }

    // 检查这两个Body是否可以发生碰撞
    if (SqShouldBodiesCollide(world, bodyA, bodyB) == false)
    {
        return false;
    }

    SqBroadPhaseCustomFilterFcn *customFilterFcn = world->getSqBroadPhaseCustomFilterFcn();
    if (customFilterFcn != NULL)
    {
        bool shouldCollide = customFilterFcn(shapeA, shapeB, world->getSqBroadPhaseCustomFilterContext());
        if (shouldCollide == false)
        {
            return true;
        }
    }

    return true;
}