#include "SqContactManager.h"

#include "SqManifold.h"

#include "../../common/math/SqTransform.h"
#include "../../common/math/SqMath.h"
#include "../../common/SqCore.h"

#include "../../geometry/shape/SqShapeDefine.h"
#include "../../geometry/shape/SqShape.h"

#include "../../dynamics/SqWorld.h"
#include "../../dynamics/sim/SqContactSim.h"
#include "../../dynamics/SqBody.h"

#include "SqNarrowPhase.h"

#include "collide-capsule-capsule.h"
#include "collide-capsule-circle.h"

#include "collide-circle-circle.h"

#include "collide-polygon-polygon.h"
#include "collide-polygon-circle.h"
#include "collide-polygon-capsule.h"

#include "collide-segment-circle.h"
#include "collide-segment-capsule.h"
#include "collide-segment-polygon.h"

#include "collide-chainsegment-circle.h"
#include "collide-chainsegment-capsule.h"
#include "collide-chainsegment-polygon.h"

#include <stdio.h>

using namespace phxy;

typedef SqManifold SqManifoldFcn(const SqShape *shapeA, SqTransform &xfA, const SqShape *shapeB, SqTransform &xfB, SqSimplexCache *cache);

struct SqContactRegister
{
    SqManifoldFcn *fcn;
    bool primary;
};
static struct SqContactRegister s_registers[sq_shapeTypeCount][sq_shapeTypeCount];
static bool s_initialized = false;

static SqManifold sqCircleManifold(const SqShape *shapeA, SqTransform &xfA, const SqShape *shapeB, SqTransform &xfB, SqSimplexCache *cache)
{
    return sqCollideCircles(static_cast<SqCircleShape *>((SqShape *)shapeA), xfA, static_cast<SqCircleShape *>((SqShape *)shapeB), xfB);
}

static SqManifold sqCapsuleAndCircleManifold(const SqShape *shapeA, SqTransform &xfA, const SqShape *shapeB, SqTransform &xfB, SqSimplexCache *cache)
{
    return sqCollideCapsuleAndCircle(static_cast<SqCapsuleShape *>((SqShape *)shapeA), xfA, static_cast<SqCircleShape *>((SqShape *)shapeB), xfB);
}

static SqManifold sqCapsuleManifold(const SqShape *shapeA, SqTransform &xfA, const SqShape *shapeB, SqTransform &xfB, SqSimplexCache *cache)
{
    return sqCollideCapsules(static_cast<SqCapsuleShape *>((SqShape *)shapeA), xfA, static_cast<SqCapsuleShape *>((SqShape *)shapeB), xfB);
}

static SqManifold sqPolygonAndCircleManifold(const SqShape *shapeA, SqTransform &xfA, const SqShape *shapeB, SqTransform &xfB, SqSimplexCache *cache)
{
    return sqCollidePolygonAndCircle(static_cast<SqPolygonShape *>((SqShape *)shapeA), xfA, static_cast<SqCircleShape *>((SqShape *)shapeB), xfB);
}

static SqManifold sqPolygonAndCapsuleManifold(const SqShape *shapeA, SqTransform &xfA, const SqShape *shapeB, SqTransform &xfB, SqSimplexCache *cache)
{
    return sqCollidePolygonAndCapsule(static_cast<SqPolygonShape *>((SqShape *)shapeA), xfA, static_cast<SqCapsuleShape *>((SqShape *)shapeB), xfB);
}

static SqManifold sqPolygonManifold(const SqShape *shapeA, SqTransform &xfA, const SqShape *shapeB, SqTransform &xfB, SqSimplexCache *cache)
{
    return sqCollidePolygons(static_cast<SqPolygonShape *>((SqShape *)shapeA), xfA, static_cast<SqPolygonShape *>((SqShape *)shapeB), xfB);
}

static SqManifold sqSegmentAndCircleManifold(const SqShape *shapeA, SqTransform &xfA, const SqShape *shapeB, SqTransform &xfB, SqSimplexCache *cache)
{
    return sqCollideSegmentAndCircle(static_cast<SqEdgeShape *>((SqShape *)shapeA), xfA, static_cast<SqCircleShape *>((SqShape *)shapeB), xfB);
}

