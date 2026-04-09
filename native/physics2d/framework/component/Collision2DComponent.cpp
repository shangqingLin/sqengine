#include "Collision2DComponent.h"
#include "RigidBody2DComponent.h"
#include "../PhysicsSystem.h"
#include "../../../engine/2d/components/Transform2DComponent.h"

using namespace physics2d;

Collision2DComponent::Collision2DComponent()
{
}

void Collision2DComponent::onAwake()
{
   createBody();
}

void Collision2DComponent::createBody()
{
   RigidBody2DComponent *rigidComponent = node->getComponent<RigidBody2DComponent>();
   if (rigidComponent)
   {
      bodyId = rigidComponent->getBody();
      ownerBody = false;
      // printf("========create from rigid %d \n",node->nativeId);
   }
   else
   {
      ownerBody = true;
      phxy::SqBodyDef def;
      def.type = phxy::SqBodyType::sq_staticBody;
      def.userData = this->node;
      def.isEnabled = node->activeInHierarchy();
      float unit = PhysicsSystem::getInstance()->getUnitsPerMeter();
      const Mat3 &mat = node->getComponent<Transform2DComponent>()->getWorldTransform();

      // printf(" node %d collisoin component pos %f %f \n", node->nativeId, mat.data[4], mat.data[5]);
      // mat.print();

      def.position.x = mat.data[4] / unit;
      def.position.y = mat.data[5] / unit;
      def.rotation.c = mat.data[0];
      def.rotation.s = mat.data[1];
      bodyId = PhysicsSystem::getInstance()->getWorld()->createBody(def);
      onNodeTransformChange();
      node->on(NodeEventType::TRASNFORM_CHANGE, std::bind(&Collision2DComponent::onNodeTransformChange, this));

      // printf("========create collision %p %d \n", PhysicsSystem::getInstance()->getWorld()->getBody(bodyId), node->nativeId);
   }
}

void Collision2DComponent::onNodeTransformChange()
{

   physics2d::PhysicsWorld *world = PhysicsSystem::getInstance()->getWorld();
   if (world->isLockSyncPhysicsToScene())
      return;

   int flag = 0;
   int nodeFlag = node->getChangeFlag();

   // printf("Collision2DComponent::onNodeTransformChange() %d \n", node->nativeId);

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
}

void Collision2DComponent::copyBaseDef(phxy::SqShapeDef &def)
{
   if (baseDef.has_value())
   {
      phxy::SqShapeDef &base = baseDef.value();
      def.userData = base.userData;
      def.material = base.material;
      def.density = base.density;
      def.filter = base.filter;
      def.isSensor = base.isSensor;
      def.sensorAABB = base.sensorAABB;
      def.customColor = base.customColor;
      baseDef.reset();
   }
}

void Collision2DComponent::setFilter(unsigned int categoryBits, unsigned int maskBits)
{
   if (shape)
   {
      phxy::SqShapeFilter f;
      f.categoryBits = categoryBits;
      f.maskBits = maskBits;
      shape->setFilter(f);
   }
   else
   {
      if (!baseDef.has_value())
      {
         baseDef = phxy::SqShapeDef();
      }
      baseDef.value().filter = phxy::SqShapeFilter{categoryBits, maskBits};
   }
}

void Collision2DComponent::getFilter(unsigned int &categoryBits, unsigned int &maskBits)
{
   if (shape)
   {
      phxy::SqShapeFilter filter = shape->getFilter();
      categoryBits = filter.categoryBits;
      maskBits = filter.maskBits;
   }
   else
   {
      if (baseDef.has_value())
      {
         categoryBits = baseDef.value().filter.categoryBits;
         maskBits = baseDef.value().filter.maskBits;
      }
   }
}

void Collision2DComponent::onEnable()
{
   if (ownerBody)
   {
      phxy::SqBody *body = PhysicsSystem::getInstance()->getWorld()->getBody(bodyId);
      body->enable();
   }
}

void Collision2DComponent::onDisable()
{
   if (ownerBody)
   {
      phxy::SqBody *body = PhysicsSystem::getInstance()->getWorld()->getBody(bodyId);
      body->disable();
   }
}

int Collision2DComponent::getBody()
{
   return bodyId;
}

void Collision2DComponent::setOffset(float x, float y)
{
   Vec2 p(x, y);
   offset = p;
}

void Collision2DComponent::setDensity(float density)
{
   if (shape)
   {
      shape->setDensity(density);
   }
   else
   {
      if (!baseDef.has_value())
      {
         baseDef = phxy::SqShapeDef();
      }
      baseDef.value().density = density;
   }
}

