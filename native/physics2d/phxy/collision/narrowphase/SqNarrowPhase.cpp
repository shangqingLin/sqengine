#include "SqNarrowPhase.h"
#include "../../dynamics/SqWorld.h"
#include "../../dynamics/sim/SqBodySim.h"
#include "../../dynamics/sim/SqContactSim.h"
#include <stdio.h>
#include "../../common/math/SqMath.h"

using namespace phxy;

SqNarrowPhase::SqNarrowPhase(SqWorld *world) : world(world), contactManager(world, this)
{
}

void SqNarrowPhase::createContact(SqShape *shapeA, SqShape *shapeB)
{
    contactManager.createContact(shapeA, shapeB);
}

bool SqNarrowPhase::hasContact(SqShape *shapeA, SqShape *shapeB)
{
    return contactManager.hasContact(shapeA, shapeB);
}

void SqNarrowPhase::collideTask(int contactCount, SqStepContext *stepContext)
{

    SqWorld *world = stepContext->world;
    SqContactSim **contactSims = stepContext->contacts;
    for (int n = 0; n < contactCount; ++n)
    {
        SqContactSim *contactSim = contactSims[n];
        SqShape *shapeA = contactSim->shapeA;
        SqShape *shapeB = contactSim->shapeB;

        SqContact *contact = contactManager.getContact(contactSim->contactId);

        bool overlap = SqAABB::Overlaps(shapeA->getFatAABB(), shapeB->getFatAABB());

        if (overlap == false)
        {
            // 这个接触不再重叠了，可以删除了，通知外部这个Contact End了
            contactSim->simFlags |= SqContactSimFlags::sq_simDisjoint; // 标记这个碰撞点是因为AABB不重叠的方式失去碰撞的
            contactSim->simFlags &= ~SqContactSimFlags::sq_simTouchingFlag;
            stepContext->contactStateBitSet.addValue(contactSim->contactId);
        }
        else
        {
            // AABB仍然重叠

            bool wasTouching = (contactSim->simFlags & SqContactSimFlags::sq_simTouchingFlag);

            SqBody *bodyA = world->getBody(shapeA->getBody());
            SqBody *bodyB = world->getBody(shapeB->getBody());

            SQ_ASSERT(bodyA && bodyB);

            SqBodySim *bodySimA = bodyA->getBodySim();
            SqBodySim *bodySimB = bodyB->getBodySim();

            contactSim->bodyAIndex = bodyA->isAwake() ? bodyA->bodyIndex : SQ_NULL_INDEX;
            contactSim->bodyBIndex = bodyB->isAwake() ? bodyB->bodyIndex : SQ_NULL_INDEX;

            const SqTransform &transformA = bodySimA->transform;
            const SqTransform &transformB = bodySimB->transform;

            SqVec2 centerOffsetA = SqTransform::transformVector(transformA, bodySimA->localCenter);
            SqVec2 centerOffsetB = SqTransform::transformVector(transformB, bodySimB->localCenter);

            // 计算真实的碰撞的情况
            bool touching = contactManager.updateContact(contactSim, shapeA, transformA, centerOffsetA, shapeB, transformB, centerOffsetB);

            // {
            //     SqBody *bodyA = world->getBody(contactSim->shapeA->getBody());
            //     SqBody *bodyB = world->getBody(contactSim->shapeB->getBody());
            //     printf("test update contact %d %d %d %p %p %d %d touching %d \n", contactSim->contactId,
            //            bodyA->a, bodyB->a, bodyA, bodyB, bodyA->bodyIndex, bodyB->bodyIndex, touching);
            // }

            // printf("==== contact %d touching %d flag %u\n", contactSim->contactId, touching, contactSim->simFlags);

            // State changes that affect island connectivity. Also affects contact events.
            if (touching == true && wasTouching == false)
            {
                // printf("collideTask add contact %p %d %p \n", contact, contactSim->contactId, contactSim);
                // 从真实的多边形不接触状态进入到接触的状态
                contactSim->simFlags |= SqContactSimFlags::sq_simStartedTouching;
                stepContext->contactStateBitSet.addValue(contactSim->contactId);
            }
            else if (touching == false && wasTouching == true)
            {

                // printf("collideTask >>>>>> contact %u \n", contactSim->simFlags);

                // AABB还在重叠状态，但真实的多边形已经不接触了
                contactSim->simFlags |= SqContactSimFlags::sq_simStoppedTouching;
                stepContext->contactStateBitSet.addValue(contactSim->contactId);
                // printf("collideTask remove contact %p %d %p %u %d \n", contact,
                //        contactSim->contactId, contactSim,
                //        contactSim->simFlags);
            }
            else if (contactSim->simFlags & SqContactSimFlags::sq_simTouchingFlag)
            {
                // 注意这里处理顺序，先要处理sq_simStartedTouching

                SqBody *bodyA = world->getBody(contactSim->shapeA->getBody());
                SqBody *bodyB = world->getBody(contactSim->shapeB->getBody());

                // printf("=====update contact %d %d %d %p %p %d %d \n", contactSim->contactId,
                //        bodyA->a, bodyB->a, bodyA, bodyB, bodyA->bodyIndex, bodyB->bodyIndex);

                if (bodyA->isEnableHitUpdateContactEvent() || bodyB->isEnableHitUpdateContactEvent())
                {
                    SqContactHitUpdateEvent *hitEvent = world->contactHitEvents.Add();
                    hitEvent->contactId = contactSim->contactId;
                    hitEvent->shapeA = contactSim->shapeA;
                    hitEvent->shapeB = contactSim->shapeB;
                    hitEvent->maniflod = &contactSim->manifold;
                }
            }
        }
    }
}

