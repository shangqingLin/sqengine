#include "SqWorld.h"
#include "SqWorldDraw.h"
#include <stdio.h>

#include "../common/math/SqMath.h"
#include "../geometry/shape/SqShapeDefine.h"
#include "../geometry/shape/SqCapsuleShape.h"
#include "../geometry/shape/SqChainShape.h"
#include "../geometry/shape/SqCircleShape.h"
#include "../geometry/shape/SqEdgeShape.h"
#include "../geometry/shape/SqPolygonShape.h"
#include "./joint/SqRevoluteJoint.h"
#include "./joint/SqDistanceJoint.h"
#include "./joint/SqMotorJoint.h"
#include "./joint/SqMouseJoint.h"
#include "./joint/SqPrismaticJoint.h"
#include "./joint/SqRevoluteJoint.h"
#include "./joint/SqWeldJoint.h"
#include "./joint/SqWheelJoint.h"
#include "../collision/broadphase/SqBroadPhaseFilter.h"
#include "../particle/SqParticleSystemWorld.h"

using namespace phxy;

static float SqDefaultFrictionCallback(float frictionA, int materialA, float frictionB, int materialB)
{
    return sqrtf(frictionA * frictionB);
}

static float SqDefaultRestitutionCallback(float restitutionA, int materialA, float restitutionB, int materialB)
{
    return max(restitutionA, restitutionB);
}

SqWorld::SqWorld() : broadPhase(this),
                     narrowPhase(this),
                     solver(this),
                     constraintGraph(this),
                     sensor(this)
{
    particleSystem = nullptr;
    customBroadPhaseFilterFcn = nullptr;
    customBroadPhaseFilterContext = nullptr;
    beforeSoveCustomFunc = nullptr;
    gravity.x = 0.f;
    gravity.y = -10.f;
    enableContinuous = true;
    enableSleep = true;
    enableWarmStart = true;
    frictionCallback = &SqDefaultFrictionCallback;
    restitutionCallback = &SqDefaultRestitutionCallback;
    solverSets.resize(8);
    contactBeginEvents.resize(10);
    contactEndEvents.resize(10);
    contactHitEvents.resize(10);
    bodyArray.resize(50);
    shapeArray.resize(100);

    initSloverSet(sq_staticSet);
    initSloverSet(sq_disabledSet);
    initSloverSet(sq_awakeSet);

    float lengthUnitsPerMeter = SqConfig::getInstance()->getLengthUnitsPerMeter();
    contactSpeed = 3.0f * lengthUnitsPerMeter;
    contactHertz = 30.f;
    contactDampingRatio = 10.0f;
}

void SqWorld::initSloverSet(int setIndex)
{
    solverSets.push(SqSolverSet());
    int useId = sloverIdPool.get();
    SQ_ASSERT(useId == setIndex);
    SqSolverSet *set = solverSets.get(setIndex);
    set->setIndex = setIndex;
    set->world = this;
}

int SqWorld::createBody(const SqBodyDef *def)
{
    SQ_ASSERT(!lock);
    SQ_ASSERT(SqVec2::isValid(def->position));
    SQ_ASSERT(SqRot::isValid(def->rotation));
    SQ_ASSERT(SqVec2::isValid(def->linearVelocity));
    SQ_ASSERT(isValidFloat(def->angularVelocity));
    SQ_ASSERT(isValidFloat(def->linearDamping) && def->linearDamping >= 0.0f);
    SQ_ASSERT(isValidFloat(def->angularDamping) && def->angularDamping >= 0.0f);
    SQ_ASSERT(isValidFloat(def->sleepThreshold) && def->sleepThreshold >= 0.0f);
    SQ_ASSERT(isValidFloat(def->gravityScale));

    int bodyIndex = bodyIdPool.get();
    if (bodyIndex == bodyArray.getCount())
    {
        bodyArray.Add();
    }
    SqBody *body = bodyArray.get(bodyIndex);
    body->reset();
    body->bodyIndex = bodyIndex;
    body->world = this;
    body->userData = def->userData;
    body->type = def->type;

    bool isAwake = (def->isAwake || def->enableSleep == false) && def->isEnabled;
    SqSolverSet *set = nullptr;

    if (def->isEnabled == false)
    {
        set = getSloverSet(sq_disabledSet);
    }
    else if (def->type == sq_staticBody)
    {
        set = getSloverSet(sq_staticSet);
    }
    else if (isAwake == true)
    {
        set = getSloverSet(sq_awakeSet);
    }
    else
    {
        // 表示是dynamic的Body，但处于Sleep状态的Body
        set = createSloverSet();
    }

    SQ_ASSERT(set);

    // printf("++++++create setindex %p %d bodyIndex %d \n", body, set->setIndex,bodyIndex);

    body->setIndex = set->setIndex;
    body->simIndex = set->bodySims.getCount();
    SqBodySim *bodySim = set->bodySims.Add();
    bodySim->reset();
    bodySim->transform.p = def->position;
    bodySim->transform.q = def->rotation;
    bodySim->center = def->position;
    bodySim->bodyIndex = bodyIndex;
    bodySim->linearVelocity = def->linearVelocity;
    bodySim->angularVelocity = def->angularVelocity;
    bodySim->prevSim.center = bodySim->center;
    bodySim->prevSim.rotation = def->rotation;

    uint32_t lockFlags = 0;
    lockFlags |= def->lockLinearX ? SqBodyFlags::sq_lockLinearX : 0;
    lockFlags |= def->lockLinearY ? SqBodyFlags::sq_lockLinearY : 0;
    lockFlags |= def->loclAngularZ ? SqBodyFlags::sq_lockAngularZ : 0;
    bodySim->flags = lockFlags;
    bodySim->flags |= def->isBullet ? sq_isBullet : 0;
    bodySim->flags |= def->allowFastRotation ? sq_allowFastRotation : 0;
    bodySim->flags |= def->enableContinuous ? sq_enableContinuous : 0;
    bodySim->flags |= def->enableSleep ? sq_enableSleep : 0;

    // dynamic和kinematic类的body必须加入到island中
    // 初始创建body到拥有一个单独的island
    if (set->setIndex >= sq_awakeSet)
    {
        body->createIslandForBody(set->setIndex);
    }

    // printf("createBody %p %d \n", body, bodyIndex);
    return body->bodyIndex;
}