void Collision2DComponent::setFriction(float friction)
{
   if (shape)
   {
      shape->setFriction(friction);
   }
   else
   {
      if (!baseDef.has_value())
      {
         baseDef = phxy::SqShapeDef();
      }
      baseDef.value().material.friction = friction;
   }
}

void Collision2DComponent::setRestitution(float restitution)
{

   if (shape)
   {
      shape->setRestitution(restitution);
   }
   else
   {
      if (!baseDef.has_value())
      {
         baseDef = phxy::SqShapeDef();
      }
      baseDef.value().material.restitution = restitution;
   }
}

void Collision2DComponent::setTangentSpeed(float speed)
{
   if (shape)
   {
      shape->setTangentSpeed(speed);
   }
   else
   {
      if (!baseDef.has_value())
      {
         baseDef = phxy::SqShapeDef();
      }
      baseDef.value().material.tangentSpeed = speed;
   }
}

void Collision2DComponent::setSensor(bool b)
{
   if (shape)
   {
      shape->setSensor(b);
   }
   else
   {

      // 默认值就是false
      if (!b)
         return;

      if (!baseDef.has_value())
      {
         baseDef = phxy::SqShapeDef();
      }
      baseDef.value().isSensor = b;
   }
}

void Collision2DComponent::setSensorAABB(bool b)
{

   if (shape)
   {
      shape->sensorAABB(b);
   }
   else
   {
      if (!b)
         return;

      if (!baseDef.has_value())
      {
         baseDef = phxy::SqShapeDef();
      }
      baseDef.value().sensorAABB = b;
   }
}

void Collision2DComponent::setCustomColor(uint32_t color)
{
   if (shape)
   {
      shape->setCustomColor(color);
   }
   else
   {
      if (!baseDef.has_value())
      {
         baseDef = phxy::SqShapeDef();
      }
      baseDef.value().customColor = color;
   }
}

void Collision2DComponent::onBeginContact(const phxy::SqContactBeginEvent *event)
{
   if (beginContactCall.has_value())
   {
      beginContactCall.value()(event);
   }

   if (!(jsContactListener & 1))
      return;

   if (node->nativeId != -1)
   {
      bridge::NativeToJsObject &nativeToJs = PhysicsSystem::getInstance()->getWorld()->getNativeToJsObject();
      nativeToJs.beginOp(PhysicsWorld::bridgeOp_onCollisionBeginContact, true);

      phxy::SqShape *shapeB = event->shapeB;
      if (event->shapeA->getBody() != bodyId)
      {
         shapeB = event->shapeA;
      }
      Node *nodeB = (Node *)PhysicsSystem::getInstance()->getWorld()->getBody(shapeB->getBody())->getUserData();
      nativeToJs.writeOpArg(node->nativeId);
      nativeToJs.writeOpArg(nodeB->nativeId);
      nativeToJs.writeOpArg(event->contactId);
      unsigned int dataAdress = reinterpret_cast<unsigned int>(event);
      nativeToJs.writeOpArg(dataAdress);
      nativeToJs.endOp();
   }
}

void Collision2DComponent::setBeginContactCallback(std::function<physics2d::BeginContactCallback> beginContactCallback)
{
   this->beginContactCall = beginContactCallback;
   phxy::SqBody *body = PhysicsSystem::getInstance()->getWorld()->getBody(bodyId);
   body->enableBeginContactEvent(true);
}

void Collision2DComponent::offBeginContactCallback()
{
   phxy::SqBody *body = PhysicsSystem::getInstance()->getWorld()->getBody(bodyId);
   body->enableBeginContactEvent(false);
   this->beginContactCall.reset();
}

void Collision2DComponent::onEndContact(const phxy::SqContactEndEvent *event)
{
   if (endContactCall.has_value())
   {
      endContactCall.value()(event);
   }

   if (!(jsContactListener & 1))
      return;

   if (node->nativeId != -1)
   {
      bridge::NativeToJsObject &nativeToJs = PhysicsSystem::getInstance()->getWorld()->getNativeToJsObject();
      nativeToJs.beginOp(PhysicsWorld::bridgeOp_onCollisionEndContact, true);
      nativeToJs.writeOpArg(node->nativeId);
      phxy::SqShape *shapeB = event->shapeB;
      if (event->shapeA->getBody() != bodyId)
      {
         shapeB = event->shapeA;
      }
      Node *nodeB = (Node *)PhysicsSystem::getInstance()->getWorld()->getBody(shapeB->getBody())->getUserData();
      nativeToJs.writeOpArg(nodeB->nativeId);
      nativeToJs.writeOpArg(event->contactId);
      nativeToJs.endOp();
   }
}

