#include "SqSolverSet.h"
#include "./sim/SqContactSim.h"
#include "../collision/narrowphase/SqContact.h"
#include "SqWorld.h"

using namespace phxy;

SqSolverSet::~SqSolverSet()
{
    shrink();
}

void SqSolverSet::shrink()
{
    setIndex = SQ_NULL_INDEX;
    world = nullptr;
    bodySims.shrink();
    contactSims.shrink();
    jointSims.shrink();
    islandSims.shrink();
}

/**
 * 注意：外部调用removeContact之后，外部不能再使用此simIndex了
 */
void SqSolverSet::removeContact(int simIndex)
{
    // removeSwap的删除的原理：是将数组最后的一个元素填补到simIndex的位置上
    int movedLocalIndex = contactSims.removeSwap(simIndex);
    if (movedLocalIndex != SQ_NULL_INDEX)
    {
        // 最后一个元素挪了位置之后需要更新相关信息
        SqContactSim *movedContactSim = contactSims.get(simIndex);
        SqContact *movedContact = world->narrowPhase.getContact(movedContactSim->contactId);
        SQ_ASSERT(movedContact->simIndex == movedLocalIndex);
        movedContact->simIndex = simIndex;
    }
}

SqContactSim *SqSolverSet::moveToContact(SqContactSim *simSrc)
{

    // if (setIndex == sq_awakeSet)
    // {
    //     printf("moveToContact before ++++++++\n");
    //     for (int i = 0; i < contactSims.getCount(); ++i)
    //     {
    //         printf(" before %p %u \n",contactSims.get(i), contactSims.get(i)->simFlags);
    //     }
    // }

    int newSimIndex = contactSims.getCount();

    // Add可能造成重新内存分配，外部的地址可能改变
    SqContactSim *dstSim = contactSims.Add();
    memcpy(dstSim, simSrc, sizeof(SqContactSim));
    SqContact *contact = world->narrowPhase.getContact(simSrc->contactId);
    contact->setIndex = setIndex;
    contact->simIndex = newSimIndex;

    // if (setIndex == sq_awakeSet)
    // {
    //     printf("moveToContact after ++++++++\n");
    //     for (int i = 0; i < contactSims.getCount(); ++i)
    //     {
    //         printf(" after %p %u \n", contactSims.get(i), contactSims.get(i)->simFlags);
    //     }
    // }
    return dstSim;
}

SqBodySim *SqSolverSet::moveToBody(SqBodySim *simBody)
{
    int newSimIndex = bodySims.getCount();
    SqBodySim *dstSim = bodySims.Add();
    memcpy(dstSim, simBody, sizeof(SqBodySim));
    SqBody *body = world->getBody(simBody->bodyIndex);
    body->setIndex = setIndex;
    body->simIndex = newSimIndex;
    return dstSim;
}

void SqSolverSet::removeBody(int simIndex)
{
    int movedIndex = bodySims.removeSwap(simIndex);
    if (movedIndex != SQ_NULL_INDEX)
    {
        SqBodySim *movedSim = bodySims.get(simIndex);
        int movedId = movedSim->bodyIndex;
        SqBody *movedBody = world->getBody(movedId);
        SQ_ASSERT(movedBody->simIndex == movedIndex);
        movedBody->simIndex = simIndex;
    }
}

SqJointSim *SqSolverSet::moveToJoint(SqJointSim *simSrc)
{
    int newSimIndex = jointSims.getCount();
    SqJointSim *dstSim = jointSims.Add();
    memcpy(dstSim, simSrc, sizeof(SqJointSim));
    SqJoint *joint = world->getJoint(simSrc->jointId);
    joint->setIndex = setIndex;
    joint->simIndex = newSimIndex;
    return dstSim;
}

void SqSolverSet::removeJoint(int simIndex)
{
    int movedIndex = jointSims.removeSwap(simIndex);
    if (movedIndex != SQ_NULL_INDEX)
    {
        SqJointSim *movedJointSim = jointSims.get(simIndex);
        int movedId = movedJointSim->jointId;
        SqJoint *movedJoint = world->getJoint(movedId);
        SQ_ASSERT(movedJoint->simIndex == movedIndex);
        movedJoint->simIndex = simIndex;
    }
}

SqIslandSim *SqSolverSet::moveToIsland(SqIslandSim *simSrc)
{

    // SqIsland *island1 = world->getIsland(simSrc->islandId);
    // printf("moveToIsland from setIndex %d to setIndex %d island %d \n", island1->setIndex, setIndex, island1->islandId);
    // for (int i = 0; i < islandSims.getCount(); ++i)
    // {
    //     SqIslandSim *sim = islandSims.get(i);
    //     if (sim->islandId == simSrc->islandId)
    //     {
    //         printf(" moveToIsland check set %d island %d \n", setIndex, simSrc->islandId);
    //         SQ_ASSERT(false);
    //     }
    // }

    int newSimIndex = islandSims.getCount();
    SqIslandSim *dstSim = islandSims.Add();
    memcpy(dstSim, simSrc, sizeof(SqIslandSim));
    SqIsland *island = world->getIsland(simSrc->islandId);
    island->setIndex = setIndex;
    island->simIndex = newSimIndex;

    return dstSim;
}

