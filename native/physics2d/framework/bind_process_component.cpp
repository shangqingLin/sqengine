#include "bind_process_component.h"
#include "../../bindings/binding.h"
#include "../../engine/core/math/math.h"
#include "../../engine/core/math/Vec2.h"
#include "../../engine/assets/AssetManager.h"
#include "./component/BoxCollision2DComponent.h"
#include "./component/CapsuleCollision2DComponent.h"
#include "./component/ChainShape2DComponent.h"
#include "./component/CircleCollision2DComponent.h"
#include "./component/Collision2DComponent.h"
#include "./component/PolygonCollision2DComponent.h"
#include "./component/RigidBody2DComponent.h"
#include "./component/RevoluteJoint2DComponent.h"
#include "./component/WheelJoint2DComponent.h"
#include "./component/WeldJoint2DComponent.h"
#include "./component/MotorJoint2DComponent.h"
#include "./component/MouseJoint2DComponent.h"
#include "./component/PrismaticJoint2DComponent.h"
#include "./component/DistanceJoint2DComponent.h"
#include "./component/JointConnect2DComponent.h"
#include "./component/Character2DComponent.h"
#include "./component/WallSlide2DComponent.h"
#include "./component/ParticlePhysics2DComponent.h"
#include "./component/PBD2DComponent.h"

using namespace physics2d;

void physics2d::processPhysicsRigidBody2DComponent(bridge::ComponentType type, char nodeOp, char op, ArrayBuffer &buffer, Node *node)
{

    // printf("+++++++++++++++++++processPhysicsRigidBody2DComponent \n");

    if (nodeOp == 1)
    {
        node->addComponent<physics2d::RigidBody2DComponent>();
        return;
    }
    else if (nodeOp == 2)
    {
        node->removeComponent<physics2d::RigidBody2DComponent>();
        return;
    }

    physics2d::RigidBody2DComponent *component = node->getComponent<physics2d::RigidBody2DComponent>();

    switch (op)
    {
    case 1:
    {
        component->setSleepMode(RigidbodySleepMode2D(*buffer.popp<char>()));
        break;
    }
    case 2:
    {
        component->setBullet(*buffer.popp<char>());
        break;
    }
    case 3:
    {
        char type = *buffer.popp<char>();
        component->setType(phxy::SqBodyType(type));
        break;
    }
    case 4:
    {
        component->setLinearDamping(*buffer.popp<float>());
        break;
    }
    case 5:
    {
        component->setAngularDamping(*buffer.popp<float>());
        break;
    }
    case 6:
    {
        component->setLinearVelocity(*buffer.popp<float>(), *buffer.popp<float>());
        break;
    }
    case 7:
    {
        component->setAngularVelocity(*buffer.popp<float>());
        break;
    }
    case 8:
    {
        component->setFixedRotation(*buffer.popp<char>());
        break;
    }
    case 9:
    {
        Vec2 force(*buffer.popp<float>(), *buffer.popp<float>());
        Vec2 point(*buffer.popp<float>(), *buffer.popp<float>());
        component->ApplyForce(force, point, *buffer.popp<char>());
        break;
    }
    case 10:
    {
        Vec2 force(*buffer.popp<float>(), *buffer.popp<float>());
        component->ApplyForceToCenter(force, *buffer.popp<char>());
        break;
    }
    case 11:
    {
        component->ApplyTorque(*buffer.popp<float>(), *buffer.popp<char>());
        break;
    }
    case 12:
    {
        Vec2 force(*buffer.popp<float>(), *buffer.popp<float>());
        Vec2 point(*buffer.popp<float>(), *buffer.popp<float>());
        component->ApplyLinearImpulse(force, point, *buffer.popp<char>());
        break;
    }
    case 13:
    {
        Vec2 force(*buffer.popp<float>(), *buffer.popp<float>());
        component->ApplyLinearImpulseToCenter(force, *buffer.popp<char>());
        break;
    }
    case 14:
    {
        component->ApplyAngularImpulse(*buffer.popp<float>(), *buffer.popp<char>());
        break;
    }
    case 15:
    {
        component->SetGravityScale(*buffer.popp<float>());
        break;
    }
    case 16:
    {
        component->SetActive(*buffer.popp<char>());
        break;
    }
    case 17:
        component->setMass(*buffer.popp<float>());
        break;
    case 18:
        component->setRotationalInertia(*buffer.popp<float>());
        break;
    case 19:
    {
        component->nativeToJs.beginOpync();
        component->nativeToJs.writeOpArg(component->getMass());
        component->nativeToJs.endOpSync();
        break;
    }
    case 20:
    {
        component->setEnableContinuous(*buffer.popp<char>());
        break;
    }
    case 21:
    {
        component->setAwake(*buffer.popp<char>());
        break;
    }
    case 22:
        component->setFixedPosition(*buffer.popp<char>(), *buffer.popp<char>());
        break;
    case 23:
        component->setEnableParticlePressure(*buffer.popp<char>());
        break;
    case 24:
        component->setEnablePartcileDamping(*buffer.popp<char>());
        break;
    }
}

void physics2d::processPhysicsCollision2DComponent(Collision2DComponent *component, bridge::ComponentType type, char nodeOp, char op, ArrayBuffer &buffer, Node *node)
{

    // printf("collision2D op %d \n",op);

    switch (op)
    {
    case 3:
    {
        char b = *buffer.popp<char>();
        char type = *buffer.popp<char>();
        int classKey = 0;
        if (b)
        {
            classKey = *buffer.popp<int>();
        }
        component->setBridgeHasContactListener(b, type, classKey);
        break;
    }
    case 4:
    {
        component->setOffset(*buffer.popp<float>(), *buffer.popp<float>());
        break;
    }

    case 5:
    {
        component->setDensity(*buffer.popp<float>());
        break;
    }
    case 6:
    {
        component->setFriction(*buffer.popp<float>());
        break;
    }
    case 7:
    {
        component->setRestitution(*buffer.popp<float>());
        break;
    }
    case 8:
    {
        component->setSensor(*buffer.popp<char>());
        break;
    }
    case 9:
    {
        component->setFilter(*buffer.popp<unsigned int>(), *buffer.popp<unsigned int>());
        break;
    }
    case 10:
    {
        component->setCustomColor(*buffer.popp<unsigned int>());
        break;
    }
    case 11:
    {
        component->setSensorAABB(*buffer.popp<char>());
        break;
    }
    case 12:
    {
        component->setTangentSpeed(*buffer.popp<float>());
        break;
    }
    default:
    {
        SQ_ASSERT(false);
    }
    }
}

void physics2d::processPhysicsChainShape2DComponent(bridge::ComponentType type, char nodeOp, char op, ArrayBuffer &buffer, Node *node)
{
    // printf("+++++++++++++++++++processPhysicsChainShape2DComponent %d \n", op);

    if (nodeOp == 1)
    {
        node->addComponent<physics2d::ChainShape2DComponent>();
        return;
    }
    else if (nodeOp == 2)
    {
        node->removeComponent<physics2d::ChainShape2DComponent>();
        return;
    }

    physics2d::ChainShape2DComponent *component = node->getComponent<physics2d::ChainShape2DComponent>();
    if (op <= 50)
    {
        physics2d::processPhysicsCollision2DComponent(component, type, nodeOp, op, buffer, node);
        return;
    }

    switch (op)
    {
    case 51:
    {
        int numPoints = *buffer.popp<int>();
        component->createChain((float *)buffer.readBuffer(numPoints * 8), numPoints);
        break;
    }
    case 52:
    {
        component->setOneSided(*buffer.popp<char>());
        break;
    }
    case 53:
    {
        int numPoints = *buffer.popp<int>();
        component->createChainEnableLink((float *)buffer.readBuffer(numPoints * 8), (int *)buffer.readBuffer(numPoints * 8), numPoints);
        break;
    }
    case 54:
    {
        component->insertBefore(*buffer.popp<int>(), *buffer.popp<int>(), *buffer.popp<float>(), *buffer.popp<float>());
        break;
    }
    case 55:
    {
        component->insertAfter(*buffer.popp<int>(), *buffer.popp<int>(), *buffer.popp<float>(), *buffer.popp<float>());
        break;
    }
    case 56:
    {
        component->modify(*buffer.popp<int>(), *buffer.popp<float>(), *buffer.popp<float>());
        break;
    }
    case 57:
    {
        component->remove(*buffer.popp<int>());
        break;
    }
    default:
    {
        SQ_ASSERT(false);
    }
    }
}