void Collision2DComponent::setEndContactCallback(std::function<physics2d::EndContactCallback> endContactCallback)
{
   this->endContactCall = endContactCallback;
   phxy::SqBody *body = PhysicsSystem::getInstance()->getWorld()->getBody(bodyId);
   body->enableEndContactEvent(true);
}

void Collision2DComponent::offEndContactCallback()
{
   this->endContactCall.reset();
   phxy::SqBody *body = PhysicsSystem::getInstance()->getWorld()->getBody(bodyId);
   body->enableEndContactEvent(false);
}

void Collision2DComponent::onHitUpdateContact(const phxy::SqContactHitUpdateEvent *event)
{
   if (hitUpdateCall.has_value())
   {
      hitUpdateCall.value()(event);
   }
}

void Collision2DComponent::setHitUpdateContactCallback(std::function<physics2d::HitUpdateContactCallback> callback)
{
   hitUpdateCall = callback;
   phxy::SqBody *body = PhysicsSystem::getInstance()->getWorld()->getBody(bodyId);
   body->enableHitUpdateContactEvent(true);
}

void physics2d::preSolver(const phxy::SqContact *const contact, const phxy::SqManifold *const manifold, void *context)
{
   Collision2DComponent *component = (Collision2DComponent *)context;
   if (component->prevSolveContactCall.has_value())
   {
      component->prevSolveContactCall.value()(contact, manifold);
   }
}

void Collision2DComponent::setPreSolveCallback(std::function<physics2d::PreSolveCallback> preSolveCallback)
{
   this->prevSolveContactCall = preSolveCallback;
   phxy::SqBody *body = PhysicsSystem::getInstance()->getWorld()->getBody(bodyId);
   // body->setPreSolveCallback(&physics2d::preSolver, this);
}

// void physics2d::postSolver(const phxy::SqContact *const contact, const phxy::SqManifold *const manifold, void *context)
// {
//    Collision2DComponent *component = (Collision2DComponent *)context;
//    if (component->postSolveContactCall.has_value())
//    {
//       component->postSolveContactCall.value()(contact, manifold);
//       return;
//    }
// }

void Collision2DComponent::setPostSolveCallback(std::function<physics2d::PostSolveCallback> endSolveCallback)
{
   this->postSolveContactCall = endSolveCallback;
   phxy::SqBody *body = PhysicsSystem::getInstance()->getWorld()->getBody(bodyId);
   // body->setPostSolveCallback(&physics2d::postSolver, this);
}

void Collision2DComponent::onSensorBegin(const phxy::SqSensorBeginEvent *event)
{
   // printf("+++++++++++++++++++onSensorBegin %d \n", bodyId);

   phxy::SqBody *body = PhysicsSystem::getInstance()->getWorld()->getBody(event->shapeB->getBody());
   Node *nodeB = (Node *)body->getUserData();

   if (sensorBeginCallback.has_value())
   {
      sensorBeginCallback.value()(nodeB);
   }

   if (!(jsContactListener & 4)) // 4 = 1 << 2
      return;

   if (node->nativeId != -1 && nodeB->nativeId != -1)
   {
      bridge::NativeToJsObject &nativeToJs = PhysicsSystem::getInstance()->getWorld()->getNativeToJsObject();
      nativeToJs.beginOp(PhysicsWorld::bridgeOp_onCollisionSensorBegin, true);
      nativeToJs.writeOpArg(node->nativeId);
      nativeToJs.writeOpArg(nodeB->nativeId);
      nativeToJs.writeOpArg(classKey);
      nativeToJs.endOp();
   }
}

void Collision2DComponent::setSensorBeginCallback(std::function<SensorBeginCallback> callback)
{
   sensorBeginCallback = callback;
   phxy::SqBody *body = PhysicsSystem::getInstance()->getWorld()->getBody(bodyId);
   body->enableSensorBeginEvent(true);
}