static SqManifold sqSegmentAndCapsuleManifold(const SqShape *shapeA, SqTransform &xfA, const SqShape *shapeB, SqTransform &xfB, SqSimplexCache *cache)
{
    return sqCollideSegmentAndCapsule(static_cast<SqEdgeShape *>((SqShape *)shapeA), xfA, static_cast<SqCapsuleShape *>((SqShape *)shapeB), xfB);
}

static SqManifold sqSegmentAndPolygonManifold(const SqShape *shapeA, SqTransform &xfA, const SqShape *shapeB, SqTransform &xfB, SqSimplexCache *cache)
{
    return sqCollideSegmentAndPolygon(static_cast<SqEdgeShape *>((SqShape *)shapeA), xfA, static_cast<SqPolygonShape *>((SqShape *)shapeB), xfB);
}

static SqManifold sqChainSegmentAndCircleManifold(const SqShape *shapeA, SqTransform &xfA, const SqShape *shapeB, SqTransform &xfB, SqSimplexCache *cache)
{
    return sqCollideChainSegmentAndCircle(static_cast<SqChainEdgeShape *>((SqShape *)shapeA), xfA, static_cast<SqCircleShape *>((SqShape *)shapeB), xfB);
}

static SqManifold sqChainSegmentAndCapsuleManifold(const SqShape *shapeA, SqTransform &xfA, const SqShape *shapeB, SqTransform &xfB, SqSimplexCache *cache)
{
    return sqCollideChainSegmentAndCapsule(static_cast<SqChainEdgeShape *>((SqShape *)shapeA), xfA, static_cast<SqCapsuleShape *>((SqShape *)shapeB), xfB, cache);
}

static SqManifold sqChainSegmentAndPolygonManifold(const SqShape *shapeA, SqTransform &xfA, const SqShape *shapeB, SqTransform &xfB, SqSimplexCache *cache)
{
    return sqCollideChainSegmentAndPolygon(static_cast<SqChainEdgeShape *>((SqShape *)shapeA), xfA, static_cast<SqPolygonShape *>((SqShape *)shapeB), xfB, cache);
}

static void sqAddType(SqManifoldFcn *fcn, SqShapeType type1, SqShapeType type2)
{
    SQ_ASSERT(0 <= type1 && type1 < sq_shapeTypeCount);
    SQ_ASSERT(0 <= type2 && type2 < sq_shapeTypeCount);

    s_registers[type1][type2].fcn = fcn;
    s_registers[type1][type2].primary = true;

    if (type1 != type2)
    {
        s_registers[type2][type1].fcn = fcn;
        s_registers[type2][type1].primary = false;
    }
}

SqContactManager::SqContactManager(SqWorld *world, SqNarrowPhase *narrowPhase) : world(world), narrowPhase(narrowPhase), pairSet(16)
{
    if (s_initialized == false)
    {
        s_initialized = true;
        sqAddType(sqCircleManifold, sq_circleShape, sq_circleShape);
        sqAddType(sqCapsuleAndCircleManifold, sq_capsuleShape, sq_circleShape);
        sqAddType(sqCapsuleManifold, sq_capsuleShape, sq_capsuleShape);
        sqAddType(sqPolygonAndCircleManifold, sq_polygonShape, sq_circleShape);
        sqAddType(sqPolygonAndCapsuleManifold, sq_polygonShape, sq_capsuleShape);
        sqAddType(sqPolygonManifold, sq_polygonShape, sq_polygonShape);
        sqAddType(sqSegmentAndCircleManifold, sq_segmentShape, sq_circleShape);
        sqAddType(sqSegmentAndCapsuleManifold, sq_segmentShape, sq_capsuleShape);
        sqAddType(sqSegmentAndPolygonManifold, sq_segmentShape, sq_polygonShape);
        sqAddType(sqChainSegmentAndCircleManifold, sq_chainSegmentShape, sq_circleShape);
        sqAddType(sqChainSegmentAndCapsuleManifold, sq_chainSegmentShape, sq_capsuleShape);
        sqAddType(sqChainSegmentAndPolygonManifold, sq_chainSegmentShape, sq_polygonShape);
    }
}