void physics2d::processPhysicsBoxShape2DComponent(bridge::ComponentType type, char nodeOp, char op, ArrayBuffer &buffer, Node *node)
{
    if (nodeOp == 1)
    {
        node->addComponent<physics2d::BoxCollision2DComponent>();
        return;
    }
    else if (nodeOp == 2)
    {
        node->removeComponent<physics2d::BoxCollision2DComponent>();
        return;
    }

    physics2d::BoxCollision2DComponent *component = node->getComponent<physics2d::BoxCollision2DComponent>();
    if (op <= 50)
    {
        physics2d::processPhysicsCollision2DComponent(component, type, nodeOp, op, buffer, node);
        return;
    }

    switch (op)
    {
    case 51:
        component->create(*buffer.popp<float>(), *buffer.popp<float>());
        break;
    default:
        break;
    }
}

void physics2d::processPhysicsCapsuleShape2DComponent(bridge::ComponentType type, char nodeOp, char op, ArrayBuffer &buffer, Node *node)
{

    // printf("processPhysicsCapsuleShape2DComponent %d %d %d \n", node->nativeId, nodeOp, op);
    if (nodeOp == 1)
    {
        node->addComponent<physics2d::CapsuleCollision2DComponent>();
        return;
    }
    else if (nodeOp == 2)
    {
        node->removeComponent<physics2d::CapsuleCollision2DComponent>();
        return;
    }

    physics2d::CapsuleCollision2DComponent *component = node->getComponent<physics2d::CapsuleCollision2DComponent>();
    if (op <= 50)
    {
        physics2d::processPhysicsCollision2DComponent(component, type, nodeOp, op, buffer, node);
        return;
    }
    switch (op)
    {
    case 51:
    {
        component->create(
            *buffer.popp<float>(),
            *buffer.popp<float>(),
            *buffer.popp<float>(),
            *buffer.popp<float>(),
            *buffer.popp<float>());
        break;
    }
    }
}

void physics2d::processPhysicsParticle2DComponent(bridge::ComponentType type, char nodeOp, char op, ArrayBuffer &buffer, Node *node)
{

    if (nodeOp == 1)
    {
        node->addComponent<physics2d::ParticlePhysics2DComponent>();
        return;
    }
    else if (nodeOp == 2)
    {
        node->removeComponent<physics2d::ParticlePhysics2DComponent>();
        return;
    }

    physics2d::ParticlePhysics2DComponent *component = node->getComponent<physics2d::ParticlePhysics2DComponent>();

    switch (op)
    {
    case 1:
    {
        int state = *buffer.popp<int>();
        char ShapeType = *buffer.popp<char>();

        phxy::SqParticleGroupDef def;
        if (state & 1)
        {
            def.flags = *buffer.popp<unsigned int>();
        }

        if (state & 2)
        {
            def.groupFlags = *buffer.popp<unsigned int>();
        }

        if (state & 4)
        {
            def.position.x = *buffer.popp<float>();
            def.position.y = *buffer.popp<float>();
        }

        if (state & 8)
        {
            def.angle = *buffer.popp<float>();
        }

        if (state & 16)
        {
            def.linearVelocity.x = *buffer.popp<float>();
            def.linearVelocity.y = *buffer.popp<float>();
        }

        if (state & 32)
        {
            def.angularVelocity = *buffer.popp<float>();
        }

        if (state & 64)
        {
            def.color.r = *buffer.popp<unsigned char>();
            def.color.g = *buffer.popp<unsigned char>();
            def.color.b = *buffer.popp<unsigned char>();
            def.color.a = *buffer.popp<unsigned char>();
            // printf("------------------------color %u %u  %u %u \n",def.color.r ,def.color.g ,def.color.b , def.color.a);
        }

        if (state & 128)
        {
            def.strength = *buffer.popp<float>();
        }

        if (state & 256)
        {
            def.stride = *buffer.popp<float>();
        }

        if (state & 512)
        {
            def.particleCount = *buffer.popp<int>();
        }

        if (state & 1024)
        {
            def.lifetime = *buffer.popp<float>();
        }

        // if(state & 2048){
        // 	def.group = *buffer.popp<float>();
        // }

        phxy::SqPolygonShape *shape = new phxy::SqPolygonShape;
        shape->setAsBox(100, 100, phxy::SqVec2(), phxy::SqRot());
        def.shape = shape;
        component->addParticleGroup(def);
        break;
    }
    case 2:
    {
        int &assetId = *buffer.popp<int>();
        Material *material = static_cast<Material *>(AssetManager::getInstance()->findById(assetId));
        component->setMaterial(material);
        break;
    }
    case 4:
    {

        break;
    }
    case 5:
    {
        component->setGravityScale(*buffer.popp<float>());
        break;
    }
    case 6:
    {
        component->setParticleFlags(*buffer.popp<int>(), *buffer.popp<unsigned int>());
        break;
    }
    case 7:
    {
        Vec2 force;
        int first = *buffer.popp<int>();
        int last = *buffer.popp<int>();
        force.x = *buffer.popp<float>();
        force.y = *buffer.popp<float>();
        component->applyLinearImpulseInRang(first, last, force);
        break;
    }
    case 8:
    {
        Vec2 force;
        int first = *buffer.popp<int>();
        int last = *buffer.popp<int>();
        force.x = *buffer.popp<float>();
        force.y = *buffer.popp<float>();
        component->applyForceInRang(first, last, force);
        break;
    }
    case 9:
    {
        component->setViscousStrength(*buffer.popp<float>());
        break;
    }
    case 10:
    {
        component->setSurfaceTensionPressureStrength(*buffer.popp<float>());
        break;
    }
    case 11:
    {
        component->setSurfaceTensionNormalStrength(*buffer.popp<float>());
        break;
    }
    case 12:
    {
        ParticlePhysicsCreateDefine def;
        unsigned int state = *buffer.popp<unsigned int>();
        if (state & (1 << 1))
        {
            def.count = *buffer.popp<int>();
        }

        if (state & (1 << 2))
        {
            def.flags = *buffer.popp<unsigned int>();
        }

        if (state & (1 << 3))
        {
            def.lifetime = *buffer.popp<int>();
        }

        if (state & (1 << 4))
        {
            def.position.x = *buffer.popp<float>();
            def.position.y = *buffer.popp<float>();
        }

        if (state & (1 << 5))
        {
            def.velocity.x = *buffer.popp<float>();
            def.velocity.y = *buffer.popp<float>();
        }

        if (state & (1 << 6))
        {
            def.r = *buffer.popp<unsigned char>();
            def.g = *buffer.popp<unsigned char>();
            def.b = *buffer.popp<unsigned char>();
            def.a = *buffer.popp<unsigned char>();
        }
        component->createParticle(&def);
        break;
    }
    case 13:
    {
        component->setParticleRadius(*buffer.popp<float>());
        break;
    }
    case 14:
    {
        break;
    }
    case 15:
    {
        component->allocate(*buffer.popp<int>());
        break;
    }
    case 16:
    {
        component->setPressureStrength(*buffer.popp<float>());
        break;
    }
    case 17:
    {
        component->setDestructionByAge(*buffer.popp<char>());
        break;
    }
    case 18:
    {
        int num = *buffer.popp<int>();
        for (int i = 0; i < num; ++i)
        {
            component->destroyParticle(*buffer.popp<int>());
        }
        break;
    }
    case 19:
    {
        component->enableListener(*buffer.popp<char>(), *buffer.popp<char>());
        break;
    }
    case 20:
    {
        component->setBodyContactFilter(*buffer.popp<uint32_t>(), *buffer.popp<uint32_t>());
        break;
    }
    default:
        SQ_ASSERT(false);
    }
}

