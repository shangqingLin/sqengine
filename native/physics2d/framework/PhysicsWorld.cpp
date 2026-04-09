#include "PhysicsWorld.h"
#include "PhysicsSystem.h"
#include "./component/Collision2DComponent.h"
#include "./component/ParticlePhysics2DComponent.h"
#include "../../engine/2d/2d.h"

using namespace physics2d;

PhysicsWorld::PhysicsWorld() : particleWorld(nullptr),
                               nativeToJsObject(bridge::NativeObjectType::physics)
{
    world = new phxy::SqWorld();
}

PhysicsWorld::~PhysicsWorld()
{
    delete world;

#ifdef PHYSICS_DEBUG
    if (debugDraw)
        delete debugDraw;
#endif
}

void PhysicsWorld::syncSceneToPhysics()
{

    // printf("syncSceneToPhysics begin\n");
    const phxy::SqArray<phxy::SqSolverSet> &solveSets = world->getSolverSets();
    for (int n = 0; n < solveSets.getCount(); ++n)
    {
        const phxy::SqSolverSet *set = solveSets.get(n);

        // printf("?????? %d %d\n",n,set->bodySims.getCount());

        for (int s = 0; s < set->bodySims.getCount(); ++s)
        {
            phxy::SqBodySim *sim = set->bodySims.get(s);
            phxy::SqBody *body = world->getBody(sim->bodyIndex);
            Node *node = static_cast<Node *>(body->getUserData());
            SQ_ASSERT(node);
            int bodyFlag = body->getBodyFlag();

            // if (node->nativeId == 26)
            // printf("syncSceneToPhysics node %d bodyId %d flag %d bodyFlag %d \n", node->nativeId, body->getBodyId(), node->getChangeFlag(), bodyFlag);

            if (bodyFlag & phxy::SqBodyFlags::sq_node_transform)
            {
                syncNodeToPhysics(node, body);
                bodyFlag &= ~phxy::SqBodyFlags::sq_node_transform;
                body->setBodyFlag(bodyFlag);
            }
        }
    }

    // printf("syncSceneToPhysics end\n");
}

void PhysicsWorld::syncNodeToPhysics(Node *node, phxy::SqBody *body)
{

    int bodyFlag = body->getBodyFlag();
    Transform2DComponent *transform = node->getComponent<Transform2DComponent>();
    const Mat3 &worldMat = transform->getWorldTransform();

    // printf("PhysicsWorld::syncNodeToPhysics %d %d %d \n", node->nativeId, bodyFlag,node->getChangeFlag());
    //  if (node->nativeId == 26)
    //  {
    //      printf("syncNodeToPhysics node %d bodyId %d flag %d bodyFlag %d \n", node->nativeId, body->getBodyId(), node->getChangeFlag(), bodyFlag);
    //      worldMat.print();
    //  }

    if (bodyFlag & phxy::SqBodyFlags::sq_node_transform_pos_rot)
    {

        float units = 1.0f / PhysicsSystem::getInstance()->getUnitsPerMeter();
        // printf("units %f \n", units);

        phxy::SqTransform &bodyTransform = (phxy::SqTransform &)body->getTransform();
        phxy::SqVec2 pos = bodyTransform.p;
        phxy::SqRot rot = bodyTransform.q;

        // box2D不能支持设置不同的RotateXh和RotateY，只能是一样的rotate值
        float rx, ry;
        worldMat.getRotation(rx, ry);
        rx = cos(rx);
        ry = sin(ry);

        worldMat.getTranslation(pos.x, pos.y);

        // printf("syncNodeToPhysics %d pos: %f %f \n", node->nativeId, pos.x, pos.y);

        pos.x *= units;
        pos.y *= units;

        rot.c = rx;
        rot.s = ry;
        body->setTransform(pos, rot);

        // if (node->nativeId == 53 || node->nativeId == 26)
        // {
        // printf("fuckyou %d \n",node->nativeId);
        //     worldMat.print();
        // }

        // if (node->nativeId == 56)
        // printf("syncNodeToPhysics %d pos: %f %f rot: %f %f localPos %f %f \n", node->nativeId,
        //        pos.x, pos.y,
        //        rx, ry,
        //        transform->getX(), transform->getY());
    }

    // printf("syncNodeToPhysics native Id %d bit %d \n ",node->nativeId,node->getChangeFlag());
    if (bodyFlag & phxy::SqBodyFlags::sq_node_transform_scale)
    {
        float sx, sy;
        worldMat.getScale(sx, sy);

        // float fuckX, fuckY;
        // body->getScale(fuckX, fuckY);
        // printf("============== %p scale %f %f %d fuck %f %f\n", body, sx, sy, node->nativeId, fuckX, fuckY);

        body->setScale(sx, sy);
    }
}