void SqContactManager::createContact(SqShape *shapeA, SqShape *shapeB)
{
    SqShapeType type1 = shapeA->type;
    SqShapeType type2 = shapeB->type;

    SQ_ASSERT(0 <= type1 && type1 < sq_shapeTypeCount);
    SQ_ASSERT(0 <= type2 && type2 < sq_shapeTypeCount);

    if (s_registers[type1][type2].fcn == NULL)
    {
        // For example, no segment vs segment collision
        return;
    }

    if (s_registers[type1][type2].primary == false)
    {
        // flip order
        createContact(shapeB, shapeA);
        return;
    }

    SqBody *bodyA = world->getBody(shapeA->getBody());
    SqBody *bodyB = world->getBody(shapeB->getBody());

    SQ_ASSERT(bodyA && bodyB);
    SQ_ASSERT(bodyA->setIndex != sq_disabledSet && bodyB->setIndex != sq_disabledSet);
    SQ_ASSERT(bodyA->setIndex != sq_staticSet || bodyB->setIndex != sq_staticSet);

    int setIndex;
    if (bodyA->isAwake() || bodyB->isAwake())
    {
        setIndex = sq_awakeSet;
    }
    else
    {
        // sleeping and non-touching contacts live in the disabled set
        // later if this set is found to be touching then the sleeping
        // islands will be linked and the contact moved to the merged island
        setIndex = sq_disabledSet;
    }

    SqSolverSet *set = world->getSloverSet(setIndex);

    // Create contact key and contact
    int contactId = contactIdPool.get();
    if (contactId == contacts.getCount())
    {
        contacts.push(SqContact());
    }

    // printf("createContact %d \n",contactId);

    SqContact *contact = contacts.get(contactId);
    contact->reset();
    contact->contactId = contactId;
    contact->setIndex = setIndex;
    contact->simIndex = set->contactSims.getCount();
    contact->shapeA = shapeA;
    contact->shapeB = shapeB;

    // printf("createContact setIndex %d contactId %d  %p %p shapeA %d shapeB %d \n", setIndex, contact->contactId, shapeA, shapeB, shapeA->type, shapeB->type);

    // SQ_ASSERT(shapeA->sensorIndex == SQ_NULL_INDEX && shapeB->sensorIndex == SQ_NULL_INDEX);

    // if (shapeA->enableContactEvents || shapeB->enableContactEvents)
    // {
    //     contact->flags |= b2_contactEnableContactEvents;
    // }

    // Connect to body A
    {
        contact->edges[0].bodyIndex = shapeA->getBody();
        contact->edges[0].nextKey = bodyA->headContactKey;
        int keyA = (contactId << 1) | 0;
        int headContactKey = bodyA->headContactKey;
        if (headContactKey != SQ_NULL_INDEX)
        {
            SqContact *headContact = narrowPhase->getContact(headContactKey >> 1);
            headContact->edges[headContactKey & 1].prevKey = keyA;
        }
        bodyA->headContactKey = keyA;
        bodyA->contactCount += 1;
    }

    // Connect to body B
    {
        contact->edges[1].bodyIndex = shapeB->getBody();
        contact->edges[1].prevKey = SQ_NULL_INDEX;
        contact->edges[1].nextKey = bodyB->headContactKey;
        int keyB = (contactId << 1) | 1;
        int headContactKey = bodyB->headContactKey;
        if (bodyB->headContactKey != SQ_NULL_INDEX)
        {
            SqContact *headContact = narrowPhase->getContact(headContactKey >> 1);
            headContact->edges[headContactKey & 1].prevKey = keyB;
        }
        bodyB->headContactKey = keyB;
        bodyB->contactCount += 1;
    }

    // Add to pair set for fast lookup
    uint64_t pairKey = SQ_SHAPE_PAIR_KEY(shapeA, shapeB);
    pairSet.addKey(pairKey);

    // Contacts are created as non-touching. Later if they are found to be touching
    // they will link islands and be moved into the constraint graph.
    SqContactSim *contactSim = set->contactSims.Add();
    contactSim->reset();
    contactSim->contactId = contact->contactId;
    contactSim->shapeA = shapeA;
    contactSim->shapeB = shapeB;
    // contactSim->bodySimIndexA = shapeA
    // These also get updated in the narrow phase
    contactSim->friction = world->frictionCallback(shapeA->getFriction(), 0, shapeB->getFriction(), 0);
    contactSim->restitution = world->restitutionCallback(shapeA->getRestitution(), 0, shapeB->getRestitution(), 0);
    contactSim->tangentSpeed = shapeA->getTangentSpeed() + shapeB->getTangentSpeed();

    // if (shapeA->enablePreSolveEvents || shapeB->enablePreSolveEvents)
    // {
    //     contactSim->simFlags |= b2_simEnablePreSolveEvents;
    // }
}