void SqWorld::removeBody(int bodyId)
{
    SQ_ASSERT(!lock);

    // printf("removeBody %d \n", bodyId);
    SqBody *body = getBody(bodyId);
    body->destroy();
    bodyIdPool.recovery(bodyId);
}

bool SqWorld::bodyIsDestroy(int bodyId)
{
    return bodyIdPool.hasId(bodyId);
}

SqShape *SqWorld::createShape(int bodyId, const SqShapeDef &def)
{
    SQ_ASSERT(!lock);
    SqShape *shape;
    switch (def.type)
    {
    case SqShapeType::sq_boxShape:
    {
        const SqBoxShapeDef &eDef = static_cast<const SqBoxShapeDef &>(def);
        SqPolygonShape *cShape = new SqPolygonShape();
        cShape->setAsBox(eDef.halfWidth, eDef.halfHeight, eDef.center, eDef.rotation);
        shape = cShape;
        break;
    }
    case SqShapeType::sq_polygonShape:
    {
        const SqPolygonShapeDef &eDef = static_cast<const SqPolygonShapeDef &>(def);
        SqPolygonShape *cShape = new SqPolygonShape();
        cShape->setPolygon(eDef.hull, eDef.radius);
        shape = cShape;
        break;
    }
    case SqShapeType::sq_chainShape:
    {
        const SqChainDef &eDef = static_cast<const SqChainDef &>(def);
        SqChainShape *cShape = new SqChainShape();
        cShape->setChain(eDef.points, eDef.count, eDef.isLoop);
        shape = cShape;
        break;
    }
    case SqShapeType::sq_circleShape:
    {
        const SqCircleShapeDef &eDef = static_cast<const SqCircleShapeDef &>(def);
        SqCircleShape *cShape = new SqCircleShape();
        cShape->setCircle(eDef.center, eDef.radius);
        shape = cShape;
        break;
    }
    case SqShapeType::sq_segmentShape:
    {
        const SqEdgeShapeDef &eDef = static_cast<const SqEdgeShapeDef &>(def);
        SqEdgeShape *cShape = new SqEdgeShape();
        cShape->setEdge(eDef.point1, eDef.point2);
        shape = cShape;
        break;
    }
    case SqShapeType::sq_capsuleShape:
    {
        const SqCapusleShapeDef &eDef = static_cast<const SqCapusleShapeDef &>(def);
        SqCapsuleShape *cShape = new SqCapsuleShape();
        cShape->setCapsule(eDef.center1, eDef.center2, eDef.radius);
        shape = cShape;
        break;
    }

    default:
        SQ_ASSERT(false);
    }

    shape->world = this;
    shape->setFriction(def.material.friction);
    shape->setRestitution(def.material.restitution);
    shape->setTangentSpeed(def.material.tangentSpeed);
    shape->density = def.density;
    shape->setFilter(def.filter);
    shape->setCustomColor(def.customColor);
    int shapeId = shapeIdPool.get();
    shape->shapeIndex = shapeId;

    // printf("==== fuck %d %d \n",shapeId,shapeArray.getCount());
    if (shapeId == shapeArray.getCount())
    {
        shapeArray.Add();
    }
    shapeArray.set(shapeId, shape);
    getBody(bodyId)->addShape(shape);

    // printf("++++ shape count %d \n",  shapeArray.getCount());

    if (def.isSensor)
    {
        shape->setSensor(true);
        shape->sensorAABB(def.sensorAABB);
    }
    return shape;
}

void SqWorld::destroyShape(SqShape *shape)
{
    SQ_ASSERT(!lock);
    getBody(shape->getBody())->removeShape(shape);
    shapeIdPool.recovery(shape->shapeIndex);
    // printf("++++recvoery shape id %d \n", shape->shapeIndex);
    delete shape;
}

void SqWorld::narrowPhaseStep(SqStepContext &context)
{
    int contactCount = 0;

    SqArray<SqContactSim> &awakeContactSims = solverSets.get(sq_awakeSet)->contactSims;
    int nonTouchingCount = awakeContactSims.getCount();
    contactCount += nonTouchingCount;

    if (contactCount > 0)
    {

        SqContactSim **contactSims = (SqContactSim **)sqstd::StackTempArenaAllocator::getInstance()->allocateChunk(contactCount * sizeof(SqContactSim *));
        context.contacts = contactSims;
        context.contactCount = contactCount;
        for (int i = 0; i < contactCount; ++i)
        {
            contactSims[i] = awakeContactSims.get(i);
        }
        narrowPhase.collide(context);
        sqstd::StackTempArenaAllocator::getInstance()->freeChunk(contactSims);
        context.contactCount = 0;
        context.contacts = nullptr;
    }
}

// #include "../collision/narrowphase/collide-chainsegment-polygon.h"

// SqChainEdgeShape edgeShape;
// SqTransform edgeTransform;

// SqChainEdgeShape ghostShape1;
// SqChainEdgeShape ghostShape2;

// SqPolygonShape polygonShape;
// SqTransform polygonTransform;

void SqWorld::beforeStep()
{
    contactBeginEvents.clear();
    contactEndEvents.clear();
    contactHitEvents.clear();
    sensorBeginEvents.clear();
    sensorEndEvents.clear();
}

void SqWorld::step(float dt, int subStepCount)
{

    // return;

    if (dt == 0.f)
        return;

#if 1
    if (_enableNextStep)
    {
        if (!_triggerNextStep)
            return;
        _triggerNextStep = false;
    }
#endif

    // stop = true;

    // edgeShape.point1 = SqVec2(100, 0);
    // edgeShape.point2 = SqVec2(-100, 0);

    // edgeShape.ghost1 = SqVec2(120, -1);
    // edgeShape.ghost2 = SqVec2(-120, 1);

    // ghostShape1.point1 = edgeShape.point1;
    // ghostShape1.point2 = edgeShape.ghost1;

    // ghostShape2.point1 = edgeShape.point2;
    // ghostShape2.point2 = edgeShape.ghost2;

    // polygonShape.setAsBox(25, 25, SqVec2(), SqRot());
    // SqSimplexCache cache;

    // polygonTransform.p.y = 34.16;

    // float PI = 3.1415926;
    // polygonTransform.q = SqRot(60 * PI / 180);
    // SqManifold manifold = sqCollideChainSegmentAndPolygon(&edgeShape, edgeTransform, &polygonShape, polygonTransform, &cache);
    // for (int i = 0; i < manifold.pointCount; ++i)
    // {
    //     SqManifoldPoint &point = manifold.points[i];
    //     printf("%d anchorA %f %f anchorB %f %f separation %f \n",
    //            i,
    //            point.anchorA.x, point.anchorA.y,
    //            point.anchorB.x, point.anchorB.y,
    //            point.separation);
    // }

    // printf("=======update \n");

    SqStepContext context;
    context.subStepCount = max(1, subStepCount);
    context.world = this;
    this->dt = dt;

    context.dt = dt;
    context.inv_dt = 1.0f / dt;
    context.h = dt / context.subStepCount;
    context.inv_h = context.subStepCount * context.inv_dt;

    // 因为particleSystem中粒子会对Body应用力并WakeUp Body，所以必须在Locak=true之前调用
    if (particleSystem)
        particleSystem->step(context);

    lock = true;

    // printf("================update\n");
    // context
    broadPhase.updatePairs();

    float contactHertz = min(this->contactHertz, 0.125f * context.inv_h);
    contactSoftness.step(contactHertz, contactDampingRatio, context.h);
    staticSoftness.step(contactHertz * 2.0f, contactDampingRatio, context.h);
    narrowPhaseStep(context);
    if (beforeSoveCustomFunc)
    {
        beforeSoveCustomFunc(&context);
    }

    solver.solve(context);

    lock = false;
}