void physics2d::processPhysicsCircleCollision2DComponent(bridge::ComponentType type, char nodeOp, char op, ArrayBuffer &buffer, Node *node)
{
    if (nodeOp == 1)
    {
        node->addComponent<physics2d::CircleCollision2DComponent>();
        return;
    }
    else if (nodeOp == 2)
    {
        node->removeComponent<physics2d::CircleCollision2DComponent>();
        return;
    }

    physics2d::CircleCollision2DComponent *component = node->getComponent<physics2d::CircleCollision2DComponent>();

    // printf("circle %d \n",op);

    if (op <= 50)
    {
        physics2d::processPhysicsCollision2DComponent(component, type, nodeOp, op, buffer, node);
        return;
    }
    switch (op)
    {
    case 51:
    {
        component->setRadius(*buffer.popp<float>());
        break;
    }
    default:
        break;
    }
}

void physics2d::processPolygonCollision2DComponent(bridge::ComponentType type, char nodeOp, char op, ArrayBuffer &buffer, Node *node)
{
    if (nodeOp == 1)
    {
        node->addComponent<physics2d::PolygonCollision2DComponent>();
        return;
    }
    else if (nodeOp == 2)
    {
        node->removeComponent<physics2d::PolygonCollision2DComponent>();
        return;
    }

    physics2d::PolygonCollision2DComponent *component = node->getComponent<physics2d::PolygonCollision2DComponent>();
    if (op <= 50)
    {
        physics2d::processPhysicsCollision2DComponent(component, type, nodeOp, op, buffer, node);
        return;
    }

    switch (op)
    {
    case 51:
    {
        int &numPoints = *buffer.popp<int>();
        component->setPoints((float *)buffer.readBuffer(numPoints * 8), numPoints);
        break;
    }
    default:
        break;
    }
}

void physics2d::processCharacter2DComponent(bridge::ComponentType type, char nodeOp, char op, ArrayBuffer &buffer, Node *node)
{
    if (nodeOp == 1)
    {
        node->addComponent<physics2d::Character2DComponent>();
        return;
    }
    else if (nodeOp == 2)
    {
        node->removeComponent<physics2d::Character2DComponent>();
        return;
    }

    physics2d::Character2DComponent *component = node->getComponent<physics2d::Character2DComponent>();
    switch (op)
    {
    case 1:
    {
        component->moveLeft(*buffer.popp<char>());
        break;
    }
    case 2:
    {
        component->jump();
        break;
    }
    case 3:
    {
        component->moveRight(*buffer.popp<char>());
        break;
    }
    case 4:
    {
        component->setFilter(*buffer.popp<unsigned int>(), *buffer.popp<unsigned int>());
        break;
    }
    }
}

void physics2d::processJoint2DComponent(bridge::ComponentType type, char nodeOp, char op, ArrayBuffer &buffer, Node *node, Joint2DComponent *component)
{
    switch (op)
    {
    case 1:
        component->setConnectNode(bridge::JsToNativeObjectManager::getInstance()->getById<Node>(*buffer.popp<int>()));
        break;
    case 2:
        component->setCollideConnected(*buffer.popp<char>());
        break;
    case 3:
    {
        Vec2 vec;
        vec.x = *buffer.popp<float>();
        vec.y = *buffer.popp<float>();
        component->setConnectNodeLocalAnchor(vec);
        break;
    }
    case 4:
    {
        Vec2 vec;
        vec.x = *buffer.popp<float>();
        vec.y = *buffer.popp<float>();
        component->setOwnerLocalAnchor(vec);
        break;
    }
    case 5:
    {
        component->setConnectNodeRotate(*buffer.popp<float>());
        break;
    }
    case 6:
    {
        component->setOwnerNodeRotate(*buffer.popp<float>());
        break;
    }
    case 7:
    {
        component->setConstraintHertz(*buffer.popp<float>());
        break;
    }
    case 8:
    {
        component->setConstraintDampingRatio(*buffer.popp<float>());
        break;
    }
    case 9:
    {
        component->setForceThreshold(*buffer.popp<float>());
        break;
    }
    case 10:
    {
        component->setTorqueThreshold(*buffer.popp<float>());
        break;
    }
    default:
        SQ_ASSERT(false);
    }
}

void physics2d::processPrismaticJoint2DComponent(bridge::ComponentType type, char nodeOp, char op, ArrayBuffer &buffer, Node *node)
{
    if (nodeOp == 1)
    {
        node->addComponent<physics2d::PrismaticJoint2DComponent>();
        return;
    }
    else if (nodeOp == 2)
    {
        node->removeComponent<physics2d::PrismaticJoint2DComponent>();
        return;
    }

    PrismaticJoint2DComponent *component = node->getComponent<PrismaticJoint2DComponent>();
    if (op <= 50)
    {
        physics2d::processJoint2DComponent(type, nodeOp, op, buffer, node, component);
        return;
    }

    switch (op)
    {
    case 51:
    {
        component->setEnableSpring(*buffer.popp<char>());
        break;
    }
    case 52:
    {
        component->setHertz(*buffer.popp<float>());
        break;
    }
    case 53:
    {
        component->setDampingRatio(*buffer.popp<float>());
        break;
    }
    case 54:
    {
        component->setEnableLimit(*buffer.popp<char>());
        break;
    }
    case 55:
    {
        component->setLowerTranslation(*buffer.popp<float>());
        break;
    }
    case 56:
    {
        component->setUpperTranslation(*buffer.popp<float>());
        break;
    }
    case 57:
    {
        component->setEnableMotor(*buffer.popp<char>());
        break;
    }
    case 58:
    {
        component->setMaxMotorForce(*buffer.popp<float>());
        break;
    }
    case 59:
    {
        component->setMotorSpeed(*buffer.popp<float>());
        break;
    }
    case 60:
    {
        component->SetEnableSyncAngle(*buffer.popp<char>());
        break;
    }
    default:
        break;
    }
}

void physics2d::processDistanceJoint2DComponent(bridge::ComponentType type, char nodeOp, char op, ArrayBuffer &buffer, Node *node)
{
    if (nodeOp == 1)
    {
        node->addComponent<physics2d::DistanceJoint2DComponent>();
        return;
    }
    else if (nodeOp == 2)
    {
        node->removeComponent<physics2d::DistanceJoint2DComponent>();
        return;
    }

    DistanceJoint2DComponent *component = node->getComponent<DistanceJoint2DComponent>();
    if (op <= 50)
    {
        physics2d::processJoint2DComponent(type, nodeOp, op, buffer, node, component);
        return;
    }

    switch (op)
    {
    case 51:
    {
        component->setLength(*buffer.popp<float>());
        break;
    }
    case 52:
    {
        component->setEnableSpring(*buffer.popp<char>());
        break;
    }
    case 53:
    {
        component->setHertz(*buffer.popp<float>());
        break;
    }
    case 54:
    {
        component->setDampingRatio(*buffer.popp<float>());
        break;
    }
    case 55:
    {
        component->setEnableLimit(*buffer.popp<char>());
        break;
    }
    case 56:
    {
        component->setMaxLength(*buffer.popp<float>());
        break;
    }
    case 57:
    {
        component->setMaxMotorForce(*buffer.popp<float>());
        break;
    }
    case 58:
    {
        component->setEnableMotor(*buffer.popp<char>());
        break;
    }
    case 59:
    {
        component->setMinLength(*buffer.popp<float>());
        break;
    }
    case 60:
    {
        component->setMotorSpeed(*buffer.popp<float>());
        break;
    }
    default:
        break;
    }
}