bool SqContactManager::hasContact(SqShape *shapeA, SqShape *shapeB)
{
    uint64_t pairKey = SQ_SHAPE_PAIR_KEY(shapeA, shapeB);
    return pairSet.containsKey(pairKey);
}

/**
 * 移除一个碰撞点点，当如下情况发生时就会移除碰撞点
 * - broad-phase proxies stop overlapping
 * - a body is destroyed
 * - a body is disabled
 * - a body changes type from dynamic to kinematic or static
 * - a shape is destroyed
 * - contact filtering is modified
 */
void SqContactManager::removeContact(SqContact *contact, bool wakeBodies)
{
    SQ_ASSERT(contact && contact->contactId != SQ_NULL_INDEX);
    uint64_t pairKey = SQ_SHAPE_PAIR_KEY(contact->shapeA, contact->shapeB);
    pairSet.removeKey(pairKey);

    SqContactEdge *edgeA = contact->edges + 0;
    SqContactEdge *edgeB = contact->edges + 1;

    SqBody *bodyA = world->getBody(contact->shapeA->getBody());
    SqBody *bodyB = world->getBody(contact->shapeB->getBody());

    uint32_t flags = contact->flags;
    bool touching = (flags & sq_contactTouchingFlag) != 0;

    // printf("remove contact %d touching %d %d \n", contact->contactId, touching, bodyA->isEnableEndContactEvent() || bodyB->isEnableEndContactEvent());

    if (touching)
    {
        if (bodyA->isEnableEndContactEvent() || bodyB->isEnableEndContactEvent())
        {

            // printf("fuck b end evnt %d %d %d \n", contact->contactId, bodyA->bodyFlag, bodyB->bodyFlag);
            SqContactEndEvent *hitEvent = world->contactEndEvents.Add();
            hitEvent->contactId = contact->contactId;
            hitEvent->shapeA = contact->shapeA;
            hitEvent->shapeB = contact->shapeB;
            hitEvent->bodyAId = bodyA->getBodyId();
            hitEvent->bodyBId = bodyB->getBodyId();
        }
    }

    // 从BodyA中移除这个Contact
    if (edgeA->prevKey != SQ_NULL_INDEX)
    {
        SqContact *prevContact = getContact(edgeA->prevKey >> 1);
        SqContactEdge *prevEdge = prevContact->edges + (edgeA->prevKey & 1);
        prevEdge->nextKey = edgeA->nextKey;
    }

    if (edgeA->nextKey != SQ_NULL_INDEX)
    {
        SqContact *nextContact = getContact(edgeA->nextKey >> 1);
        SqContactEdge *nextEdge = nextContact->edges + (edgeA->nextKey & 1);
        nextEdge->prevKey = edgeA->prevKey;
    }

    int contactId = contact->contactId;
    int edgeKeyA = (contactId << 1) | 0;
    if (bodyA->headContactKey == edgeKeyA)
    {
        bodyA->headContactKey = edgeA->nextKey;
    }
    bodyA->contactCount -= 1;

    // 从BodyB中移除这个Contact
    if (edgeB->prevKey != SQ_NULL_INDEX)
    {
        SqContact *prevContact = getContact(edgeB->prevKey >> 1);
        SqContactEdge *prevEdge = prevContact->edges + (edgeB->prevKey & 1);
        prevEdge->nextKey = edgeB->nextKey;
    }

    if (edgeB->nextKey != SQ_NULL_INDEX)
    {
        SqContact *nextContact = getContact(edgeB->nextKey >> 1);
        SqContactEdge *nextEdge = nextContact->edges + (edgeB->nextKey & 1);
        nextEdge->prevKey = edgeB->prevKey;
    }

    int edgeKeyB = (contactId << 1) | 1;
    if (bodyB->headContactKey == edgeKeyB)
    {
        bodyB->headContactKey = edgeB->nextKey;
    }
    bodyB->contactCount -= 1;

    if (contact->islandId != SQ_NULL_INDEX)
    {
        world->unlinkContactFromIsland(contact);
    }

    // if (contact->colorIndex != B2_NULL_INDEX)
    // {
    //     // contact is an active constraint
    //     SQ_ASSERT(contact->setIndex == b2_awakeSet);
    //     b2RemoveContactFromGraph(world, bodyIdA, bodyIdB, contact->colorIndex, contact->localIndex);
    // }
    // else
    // {
    // contact is non-touching or is sleeping
    // SQ_ASSERT(contact->setIndex != sq_awakeSet || (contact->flags & sq_contactTouchingFlag) == 0);

    SqSolverSet *set = world->getSloverSet(contact->setIndex);
    set->removeContact(contact->simIndex);
    contactIdPool.recovery(contact->contactId);
    contact->reset();

    if (wakeBodies && touching)
    {
        bodyA->wakeBody();
        bodyB->wakeBody();
    }
}