void SqSolverSet::removeIsland(int simIndex)
{

    // SqIslandSim *sim = islandSims.get(simIndex);
    // SqIsland *island = world->getIsland(sim->islandId);
    // printf("removeIsland setIndex %d simIndex %d island %d \n", setIndex, simIndex, island->islandId);

    int movedIndex = islandSims.removeSwap(simIndex);
    if (movedIndex != SQ_NULL_INDEX)
    {
        SqIslandSim *movedElement = islandSims.get(simIndex);
        SqIsland *movedIsland = world->getIsland(movedElement->islandId);
        SQ_ASSERT(movedIsland->simIndex == movedIndex);
        movedIsland->simIndex = simIndex;
    }
}

int SqSolverSet::addIslandSim()
{
    int simIndex = islandSims.getCount();
    SqIslandSim *sim = islandSims.Add();
    return simIndex;
}

void SqSolverSet::copyTo(SqSolverSet *copySet)
{
    SqSolverSet *disabledSet = world->getSloverSet(sq_disabledSet);
    int bodyCount = bodySims.getCount();
    for (int i = 0; i < bodyCount; ++i)
    {
        SqBodySim *simSrc = bodySims.get(i);
        SqBody *body = world->getBody(simSrc->bodyIndex);
        SQ_ASSERT(body->setIndex == setIndex);

        copySet->moveToBody(simSrc);

        // move non-touching contacts from disabled set to awake set
        int contactKey = body->headContactKey;
        while (contactKey != SQ_NULL_INDEX)
        {
            int edgeIndex = contactKey & 1;
            int contactId = contactKey >> 1;

            SqContact *contact = world->narrowPhase.getContact(contactId);
            contactKey = contact->edges[edgeIndex].nextKey;

            // 这里有点特殊：
            // 在创建碰撞点时 sleep的Body上的碰撞点是固定存储到disableSet中的
            //
            if (contact->setIndex != sq_disabledSet)
            {
                SQ_ASSERT(contact->setIndex == sq_awakeSet || contact->setIndex == setIndex);
                continue;
            }

            int localIndex = contact->simIndex;
            SqContactSim *contactSim = disabledSet->contactSims.get(localIndex);

            SQ_ASSERT((contact->flags & sq_contactTouchingFlag) == 0 && contactSim->manifold.pointCount == 0);
            copySet->moveToContact(contactSim);
            disabledSet->removeContact(localIndex);
        }
    }

    {
        int contactCount = contactSims.getCount();
        for (int i = 0; i < contactCount; ++i)
        {
            SqContactSim *contactSim = contactSims.get(i);
            SqContact *contact = world->narrowPhase.getContact(contactSim->contactId);
            SQ_ASSERT(contact->flags & sq_contactTouchingFlag);
            SQ_ASSERT(contactSim->simFlags & sq_simTouchingFlag);
            SQ_ASSERT(contactSim->manifold.pointCount > 0);
            SQ_ASSERT(contact->setIndex == setIndex);
            // if(copySet->setIndex == sq_awakeSet)  b2AddContactToGraph(world, contactSim, contact);
            // contact->setIndex = copySet->setIndex;
            copySet->moveToContact(contactSim);
        }
    }

    {
        int jointCount = jointSims.getCount();
        for (int i = 0; i < jointCount; ++i)
        {
            SqJointSim *jointSim = jointSims.get(i);
            SqJoint *joint = world->getJoint(jointSim->jointId);
            SQ_ASSERT(joint->setIndex == setIndex);
            // if(copySet->setIndex == sq_awakeSet) b2AddJointToGraph(world, jointSim, joint);
            // joint->setIndex = copySet->setIndex;
            copySet->moveToJoint(jointSim);
        }
    }

    {
        int islandCount = islandSims.getCount();
        for (int i = 0; i < islandCount; ++i)
        {
            SqIslandSim *islandSrc = islandSims.get(i);
            SqIsland *island = world->getIsland(islandSrc->islandId);
            copySet->moveToIsland(islandSrc);
        }
    }
}

/**
 * 将当前Body从sourceSet转移到targetSet
 */
void SqSolverSet::transferBody(SqSolverSet *targetSet, SqBody *body)
{
    SQ_ASSERT(body->setIndex == setIndex);
    int sourceIndex = body->simIndex;
    SqBodySim *sourceSim = bodySims.get(body->simIndex);
    targetSet->moveToBody(sourceSim);
    removeBody(sourceIndex);
}

void SqSolverSet::transferJoint(SqSolverSet *targetSet, SqJoint *joint)
{
    SQ_ASSERT(joint->setIndex == setIndex);
    int oldSimIndex = joint->simIndex;
    SqJointSim *sourceSim = jointSims.get(joint->simIndex);
    targetSet->moveToJoint(sourceSim);
    removeJoint(oldSimIndex);
}