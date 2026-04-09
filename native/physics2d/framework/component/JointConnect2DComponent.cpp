#include "JointConnect2DComponent.h"
#include "../PhysicsSystem.h"
#include "Joint2DComponent.h"

using namespace physics2d;

void JointConnect2DComponent::create(JointType type, Node *nodeA, Node *nodeB, phxy::SqBaseJointDef &def)
{
    sqstd::hash_t key = getHash(type, nodeA, nodeB);

    SQ_ASSERT(jointMap.find(key) == jointMap.end());
    if (jointMap.find(key) != jointMap.end())
    {
        SQ_ASSERT(false);
        return;
    }

    phxy::SqWorld *world = PhysicsSystem::getInstance()->getWorld()->getPyxhWorld();
    physics2d::setDefBody(def, nodeA, nodeB);
    phxy::SqJoint *joint = nullptr;
    switch (type)
    {
    case JointType::DISTANCE:
    {
        phxy::SqDistanceJointDef *adef = static_cast<phxy::SqDistanceJointDef *>(&def);
        joint = world->createDistanceJoint(*adef);
        break;
    }
    case JointType::MOTOR:
    {
        phxy::SqMotorJointDef *adef = static_cast<phxy::SqMotorJointDef *>(&def);
        joint = world->createMotorJoint(*adef);
        break;
    }
    case JointType::MOUSE:
    {
        phxy::SqMouseJointDef *adef = static_cast<phxy::SqMouseJointDef *>(&def);
        joint = world->createMouseJoint(*adef);
        break;
    }
    case JointType::PRISMATIC:
    {
        phxy::SqPrismaticJointDef *adef = static_cast<phxy::SqPrismaticJointDef *>(&def);
        joint = world->createPrimaticJoint(*adef);
        break;
    }
    case JointType::REVOLUTE:
    {
        phxy::SqRevoluteJointDef *adef = static_cast<phxy::SqRevoluteJointDef *>(&def);
        // if(adef->localAnchorAPivot.has_value()){
        //     Vec2 point;
        //     adef->bodyA->syncTransformToPhysics();
        //     adef->bodyA->getLocalPoint(adef->localAnchorAPivot.value(),point);
        //     adef->localAnchorA = point;
        // }

        // if(adef->localAnchorBPivot.has_value()){
        //     Vec2 point;
        //     adef->bodyB->syncTransformToPhysics();
        //     adef->bodyB->getLocalPoint(adef->localAnchorBPivot.value(),point);
        //     adef->localAnchorB = point;
        // }
        // if(adef->localAnchorAPivot.has_value() && adef->localAnchorBPivot.has_value()){
        //     printf("====== (%f %f %f %f) localA %f %f  local B %f %f \n",
        //     adef->localAnchorAPivot.value().x,adef->localAnchorAPivot.value().y,
        //     adef->localAnchorBPivot.value().x,adef->localAnchorBPivot.value().y,
        //     adef->localAnchorA.value().x,adef->localAnchorA.value().y,adef->localAnchorB.value().x,adef->localAnchorB.value().y);
        // }
        joint = world->createRevoluteJoint(*adef);
        break;
    }
    case JointType::WELD:
    {
        phxy::SqWeldJointDef *adef = static_cast<phxy::SqWeldJointDef *>(&def);
        joint = world->createWeldJoint(*adef);
        break;
    }
    case JointType::WHEEL:
    {
        phxy::SqWheelJointDef *adef = static_cast<phxy::SqWheelJointDef *>(&def);
        joint = world->createWheelJoint(*adef);
        break;
    }
    }

    if (joint)
    {
        jointMap[key] = joint;
    }
}

phxy::SqJoint *JointConnect2DComponent::findJoint(JointType type, Node *nodeA, Node *nodeB)
{
    sqstd::hash_t key = getHash(type, nodeA, nodeB);
    std::unordered_map<sqstd::hash_t, phxy::SqJoint *>::iterator it = jointMap.find(key);
    return it == jointMap.end() ? nullptr : it->second;
}

sqstd::hash_t JointConnect2DComponent::getHash(JointType type, Node *nodeA, Node *nodeB)
{
    unsigned int idA = Math::min((unsigned int)nodeA, (unsigned int)nodeB);
    unsigned int idB = Math::max((unsigned int)nodeA, (unsigned int)nodeB);
    sqstd::hash_t _hash = 888;
    sqstd::hash_combine(_hash, idA);
    sqstd::hash_combine(_hash, idB);
    sqstd::hash_combine(_hash, toNumber(type));
    return _hash;
}

void JointConnect2DComponent::setEnableSim(bool b)
{
    std::unordered_map<sqstd::hash_t, phxy::SqJoint *>::iterator it = jointMap.begin();
    while (it != jointMap.end())
    {
        it->second->setEnableSim(b);
        ++it;
    }
}

void JointConnect2DComponent::remove(JointType type, Node *nodeA, Node *nodeB)
{
    phxy::SqWorld *world = PhysicsSystem::getInstance()->getWorld()->getPyxhWorld();
    sqstd::hash_t _hash = getHash(type, nodeA, nodeB);
    std::unordered_map<sqstd::hash_t, phxy::SqJoint *>::iterator it = jointMap.find(_hash);
    if (it != jointMap.end())
    {
        world->destroyJoint(it->second, false);
        jointMap.erase(it);
    }
    else
    {
        _hash = getHash(type, nodeB, nodeA);
        it = jointMap.find(_hash);
        if (it != jointMap.end())
        {
            world->destroyJoint(it->second, false);
            jointMap.erase(it);
        }
    }
}

void JointConnect2DComponent::removeAll()
{
    phxy::SqWorld *world = PhysicsSystem::getInstance()->getWorld()->getPyxhWorld();
    std::unordered_map<sqstd::hash_t, phxy::SqJoint *>::iterator it = jointMap.begin();
    while (it != jointMap.end())
    {
        world->destroyJoint(it->second, false);
        ++it;
    }
    jointMap.clear();
}

JointConnect2DComponent::~JointConnect2DComponent()
{
    removeAll();
}