void SqContactManager::removeContactById(int contactId, bool wakeBodies)
{
    // printf("====removeContactById %d \n", contactId);
    removeContact(contacts.get(contactId), wakeBodies);
}

/**
 * 移除指定的Body上所有的碰撞点
 */
void SqContactManager::removeContact(int bodyIndex, bool wakeBodies)
{

    // printf("====removeContact bodyId %d \n", bodyIndex);

    SqBody *body = world->getBody(bodyIndex);
    int edgeKey = body->headContactKey;
    while (edgeKey != SQ_NULL_INDEX)
    {
        int contactId = edgeKey >> 1;
        int edgeIndex = edgeKey & 1;

        SqContact *contact = getContact(contactId);
        edgeKey = contact->edges[edgeIndex].nextKey;
        removeContact(contact, wakeBodies);
    }
    // b2ValidateSolverSets( world );
}

/**
 * 移除碰撞点是由BodyA和BodyB产生的碰撞点
 */
void SqContactManager::removeContact(int bodyAIndex, int bodyBIndex, bool wakeBodies)
{
    // printf("====removeContact two bodyId %d %d \n", bodyAIndex, bodyBIndex);

    int contactKey;
    int otherBody;

    SqBody *bodyA = world->getBody(bodyAIndex);
    SqBody *bodyB = world->getBody(bodyBIndex);

    // 遍历碰撞点数量够少的那个body，减少遍历次数
    if (bodyA->contactCount < bodyB->contactCount)
    {
        contactKey = bodyA->headContactKey;
        otherBody = bodyBIndex;
    }
    else
    {
        contactKey = bodyB->headContactKey;
        otherBody = bodyAIndex;
    }

    while (contactKey != SQ_NULL_INDEX)
    {
        int contactId = contactKey >> 1;
        int edgeIndex = contactKey & 1;

        SqContact *contact = getContact(contactId);
        contactKey = contact->edges[edgeIndex].nextKey;

        int otherEdgeIndex = edgeIndex ^ 1;
        if (contact->edges[otherEdgeIndex].bodyIndex == otherBody)
        {
            removeContact(contact, wakeBodies);
        }
    }

    // b2ValidateSolverSets(world);
}

SqManifold SqContactManager::computeManifold(SqShape *shapeA, SqTransform &transformA, SqShape *shapeB, SqTransform &transformB)
{
    SqManifoldFcn *fcn = s_registers[shapeA->type][shapeB->type].fcn;
    SqSimplexCache cache;
    return fcn(shapeA, transformA, shapeB, transformB, &cache);
}

