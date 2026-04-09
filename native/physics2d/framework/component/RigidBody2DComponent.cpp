#include "RigidBody2DComponent.h"
#include "../PhysicsSystem.h"
#include "Collision2DComponent.h"
#include "../../../engine/2d/components/Transform2DComponent.h"

using namespace physics2d;

RigidBody2DComponent::RigidBody2DComponent() : Component()
{
}

void RigidBody2DComponent::onAwake()
{
    createBody();
    //   printf("onAwake RigidBody2DComponent  %p %d \n", node, node->nativeId);
}

void RigidBody2DComponent::onEnable()
{

    // printf("enabled RigidBody2DComponent  %p %d \n", node, node->nativeId);

    if (ownerBody && (state & Component::DISABLE)) // 表示一定是从节点树中移除再恢复的
    {
        phxy::SqBody *body = PhysicsSystem::getInstance()->getWorld()->getBody(bodyId);
        body->enable();
    }
}

void RigidBody2DComponent::onStart()
{
    phxy::SqBody *body = PhysicsSystem::getInstance()->getWorld()->getBody(bodyId);
    body->setAwake(sleepMode != RigidbodySleepMode2D::StartAsleep);
    // printf("======================start %d \n", sleepMode);
}

void RigidBody2DComponent::onDisable()
{
    if (ownerBody && (state & Component::ENABLE))
    {
        phxy::SqBody *body = PhysicsSystem::getInstance()->getWorld()->getBody(bodyId);
        body->disable();
    }
}