void physics2d::processRevoluteJoint2DComponent(bridge::ComponentType type, char nodeOp, char op, ArrayBuffer &buffer, Node *node)
{
    if (nodeOp == 1)
    {
        node->addComponent<physics2d::RevoluteJoint2DComponent>();
        return;
    }
    else if (nodeOp == 2)
    {
        node->removeComponent<physics2d::RevoluteJoint2DComponent>();
        return;
    }

    RevoluteJoint2DComponent *component = node->getComponent<RevoluteJoint2DComponent>();
    if (op <= 50)
    {
        physics2d::processJoint2DComponent(type, nodeOp, op, buffer, node, component);
        return;
    }

    switch (op)
    {

    case 51:
    {
        component->setEnableSpring(*buffer.popp<char>());
        break;
    }
    case 52:
    {
        component->setTargetAngle(*buffer.popp<float>());
        break;
    }
    case 53:
    {
        component->setHertz(*buffer.popp<float>());
        break;
    }
    case 54:
    {
        component->setDampingRatio(*buffer.popp<float>());
        break;
    }
    case 55:
    {
        component->setEnableLimit(*buffer.popp<char>());
        break;
    }
    case 56:
    {
        component->setLowerAngle(*buffer.popp<float>());
        break;
    }
    case 57:
    {
        component->setUpperAngle(*buffer.popp<float>());
        break;
    }
    case 58:
    {
        component->setEnableMotor(*buffer.popp<char>());
        break;
    }
    case 59:
    {
        component->setMaxMotorTorque(*buffer.popp<float>());
        break;
    }
    case 60:
    {
        component->setMotorSpeed(*buffer.popp<float>());
        break;
    }
    }
}

void physics2d::processWheelJoint2DComponent(bridge::ComponentType type, char nodeOp, char op, ArrayBuffer &buffer, Node *node)
{
    if (nodeOp == 1)
    {
        node->addComponent<physics2d::WheelJoint2DComponent>();
        return;
    }
    else if (nodeOp == 2)
    {
        node->removeComponent<physics2d::WheelJoint2DComponent>();
        return;
    }

    WheelJoint2DComponent *component = node->getComponent<WheelJoint2DComponent>();
    if (op <= 50)
    {
        physics2d::processJoint2DComponent(type, nodeOp, op, buffer, node, component);
        return;
    }

    switch (op)
    {
    case 51:
    {
        component->setEnableSpring(*buffer.popp<char>());
        break;
    }
    case 52:
    {
        component->setHertz(*buffer.popp<float>());
        break;
    }
    case 53:
    {
        component->setDampingRatio(*buffer.popp<float>());
        break;
    }
    case 54:
    {
        component->setEnableLimit(*buffer.popp<char>());
        break;
    }
    case 55:
    {
        component->setLowerTranslation(*buffer.popp<float>());
        break;
    }
    case 56:
    {
        component->setUpperTranslation(*buffer.popp<float>());
        break;
    }
    case 57:
    {
        component->setEnableMotor(*buffer.popp<char>());
        break;
    }
    case 58:
    {
        component->setMaxMotorTorque(*buffer.popp<float>());
        break;
    }
    case 59:
    {
        component->setMotorSpeed(*buffer.popp<float>());
        break;
    }
    default:
        break;
    }
}

void physics2d::processWeldJoint2DComponent(bridge::ComponentType type, char nodeOp, char op, ArrayBuffer &buffer, Node *node)
{
    if (nodeOp == 1)
    {
        node->addComponent<physics2d::WeldJoint2DComponent>();
        return;
    }
    else if (nodeOp == 2)
    {
        node->removeComponent<physics2d::WeldJoint2DComponent>();
        return;
    }

    WeldJoint2DComponent *component = node->getComponent<WeldJoint2DComponent>();
    if (op <= 50)
    {
        physics2d::processJoint2DComponent(type, nodeOp, op, buffer, node, component);
        return;
    }

    switch (op)
    {
    case 51:
    {
        component->setLinearHertz(*buffer.popp<float>());
        break;
    }
    case 52:
    {
        component->setAngularHertz(*buffer.popp<float>());
        break;
    }
    case 53:
    {
        component->setLinearDampingRatio(*buffer.popp<float>());
        break;
    }
    case 54:
    {
        component->setAngularDampingRatio(*buffer.popp<float>());
        break;
    }
    default:
        SQ_ASSERT(false);
    }
}

void physics2d::processMouseJoint2DComponent(bridge::ComponentType type, char nodeOp, char op, ArrayBuffer &buffer, Node *node)
{
    if (nodeOp == 1)
    {
        node->addComponent<physics2d::MouseJoint2DComponent>();
        return;
    }
    else if (nodeOp == 2)
    {
        node->removeComponent<physics2d::MouseJoint2DComponent>();
        return;
    }

    MouseJoint2DComponent *component = node->getComponent<MouseJoint2DComponent>();
    if (op <= 10)
    {
        physics2d::processJoint2DComponent(type, nodeOp, op, buffer, node, component);
        return;
    }

    switch (op)
    {
    case 51:
    {
        Vec2 vec;
        vec.x = *buffer.popp<float>();
        vec.y = *buffer.popp<float>();
        component->setTarget(vec);
        break;
    }
    case 52:
    {
        component->setHertz(*buffer.popp<float>());
        break;
    }
    case 53:
    {
        component->setDampingRatio(*buffer.popp<float>());
        break;
    }
    case 54:
    {
        component->setMaxForce(*buffer.popp<float>());
        break;
    }
    default:
        SQ_ASSERT(false);
    }
}

void physics2d::processMotorJoint2DComponent(bridge::ComponentType type, char nodeOp, char op, ArrayBuffer &buffer, Node *node)
{
    if (nodeOp == 1)
    {
        node->addComponent<physics2d::MotorJoint2DComponent>();
        return;
    }
    else if (nodeOp == 2)
    {
        node->removeComponent<physics2d::MotorJoint2DComponent>();
        return;
    }

    MotorJoint2DComponent *component = node->getComponent<MotorJoint2DComponent>();
    if (op <= 50)
    {
        physics2d::processJoint2DComponent(type, nodeOp, op, buffer, node, component);
        return;
    }

    switch (op)
    {
    case 51:
    {
        component->setMaxForce(*buffer.popp<float>());
        break;
    }
    case 52:
    {
        component->setMaxTorque(*buffer.popp<float>());
        break;
    }
    case 53:
    {
        component->setCorrectionFactor(*buffer.popp<float>());
        break;
    }
    default:
        break;
    }
}

