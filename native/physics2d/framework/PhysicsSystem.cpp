#include "PhysicsSystem.h"
#include "PyhsicsSystemBridge.h"
#include "../../engine/scene/NodeBridge.h"
#include "bind_process_component.h"

using namespace physics2d;

static physics2d::PhysicsSystem *_inst = nullptr;


PhysicsSystem::PhysicsSystem()
{
    _inst = this;
}

void PhysicsSystem::init()
{
    world = new PhysicsWorld();
    bridge::DispatchManager::getInstance()->registerObjectBridge(bridge::ObjectType::PyhsicsSystem, new physics2d::PyhsicsSystemBridge);
    bridge::NodeBridge *nodeBridge = bridge::NodeBridge::getInstance();
    nodeBridge->registerNodeProcessComponent(bridge::ComponentType::BoxCollision2DComponent, processPhysicsBoxShape2DComponent);
    nodeBridge->registerNodeProcessComponent(bridge::ComponentType::CapsuleCollision2DComponent, processPhysicsCapsuleShape2DComponent);
    nodeBridge->registerNodeProcessComponent(bridge::ComponentType::ChainShape2DComponent, processPhysicsChainShape2DComponent);
    nodeBridge->registerNodeProcessComponent(bridge::ComponentType::CircleCollision2DComponent, processPhysicsCircleCollision2DComponent);
    nodeBridge->registerNodeProcessComponent(bridge::ComponentType::PolygonCollision2DComponent, processPolygonCollision2DComponent);
    nodeBridge->registerNodeProcessComponent(bridge::ComponentType::RigidBody2DComponent, processPhysicsRigidBody2DComponent);
    nodeBridge->registerNodeProcessComponent(bridge::ComponentType::ParticlePhysics2DComponent, processPhysicsParticle2DComponent);
    nodeBridge->registerNodeProcessComponent(bridge::ComponentType::Character2DComponent, processCharacter2DComponent);
    nodeBridge->registerNodeProcessComponent(bridge::ComponentType::DistanceJoint2DComponent, processDistanceJoint2DComponent);
    nodeBridge->registerNodeProcessComponent(bridge::ComponentType::MotorJoint2DComponent, processMotorJoint2DComponent);
    nodeBridge->registerNodeProcessComponent(bridge::ComponentType::MouseJoint2DComponent, processMouseJoint2DComponent);
    nodeBridge->registerNodeProcessComponent(bridge::ComponentType::PrismaticJoint2DComponent, processPrismaticJoint2DComponent);
    nodeBridge->registerNodeProcessComponent(bridge::ComponentType::RevoluteJoint2DComponent, processRevoluteJoint2DComponent);
    nodeBridge->registerNodeProcessComponent(bridge::ComponentType::WeldJoint2DComponent, processWeldJoint2DComponent);
    nodeBridge->registerNodeProcessComponent(bridge::ComponentType::WheelJoint2DComponent, processWheelJoint2DComponent);
    nodeBridge->registerNodeProcessComponent(bridge::ComponentType::PBD2DComponent, processPBD2DComponent);
    nodeBridge->registerNodeProcessComponent(bridge::ComponentType::JointConnect2DComponent, processJointConnect2DComponent);
    nodeBridge->registerNodeProcessComponent(bridge::ComponentType::WallSlide2DComponent, processWallSlide2DComponent);
}

PhysicsSystem::~PhysicsSystem()
{
    delete world;
    world = NULL;
}

void PhysicsSystem::setDebugDraw(bool enable, Node *node)
{
    world->setDebugDraw(enable, node);
}

void PhysicsSystem::setUnitsPerMeter(int pixleNum)
{
    unitsPerMeter = pixleNum;
}

PhysicsSystem *PhysicsSystem::getInstance()
{
    return _inst;
}

void PhysicsSystem::startUpdate()
{
    world->beforeStep();
}

void PhysicsSystem::update(float dt)
{
    dt *= 0.001;
    dt = Math::clamp(dt, 0.f, 0.033f);
    world->syncSceneToPhysics();
    world->step(dt);
    world->syncPhysicsToScene();
}

void PhysicsSystem::enableNextStep(bool b)
{
    world->getPyxhWorld()->enableNextStep(b);
}

void PhysicsSystem::nextStep()
{
    world->getPyxhWorld()->nextStep();
}

void PhysicsSystem::postUpdate()
{
    world->drawDebug();
}

void PhysicsSystem::SetDrawFlag(unsigned int flag)
{
#ifdef PHYSICS_DEBUG
    world->getDebugDraw()->SetDrawFlag(flag);
#endif
}