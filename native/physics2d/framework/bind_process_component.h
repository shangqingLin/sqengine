#pragma once
#include "../../bindings/define.h"
#include "./component/Collision2DComponent.h"

namespace physics2d
{
    class Joint2DComponent;
    void processPhysicsCollision2DComponent(Collision2DComponent* component, bridge::ComponentType type, char nodeOp, char op, ArrayBuffer &buffer, Node *node);
    
    void processPhysicsChainShape2DComponent(bridge::ComponentType type, char nodeOp, char op, ArrayBuffer &buffer, Node *node);
    void processPhysicsRigidBody2DComponent(bridge::ComponentType type, char nodeOp, char op, ArrayBuffer &buffer, Node *node);
    void processPhysicsBoxShape2DComponent(bridge::ComponentType type, char nodeOp, char op, ArrayBuffer &buffer, Node *node);
    void processPhysicsCapsuleShape2DComponent(bridge::ComponentType type, char nodeOp, char op, ArrayBuffer &buffer, Node *node);
    void processPhysicsParticle2DComponent(bridge::ComponentType type, char nodeOp, char op, ArrayBuffer &buffer, Node *node);
    void processPhysicsCircleCollision2DComponent(bridge::ComponentType type, char nodeOp, char op, ArrayBuffer &buffer, Node *node);
    void processPolygonCollision2DComponent(bridge::ComponentType type, char nodeOp, char op, ArrayBuffer &buffer, Node *node);
    void processCharacter2DComponent(bridge::ComponentType type, char nodeOp, char op, ArrayBuffer &buffer, Node *node);
    
    void processJoint2DComponent(bridge::ComponentType type, char nodeOp, char op, ArrayBuffer &buffer, Node *node,Joint2DComponent* component);
    void processPrismaticJoint2DComponent(bridge::ComponentType type, char nodeOp, char op, ArrayBuffer &buffer, Node *node);
    void processDistanceJoint2DComponent(bridge::ComponentType type, char nodeOp, char op, ArrayBuffer &buffer, Node *node);
    void processRevoluteJoint2DComponent(bridge::ComponentType type, char nodeOp, char op, ArrayBuffer &buffer, Node *node);
    void processWheelJoint2DComponent(bridge::ComponentType type, char nodeOp, char op, ArrayBuffer &buffer, Node *node);
    void processWeldJoint2DComponent(bridge::ComponentType type, char nodeOp, char op, ArrayBuffer &buffer, Node *node);
    void processMouseJoint2DComponent(bridge::ComponentType type, char nodeOp, char op, ArrayBuffer &buffer, Node *node);
    void processMotorJoint2DComponent(bridge::ComponentType type, char nodeOp, char op, ArrayBuffer &buffer, Node *node);
    void processPBD2DComponent(bridge::ComponentType type, char nodeOp, char op, ArrayBuffer &buffer, Node *node);
    void processJointConnect2DComponent(bridge::ComponentType type, char nodeOp, char op, ArrayBuffer &buffer, Node *node);

    void processWallSlide2DComponent(bridge::ComponentType type, char nodeOp, char op, ArrayBuffer &buffer, Node *node);
}