void SqWorld::debugDraw(SqDebugDraw *draw)
{
    // drawShape(draw, &edgeShape, edgeTransform, SqHexColor::sq_colorGold);
    // drawShape(draw, &ghostShape1, edgeTransform, SqHexColor::sq_colorBlue );
    // drawShape(draw, &ghostShape2, edgeTransform, SqHexColor::sq_colorBlue);
    // if (polygonShape.count > 0)
    //     drawShape(draw, &polygonShape, polygonTransform, SqHexColor::sq_colorBeige);

    worldDebugDraw.debugDraw(this, draw);
}

void SqWorld::setBroadPhaseCustomFilter(SqBroadPhaseCustomFilterFcn *func, void *context)
{
    customBroadPhaseFilterFcn = func;
    customBroadPhaseFilterContext = context;
}

void SqWorld::removeSloverSet(SqSolverSet *set)
{
    sloverIdPool.recovery(set->setIndex);
    set->shrink();
}
void SqWorld::removeSloverSet(int setIndex)
{
    removeSloverSet(solverSets.get(setIndex));
}
SqSolverSet *SqWorld::getSloverSet(int index) const
{
    return solverSets.get(index);
}

SqSolverSet *SqWorld::createSloverSet()
{
    SqSolverSet *set = nullptr;
    int setId = sloverIdPool.get();
    if (setId == solverSets.getCount())
    {
        solverSets.push(SqSolverSet());
        set = solverSets.get(setId);
    }
    else
    {
        set = solverSets.get(setId);
        SQ_ASSERT(set->setIndex == SQ_NULL_INDEX);
    }

    set->setIndex = setId;
    set->world = this;
    return set;
}

/**
 * 将Sleep状态中的Set 的Body移到Awake Set中，相当于唤醒这个Sleep Set中所有的Body
 */
void SqWorld::moveSleepSetToAwakeSet(int sleepSetIndex)
{
    // 确保传进来的是Sleep Set
    SQ_ASSERT(sleepSetIndex >= sq_firstSleepingSet);
    SqSolverSet *sleepSet = solverSets.get(sleepSetIndex);
    SqSolverSet *awakeSet = solverSets.get(sq_awakeSet);
    sleepSet->copyTo(awakeSet);
    removeSloverSet(sleepSet);
    // b2ValidateSolverSets(world);
}

void SqWorld::mergeSolverSets(int setId1, int setId2)
{

    SqSolverSet *set1 = solverSets.get(setId1);
    SqSolverSet *set2 = solverSets.get(setId2);

    // 将Body比较少的那一个Set合并到Body比较多的Set中
    if (set1->bodySims.getCount() < set2->bodySims.getCount())
    {
        SqSolverSet *tempSet = set1;
        set1 = set2;
        set2 = tempSet;

        int tempId = setId1;
        setId1 = setId2;
        setId2 = tempId;
    }

    {
        // copy body
        int bodyCount = set2->bodySims.getCount();
        for (int i = 0; i < bodyCount; ++i)
        {
            SqBodySim *simSrc = set2->bodySims.get(i);
            SqBody *body = getBody(simSrc->bodyIndex);
            SQ_ASSERT(body->setIndex == setId2);
            body->setIndex = setId1;
            body->simIndex = set1->bodySims.getCount();
            SqBodySim *simDst = set1->bodySims.Add();
            memcpy(simDst, simSrc, sizeof(SqBodySim));
        }
    }

    {
        // copy contacts
        int contactCount = set2->contactSims.getCount();
        for (int i = 0; i < contactCount; ++i)
        {
            SqContactSim *contactSrc = set2->contactSims.get(i);
            SqContact *contact = narrowPhase.getContact(contactSrc->contactId);
            SQ_ASSERT(contact->setIndex == setId2);
            contact->setIndex = setId1;
            contact->simIndex = set1->contactSims.getCount();
            SqContactSim *contactDst = set1->contactSims.Add();
            memcpy(contactDst, contactSrc, sizeof(SqContactSim));
        }
    }

    {
        // copy joints
        int jointCount = set2->jointSims.getCount();
        for (int i = 0; i < jointCount; ++i)
        {
            SqJointSim *jointSrc = set2->jointSims.get(i);
            SqJoint *joint = getJoint(jointSrc->jointId);
            SQ_ASSERT(joint->setIndex == setId2);
            joint->setIndex = setId1;
            joint->simIndex = set1->jointSims.getCount();

            SqJointSim *jointDst = set1->jointSims.Add();
            memcpy(jointDst, jointSrc, sizeof(SqJointSim));
        }
    }

    // transfer islands
    // {
    //     int islandCount = set2->islandSims.count;
    //     for (int i = 0; i < islandCount; ++i)
    //     {
    //         b2IslandSim *islandSrc = set2->islandSims.data + i;
    //         int islandId = islandSrc->islandId;

    //         b2Island *island = b2IslandArray_Get(&world->islands, islandId);
    //         island->setIndex = setId1;
    //         island->localIndex = set1->islandSims.count;

    //         b2IslandSim *islandDst = b2IslandSimArray_Add(&set1->islandSims);
    //         memcpy(islandDst, islandSrc, sizeof(b2IslandSim));
    //     }
    // }

    removeSloverSet(set2);

    // b2ValidateSolverSets(world);
}