void PhysicsWorld::syncPhysicsToScene()
{

    // 使用此变量取消由物理引擎引起的位置变化，否则与上面的syncNodeToPhysics造成循环触发了
    lockSyncPhysicsToScene = true;
    float units = PhysicsSystem::getInstance()->getUnitsPerMeter();
    const phxy::SqSolverSet *set = world->getSloverSet(phxy::SqSetType::sq_awakeSet);
    for (int s = 0; s < set->bodySims.getCount(); ++s)
    {
        phxy::SqBodySim *sim = set->bodySims.get(s);
        phxy::SqBody *body = world->getBody(sim->bodyIndex);

        Node *node = static_cast<Node *>(body->getUserData());
        SQ_ASSERT(node);

        Transform2DComponent *transform = node->getComponent<Transform2DComponent>();
        Mat3 &current = transform->getWorldTransform();
        const phxy::SqTransform &bodyTransform = body->getTransform();

        // 不要直接将物理引擎中的变换直接覆盖到Node的Transform中，因为那样会将Node上设置的缩放覆盖掉了
        float x = bodyTransform.p.x * units;
        float y = bodyTransform.p.y * units;

        transform->setWorldPosition(x, y);
        float angle = Math::radianToAngle(phxy::SqRot::GetAngle(bodyTransform.q));

        // if (node->nativeId == 26)
        // printf("syncPhysicsToScene %d  angle %f pos:(%f %f) \n", node->nativeId, angle, x, y);

        transform->setRotate(angle);
    }
    lockSyncPhysicsToScene = false;

    // printf("PhysicsWorld::syncPhysicsToScene end \n");
}

int PhysicsWorld::createBody(phxy::SqBodyDef &def)
{
    return world->createBody(&def);
}

phxy::SqBody *PhysicsWorld::getBody(int bodyId)
{
    return world->getBody(bodyId);
}

void PhysicsWorld::removeBody(int bodyId)
{
    world->removeBody(bodyId);
}

phxy::SqShape *PhysicsWorld::createShape(int bodyId, const phxy::SqShapeDef &def)
{
    return world->createShape(bodyId, def);
}

void PhysicsWorld::setDebugDraw(bool enable, Node *node)
{
#ifdef PHYSICS_DEBUG
    if (enable)
    {
        if (!debugDraw)
        {
            debugDrawNode = node;
            Graphics *graphics = debugDrawNode->addComponent<Graphics>();
            debugDraw = new PhysicsDraw(graphics, this);
        }
    }
    else
    {
        removeDebugDraw();
    }
#endif
}

void PhysicsWorld::removeDebugDraw()
{
#ifdef PHYSICS_DEBUG
    if (debugDraw)
    {
        delete debugDraw;
        debugDraw = nullptr;
        debugDrawNode = nullptr;
    }
#endif
}

void PhysicsWorld::drawDebug()
{

#ifdef PHYSICS_DEBUG
    if (debugDraw)
    {
        Graphics *graphics = debugDrawNode->getComponent<Graphics>();
        graphics->clear();
    }
#endif
}

void PhysicsWorld::beforeStep()
{
    world->beforeStep();
}