void Collision2DComponent::onSensorEnd(const phxy::SqSensorEndEvent *event)
{
   // printf("+++++++++++++++++++onSensorEnd %d \n", bodyId);

   phxy::SqBody *body = PhysicsSystem::getInstance()->getWorld()->getBody(event->shapeB->getBody());
   Node *node = (Node *)body->getUserData();

   if (sensorEndCallback.has_value())
   {
      sensorEndCallback.value()(node);
   }

   if (!(jsContactListener & 8)) // 8 = 1 << 3
      return;

   SQ_ASSERT(node->nativeId != -1 && node->nativeId != -1);
   if (node->nativeId != -1 && node->nativeId != -1)
   {
      bridge::NativeToJsObject &nativeToJs = PhysicsSystem::getInstance()->getWorld()->getNativeToJsObject();
      nativeToJs.beginOp(PhysicsWorld::bridgeOp_onCollisionSensorEnd, true);
      nativeToJs.writeOpArg(node->nativeId);
      nativeToJs.writeOpArg(node->nativeId);
      nativeToJs.writeOpArg(classKey);
      nativeToJs.endOp();
   }
}

void Collision2DComponent::setSensorEndCallback(std::function<SensorEndCallback> callback)
{
   sensorEndCallback = callback;
   phxy::SqBody *body = PhysicsSystem::getInstance()->getWorld()->getBody(bodyId);
   body->enableSensorEndEvent(true);
}

void Collision2DComponent::onParticleBegin(Node *contactNode, int particleId)
{
   if (jsContactListener & 16)
   {
      bridge::NativeToJsObject &nativeToJs = PhysicsSystem::getInstance()->getWorld()->getNativeToJsObject();
      nativeToJs.beginOp(PhysicsWorld::bridgeOp_onParticleBeginContantBody_to_body, true);
      nativeToJs.writeOpArg(node->nativeId);
      nativeToJs.writeOpArg(contactNode->nativeId);
      nativeToJs.writeOpArg(particleId);
      nativeToJs.writeOpArg(classKey);
      nativeToJs.endOp();
   }
}

void Collision2DComponent::onParticleEnd(Node *contactNode, int particleId)
{
   if (jsContactListener & 32)
   {
      bridge::NativeToJsObject &nativeToJs = PhysicsSystem::getInstance()->getWorld()->getNativeToJsObject();
      nativeToJs.beginOp(PhysicsWorld::bridgeOp_onParticleEndContantBody_to_body, true);
      nativeToJs.writeOpArg(node->nativeId);
      nativeToJs.writeOpArg(contactNode->nativeId);
      nativeToJs.writeOpArg(particleId);
      nativeToJs.writeOpArg(classKey);
      nativeToJs.endOp();
   }
}

void Collision2DComponent::setBridgeHasContactListener(bool b, char type, int classKey)
{
   // printf("+++++++++++++++++++setBridgeHasContactListener %d %d %d \n", b, type, bodyId);
   phxy::SqBody *body = PhysicsSystem::getInstance()->getWorld()->getBody(bodyId);
   if (type == 1)
   {
      if (b)
      {
         this->classKey = classKey;
         jsContactListener |= 1;
      }
      else
      {
         jsContactListener &= ~1;
      }
      body->enableBeginContactEvent(b);
      body->enableEndContactEvent(b);
   }
   else if (type == 2)
   {

      if (b)
      {
         this->classKey = classKey;
         jsContactListener |= 4; // 1 << 2;
      }
      else
      {
         jsContactListener &= ~4;
      }
      body->enableSensorBeginEvent(b);
   }
   else if (type == 3)
   {
      if (b)
      {
         this->classKey = classKey;
         jsContactListener |= 8; // 1 << 3;
      }
      else
      {
         jsContactListener &= ~8; //(1 << 3);
      }
      body->enableSensorEndEvent(b);
   }
   else if (type == 4)
   {
      // onParticleBegin
      if (b)
      {
         this->classKey = classKey;
         jsContactListener |= 16; // 1 << 4;
      }
      else
      {
         jsContactListener &= ~16; //(1 << 4);
      }
   }
   else if (type == 5)
   {
      // onParticleEnd
      if (b)
      {
         this->classKey = classKey;
         jsContactListener |= 32; // 1 << 5;
      }
      else
      {
         jsContactListener &= ~32; //(1 << 5);
      }
   }
}

float Collision2DComponent::getMass()
{
   phxy::SqMassData massData = shape->computeShapeMass();
   return massData.mass;
}

Collision2DComponent::~Collision2DComponent()
{

   // printf("++++++++++++++Collision2DComponent delete %d bodyId %d \n",node->nativeId,bodyId);

   if (ownerBody)
   {
      node->off(NodeEventType::TRASNFORM_CHANGE, std::bind(&Collision2DComponent::onNodeTransformChange, this));
      PhysicsSystem::getInstance()->getWorld()->removeBody(bodyId);
      bodyId = -1;
   }

   // 在removeBody内会对shape进行delete
   shape = nullptr;
}