void SqWorld::destroyJoint(SqJoint *joint, bool wakeBodies)
{
    SQ_ASSERT(!lock);
    SqJointEdge *edgeA = joint->edges + 0;
    SqJointEdge *edgeB = joint->edges + 1;

    int idA = edgeA->bodyId;
    int idB = edgeB->bodyId;
    SqBody *bodyA = getBody(idA);
    SqBody *bodyB = getBody(idB);

    bodyA->removeJointFromLink(joint);
    bodyB->removeJointFromLink(joint);

    if (joint->islandId != SQ_NULL_INDEX)
    {
        SQ_ASSERT(joint->isEnable());
        unlinkJointFromIsland(joint);
    }
    else
    {
        SQ_ASSERT(!joint->isEnable());
    }

    // Remove joint from solver set that owns it
    int setIndex = joint->setIndex;
    int localIndex = joint->simIndex;

    // if (setIndex == sq_awakeSet)
    // {
    //     // b2RemoveJointFromGraph(world, joint->edges[0].bodyId, joint->edges[1].bodyId, joint->colorIndex, localIndex);
    // }
    // else
    // {
    SqSolverSet *set = getSloverSet(setIndex);
    set->removeJoint(localIndex);
    // }

    jointArray.set(joint->jointIndex, nullptr);
    jointIdPool.recovery(joint->jointIndex);
    if (wakeBodies)
    {
        bodyA->wakeBody();
        bodyB->wakeBody();
    }

    if (joint->deleteCallback)
    {
        joint->deleteCallback(joint->deleteCallbackContext);
    }
    joint->reset();
    delete joint;

    // b2ValidateSolverSets(world);
}

/**
 * 为SqJoint创建SqJointSim
 *
 */
void SqWorld::createJoint(SqJoint *joint, SqBaseJointDef &def)
{

    SQ_ASSERT(!lock);

    SqBody *bodyA = getBody(def.bodyAId);
    SqBody *bodyB = getBody(def.bodyBId);

    joint->world = this;

    int jointId = jointIdPool.get();
    if (jointId == jointArray.getCount())
    {
        jointArray.Add();
    }
    jointArray.set(jointId, joint);
    joint->jointIndex = jointId;
    joint->collideConnected = def.collideConnected;
    bodyA->addJointToLink(joint, 0);
    bodyB->addJointToLink(joint, 1);

    SqJointSim *jointSim = nullptr;
    if (bodyA->setIndex == sq_disabledSet || bodyB->setIndex == sq_disabledSet)
    {
        // 只要其中有一个为disable的，则这个Joint都不会被模拟
        SqSolverSet *set = solverSets.get(sq_disabledSet);
        joint->setIndex = sq_disabledSet;
        joint->simIndex = set->jointSims.getCount();

        jointSim = set->jointSims.Add();
        jointSim->reset();
    }
    else if (bodyA->isStatic() && bodyB->isStatic())
    {
        SqSolverSet *set = solverSets.get(sq_staticSet);
        joint->setIndex = sq_staticSet;
        joint->simIndex = set->jointSims.getCount();
        jointSim = set->jointSims.Add();
        jointSim->reset();
    }
    else if (bodyA->isAwake() || bodyB->isAwake())
    {
        // 如果有一个Body在Sleep中，则唤醒其中的Body
        if (bodyA->isSleep())
        {
            bodyA->wakeBody();
        }
        else if (bodyB->isSleep())
        {
            bodyB->wakeBody();
        }

        joint->setIndex = sq_awakeSet;
        SqSolverSet *set = solverSets.get(sq_awakeSet);
        joint->simIndex = set->jointSims.getCount();
        jointSim = set->jointSims.Add();
        jointSim->reset();

        // jointSim = b2CreateJointInGraph(world, joint);
        // jointSim->jointId = jointId;
        // jointSim->bodyIdA = bodyIdA;
        // jointSim->bodyIdB = bodyIdB;
    }
    else
    {

        // 到这里表示其中有一个是Sleep中，另外一个为静态的 或 两个都是Sleep的
        SQ_ASSERT(bodyA->isSleep() || bodyB->isSleep());
        SQ_ASSERT(!bodyA->isStatic() || !bodyB->isStatic());

        // 将Joint存放到Sleep的Set中
        int setIndex = bodyA->isSleep() ? bodyA->setIndex : bodyB->setIndex;
        SqSolverSet *set = solverSets.get(setIndex);
        joint->setIndex = setIndex;
        joint->simIndex = set->jointSims.getCount();
        jointSim = set->jointSims.Add();

        if (bodyA->setIndex != bodyB->setIndex && bodyA->isSleep() && bodyB->isSleep())
        {
            mergeSolverSets(bodyA->setIndex, bodyB->setIndex);
            SQ_ASSERT(bodyA->setIndex == bodyB->setIndex);

            // 由于megre操作改变了set了
            setIndex = bodyA->setIndex;
            SqSolverSet *mergedSet = solverSets.get(setIndex);
            jointSim = mergedSet->jointSims.get(joint->simIndex);
        }
        SQ_ASSERT(joint->setIndex == setIndex);
    }

    jointSim->reset();
    jointSim->jointId = jointId;
    jointSim->bodyAIndex = bodyA->bodyIndex;
    jointSim->bodyBIndex = bodyB->bodyIndex;
    jointSim->localFrameA = def.localFrameA;
    jointSim->localFrameB = def.localFrameB;

    // printf("localFrameA %f %f %f %f localFrameB %f %f %f %f \n",
    //        jointSim->localFrameA.p.x, jointSim->localFrameA.p.y, jointSim->localFrameA.q.s, jointSim->localFrameA.q.c,
    //        jointSim->localFrameB.p.x, jointSim->localFrameB.p.y, jointSim->localFrameB.q.s, jointSim->localFrameB.q.c
    //     );

    jointSim->enableSoftness = def.enableSoftness;
    jointSim->constraintHertz = def.constraintHertz;
    jointSim->constraintDampingRatio = def.constraintDampingRatio;

    SQ_ASSERT(isValidFloat(def.forceThreshold) && def.forceThreshold >= 0.0f);
    SQ_ASSERT(isValidFloat(def.torqueThreshold) && def.torqueThreshold >= 0.0f);

    jointSim->forceThreshold = def.forceThreshold;
    jointSim->torqueThreshold = def.torqueThreshold;

    if (joint->isEnable())
    {
        linkJointToIsland(joint, true);
    }

    if (def.collideConnected == false)
    {
        narrowPhase.removeContact(bodyA->bodyIndex, bodyB->bodyIndex, false);
    }

    // b2ValidateSolverSets(world);
}