void physics2d::processPBD2DComponent(bridge::ComponentType type, char nodeOp, char op, ArrayBuffer &buffer, Node *node)
{
    if (nodeOp == 1)
    {
        node->addComponent<physics2d::PBD2DComponent>();
        return;
    }
    else if (nodeOp == 2)
    {
        node->removeComponent<physics2d::PBD2DComponent>();
        return;
    }

    physics2d::PBD2DComponent *component = node->getComponent<physics2d::PBD2DComponent>();
    switch (op)
    {
    case 1:
    {
        int &numPoints = *buffer.popp<int>();
        component->create((float *)buffer.readBuffer(numPoints * 8), (float *)buffer.readBuffer(numPoints * 4), numPoints);
        break;
    }
    case 2:
    {
        int &pointIndex = *buffer.popp<int>();
        Vec2 velocity;
        velocity.x = *buffer.popp<float>();
        velocity.y = *buffer.popp<float>();
        component->setLinearVelocity(pointIndex, velocity);
        break;
    }
    case 3:
    {
        int &pointIndex = *buffer.popp<int>();
        Vec2 pos;
        pos.x = *buffer.popp<float>();
        pos.y = *buffer.popp<float>();
        component->move(pointIndex, pos);
        break;
    }
    case 4:
        component->reset();
        break;
    case 5:
    {
        phxy::PBDTuning tuning;
        int &state = *buffer.popp<int>();
        if (state & (1 << 1))
        {
            tuning.stretchingModel = phxy::PBDStretchingModel(*buffer.popp<char>());
        }
        if (state & (1 << 2))
        {
            tuning.bendingModel = phxy::PBDBendingModel(*buffer.popp<char>());
        }
        if (state & (1 << 3))
        {
            tuning.damping = *buffer.popp<float>();
        }

        if (state & (1 << 4))
        {
            tuning.stretchStiffness = *buffer.popp<float>();
        }

        if (state & (1 << 5))
        {
            tuning.stretchHertz = *buffer.popp<float>();
        }

        if (state & (1 << 6))
        {
            tuning.stretchDamping = *buffer.popp<float>();
        }

        if (state & (1 << 7))
        {
            tuning.bendStiffness = *buffer.popp<float>();
        }

        if (state & (1 << 8))
        {
            tuning.bendHertz = *buffer.popp<float>();
        }

        if (state & (1 << 9))
        {
            tuning.bendDamping = *buffer.popp<float>();
        }

        if (state & (1 << 10))
        {
            tuning.isometric = *buffer.popp<char>();
        }

        if (state & (1 << 11))
        {
            tuning.fixedEffectiveMass = *buffer.popp<char>();
        }

        if (state & (1 << 12))
        {
            tuning.warmStart = *buffer.popp<char>();
        }
        component->setTuning(tuning);
        break;
    }
    }
}

static void createRevoluteJoint(physics2d::JointConnect2DComponent *component, ArrayBuffer &buffer, Node *node)
{
    bool createType = *buffer.popp<char>() == 1;
    int &state = *buffer.popp<int>();
    Node *nodeA = bridge::JsToNativeObjectManager::getInstance()->getById<Node>(*buffer.popp<int>());
    Node *nodeB = bridge::JsToNativeObjectManager::getInstance()->getById<Node>(*buffer.popp<int>());

    phxy::SqRevoluteJointDef def;
    phxy::SqRevoluteJoint *joint = nullptr;
    if (!createType)
    {
        joint = static_cast<phxy::SqRevoluteJoint *>(component->findJoint(physics2d::JointType::REVOLUTE, nodeA, nodeB));
    }

    if (state & (1 << 1))
    {
        if (createType)
        {
            def.localFrameA.p.x = *buffer.popp<float>();
            def.localFrameA.p.y = *buffer.popp<float>();
        }
        else
        {
            phxy::SqTransform localFrameA = joint->getLocalFrameATransform();
            localFrameA.p.x = *buffer.popp<float>();
            localFrameA.p.y = *buffer.popp<float>();
            joint->setLocalFrameATransform(localFrameA);
        }
    }

    if (state & (1 << 2))
    {
        if (createType)
        {
            def.localFrameB.p.x = *buffer.popp<float>();
            def.localFrameB.p.y = *buffer.popp<float>();
        }
        else
        {
            phxy::SqTransform localFrameB = joint->getLocalFrameBTransform();
            localFrameB.p.x = *buffer.popp<float>();
            localFrameB.p.y = *buffer.popp<float>();
            joint->setLocalFrameBTransform(localFrameB);
        }
    }

    if (state & (1 << 3))
    {
        if (createType)
        {
            def.targetAngle = *buffer.popp<float>();
        }
        else
        {
            joint->setTargetAngle(Math::angleToRadian(*buffer.popp<float>()));
        }
    }

    if (state & (1 << 4))
    {
        if (createType)
        {
            def.enableSpring = *buffer.popp<char>();
        }
        else
        {
            joint->setEnableLimit(*buffer.popp<char>());
        }
    }

    if (state & (1 << 5))
    {
        if (createType)
        {
            def.hertz = *buffer.popp<float>();
        }
        else
        {
            joint->setSpringHertz(*buffer.popp<float>());
        }
    }

    if (state & (1 << 6))
    {
        if (createType)
        {
            def.dampingRatio = *buffer.popp<float>();
        }
        else
        {
            joint->setSpringDampingRatio(*buffer.popp<float>());
        }
    }

    if (state & (1 << 7))
    {
        if (createType)
        {
            def.enableLimit = *buffer.popp<char>();
        }
        else
        {
            joint->setEnableLimit(*buffer.popp<char>());
        }
    }

    if (state & (1 << 8))
    {
        if (createType)
        {
            def.lowerAngle = *buffer.popp<float>();
        }
        else
        {
            joint->setLimits(Math::angleToRadian(*buffer.popp<float>()), joint->getUpperLimit());
        }
    }

    if (state & (1 << 9))
    {
        if (createType)
        {
            def.upperAngle = *buffer.popp<float>();
        }
        else
        {
            joint->setLimits(joint->getLowerLimit(), Math::angleToRadian(*buffer.popp<float>()));
        }
    }

    if (state & (1 << 10))
    {
        if (createType)
        {
            def.enableMotor = *buffer.popp<char>();
        }
        else
        {
            joint->setEnableMotor(*buffer.popp<char>());
        }
    }

    if (state & (1 << 11))
    {
        if (createType)
        {
            def.maxMotorTorque = *buffer.popp<float>();
        }
        else
        {
            joint->setMaxMotorTorque(*buffer.popp<float>());
        }
    }

    if (state & (1 << 12))
    {
        if (createType)
        {
            def.motorSpeed = *buffer.popp<float>();
        }
        else
        {
            joint->setMotorSpeed(*buffer.popp<float>());
        }
    }

    if (state & (1 << 13))
    {
        Vec2 vec;
        vec.x = *buffer.popp<float>();
        vec.y = *buffer.popp<float>();
        // def.localAnchorAPivot = vec;
    }

    if (state & (1 << 14))
    {
        Vec2 vec;
        vec.x = *buffer.popp<float>();
        vec.y = *buffer.popp<float>();
        // def.localAnchorBPivot = vec;
    }

    if (state & (1 << 15))
    {
        if (createType)
        {
            def.collideConnected = *buffer.popp<char>();
        }
        else
        {
            joint->setCollideConnected(*buffer.popp<char>());
        }
    }

    if (state & (1 << 16))
    {
        if (createType)
        {
            def.localFrameA.q = phxy::SqRot(Math::angleToRadian(*buffer.popp<float>()));
        }
        else
        {
            phxy::SqTransform localFrameA = joint->getLocalFrameATransform();
            localFrameA.q = phxy::SqRot(Math::angleToRadian(*buffer.popp<float>()));
            joint->setLocalFrameATransform(localFrameA);
        }
    }

    if (state & (1 << 17))
    {
        if (createType)
        {
            def.localFrameB.q = phxy::SqRot(Math::angleToRadian(*buffer.popp<float>()));
        }
        else
        {
            phxy::SqTransform localFrameB = joint->getLocalFrameBTransform();
            localFrameB.q = phxy::SqRot(Math::angleToRadian(*buffer.popp<float>()));
            joint->setLocalFrameBTransform(localFrameB);
        }
    }

    if (createType)
        component->create(physics2d::JointType::REVOLUTE, nodeA, nodeB, def);
}