void RigidBody2DComponent::createBody()
{
    Collision2DComponent *collision2DComponent = node->getComponent<Collision2DComponent>();
    if (collision2DComponent)
    {

        ownerBody = false;
        bodyId = collision2DComponent->getBody();

        // printf("========createbody %d \n", node->nativeId);

        if (def.has_value())
        {
            phxy::SqBody *body = PhysicsSystem::getInstance()->getWorld()->getBody(bodyId);
            body->setType(def->type.has_value() ? def->type.value() : phxy::SqBodyType::sq_dynamicBody);
            if (this->def->linearVelocity.has_value())
                body->setLinearVelocity(phxy::SqVec2(this->def->linearVelocity.value().x, this->def->linearVelocity.value().y));
            if (this->def->angularVelocity.has_value())
                body->setAngularVelocity(this->def->angularVelocity.value());
            if (this->def->linearDamping.has_value())
                body->setLinearDamping(this->def->linearDamping.value());
            if (this->def->angularDamping.has_value())
                body->setAngularDamping(this->def->angularDamping.value());
            if (this->def->gravityScale.has_value())
                body->setGravityScale(this->def->gravityScale.value());
            if (this->def->sleepThreshold.has_value())
                body->setSleepThreshold(this->def->sleepThreshold.value());
            if (this->def->enableParticlePressure.has_value())
                body->setEnableParticlePressure(this->def->enableParticlePressure.value());
            if (this->def->enableParticleDamping.has_value())
                body->setEnablePartcileDamping(this->def->enableParticleDamping.value());

            phxy::SqBodyMotionLocks locks = body->getMotionLocks();
            locks.linearX = this->def->lockLinearX.has_value() ? this->def->lockLinearX.value() : locks.linearX;
            locks.linearY = this->def->lockLinearY.has_value() ? this->def->lockLinearY.value() : locks.linearY;
            locks.angularZ = this->def->loclAngularZ.has_value() ? this->def->loclAngularZ.value() : locks.angularZ;
            // if (this->def->allowFastRotation.has_value())
            // def.allowFastRotation = this->def->allowFastRotation.value();
            body->setMotionLocks(locks);

            if (this->def->enableSleep.has_value())
                body->setEnableSleep(this->def->enableSleep.value());
            if (this->def->isAwake.has_value())
                body->setAwake(this->def->isAwake.value());
            if (this->def->isBullet.has_value())
                body->setBullet(this->def->isBullet.value());

            if (this->def->isEnabled.has_value())
                if (this->def->isEnabled.value())
                    body->enable();
                else
                    body->disable();

            if (this->def->enableContinuous.has_value())
                body->setEnableContinuous(this->def->enableContinuous.value());
            def.reset();
        }
    }
    else
    {
        ownerBody = true;
        phxy::SqBodyDef def;
        def.type = phxy::SqBodyType::sq_dynamicBody;
        def.userData = this->node;
        def.isEnabled = true;
        float unit = PhysicsSystem::getInstance()->getUnitsPerMeter();
        const Mat3 &mat = node->getComponent<Transform2DComponent>()->getWorldTransform();
        def.position.x = mat.data[4] / unit;
        def.position.y = mat.data[5] / unit;
        def.rotation.c = mat.data[0];
        def.rotation.s = mat.data[1];

        if (this->def.has_value())
        {
            if (this->def->type.has_value())
                def.type = this->def->type.value();
            if (this->def->linearVelocity.has_value())
                def.linearVelocity = phxy::SqVec2(this->def->linearVelocity.value().x, this->def->linearVelocity.value().y);
            if (this->def->angularVelocity.has_value())
                def.angularVelocity = this->def->angularVelocity.value();
            if (this->def->linearDamping.has_value())
                def.linearDamping = this->def->linearDamping.value();
            if (this->def->angularDamping.has_value())
                def.angularDamping = this->def->angularDamping.value();
            if (this->def->gravityScale.has_value())
                def.gravityScale = this->def->gravityScale.value();
            if (this->def->sleepThreshold.has_value())
                def.sleepThreshold = this->def->sleepThreshold.value();

            if (this->def->lockLinearX.has_value())
                def.lockLinearX = this->def->lockLinearX.value();
            if (this->def->lockLinearY.has_value())
                def.lockLinearY = this->def->lockLinearY.value();
            if (this->def->loclAngularZ.has_value())
                def.loclAngularZ = this->def->loclAngularZ.value();

            if (this->def->enableSleep.has_value())
                def.enableSleep = this->def->enableSleep.value();
            if (this->def->isAwake.has_value())
                def.isAwake = this->def->isAwake.value();
            if (this->def->isBullet.has_value())
                def.isBullet = this->def->isBullet.value();
            if (this->def->allowFastRotation.has_value())
                def.allowFastRotation = this->def->allowFastRotation.value();
            if (this->def->enableContinuous.has_value())
                def.enableContinuous = this->def->enableContinuous.value();
            if (this->def->enableParticlePressure.has_value())
                def.enableParticlePressure = this->def->enableParticlePressure.value();
            if (this->def->enableParticleDamping.has_value())
                def.enableParticleDamping = this->def->enableParticleDamping.value();

            def.isEnabled = this->def->isEnabled.has_value() ? this->def->isEnabled.value() : true;

            this->def.reset();
        }

        bodyId = PhysicsSystem::getInstance()->getWorld()->createBody(def);
        onNodeTransformChange();
        node->on(NodeEventType::TRASNFORM_CHANGE, std::bind(&RigidBody2DComponent::onNodeTransformChange, this));
        // printf("------------ RigidBody2DComponent create nativeId %d body %d \n", node->nativeId, bodyId);
    }
}

void RigidBody2DComponent::onNodeTransformChange()
{
    physics2d::PhysicsWorld *world = PhysicsSystem::getInstance()->getWorld();

    // printf("RigidBody2DComponent::onNodeTransformChange()a  %d %d \n", node->nativeId,world->isLockSyncPhysicsToScene());

    if (world->isLockSyncPhysicsToScene())
        return;
    int flag = 0;
    int nodeFlag = node->getChangeFlag();

    if (nodeFlag & TransformBit::POSITION || nodeFlag & TransformBit::ROTATION)
    {
        flag = phxy::SqBodyFlags::sq_node_transform_pos_rot;
    }

    if (nodeFlag & TransformBit::SCALE)
    {
        flag |= phxy::SqBodyFlags::sq_node_transform_scale;
    }

    phxy::SqBody *body = world->getBody(bodyId);
    flag = body->getBodyFlag() | flag;
    body->setBodyFlag(flag);

    // printf("RigidBody2DComponent::onNodeTransformChange() %d %d %d bodyId %d \n", node->nativeId, flag, node->getChangeFlag(),bodyId);
}

