#include "WallSlide2DComponent.h"
#include "../../../engine/core/math/math.h"
#include "../PhysicsSystem.h"

#include <stdio.h>

using namespace physics2d;

WallSlide2DComponent::WallSlide2DComponent()
    : collisionShape(nullptr)
{
}

void WallSlide2DComponent::onInitialize()
{
    checkCollisionShape();
    if (!collisionShape)
    {
        node->on(NodeEventType::COMPONENT_ADD, std::bind(&WallSlide2DComponent::checkCollisionShape, this));
    }
}

void WallSlide2DComponent::checkCollisionShape()
{
    collisionShape = node->getComponent<Collision2DComponent>();
    if (collisionShape)
    {
        //collisionShape->setPostSolveallback(std::bind(&WallSlide2DComponent::onPostContact, this, std::placeholders::_1, std::placeholders::_2));
        node->off(NodeEventType::COMPONENT_ADD, std::bind(&WallSlide2DComponent::checkCollisionShape, this));
    }
}

void WallSlide2DComponent::onPostContact(const phxy::SqContact *const contact, const phxy::SqManifold *const manifold)
{
    phxy::SqVec2 tangent = phxy::SqVec2::RightPerp(manifold->normal);

    phxy::SqBody *body = PhysicsSystem::getInstance()->getWorld()->getBody(collisionShape->getBody());

    phxy::SqVec2 linearVelocity = body->getLinearVelocity();
    float velocityLength = phxy::SqVec2::Dot(linearVelocity, tangent);

    // 先消除切线方向上的速度，使之保持静止
    phxy::SqVec2 tangentLinearVelocity = phxy::SqVec2::MulSV(-velocityLength, tangent);

    float angle = Math::getAngle(tangent.x, tangent.y);

    if (angle <= 90)
    {
        // 第一象限
    }
    else if (angle > 90.f && angle <= 180.f)
    {
        // 第二象限
        angle = 180.f - angle;
    }
    else if (angle > 180.f && angle <= 270.f)
    {
        // 第三象限
        angle -= 180.f;
    }
    else if (angle > 270.f && angle < 360.f)
    {
        // 第四象限
        angle = 360.f - angle;
    }

    bool move = angle >= slopMin && angle <= slopMax;
    if (!move)
    {

        /**
         * 上一帧遇到的斜坡让其停止了，但下一帧外部可能设置往相反的方向走
         * 但由于精度问题，你就算下帧往相反的方向走了，但还是检查到在不合法的斜坡上
         * 造成外部设置了往相反方向运动都动不了
         */

         


        if (hasStopSlopEvent)
        {
            bridge::NativeToJsObject &nativeToJs = PhysicsSystem::getInstance()->getWorld()->getNativeToJsObject();
            //nativeToJs.beginOp(PhysicsWorld::bridgeOp_WallSide_StopSlop);
            nativeToJs.writeOpArg(node->nativeId);
            nativeToJs.endOp();
        }
    }

    if (move)
    {

        // 加上我们这里自己的速度，就可以控制运动啦
        tangentLinearVelocity = phxy::SqVec2::Add(tangentLinearVelocity, phxy::SqVec2::MulSV(moveDirection * moveSpeed, tangent));
    }

    linearVelocity = phxy::SqVec2::Add(tangentLinearVelocity, linearVelocity);
    body->setLinearVelocity(linearVelocity);
}

void WallSlide2DComponent::setSlop(float min, float max)
{
    slopMin = Math::min(min, max);
    slopMax = Math::max(min, max);
}

void WallSlide2DComponent::moveForward()
{
    moveDirection = 1;
}

void WallSlide2DComponent::moveBack()
{
    moveDirection = -1;
}

void WallSlide2DComponent::stop()
{
    moveDirection = 0;
}

void WallSlide2DComponent::setSpeed(float speed)
{
    moveSpeed = speed;
}

WallSlide2DComponent::~WallSlide2DComponent()
{
    node->off(NodeEventType::COMPONENT_ADD, std::bind(&WallSlide2DComponent::checkCollisionShape, this));
}

void WallSlide2DComponent::setHasEvent(bool b)
{
    hasStopSlopEvent = b;
}