static void createPrismaticJoint(physics2d::JointConnect2DComponent *component, ArrayBuffer &buffer, Node *node)
{
    bool createType = *buffer.popp<char>() == 1;
    int &state = *buffer.popp<int>();
    Node *nodeA = bridge::JsToNativeObjectManager::getInstance()->getById<Node>(*buffer.popp<int>());
    Node *nodeB = bridge::JsToNativeObjectManager::getInstance()->getById<Node>(*buffer.popp<int>());

    phxy::SqPrismaticJointDef def;
    phxy::SqPrismaticJoint *joint = nullptr;
    if (!createType)
    {
        joint = static_cast<phxy::SqPrismaticJoint *>(component->findJoint(physics2d::JointType::PRISMATIC, nodeA, nodeB));
        // printf("joint %p \n", joint);
    }

    if (state & (1 << 1))
    {
        if (createType)
        {
            def.localFrameA.p.x = *buffer.popp<float>();
            def.localFrameA.p.y = *buffer.popp<float>();
        }
        else
        {
            phxy::SqTransform localFrameA = joint->getLocalFrameATransform();
            localFrameA.p.x = *buffer.popp<float>();
            localFrameA.p.y = *buffer.popp<float>();
            joint->setLocalFrameATransform(localFrameA);
        }
    }

    if (state & (1 << 2))
    {
        if (createType)
        {
            def.localFrameB.p.x = *buffer.popp<float>();
            def.localFrameB.p.y = *buffer.popp<float>();
        }
        else
        {
            phxy::SqTransform localFrameB = joint->getLocalFrameBTransform();
            localFrameB.p.x = *buffer.popp<float>();
            localFrameB.p.y = *buffer.popp<float>();
            joint->setLocalFrameBTransform(localFrameB);
        }
    }

    if (state & (1 << 3))
    {
        // Vec2 vec;
        // vec.x = *buffer.popp<float>();
        // vec.y = *buffer.popp<float>();
        // def.localAxisA = vec;
    }

    if (state & (1 << 4))
    {
        // def.referenceAngle = *buffer.popp<float>();
    }

    if (state & (1 << 5))
    {
        if (createType)
        {
            def.enableSpring = *buffer.popp<char>();
        }
        else
        {
            joint->EnableSpring(*buffer.popp<char>());
        }
    }

    if (state & (1 << 6))
    {
        if (createType)
        {
            def.hertz = *buffer.popp<float>();
        }
        else
        {
            joint->SetSpringHertz(*buffer.popp<float>());
        }
    }

    if (state & (1 << 7))
    {
        if (createType)
        {
            def.dampingRatio = *buffer.popp<float>();
        }
        else
        {
            joint->SetSpringDampingRatio(*buffer.popp<float>());
        }
    }

    if (state & (1 << 8))
    {
        if (createType)
        {
            def.enableLimit = *buffer.popp<char>();
        }
        else
        {
            joint->EnableLimit(*buffer.popp<char>());
        }
    }

    if (state & (1 << 9))
    {
        if (createType)
        {
            def.lowerTranslation = *buffer.popp<float>();
        }
        else
        {
            joint->SetLimits(*buffer.popp<float>(), joint->GetUpperLimit());
        }
    }

    if (state & (1 << 10))
    {
        if (createType)
        {
            def.upperTranslation = *buffer.popp<float>();
        }
        else
        {
            joint->SetLimits(joint->GetLowerLimit(), *buffer.popp<float>());
        }
    }

    if (state & (1 << 11))
    {
        if (createType)
        {
            def.enableMotor = *buffer.popp<char>();
        }
        else
        {
            joint->EnableMotor(*buffer.popp<char>());
        }
    }

    if (state & (1 << 12))
    {
        if (createType)
        {
            def.maxMotorForce = *buffer.popp<float>();
        }
        else
        {
            joint->SetMaxMotorForce(*buffer.popp<float>());
        }
    }

    if (state & (1 << 13))
    {
        if (createType)
        {
            def.motorSpeed = *buffer.popp<float>();
        }
        else
        {
            joint->SetMotorSpeed(*buffer.popp<float>());
        }
    }
    if (state & (1 << 14))
    {
        if (createType)
        {
            def.collideConnected = *buffer.popp<char>();
        }
        else
        {
            joint->setCollideConnected(*buffer.popp<char>());
        }
    }

    if (state & (1 << 15))
    {
        if (createType)
        {
            def.localFrameA.q = phxy::SqRot(Math::angleToRadian(*buffer.popp<float>()));
        }
        else
        {
            phxy::SqTransform localFrameA = joint->getLocalFrameATransform();
            localFrameA.q = phxy::SqRot(Math::angleToRadian(*buffer.popp<float>()));
            joint->setLocalFrameATransform(localFrameA);
        }
    }

    if (state & (1 << 16))
    {
        if (createType)
        {
            def.localFrameB.q = phxy::SqRot(Math::angleToRadian(*buffer.popp<float>()));
        }
        else
        {
            phxy::SqTransform localFrameB = joint->getLocalFrameBTransform();
            localFrameB.q = phxy::SqRot(Math::angleToRadian(*buffer.popp<float>()));
            joint->setLocalFrameATransform(localFrameB);
        }
    }

    if (createType)
        component->create(physics2d::JointType::PRISMATIC, nodeA, nodeB, def);
}

static void createMouseJoint(physics2d::JointConnect2DComponent *component, ArrayBuffer &buffer, Node *node)
{
    bool createType = *buffer.popp<char>() == 1;
    int &state = *buffer.popp<int>();
    Node *nodeA = bridge::JsToNativeObjectManager::getInstance()->getById<Node>(*buffer.popp<int>());
    Node *nodeB = bridge::JsToNativeObjectManager::getInstance()->getById<Node>(*buffer.popp<int>());

    phxy::SqMouseJointDef def;
    phxy::SqMouseJoint *joint = nullptr;
    if (!createType)
    {
        joint = static_cast<phxy::SqMouseJoint *>(component->findJoint(physics2d::JointType::MOUSE, nodeA, nodeB));
    }

    if (state & (1 << 1))
    {
        // Vec2 vec;
        // vec.x = *buffer.popp<float>();
        // vec.y = *buffer.popp<float>();
        // def.target = vec;
    }

    if (state & (1 << 2))
    {
        if (createType)
        {
            def.hertz = *buffer.popp<float>();
        }
        else
        {
            joint->SetSpringHertz(*buffer.popp<float>());
        }
    }

    if (state & (1 << 3))
    {
        if (createType)
        {
            def.dampingRatio = *buffer.popp<char>();
        }
        else
        {
            joint->SetSpringDampingRatio(*buffer.popp<char>());
        }
    }

    if (state & (1 << 4))
    {
        if (createType)
        {
            def.maxForce = *buffer.popp<float>();
        }
        else
        {
            joint->SetMaxForce(*buffer.popp<float>());
        }
    }

    if (state & (1 << 7))
    {
        if (createType)
        {
            def.dampingRatio = *buffer.popp<float>();
        }
        else
        {
            joint->SetSpringDampingRatio(*buffer.popp<float>());
        }
    }

    if (state & (1 << 8))
    {
        if (createType)
        {
            def.collideConnected = *buffer.popp<char>();
        }
        else
        {
            joint->setCollideConnected(*buffer.popp<char>());
        }
    }

    if (createType)
        component->create(physics2d::JointType::MOUSE, nodeA, nodeB, def);
}