SqJoint *SqWorld::createRevoluteJoint(SqRevoluteJointDef &def)
{
    SQ_ASSERT(def.lowerAngle <= def.upperAngle);
    // SQ_ASSERT(def.lowerAngle >= -0.99f * SQ_PI);
    // SQ_ASSERT(def.upperAngle <= 0.99f * SQ_PI);
    SQ_ASSERT(lock == false);

    if (lock)
    {
        return nullptr;
    }

    SqRevoluteJoint *joint = new SqRevoluteJoint();
    createJoint(joint, def);
    joint->targetAngle = clamp(def.targetAngle, -SQ_PI, SQ_PI);
    joint->hertz = def.hertz;
    joint->dampingRatio = def.dampingRatio;
    joint->lowerAngle = def.lowerAngle;
    joint->upperAngle = def.upperAngle;
    joint->maxMotorTorque = def.maxMotorTorque;
    joint->motorSpeed = def.motorSpeed;
    joint->enableSpring = def.enableSpring;
    joint->enableLimit = def.enableLimit;
    joint->enableMotor = def.enableMotor;
    return joint;
}

SqJoint *SqWorld::createWheelJoint(SqWheelJointDef &def)
{
    SQ_ASSERT(def.lowerTranslation <= def.upperTranslation);
    SQ_ASSERT(lock == false);

    if (lock)
    {
        return nullptr;
    }

    SqWheelJoint *joint = new SqWheelJoint();
    createJoint(joint, def);

    joint->maxMotorTorque = def.maxMotorTorque;
    joint->motorSpeed = def.motorSpeed;
    joint->hertz = def.hertz;
    joint->dampingRatio = def.dampingRatio;
    joint->enableSpring = def.enableSpring;
    joint->enableLimit = def.enableLimit;
    joint->lowerTranslation = def.lowerTranslation;
    joint->upperTranslation = def.upperTranslation;
    joint->enableMotor = def.enableMotor;
    return joint;
}

SqJoint *SqWorld::createWeldJoint(SqWeldJointDef &def)
{
    SQ_ASSERT(lock == false);

    if (lock)
    {
        return nullptr;
    }

    SqWeldJoint *joint = new SqWeldJoint();
    createJoint(joint, def);
    joint->linearHertz = def.linearHertz;
    joint->linearDampingRatio = def.linearDampingRatio;
    joint->angularHertz = def.angularHertz;
    joint->angularDampingRatio = def.angularDampingRatio;
    return joint;
}

SqJoint *SqWorld::createMouseJoint(SqMouseJointDef &def)
{
    SQ_ASSERT(lock == false);

    if (lock)
    {
        return nullptr;
    }
    SqMouseJoint *joint = new SqMouseJoint();
    createJoint(joint, def);

    joint->hertz = def.hertz;
    joint->dampingRatio = def.dampingRatio;
    joint->maxForce = def.maxForce;
    return joint;
}

SqJoint *SqWorld::createDistanceJoint(SqDistanceJointDef &def)
{

    SQ_ASSERT(lock == false);
    if (lock)
    {
        return nullptr;
    }
    SQ_ASSERT(isValidFloat(def.length) && def.length > 0.0f);

    SqDistanceJoint *joint = new SqDistanceJoint();
    createJoint(joint, def);

    float linearSlop = SqConfig::getInstance()->getLinearSlop();
    joint->length = max(def.length, linearSlop);
    joint->hertz = def.hertz;
    joint->dampingRatio = def.dampingRatio;
    // joint->minLength = max(def.minLength, linearSlop);
    // joint->maxLength = max(joint->minLength, def.maxLength);
    joint->minLength = def.minLength;
    joint->maxLength = def.maxLength;
    joint->maxMotorForce = def.maxMotorForce;
    joint->motorSpeed = def.motorSpeed;
    joint->enableSpring = def.enableSpring;
    joint->enableLimit = def.enableLimit;
    joint->enableMotor = def.enableMotor;

    return joint;
}

SqJoint *SqWorld::createMotorJoint(SqMotorJointDef &def)
{

    SQ_ASSERT(lock == false);

    if (lock)
    {
        return nullptr;
    }

    SqMotorJoint *joint = new SqMotorJoint();
    createJoint(joint, def);

    joint->maxForce = def.maxForce;
    joint->maxTorque = def.maxTorque;
    joint->correctionFactor = clamp(def.correctionFactor, 0.0f, 1.0f);

    return joint;
}

SqJoint *SqWorld::createPrimaticJoint(SqPrismaticJointDef &def)
{
    SQ_ASSERT(def.lowerTranslation <= def.upperTranslation);
    SQ_ASSERT(lock == false);

    if (lock)
    {
        return nullptr;
    }

    SqPrismaticJoint *joint = new SqPrismaticJoint();
    createJoint(joint, def);

    joint->hertz = def.hertz;
    joint->dampingRatio = def.dampingRatio;
    joint->targetTranslation = def.targetTranslation;
    joint->lowerTranslation = def.lowerTranslation;
    joint->upperTranslation = def.upperTranslation;
    joint->maxMotorForce = def.maxMotorForce;
    joint->motorSpeed = def.motorSpeed;
    joint->enableSpring = def.enableSpring;
    joint->enableLimit = def.enableLimit;
    joint->enableMotor = def.enableMotor;
    joint->enableSyncAngle = def.enableSyncAngle;
    return joint;
}

SqJoint *SqWorld::getJoint(int jointIndex)
{
    return *jointArray.get(jointIndex);
}

SqIsland *SqWorld::createIsland(int setId)
{
    int islandId = islandIdPool.get();
    if (islandId == islandArray.getCount())
    {
        islandArray.push(SqIsland());
    }
    SqIsland *island = islandArray.get(islandId);
    island->reset();
    island->world = this;
    island->islandId = islandId;
    island->setIndex = setId;

    SqSolverSet *set = getSloverSet(setId);
    island->simIndex = set->addIslandSim();
    SqIslandSim *sim = set->getIslandSim(island->simIndex);
    sim->islandId = islandId;

    return island;
}

