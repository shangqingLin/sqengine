#include "Joint2DComponent.h"
#include "../PhysicsSystem.h"
#include "RigidBody2DComponent.h"
#include "Collision2DComponent.h"

using namespace physics2d;

void physics2d::DeleteJointCallback(void *context)
{
    Joint2DComponent *jointComponent = (Joint2DComponent *)context;
    jointComponent->joint = nullptr; // 清空，避免重复Destroy
}

void Joint2DComponent::setConnectNode(Node *node)
{
    if (connectNode == node)
        return;
    destroyJoint();
    connectNode = node;
    // printf("====setConnectNode %d\n",this->node->activeInHierarchy());
    if (this->node->activeInHierarchy())
    {
        onSwitchConnectNode();
        onCreateJoint();
    }
}

void Joint2DComponent::destroyJoint()
{
    // printf("======destroyJoint \n");
    connectNode = nullptr;
    if (joint)
    {
        joint->setDeleteCallback(nullptr, nullptr); // 避免重复调用destroy
        PhysicsSystem::getInstance()->getWorld()->getPyxhWorld()->destroyJoint(joint, false);
        joint = nullptr;
    }
}

void Joint2DComponent::setCollideConnected(bool collide)
{
    // printf("setCollideConnected %d %p\n",collide,joint);
    if (joint)
    {
        joint->setCollideConnected(false);
    }
    else
    {
        tempDef = onCreateJointDef();
        tempDef->collideConnected = collide;
    }
}

void Joint2DComponent::setConnectNodeLocalAnchor(const Vec2 &vec)
{
    if (joint)
    {

        phxy::SqTransform transform = joint->getLocalFrameBTransform();
        transform.p.x = vec.x;
        transform.p.y = vec.y;
        joint->setLocalFrameBTransform(transform);
    }
    else
    {
        tempDef = onCreateJointDef();
        tempDef->localFrameB.p.x = vec.x;
        tempDef->localFrameB.p.y = vec.y;
    }
}

void Joint2DComponent::setOwnerLocalAnchor(const Vec2 &vec)
{
    if (joint)
    {
        phxy::SqTransform transform = joint->getLocalFrameATransform();
        transform.p.x = vec.x;
        transform.p.y = vec.y;
        joint->setLocalFrameATransform(transform);
    }
    else
    {
        tempDef = onCreateJointDef();
        tempDef->localFrameA.p.x = vec.x;
        tempDef->localFrameA.p.y = vec.y;
    }
}

void Joint2DComponent::setConnectNodeRotate(float rotate)
{
    rotate = Math::angleToRadian(rotate);
    if (joint)
    {
        phxy::SqTransform transform = joint->getLocalFrameBTransform();
        transform.q = phxy::SqRot(rotate);
        joint->setLocalFrameBTransform(transform);
    }
    else
    {
        tempDef = onCreateJointDef();
        phxy::SqRot rot(rotate);
        tempDef->localFrameB.q = rot;
    }
}

void Joint2DComponent::setOwnerNodeRotate(float rotate)
{
    if (joint)
    {
        phxy::SqTransform transform = joint->getLocalFrameATransform();
        transform.q = phxy::SqRot(rotate);
        joint->setLocalFrameATransform(transform);
    }
    else
    {
        tempDef = onCreateJointDef();
        phxy::SqRot rot;
        rot.setRadius(Math::angleToRadian(rotate));
        tempDef->localFrameA.q = rot;
    }
}

void Joint2DComponent::onEnable()
{
    // printf("====onEnable %p %p\n",connectNode,joint);
    if (connectNode && !joint)
    {
        onSwitchConnectNode();
        onCreateJoint();
    }
}

void Joint2DComponent::setEnableSim(bool b)
{
    if (joint)
    {
        joint->setEnableSim(b);
    }
}

void Joint2DComponent::onCreateJoint()
{
    /**
     * 需要监听Joint是否被移除，避免重复移除Joint
     *
     * 1、RigidBody2DComponent移除的时候，SqBody会被移除,从而导致Body上所有的Joint被移除，所以需要通知Joint组件
     * 2、BodyA和BodyB 需要创建一个Joint，然后在BodyA上了添加了一个Joint Component并链接到BodyB；
     *  当BodyB销毁时，那么Joint也一起销毁的，但BodyA并不知道 Body B 被销毁了，因此会造成BodyA还持有这个Joint Component
     *
     * 为了处理上面情况：大部分在JS端处理，这里只是避免重复Destroy Joint的情况，因此Joint组件 移除的逻辑在JS端处理
     *
     */
    joint->setDeleteCallback(&DeleteJointCallback, this);
}

static int findRigidBody(Node *node)
{
    int body = -1;
    RigidBody2DComponent *rigidBody = node->getComponent<RigidBody2DComponent>();
    if (rigidBody)
    {
        body = rigidBody->getBody();
    }
    else
    {
        Collision2DComponent *collisionComponent = node->getComponent<Collision2DComponent>();
        if (collisionComponent)
        {
            body = collisionComponent->getBody();
        }
    }
    return body;
}

void physics2d::setDefBody(phxy::SqBaseJointDef &def, Node *nodeA, Node *nodeB)
{
    int bodyA = findRigidBody(nodeA);
    int bodyB = findRigidBody(nodeB);
    SQ_ASSERT(bodyA != -1);
    SQ_ASSERT(bodyB != -1);
    def.bodyAId = bodyA;
    def.bodyBId = bodyB;
}

void Joint2DComponent::setBody(Node *nodeA, Node *nodeB, phxy::SqBaseJointDef &def)
{
    setDefBody(def, nodeA, nodeB);
}

void Joint2DComponent::setConstraintHertz(float herzt)
{
    if (joint)
    {
        joint->setConstraintHertz(herzt);
    }
    else
    {
        tempDef = onCreateJointDef();
        tempDef->constraintHertz = herzt;
    }
}

void Joint2DComponent::setConstraintDampingRatio(float damping)
{
    if (joint)
    {

        joint->setConstraintDampingRatio(damping);
    }
    else
    {
        tempDef = onCreateJointDef();
        tempDef->constraintDampingRatio = damping;
    }
}

void Joint2DComponent::setForceThreshold(float force)
{
    if (joint)
    {
        joint->setForceThreshold(force);
    }
    else
    {
        tempDef = onCreateJointDef();
        tempDef->forceThreshold = force;
    }
}

void Joint2DComponent::setTorqueThreshold(float torque)
{
    if (joint)
    {
        joint->setTorqueThreshold(torque);
    }
    else
    {
        tempDef = onCreateJointDef();
        tempDef->torqueThreshold = torque;
    }
}

Joint2DComponent::~Joint2DComponent()
{
    if (tempDef)
    {
        delete tempDef;
        tempDef = nullptr;
    }
    destroyJoint();
}