static void createWeldJoint(physics2d::JointConnect2DComponent *component, ArrayBuffer &buffer, Node *node)
{
    bool createType = *buffer.popp<char>() == 1;
    int &state = *buffer.popp<int>();
    Node *nodeA = bridge::JsToNativeObjectManager::getInstance()->getById<Node>(*buffer.popp<int>());
    Node *nodeB = bridge::JsToNativeObjectManager::getInstance()->getById<Node>(*buffer.popp<int>());

    phxy::SqWeldJointDef def;
    phxy::SqWeldJoint *joint = nullptr;
    if (!createType)
    {
        joint = static_cast<phxy::SqWeldJoint *>(component->findJoint(physics2d::JointType::WELD, nodeA, nodeB));
    }

    if (state & (1 << 1))
    {
        if (createType)
        {
            def.localFrameA.p.x = *buffer.popp<float>();
            def.localFrameA.p.y = *buffer.popp<float>();
        }
        else
        {
            phxy::SqTransform localFrameA = joint->getLocalFrameATransform();
            localFrameA.p.x = *buffer.popp<float>();
            localFrameA.p.y = *buffer.popp<float>();
            joint->setLocalFrameATransform(localFrameA);
        }
    }

    if (state & (1 << 2))
    {
        if (createType)
        {
            def.localFrameB.p.x = *buffer.popp<float>();
            def.localFrameB.p.y = *buffer.popp<float>();
        }
        else
        {
            phxy::SqTransform localFrameB = joint->getLocalFrameBTransform();
            localFrameB.p.x = *buffer.popp<float>();
            localFrameB.p.y = *buffer.popp<float>();
            joint->setLocalFrameATransform(localFrameB);
        }
    }

    if (state & (1 << 3))
    {
        // def.referenceAngle = *buffer.popp<float>();
    }

    if (state & (1 << 4))
    {
        if (createType)
        {
            def.linearHertz = *buffer.popp<float>();
        }
        else
        {
            joint->SetLinearHertz(*buffer.popp<float>());
        }
    }

    if (state & (1 << 5))
    {
        if (createType)
        {
            def.angularHertz = *buffer.popp<float>();
        }
        else
        {
            joint->SetAngularHertz(*buffer.popp<float>());
        }
    }

    if (state & (1 << 6))
    {
        if (createType)
        {
            def.linearDampingRatio = *buffer.popp<float>();
        }
        else
        {
            joint->SetLinearDampingRatio(*buffer.popp<float>());
        }
    }

    if (state & (1 << 7))
    {
        if (createType)
        {
            def.angularDampingRatio = *buffer.popp<float>();
        }
        else
        {
            joint->SetAngularDampingRatio(*buffer.popp<float>());
        }
    }

    if (state & (1 << 8))
    {
        if (createType)
        {
            def.collideConnected = *buffer.popp<char>();
        }
        else
        {
            joint->setCollideConnected(*buffer.popp<char>());
        }
    }

    if (createType)
        component->create(physics2d::JointType::WELD, nodeA, nodeB, def);
}

static void createWheelJoint(physics2d::JointConnect2DComponent *component, ArrayBuffer &buffer, Node *node)
{
    bool createType = *buffer.popp<char>() == 1;
    int &state = *buffer.popp<int>();
    Node *nodeA = bridge::JsToNativeObjectManager::getInstance()->getById<Node>(*buffer.popp<int>());
    Node *nodeB = bridge::JsToNativeObjectManager::getInstance()->getById<Node>(*buffer.popp<int>());

    phxy::SqWheelJoint *joint = nullptr;
    if (!createType)
    {
        joint = static_cast<phxy::SqWheelJoint *>(component->findJoint(physics2d::JointType::WHEEL, nodeA, nodeB));
    }

    phxy::SqWheelJointDef def;

    if (state & (1 << 1))
    {
        if (createType)
        {
            def.localFrameA.p.x = *buffer.popp<float>();
            def.localFrameA.p.y = *buffer.popp<float>();
        }
        else
        {
            phxy::SqTransform localFrameA = joint->getLocalFrameATransform();
            localFrameA.p.x = *buffer.popp<float>();
            localFrameA.p.y = *buffer.popp<float>();
            joint->setLocalFrameATransform(localFrameA);
        }
    }

    if (state & (1 << 2))
    {
        if (createType)
        {
            def.localFrameB.p.x = *buffer.popp<float>();
            def.localFrameB.p.y = *buffer.popp<float>();
        }
        else
        {
            phxy::SqTransform localFrameB = joint->getLocalFrameBTransform();
            localFrameB.p.x = *buffer.popp<float>();
            localFrameB.p.y = *buffer.popp<float>();
            joint->setLocalFrameBTransform(localFrameB);
        }
    }

    if (state & (1 << 3))
    {
        // Vec2 vec;
        // vec.x = *buffer.popp<float>();
        // vec.y = *buffer.popp<float>();
        // def.localAxisA = vec;
    }

    if (state & (1 << 4))
    {
        if (createType)
        {
            def.enableSpring = *buffer.popp<char>();
        }
        else
        {
            joint->EnableSpring(*buffer.popp<char>());
        }
    }

    if (state & (1 << 5))
    {
        if (createType)
        {
            def.hertz = *buffer.popp<float>();
        }
        else
        {
            joint->SetSpringHertz(*buffer.popp<float>());
        }
    }

    if (state & (1 << 6))
    {
        if (createType)
        {
            def.dampingRatio = *buffer.popp<float>();
        }
        else
        {
            joint->SetSpringDampingRatio(*buffer.popp<float>());
        }
    }

    if (state & (1 << 7))
    {
        if (createType)
        {
            def.enableLimit = *buffer.popp<char>();
        }
        else
        {
            joint->EnableLimit(*buffer.popp<char>());
        }
    }

    if (state & (1 << 8))
    {
        if (createType)
        {
            def.lowerTranslation = *buffer.popp<float>();
        }
        else
        {
            joint->SetLimits(*buffer.popp<float>(), joint->GetLowerLimit());
        }
    }

    if (state & (1 << 9))
    {
        if (createType)
        {
            def.upperTranslation = *buffer.popp<float>();
        }
        else
        {
            joint->SetLimits(joint->GetUpperLimit(), *buffer.popp<float>());
        }
    }

    if (state & (1 << 10))
    {
        if (createType)
        {
            def.enableMotor = *buffer.popp<char>();
        }
        else
        {
            joint->EnableMotor(*buffer.popp<char>());
        }
    }

    if (state & (1 << 11))
    {
        if (createType)
        {
            def.maxMotorTorque = *buffer.popp<float>();
        }
        else
        {
            joint->SetMaxMotorTorque(*buffer.popp<float>());
        }
    }

    if (state & (1 << 12))
    {
        float angle = Math::angleToRadian(*buffer.popp<float>());
        if (createType)
        {
            def.motorSpeed = angle;
        }
        else
        {
            joint->SetMotorSpeed(angle);
        }
    }

    if (state & (1 << 13))
    {
        if (createType)
        {
            def.collideConnected = *buffer.popp<char>();
        }
        else
        {
            joint->setCollideConnected(*buffer.popp<char>());
        }
    }

    if (createType)
        component->create(physics2d::JointType::WHEEL, nodeA, nodeB, def);
}