RigidBody2DComponent::~RigidBody2DComponent()
{

    if (ownerBody)
    {
        node->off(NodeEventType::TRASNFORM_CHANGE, std::bind(&RigidBody2DComponent::onNodeTransformChange, this));
        PhysicsSystem::getInstance()->getWorld()->removeBody(bodyId);
        bodyId = -1;
    }
}

void RigidBody2DComponent::setSleepMode(RigidbodySleepMode2D mode)
{
    sleepMode = mode;
}

RigidbodySleepMode2D RigidBody2DComponent::getSleepMode()
{
    return sleepMode;
}

void RigidBody2DComponent::setBullet(bool b)
{
    if (bodyId != -1)
    {
        PhysicsSystem::getInstance()->getWorld()->getBody(bodyId)->setBullet(b);
    }
    else
    {
        if (!def.has_value())
            def = RigidBodyDef();
        def->isBullet = b;
    }
}

void RigidBody2DComponent::setType(phxy::SqBodyType type)
{

    if (bodyId != -1)
    {
        PhysicsSystem::getInstance()->getWorld()->getBody(bodyId)->setType(type);
    }
    else
    {
        if (!def.has_value())
            def = RigidBodyDef();
        def->type = type;
    }
}

bool RigidBody2DComponent::isEnableContinuous()
{

    if (bodyId == -1)
    {
        return PhysicsSystem::getInstance()->getWorld()->getBody(bodyId)->isEnableContinuous();
    }
    else if (def.has_value())
    {
        return def.value().enableContinuous.has_value() ? def.value().enableContinuous.value() : false;
    }
    return false;
}

void RigidBody2DComponent::setEnableContinuous(bool enable)
{
    if (bodyId != -1)
    {
        PhysicsSystem::getInstance()->getWorld()->getBody(bodyId)->setEnableContinuous(enable);
    }
    else
    {
        if (!def.has_value())
            def = RigidBodyDef();
        def->enableContinuous = enable;
    }
}

void RigidBody2DComponent::setLinearDamping(float damping)
{
    PhysicsSystem::getInstance()->getWorld()->getBody(bodyId)->setLinearDamping(damping);
}

void RigidBody2DComponent::setAngularDamping(float damping)
{
    PhysicsSystem::getInstance()->getWorld()->getBody(bodyId)->setAngularDamping(damping);
}

void RigidBody2DComponent::setLinearVelocity(const Vec2 &v) const
{
    PhysicsSystem::getInstance()->getWorld()->getBody(bodyId)->setLinearVelocity(phxy::SqVec2(v.x, v.y));
}

void RigidBody2DComponent::setLinearVelocity(float x, float y) const
{
    PhysicsSystem::getInstance()->getWorld()->getBody(bodyId)->setLinearVelocity(phxy::SqVec2(x, y));
}

Vec2 RigidBody2DComponent::getLinearVelocity()
{
    phxy::SqBody *body = PhysicsSystem::getInstance()->getWorld()->getBody(bodyId);
    Vec2 velocity;
    const phxy::SqVec2 &v = body->getLinearVelocity();
    velocity.x = v.x;
    velocity.y = v.y;
    return velocity;
}

void RigidBody2DComponent::setAngularVelocity(float v)
{
    PhysicsSystem::getInstance()->getWorld()->getBody(bodyId)->setAngularVelocity(v);
}

float RigidBody2DComponent::getAngularVelocity()
{
    return PhysicsSystem::getInstance()->getWorld()->getBody(bodyId)->getAngularVelocity();
}

void RigidBody2DComponent::setFixedPosition(bool x, bool y)
{
    phxy::SqBody *body = PhysicsSystem::getInstance()->getWorld()->getBody(bodyId);
    phxy::SqBodyMotionLocks locks = body->getMotionLocks();
    locks.linearX = x;
    locks.linearY = y;
    body->setMotionLocks(locks);
}

void RigidBody2DComponent::setFixedRotation(bool b)
{
    phxy::SqBody *body = PhysicsSystem::getInstance()->getWorld()->getBody(bodyId);
    phxy::SqBodyMotionLocks locks = body->getMotionLocks();
    locks.angularZ = b;
    body->setMotionLocks(locks);
}