void PhysicsWorld::step(float dt)
{

    for (int i = 0; i < pbds.size(); ++i)
    {
        pbds[i]->Step(dt, 1);
    }

    world->step(dt, 1);
#ifdef PHYSICS_DEBUG
    if (debugDraw)
    {
        // printf("++++++++draw %d \n", pbds.size());
        world->debugDraw(&debugDraw->getDraw());
        for (int i = 0; i < pbds.size(); ++i)
        {
            pbds[i]->Draw(&debugDraw->getDraw());
        }
    }
#endif

    {
        const phxy::SqArray<phxy::SqContactBeginEvent> &beginEvents = world->getBeginContact();
        for (int i = 0; i < beginEvents.getCount(); ++i)
        {
            phxy::SqContactBeginEvent *event = beginEvents.get(i);
            phxy::SqBody *bodyA = world->getBody(event->shapeA->getBody());
            phxy::SqBody *bodyB = world->getBody(event->shapeB->getBody());

            // printf("begin contact %d \n", event->contactId);

            if (bodyA->isEnableBeginContactEvent())
            {
                Node *node = (Node *)bodyA->getUserData();
                Collision2DComponent *component = node->getComponent<Collision2DComponent>();
                component->onBeginContact(event);
            }

            if (bodyB->isEnableBeginContactEvent())
            {
                Node *node = (Node *)bodyB->getUserData();
                Collision2DComponent *component = node->getComponent<Collision2DComponent>();
                component->onBeginContact(event);
            }
        }
    }

    {
        const phxy::SqArray<phxy::SqContactHitUpdateEvent> &updateEvents = world->getHitUpdateContact();
        for (int i = 0; i < updateEvents.getCount(); ++i)
        {
            phxy::SqContactHitUpdateEvent *event = updateEvents.get(i);
            phxy::SqBody *bodyA = world->getBody(event->shapeA->getBody());
            phxy::SqBody *bodyB = world->getBody(event->shapeB->getBody());

            if (bodyA->isEnableHitUpdateContactEvent())
            {
                Node *node = (Node *)bodyA->getUserData();
                Collision2DComponent *component = node->getComponent<Collision2DComponent>();
                component->onHitUpdateContact(event);
            }

            if (bodyB->isEnableHitUpdateContactEvent())
            {
                Node *node = (Node *)bodyB->getUserData();
                Collision2DComponent *component = node->getComponent<Collision2DComponent>();
                component->onHitUpdateContact(event);
            }
        }
    }

    {
        const phxy::SqArray<phxy::SqContactEndEvent> &endEvents = world->getEndContact();

        // printf("trigger end Event Num %d \n", endEvents.getCount());

        for (int i = 0; i < endEvents.getCount(); ++i)
        {

            // 只要其中有一个Body销毁了，则会触发End事件的，销毁了就不能再触发End事件
            // 但另外一个没有被销毁的Body需要触发。
            // 因为这里可能拿到已经Destroy的Body，所以对应的Shape等全部都被销毁了，所有不能再获取获取任何信息。
            phxy::SqContactEndEvent *event = endEvents.get(i);

            // printf("end contact %d \n", event->contactId);

            int bodyAId = event->bodyAId;
            int bodyBId = event->bodyBId;

            // printf(" PhysicsWorld::step bodyB %d %d \n",bodyAId,bodyBId);

            if (!world->bodyIsDestroy(bodyAId))
            {
                phxy::SqBody *bodyA = world->getBody(bodyAId);
                if (bodyA->isEnableEndContactEvent())
                {
                    Node *node = (Node *)bodyA->getUserData();
                    SQ_ASSERT(node);
                    // printf(" PhysicsWorld::step bodyA %p %d \n", bodyA, node->nativeId);
                    Collision2DComponent *component = node->getComponent<Collision2DComponent>();
                    SQ_ASSERT(component);
                    component->onEndContact(event);
                }
            }

            if (!world->bodyIsDestroy(bodyBId))
            {
                phxy::SqBody *bodyB = world->getBody(bodyBId);
                if (bodyB->isEnableEndContactEvent())
                {
                    Node *node = (Node *)bodyB->getUserData();
                    SQ_ASSERT(node);
                    Collision2DComponent *component = node->getComponent<Collision2DComponent>();

                    // phxy::SqBody *bodyA = world->getBody(bodyAId);
                    // Node *nodeA = (Node *)bodyA->getUserData();
                    // printf(" PhysicsWorld::step bodyB %p %d %p \n", bodyB, node->nativeId,nodeA->nativeId);

                    SQ_ASSERT(component);
                    component->onEndContact(event);
                }
            }
        }
    }

    {
        const phxy::SqArray<phxy::SqSensorBeginEvent> &beginEvents = world->getSensorBeginContact();
        for (int i = 0; i < beginEvents.getCount(); ++i)
        {
            phxy::SqSensorBeginEvent *event = beginEvents.get(i);
            phxy::SqBody *bodyA = world->getBody(event->shapeA->getBody());
            phxy::SqBody *bodyB = world->getBody(event->shapeB->getBody());

            // printf(" PhysicsWorld being sensor %p %p %d %d \n", bodyA, bodyB, ((Node *)bodyA->getUserData())->nativeId, ((Node *)bodyB->getUserData())->nativeId);

            if (bodyA->isEnableSensorBeginEvent())
            {
                Node *node = (Node *)bodyA->getUserData();
                Collision2DComponent *component = node->getComponent<Collision2DComponent>();
                component->onSensorBegin(event);
            }

            if (bodyB->isEnableSensorBeginEvent())
            {
                Node *node = (Node *)bodyB->getUserData();
                Collision2DComponent *component = node->getComponent<Collision2DComponent>();
                component->onSensorBegin(event);
            }
        }
    }

    {
        const phxy::SqArray<phxy::SqSensorEndEvent> &endEvents = world->getSensorEndContact();
        for (int i = 0; i < endEvents.getCount(); ++i)
        {
            phxy::SqSensorEndEvent *event = endEvents.get(i);
            int bodyAId = event->shapeA->getBody();
            int bodyBId = event->shapeB->getBody();

            if (!world->bodyIsDestroy(bodyAId))
            {

                phxy::SqBody *bodyA = world->getBody(bodyAId);
                if (bodyA->isEnableSensorEndEvent())
                {
                    Node *node = (Node *)bodyA->getUserData();
                    Collision2DComponent *component = node->getComponent<Collision2DComponent>();
                    component->onSensorEnd(event);
                }
            }

            if (!world->bodyIsDestroy(bodyBId))
            {
                phxy::SqBody *bodyB = world->getBody(bodyBId);
                if (bodyB->isEnableSensorEndEvent())
                {
                    Node *node = (Node *)bodyB->getUserData();
                    Collision2DComponent *component = node->getComponent<Collision2DComponent>();
                    component->onSensorEnd(event);
                }
            }
        }
    }

    if (particleWorld)
    {
        phxy::SqParticleSystem *system = particleWorld->GetParticleSystemList();
        while (system)
        {
            const phxy::SqArray<phxy::SqParticleBodyContact> &contacts = system->GetBodyContacts();
            ParticlePhysics2DComponent *component = (ParticlePhysics2DComponent *)system->GetUserData();
            for (int i = 0; i < contacts.getCount(); ++i)
            {
                const phxy::SqParticleBodyContact *contact = contacts.get(i);

                if (contact->state == 1)
                {
                    component->onBeginContactBody((phxy::SqParticleBodyContact *)contact);

                    if (world->bodyIsDestroy(contact->bodyId))
                    {
                        continue;
                    }

                    phxy::SqBody *body = world->getBody(contact->bodyId);
                    Node *node = (Node *)body->getUserData();
                    Collision2DComponent *contactComponent = node->getComponent<Collision2DComponent>();
                    contactComponent->onParticleBegin(component->node, contact->particleId);
                }
                else if (contact->state == 0)
                {
                    component->onEndContactBody((phxy::SqParticleBodyContact *)contact);

                    // 如果一个Body销毁了，粒子系统内部会触发End事件的
                    if (world->bodyIsDestroy(contact->bodyId))
                    {
                        continue;
                    }

                    phxy::SqBody *body = world->getBody(contact->bodyId);
                    Node *node = (Node *)body->getUserData();
                    Collision2DComponent *contactComponent = node->getComponent<Collision2DComponent>();
                    contactComponent->onParticleEnd(component->node, contact->particleId);
                }
            }

            system = system->m_next;
        }
    }
}

phxy::SqParticleSystem *PhysicsWorld::CreateParticleSystem(phxy::b2ParticleSystemDef &def)
{
    if (!particleWorld)
        particleWorld = new phxy::SqParticleSystemWorld(this->world);
    return particleWorld->CreateParticleSystem(&def);
}

void PhysicsWorld::destroyParticleSystem(phxy::SqParticleSystem *system)
{
    particleWorld->DestroyParticleSystem(system);
}

phxy::PBD *PhysicsWorld::createPBD()
{
    phxy::PBD *pbd = new phxy::PBD();
    pbds.push_back(pbd);
    return pbd;
}

void PhysicsWorld::destroyPBD(phxy::PBD *pbd)
{
    for (int i = 0; i < pbds.size(); ++i)
    {
        if (pbds[i] == pbd)
        {
            pbds.erase(pbds.begin() + i);
            break;
        }
    }
    delete pbd;
}