void SqWorld::removeIsland(int islandId)
{
    // if (world->splitIslandId == islandId)
    // {
    //     world->splitIslandId = B2_NULL_INDEX;
    // }
    SqIsland *island = getIsland(islandId);
    SqSolverSet *set = getSloverSet(island->setIndex);
    set->removeIsland(island->simIndex);
    island->reset();
    islandIdPool.recovery(islandId);
}

/**
 * 合并Awake Set中的Island
 */
void SqWorld::mergeAwakeIslands()
{

    SqSolverSet *awakeSet = getSloverSet(sq_awakeSet);
    int awakeIslandCount = awakeSet->getIslandSimCount();

    // 必须保证Island的parent指向的是链表的root island
    for (int i = 0; i < awakeIslandCount; ++i)
    {
        int islandId = awakeSet->getIslandSim(i)->islandId;
        SqIsland *island = getIsland(islandId);
        island->findRootIsland();
    }

    // Step 2: merge every awake island into its parent (which must be a root island)
    // Reverse to support removal from awake array.
    for (int i = awakeIslandCount - 1; i >= 0; --i)
    {
        int islandId = awakeSet->getIslandSim(i)->islandId;
        SqIsland *island = getIsland(islandId);

        if (island->parentIsland == SQ_NULL_INDEX)
        {
            continue;
        }

        island->mergeToParent();

        // 经过Merge操作之后，所有的Body都在同一个island中了
        // 那么body原来的Island就可以不用了，直接删除掉就可以了
        removeIsland(islandId);
    }
    // b2ValidateConnectivity( world );
    // b2TracyCZoneEnd( merge_islands );
}

void SqWorld::linkContactToIsland(SqContact *contact)
{
    SQ_ASSERT((contact->flags & sq_contactTouchingFlag) != 0);

    SqBody *bodyA = getBody(contact->edges[0].bodyIndex);
    SqBody *bodyB = getBody(contact->edges[1].bodyIndex);

    SQ_ASSERT(bodyA->isEnabled() && bodyB->isEnabled());
    SQ_ASSERT(!bodyA->isStatic() || !bodyB->isStatic());

    // 确保两个都需要awake状态
    if (bodyA->isAwake() && bodyB->isSleep())
    {
        bodyB->wakeBody();
    }
    if (bodyB->isAwake() && bodyA->isSleep())
    {
        bodyA->wakeBody();
    }

    int islandIdA = bodyA->islandId;
    int islandIdB = bodyB->islandId;

    // 静态Body不需要模拟不需要加入到Island中，所以确保不是静态
    // 确保其中有一个Body还没有加入到Island中
    SQ_ASSERT(!bodyA->isStatic() || islandIdA == SQ_NULL_INDEX);
    SQ_ASSERT(!bodyB->isStatic() || islandIdB == SQ_NULL_INDEX);
    SQ_ASSERT(islandIdA != SQ_NULL_INDEX || islandIdB != SQ_NULL_INDEX);

    if (islandIdA == islandIdB)
    {
        getIsland(islandIdA)->addContact(contact);
        return;
    }

    // Union-find root of islandA
    SqIsland *islandA = NULL;
    if (islandIdA != SQ_NULL_INDEX)
    {
        islandA = getIsland(islandIdA)->findRootIsland();
    }

    // Union-find root of islandB
    SqIsland *islandB = NULL;
    if (islandIdB != SQ_NULL_INDEX)
    {
        islandB = getIsland(islandIdB)->findRootIsland();
    }

    SQ_ASSERT(islandA != NULL || islandB != NULL);

    if (islandA != islandB && islandA != NULL && islandB != NULL)
    {
        /**
         * 构建一条island链表，从而实现所有相关的操作都合并到根的Island中
         *
         * 比如有刚体A、B、C
         *
         * A和B发生碰撞调用此函数：将Contact加入A中。构建B->A
         * B和C发生碰撞调用此函数：将Contact加入到B中。构造C->B
         *
         * 经过上面两次之后：
         * 1、B->A
         * 2、C->B->A
         *
         * 所以当merge的时候：
         * 1、merge B ，找到A
         * 2、merge C，找到A
         *
         * 看，所有都在A上了
         */
        SQ_ASSERT(islandA != islandB);
        SQ_ASSERT(islandB->parentIsland == SQ_NULL_INDEX);
        islandB->parentIsland = islandIdA;
    }

    if (islandA != NULL)
    {
        getIsland(islandIdA)->addContact(contact);
    }
    else
    {
        getIsland(islandIdB)->addContact(contact);
    }

    // printf("add to island %d contanct %d\n",contact->islandId, contact->contactId);

    // todo why not merge the islands right here?
}

// This is called when a contact no longer has contact points or when a contact is destroyed.
void SqWorld::unlinkContactFromIsland(SqContact *contact)
{

    // printf("remove to island %d contanct %d\n",contact->islandId, contact->contactId);
    SqIsland *island = getIsland(contact->islandId);
    island->removeContact(contact);
    // b2ValidateIsland(world, islandId);
}

void SqWorld::linkJointToIsland(SqJoint *joint, bool mergeIslands)
{
    SqBody *bodyA = getBody(joint->edges[0].bodyId);
    SqBody *bodyB = getBody(joint->edges[1].bodyId);

    // 确保两个的body都是Awake状态
    if (bodyA->isAwake() && bodyB->isSleep())
    {
        bodyB->wakeBody();
    }
    else if (bodyB->isAwake() && bodyA->isSleep())
    {
        bodyA->wakeBody();
    }

    int islandIdA = bodyA->islandId;
    int islandIdB = bodyB->islandId;

    SQ_ASSERT(islandIdA != SQ_NULL_INDEX || islandIdB != SQ_NULL_INDEX);

    if (islandIdA == islandIdB)
    {
        getIsland(islandIdA)->addJoint(joint);
        return;
    }

    // Union-find root of islandA
    SqIsland *islandA = NULL;
    if (islandIdA != SQ_NULL_INDEX)
    {
        islandA = getIsland(islandIdA)->findRootIsland();
    }

    // Union-find root of islandB
    SqIsland *islandB = NULL;
    if (islandIdB != SQ_NULL_INDEX)
    {
        islandB = getIsland(islandIdB)->findRootIsland();
    }

    SQ_ASSERT(islandA != NULL || islandB != NULL);

    // Union-Find link island roots
    if (islandA != islandB && islandA != NULL && islandB != NULL)
    {
        SQ_ASSERT(islandA != islandB);
        SQ_ASSERT(islandB->parentIsland == SQ_NULL_INDEX);
        islandB->parentIsland = islandIdA;
    }

    if (islandA != NULL)
    {
        getIsland(islandIdA)->addJoint(joint);
    }
    else
    {
        getIsland(islandIdB)->addJoint(joint);
    }

    // Joints need to have islands merged immediately when they are created
    // to keep the island graph valid.
    // However, when a body type is being changed the merge can be deferred until
    // all joints are linked.
    if (mergeIslands)
    {
        mergeAwakeIslands();
    }
}