void SqNarrowPhase::afterCollideProcessContact(SqStepContext &context)
{
    // printf("afterCollideProcessContact count %d \n",context.contactCount);

    uint64_t *bits = context.contactStateBitSet.getData();
    for (int k = 0; k < context.contactStateBitSet.getBlockCount(); ++k)
    {
        uint64_t bitData = bits[k];
        while (bitData)
        {
            uint32_t ctz = sqCTZ64(bitData);
            bitData = bitData & (bitData - 1);
            int contactId = (int)(64 * k + ctz);
            SqContact *contact = contactManager.getContact(contactId);
            SqContactSim *contactSim = world->getSloverSet(contact->setIndex)->contactSims.get(contact->simIndex);
            if (contactSim->simFlags & sq_simDisjoint)
            {
                // printf("fuck remove contact %d \n", contact->contactId);
                // AABB不再重叠了
                // 注意，这里会从数组中移除Contact，则位于此数组后面的ContactSim 的 地址和index都会改变
                contactManager.removeContactById(contact->contactId, false);
            }
            else if (contactSim->simFlags & sq_simStartedTouching)
            {
                // printf("afterCollideProcessContact start %d %d %p %p %u \n", contactId, contact->contactId, contactSim, contact, contactSim->simFlags);

                // SQ_ASSERT(contact->islandId == SQ_NULL_INDEX);
                SQ_ASSERT(contactSim->manifold.pointCount > 0);
                SQ_ASSERT(contact->setIndex == sq_awakeSet);

                // Link first because this wakes colliding bodies and ensures the body sims
                // are in the correct place.
                contact->flags |= sq_contactTouchingFlag;

                world->linkContactToIsland(contact);

                /**
                 * 如果碰撞到sleep的body，那么在linkContactToIsland中就会触发sleep 到 awake的逻辑
                 * 这是可能会造成SloveSet中的contactSims数组重新分配内存，造成地址完成改变了
                 * 所以这里需要重新获取一次
                 */
                contactSim = world->getSloverSet(contact->setIndex)->contactSims.get(contact->simIndex);
                contactSim->simFlags &= ~sq_simStartedTouching;

                // Make sure these didn't change
                // SQ_ASSERT(contact->colorIndex == SQ_NULL_INDEX);

                // Contact sim pointer may have become orphaned due to awake set growth,
                // so I just need to refresh it.
                // contactSim = b2ContactSimArray_Get(&awakeSet->contactSims, localIndex);

                SqBody *bodyA = world->getBody(contact->shapeA->getBody());
                SqBody *bodyB = world->getBody(contact->shapeB->getBody());

                // printf("fuck contact begin %d %d %d %d %d\n", contact->contactId,
                //          bodyA->isEnableBeginContactEvent(), bodyB->isEnableBeginContactEvent(),bodyA->bodyIndex,bodyB->bodyIndex);

                if (bodyA->isEnableBeginContactEvent() || bodyB->isEnableBeginContactEvent())
                {
                    SqContactBeginEvent *hitEvent = world->contactBeginEvents.Add();
                    hitEvent->contactId = contact->contactId;
                    hitEvent->shapeA = contact->shapeA;
                    hitEvent->shapeB = contact->shapeB;
                    hitEvent->maniflod = &contactSim->manifold;
                }

                // b2AddContactToGraph(world, contactSim, contact);
                // b2RemoveNonTouchingContact(world, b2_awakeSet, localIndex);
            }
            else if (contactSim->simFlags & sq_simStoppedTouching)
            {

                SQ_ASSERT(contact->flags & sq_contactTouchingFlag);

                contactSim->simFlags &= ~sq_simStoppedTouching;
                contact->flags &= ~sq_contactTouchingFlag;

                SQ_ASSERT(contactSim->manifold.pointCount == 0);
                world->unlinkContactFromIsland(contact);

                SqBody *bodyA = world->getBody(contact->shapeA->getBody());
                SqBody *bodyB = world->getBody(contact->shapeB->getBody());

                if (bodyA->isEnableEndContactEvent() || bodyB->isEnableEndContactEvent())
                {

                    // printf("fuck a  end evnt %d %d %d \n", contact->contactId, bodyA->bodyIndex, bodyB->bodyIndex);
                    SqContactEndEvent *hitEvent = world->contactEndEvents.Add();
                    hitEvent->contactId = contact->contactId;
                    hitEvent->shapeA = contact->shapeA;
                    hitEvent->shapeB = contact->shapeB;
                    hitEvent->bodyAId = bodyA->getBodyId();
                    hitEvent->bodyBId = bodyB->getBodyId();
                }

                // b2AddNonTouchingContact(world, contact, contactSim);
                // b2RemoveContactFromGraph(world, bodyIdA, bodyIdB, colorIndex, localIndex);
            }
        }
    }
}

void SqNarrowPhase::collide(SqStepContext &context)
{
    context.contactStateBitSet.setAlloc(sqstd::StackTempArenaAllocator::getInstance());
    world->broadPhase.rebuildTrees();
    collideTask(context.contactCount, &context);
    afterCollideProcessContact(context);
    context.contactStateBitSet.clearData();
}