void RigidBody2DComponent::ApplyForce(const Vec2 &force, const Vec2 &point, bool wake)
{
    PhysicsSystem::getInstance()->getWorld()->getBody(bodyId)->applyForce(phxy::SqVec2(force.x, force.y), phxy::SqVec2(point.x, point.y), wake);
}

void RigidBody2DComponent::ApplyForceToCenter(const Vec2 &force, bool wake)
{
    PhysicsSystem::getInstance()->getWorld()->getBody(bodyId)->applyForceToCenter(phxy::SqVec2(force.x, force.y), wake);
}

void RigidBody2DComponent::ApplyTorque(float torque, bool wake)
{
    PhysicsSystem::getInstance()->getWorld()->getBody(bodyId)->applyTorque(torque, wake);
}

void RigidBody2DComponent::ApplyLinearImpulse(const Vec2 &impulse, const Vec2 &point, bool wake)
{
    PhysicsSystem::getInstance()->getWorld()->getBody(bodyId)->applyLinearImpulse(phxy::SqVec2(impulse.x, impulse.y), phxy::SqVec2(point.x, point.y), wake);
}

void RigidBody2DComponent::ApplyLinearImpulseToCenter(const Vec2 &impulse, bool wake)
{
    PhysicsSystem::getInstance()->getWorld()->getBody(bodyId)->applyLinearImpulseToCenter(phxy::SqVec2(impulse.x, impulse.y), wake);
}

void RigidBody2DComponent::ApplyAngularImpulse(float impulse, bool wake)
{
    PhysicsSystem::getInstance()->getWorld()->getBody(bodyId)->applyAngularImpulse(impulse, wake);
}

void RigidBody2DComponent::SetGravityScale(float scale)
{
    PhysicsSystem::getInstance()->getWorld()->getBody(bodyId)->setGravityScale(scale);
}

float RigidBody2DComponent::GetGravityScale()
{
    return PhysicsSystem::getInstance()->getWorld()->getBody(bodyId)->getGravityScale();
}

void RigidBody2DComponent::SetActive(bool active)
{

    if (bodyId != -1)
    {
        phxy::SqBody *body = PhysicsSystem::getInstance()->getWorld()->getBody(bodyId);
        if (active)
        {
            body->enable();
        }
        else
        {
            body->disable();
        }
    }
    else
    {
        if (!def.has_value())
            def = RigidBodyDef();
        def->isEnabled = active;
    }
}

void RigidBody2DComponent::setMass(float mass)
{
    // body->setMass(mass);
}

float RigidBody2DComponent::getMass()
{
    return PhysicsSystem::getInstance()->getWorld()->getBody(bodyId)->getMass();
}

void RigidBody2DComponent::setRotationalInertia(float v)
{
    phxy::SqBody *body = PhysicsSystem::getInstance()->getWorld()->getBody(bodyId);
    phxy::SqMassData massData = body->getMassData();
    massData.rotationalInertia = v;
    body->setMassData(massData);
}

float RigidBody2DComponent::getRotationalInertia()
{
    return PhysicsSystem::getInstance()->getWorld()->getBody(bodyId)->getMassData().rotationalInertia;
}

void RigidBody2DComponent::setAwake(bool b)
{
    if (bodyId != -1)
    {
        PhysicsSystem::getInstance()->getWorld()->getBody(bodyId)->setAwake(b);
    }
}

void RigidBody2DComponent::setEnableParticlePressure(bool b)
{
    if (bodyId != -1)
    {
        PhysicsSystem::getInstance()->getWorld()->getBody(bodyId)->setEnableParticlePressure(b);
    }
    else
    {
        if (!def.has_value())
            def = RigidBodyDef();
        def->enableParticlePressure = b;
    }
}

void RigidBody2DComponent::setEnablePartcileDamping(bool b)
{
    if (bodyId != -1)
    {
        PhysicsSystem::getInstance()->getWorld()->getBody(bodyId)->setEnablePartcileDamping(b);
    }
    else
    {
        if (!def.has_value())
            def = RigidBodyDef();
        def->enableParticleDamping = b;
    }
}