void SqWorld::unlinkJointFromIsland(SqJoint *joint)
{
    SqIsland *island = getIsland(joint->islandId);
    island->removeJoint(joint);
    // b2ValidateIsland(world, islandId);
}

void SqWorld::setLengthUnitsPerMeter(float lengthUnits)
{
    // SqConfig::getInstance()->setLengthUnitsPerMeter(lengthUnits);
    // sqSetLengthUnitsPerMeter(lengthUnits);
}

float SqWorld::getLengthUnitsPerMeter(void)
{
    return SqConfig::getInstance()->getLengthUnitsPerMeter();
}

typedef struct WorldMoverContext
{
    SqWorld *world;
    SqPlaneResultFcn *fcn;
    SqShapeFilter filter;
    SqCapsuleShape *mover;
    void *userContext;
} WorldMoverContext;

static bool TreeCollideCallback(int proxyId, void *userData, void *context)
{

    SqShape *shape = (SqShape *)userData;
    WorldMoverContext *worldContext = (WorldMoverContext *)context;

    if (SqShouldShapesCollide(shape->getFilter(), worldContext->filter) == false)
    {
        return true;
    }

    SqBody *body = worldContext->world->getBody(shape->getBody());
    const SqTransform &transform = body->getTransform();

    SqPlaneResult result;
    shape->CollideMover(result, worldContext->mover, transform);

    // todo handle deep overlap
    if (result.hit && SqVec2::IsNormalized(result.plane.normal))
    {
        return worldContext->fcn(shape, &result, worldContext->userContext);
    }
    return true;
}

void SqWorld::collideMover(SqCapsuleShape *mover, SqShapeFilter filter, SqPlaneResultFcn *callback, void *context)
{
    if (lock)
    {
        return;
    }

    WorldMoverContext worldContext = {this, callback, filter, mover, context};
    SqVec2 r = {mover->radius, mover->radius};
    SqAABB aabb;
    aabb.lowerBound = SqVec2::Sub(SqVec2::Min(mover->center1, mover->center2), r);
    aabb.upperBound = SqVec2::Add(SqVec2::Max(mover->center1, mover->center2), r);
    for (int i = 0; i < sq_bodyTypeCount; ++i)
    {
        broadPhase.trees[i].Query(aabb, filter.maskBits, &TreeCollideCallback, &worldContext);
    }
}

typedef struct WorldMoverCastContext
{
    SqWorld *world;
    SqShapeFilter filter;
    float fraction;
} WorldMoverCastContext;

static float MoverCastCallback(const SqShapeCastInput *input, int proxyId, void *userData, void *context)
{

    SqShape *shape = (SqShape *)userData;
    WorldMoverCastContext *worldContext = (WorldMoverCastContext *)context;
    SqWorld *world = worldContext->world;
    if (SqShouldShapesCollide(shape->getFilter(), worldContext->filter) == false)
    {
        return worldContext->fraction;
    }
    SqBody *body = world->getBody(shape->getBody());
    SqTransform transform = body->getTransform();

    SqCastOutput output;
    shape->RayCastShape(output, *input, transform);
    if (output.fraction == 0.0f)
    {
        // Ignore overlapping shapes
        return worldContext->fraction;
    }
    worldContext->fraction = output.fraction;
    return output.fraction;
}

float SqWorld::castMover(const SqCapsuleShape *mover, SqVec2 translation, SqShapeFilter filter)
{
    SQ_ASSERT(mover->radius > 2.0f * SQ_LINEAR_SLOP);
    if (lock)
    {
        return 1.0f;
    }

    SqShapeCastInput input;
    input.proxy.points[0] = mover->center1;
    input.proxy.points[1] = mover->center2;
    input.proxy.count = 2;
    input.proxy.radius = mover->radius;
    input.translation = translation;
    input.maxFraction = 1.0f;
    input.canEncroach = true;

    WorldMoverCastContext worldContext = {this, filter, 1.0f};

    for (int i = 0; i < sq_bodyTypeCount; ++i)
    {

        broadPhase.trees[i].ShapeCast(&input, filter.maskBits, &MoverCastCallback, &worldContext);

        if (worldContext.fraction == 0.0f)
        {
            return 0.0f;
        }

        input.maxFraction = worldContext.fraction;
    }

    return worldContext.fraction;
}

typedef struct WorldRayCastContext
{
    SqWorld *world;
    SqCastResultFcn *fcn;
    SqShapeFilter filter;
    float fraction;
    void *userContext;
} WorldRayCastContext;

static float RayCastCallback(const SqRayCastInput *input, int proxyId, void *userData, void *context)
{
    int shapeId = (int)userData;

    WorldRayCastContext *worldContext = (WorldRayCastContext *)context;
    SqWorld *world = worldContext->world;
    SqShape *shape = (SqShape *)userData;

    if (SqShouldShapesCollide(shape->getFilter(), worldContext->filter) == false)
    {
        return input->maxFraction;
    }

    SqBody *body = world->getBody(shape->getBody());
    SqTransform transform = body->getTransform();
    SqCastOutput output;
    shape->RayCast(output, *input, transform);

    if (output.hit)
    {
        float fraction = worldContext->fcn(shape, output.point, output.normal, output.fraction, worldContext->userContext);

        // The user may return -1 to skip this shape
        if (0.0f <= fraction && fraction <= 1.0f)
        {
            worldContext->fraction = fraction;
        }

        return fraction;
    }

    return input->maxFraction;
}