bool SqContactManager::updateContact(SqContactSim *contactSim, SqShape *shapeA, const SqTransform &transformA, SqVec2 centerOffsetA,
                                     SqShape *shapeB, const SqTransform &transformB, SqVec2 centerOffsetB)
{

    // 存储旧的
    SqManifold oldManifold = contactSim->manifold;

    // 执行碰撞检测和生成碰撞点、碰撞法线等信息
    SqManifoldFcn *fcn = s_registers[shapeA->type][shapeB->type].fcn;
    contactSim->manifold = fcn(shapeA, (SqTransform &)transformA, shapeB, (SqTransform &)transformB, &contactSim->cache);

    int pointCount = contactSim->manifold.pointCount;

    // 如果pointCount为0表示没有接触了
    bool touching = pointCount > 0;

    // printf("updateContact id %d pointCount %d \n", contactSim->contactId, pointCount);

    // Keep these updated in case the values on the shapes are modified
    contactSim->friction = world->frictionCallback(shapeA->friction, 0, shapeB->friction, 0);
    contactSim->restitution = world->restitutionCallback(shapeA->restitution, 0, shapeB->restitution, 0);
    contactSim->tangentSpeed = shapeA->getTangentSpeed() + shapeB->getTangentSpeed();

    // todo branch improves perf?
    // if (shapeA->rollingResistance > 0.0f || shapeB->rollingResistance > 0.0f)
    // {
    //     float radiusA = b2GetShapeRadius(shapeA);
    //     float radiusB = b2GetShapeRadius(shapeB);
    //     float maxRadius = max(radiusA, radiusB);
    //     contactSim->rollingResistance = max(shapeA->rollingResistance, shapeB->rollingResistance) * maxRadius;
    // }
    // else
    // {
    //     contactSim->rollingResistance = 0.0f;
    // }

    // // Get deepest point
    // for (int i = 1; i < manifold->pointCount; ++i)
    // {
    //     float separation = manifold->points[i].separation;
    //     if (separation < bestSeparation)
    //     {
    //         bestSeparation = separation;
    //         bestPoint = manifold->points[i].point;
    //     }
    // }

    // Match old contact ids to new contact ids and copy the
    // stored impulses to warm start the solver.
    int unmatchedCount = 0;
    for (int i = 0; i < pointCount; ++i)
    {
        SqManifoldPoint *mp2 = contactSim->manifold.points + i;

        // 计算从当前碰撞点到刚体质心的坐标，物理中需要使用计算某一点的速度，力矩也需要
        mp2->anchorA = SqVec2::Sub(mp2->anchorA, centerOffsetA);
        mp2->anchorB = SqVec2::Sub(mp2->anchorB, centerOffsetB);

        // printf("point %d anchorA:(%f %f) anchorB:(%f %f) \n", i, mp2->anchorA.x, mp2->anchorA.y, mp2->anchorB.x, mp2->anchorB.y);

        mp2->normalImpulse = 0.0f;
        mp2->tangentImpulse = 0.0f;
        mp2->totalNormalImpulse = 0.0f;
        mp2->normalVelocity = 0.0f;
        mp2->persisted = false;

        uint16_t id2 = mp2->id;

        for (int j = 0; j < oldManifold.pointCount; ++j)
        {
            SqManifoldPoint *mp1 = oldManifold.points + j;

            if (mp1->id == id2)
            {
                mp2->normalImpulse = mp1->normalImpulse;
                mp2->tangentImpulse = mp1->tangentImpulse;
                mp2->persisted = true;
                mp1->normalImpulse = 0.0f;
                mp1->tangentImpulse = 0.0f;
                break;
            }
        }

        unmatchedCount += mp2->persisted ? 0 : 1;
    }

    if (touching)
    {
        contactSim->simFlags |= sq_simTouchingFlag;
    }
    else
    {
        // 没有再接触，则取消接触
        contactSim->simFlags &= ~sq_simTouchingFlag;
    }

    return touching;
}