static void createMotorJoint(physics2d::JointConnect2DComponent *component, ArrayBuffer &buffer, Node *node)
{
    bool createType = *buffer.popp<char>() == 1;
    int &state = *buffer.popp<int>();
    Node *nodeA = bridge::JsToNativeObjectManager::getInstance()->getById<Node>(*buffer.popp<int>());
    Node *nodeB = bridge::JsToNativeObjectManager::getInstance()->getById<Node>(*buffer.popp<int>());

    phxy::SqMotorJointDef def;
    phxy::SqMotorJoint *joint = nullptr;
    if (!createType)
    {
        joint = static_cast<phxy::SqMotorJoint *>(component->findJoint(physics2d::JointType::MOTOR, nodeA, nodeB));
    }
    if (state & (1 << 1))
    {
        // Vec2 vec;
        // vec.x = *buffer.popp<float>();
        // vec.y = *buffer.popp<float>();
        // def.linearOffset = vec;
    }

    if (state & (1 << 2))
    {
        // def.angularOffset = *buffer.popp<float>();
    }

    if (state & (1 << 3))
    {
        if (createType)
        {
            def.maxForce = *buffer.popp<float>();
        }
        else
        {
            joint->SetMaxForce(*buffer.popp<float>());
        }
    }

    if (state & (1 << 4))
    {
        if (createType)
        {
            def.maxTorque = *buffer.popp<float>();
        }
        else
        {
            joint->SetMaxTorque(*buffer.popp<float>());
        }
    }

    if (state & (1 << 5))
    {
        if (createType)
        {
            def.correctionFactor = *buffer.popp<float>();
        }
        else
        {
            joint->SetCorrectionFactor(*buffer.popp<float>());
        }
    }
    if (state & (1 << 6))
    {
        if (createType)
        {
            def.collideConnected = *buffer.popp<char>();
        }
        else
        {
            joint->setCollideConnected(*buffer.popp<char>());
        }
    }
    component->create(physics2d::JointType::MOUSE, nodeA, nodeB, def);
}

static void createDistanceJoint(physics2d::JointConnect2DComponent *component, ArrayBuffer &buffer, Node *node)
{
    bool createType = *buffer.popp<char>() == 1;
    int &state = *buffer.popp<int>();
    Node *nodeA = bridge::JsToNativeObjectManager::getInstance()->getById<Node>(*buffer.popp<int>());
    Node *nodeB = bridge::JsToNativeObjectManager::getInstance()->getById<Node>(*buffer.popp<int>());

    phxy::SqDistanceJointDef def;
    phxy::SqDistanceJoint *joint = nullptr;
    if (!createType)
    {
        joint = static_cast<phxy::SqDistanceJoint *>(component->findJoint(physics2d::JointType::MOTOR, nodeA, nodeB));
    }
    if (state & (1 << 1))
    {
        if (createType)
        {
            def.localFrameA.p.x = *buffer.popp<float>();
            def.localFrameA.p.y = *buffer.popp<float>();
        }
        else
        {
            phxy::SqTransform localFrameA = joint->getLocalFrameATransform();
            localFrameA.p.x = *buffer.popp<float>();
            localFrameA.p.y = *buffer.popp<float>();
            joint->setLocalFrameATransform(localFrameA);
        }
    }

    if (state & (1 << 2))
    {
        if (createType)
        {

            def.localFrameB.p.x = *buffer.popp<float>();
            def.localFrameB.p.y = *buffer.popp<float>();
        }
        else
        {
            phxy::SqTransform localFrameB = joint->getLocalFrameBTransform();
            localFrameB.p.x = *buffer.popp<float>();
            localFrameB.p.y = *buffer.popp<float>();
            joint->setLocalFrameBTransform(localFrameB);
        }
    }

    if (state & (1 << 3))
    {
        if (createType)
        {
            def.length = *buffer.popp<float>();
        }
        else
        {
            joint->SetLength(*buffer.popp<float>());
        }
    }

    if (state & (1 << 4))
    {
        if (createType)
        {
            def.enableSpring = *buffer.popp<char>();
        }
        else
        {
            joint->EnableSpring(*buffer.popp<char>());
        }
    }

    if (state & (1 << 5))
    {
        if (createType)
        {
            def.hertz = *buffer.popp<float>();
        }
        else
        {
            joint->SetSpringHertz(*buffer.popp<float>());
        }
    }

    if (state & (1 << 6))
    {
        if (createType)
        {
            def.dampingRatio = *buffer.popp<float>();
        }
        else
        {
            joint->SetSpringDampingRatio(*buffer.popp<float>());
        }
    }

    if (state & (1 << 7))
    {
        if (createType)
        {
            def.enableLimit = *buffer.popp<char>();
        }
        else
        {
            joint->EnableLimit(*buffer.popp<char>());
        }
    }

    if (state & (1 << 8))
    {
        if (createType)
        {
            def.minLength = *buffer.popp<float>();
        }
        else
        {
            joint->SetLengthRange(*buffer.popp<float>(), joint->GetMaxLength());
        }
    }

    if (state & (1 << 9))
    {
        if (createType)
        {
            def.maxLength = *buffer.popp<float>();
        }
        else
        {
            joint->SetLengthRange(joint->GetMinLength(), *buffer.popp<float>());
        }
    }

    if (state & (1 << 10))
    {
        if (createType)
        {
            def.enableMotor = *buffer.popp<char>();
        }
        else
        {
            joint->EnableMotor(*buffer.popp<char>());
        }
    }

    if (state & (1 << 11))
    {
        if (createType)
        {
            def.maxMotorForce = *buffer.popp<float>();
        }
        else
        {
            joint->SetMaxMotorForce(*buffer.popp<float>());
        }
    }

    if (state & (1 << 12))
    {
        if (createType)
        {
            def.motorSpeed = *buffer.popp<float>();
        }
        else
        {
            joint->SetMotorSpeed(*buffer.popp<float>());
        }
    }

    if (state & (1 << 13))
    {
        if (createType)
        {
            def.collideConnected = *buffer.popp<char>();
        }
        else
        {
            joint->setCollideConnected(*buffer.popp<char>());
        }
    }

    if (createType)
        component->create(physics2d::JointType::DISTANCE, nodeA, nodeB, def);
}

void physics2d::processJointConnect2DComponent(bridge::ComponentType type, char nodeOp, char op, ArrayBuffer &buffer, Node *node)
{
    if (nodeOp == 1)
    {
        node->addComponent<physics2d::JointConnect2DComponent>();
        return;
    }
    else if (nodeOp == 2)
    {
        node->removeComponent<physics2d::JointConnect2DComponent>();
        return;
    }

    JointConnect2DComponent *compnent = node->getComponent<JointConnect2DComponent>();
    switch (op)
    {
    case 1:
    {
        char type = *buffer.popp<char>();
        Node *nodeA = bridge::JsToNativeObjectManager::getInstance()->getById<Node>(*buffer.popp<int>());
        Node *nodeB = bridge::JsToNativeObjectManager::getInstance()->getById<Node>(*buffer.popp<int>());
        compnent->remove(JointType(type), nodeA, nodeB);
        break;
    }
    case 2:
        compnent->removeAll();
        break;
    case 3:
        createRevoluteJoint(compnent, buffer, node);
        break;
    case 4:
        createPrismaticJoint(compnent, buffer, node);
        break;
    case 5:
        createMouseJoint(compnent, buffer, node);
        break;
    case 6:
        createWeldJoint(compnent, buffer, node);
        break;
    case 7:
        createWheelJoint(compnent, buffer, node);
        break;
    case 8:
        createMotorJoint(compnent, buffer, node);
        break;
    case 9:
        createDistanceJoint(compnent, buffer, node);
        break;
    }
}

void physics2d::processWallSlide2DComponent(bridge::ComponentType type, char nodeOp, char op, ArrayBuffer &buffer, Node *node)
{
    if (nodeOp == 1)
    {
        node->addComponent<physics2d::WallSlide2DComponent>();
        return;
    }
    else if (nodeOp == 2)
    {
        node->removeComponent<physics2d::WallSlide2DComponent>();
        return;
    }

    WallSlide2DComponent *compnent = node->getComponent<WallSlide2DComponent>();
    switch (op)
    {
    case 1:
    {
        compnent->moveForward();
        break;
    }
    case 2:
    {
        compnent->moveBack();
        break;
    }
    case 3:
    {
        compnent->stop();
        break;
    }
    case 4:
    {
        compnent->setSpeed(*buffer.popp<float>());
        break;
    }
    case 5:
    {
        compnent->setSlop(*buffer.popp<float>(), *buffer.popp<float>());
        break;
    }
    case 6:
    {
        compnent->setHasEvent(*buffer.popp<char>());
        break;
    }
    }
}