SqTreeStats SqWorld::castRay(SqVec2 origin, SqVec2 translation, SqShapeFilter filter, SqCastResultFcn *fcn, void *context)
{
    SqTreeStats treeStats;
    SQ_ASSERT(lock == false);
    if (lock)
    {
        return treeStats;
    }

    SQ_ASSERT(SqVec2::isValid(origin));
    SQ_ASSERT(SqVec2::isValid(translation));

    SqRayCastInput input = {origin, translation, 1.0f};

    WorldRayCastContext worldContext = {this, fcn, filter, 1.0f, context};

    for (int i = 0; i < sq_bodyTypeCount; ++i)
    {

        SqTreeStats treeResult = broadPhase.trees[i].RayCast(&input, filter.maskBits, &RayCastCallback, &worldContext);
        treeStats.nodeVisits += treeResult.nodeVisits;
        treeStats.leafVisits += treeResult.leafVisits;

        if (worldContext.fraction == 0.0f)
        {
            return treeStats;
        }

        input.maxFraction = worldContext.fraction;
    }

    return treeStats;
}

static float ShapeCastCallback(const SqShapeCastInput *input, int proxyId, void *userData, void *context)
{
    SqShape *shape = (SqShape *)userData;
    WorldRayCastContext *worldContext = (WorldRayCastContext *)context;
    SqWorld *world = worldContext->world;
    if (SqShouldShapesCollide(shape->getFilter(), worldContext->filter) == false)
    {
        return input->maxFraction;
    }

    SqBody *body = world->getBody(shape->getBody());
    SqTransform transform = body->getTransform();

    SqCastOutput output;
    shape->RayCastShape(output, *input, transform);
    if (output.hit)
    {
        float fraction = worldContext->fcn(shape, output.point, output.normal, output.fraction, worldContext->userContext);

        // The user may return -1 to skip this shape
        if (0.0f <= fraction && fraction <= 1.0f)
        {
            worldContext->fraction = fraction;
        }

        return fraction;
    }

    return input->maxFraction;
}

SqTreeStats SqWorld::castShape(const SqShapeProxy *proxy, SqVec2 translation, SqShapeFilter filter, SqCastResultFcn *fcn, void *context)
{
    SqTreeStats treeStats;
    SQ_ASSERT(!lock);

    if (lock)
    {
        return treeStats;
    }

    SQ_ASSERT(SqVec2::isValid(translation));

    SqShapeCastInput input;
    input.proxy = *proxy;
    input.translation = translation;
    input.maxFraction = 1.0f;

    WorldRayCastContext worldContext = {this, fcn, filter, 1.0f, context};

    for (int i = 0; i < sq_bodyTypeCount; ++i)
    {

        SqTreeStats treeResult = broadPhase.trees[i].ShapeCast(&input, filter.maskBits, &ShapeCastCallback, &worldContext);
        treeStats.nodeVisits += treeResult.nodeVisits;
        treeStats.leafVisits += treeResult.leafVisits;

        if (worldContext.fraction == 0.0f)
        {
            return treeStats;
        }

        input.maxFraction = worldContext.fraction;
    }

    return treeStats;
}

typedef struct WorldQueryContext
{
    SqWorld *world;
    SqOverlapResultFcn *fcn;
    SqShapeFilter filter;
    void *userContext;
} WorldQueryContext;

static bool TreeQueryCallback(int proxyId, void *userData, void *context)
{
    WorldQueryContext *worldContext = (WorldQueryContext *)context;
    SqWorld *world = worldContext->world;
    SqShape *shape = (SqShape *)userData;
    if (SqShouldShapesCollide(shape->getFilter(), worldContext->filter) == false)
    {
        return true;
    }
    bool result = worldContext->fcn(shape, worldContext->userContext);
    return result;
}

SqTreeStats SqWorld::overlapAABB(const SqAABB &aabb, const SqShapeFilter &filter, SqOverlapResultFcn *fcn, void *context)
{
    SqTreeStats treeStats;

    // 不清楚为什么Step中不能b2World_OverlapAABB
    // B2_ASSERT( world->locked == false );
    // if ( world->locked )
    // {
    // 	return treeStats;
    // }
    SQ_ASSERT(SqAABB::isValid(aabb));
    WorldQueryContext worldContext = {this, fcn, filter, context};
    for (int i = 0; i < sq_bodyTypeCount; ++i)
    {
        SqTreeStats treeResult = broadPhase.trees[i].Query(aabb, filter.maskBits, &TreeQueryCallback, &worldContext);
        treeStats.nodeVisits += treeResult.nodeVisits;
        treeStats.leafVisits += treeResult.leafVisits;
    }
    return treeStats;
}

typedef struct WorldOverlapContext
{
    SqWorld *world;
    SqOverlapResultFcn *fcn;
    SqShapeFilter filter;
    const SqShapeProxy *proxy;
    void *userContext;
} WorldOverlapContext;

static bool TreeOverlapCallback(int proxyId, void *userData, void *context)
{

    SqShape *shape = (SqShape *)userData;

    WorldOverlapContext *worldContext = (WorldOverlapContext *)context;
    SqWorld *world = worldContext->world;

    if (SqShouldShapesCollide(shape->getFilter(), worldContext->filter) == false)
    {
        return true;
    }

    SqBody *body = world->getBody(shape->getBody());
    SqTransform transform = body->getTransform();

    SqDistanceInput input;
    input.proxyA = *worldContext->proxy;
    input.proxyB = sqMakeProxy(shape);
    input.transformB = transform;
    input.useRadii = true;

    SqSimplexCache cache;
    SqDistanceOutput output = sqShapeDistance(&input, &cache, NULL, 0);

    float tolerance = 0.1f * SQ_LINEAR_SLOP;
    if (output.distance > tolerance)
    {
        return true;
    }
    bool result = worldContext->fcn(shape, worldContext->userContext);
    return result;
}

SqTreeStats SqWorld::overlapShape(const SqShapeProxy *proxy, const SqShapeFilter &filter, SqOverlapResultFcn *fcn, void *context)
{
    SqTreeStats treeStats;

    // b2World *world = b2GetWorldFromId(worldId);
    // B2_ASSERT(world->locked == false);
    // if (world->locked)
    // {
    //     return treeStats;
    // }

    SqAABB aabb = SqAABB::MakeAABB(proxy->points, proxy->count, proxy->radius);
    WorldOverlapContext worldContext = {
        this,
        fcn,
        filter,
        proxy,
        context,
    };

    for (int i = 0; i < sq_bodyTypeCount; ++i)
    {
        SqTreeStats treeResult = broadPhase.trees[i].Query(aabb, filter.maskBits, TreeOverlapCallback, &worldContext);

        treeStats.nodeVisits += treeResult.nodeVisits;
        treeStats.leafVisits += treeResult.leafVisits;
    }

    return treeStats;
}
