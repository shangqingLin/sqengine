#include "SqIsland.h"
#include "../common/constants-define.h"
#include "../collision/narrowphase/SqContact.h"
#include "../common/SqCore.h"
#include "./joint/SqJoint.h"
#include "SqWorld.h"

using namespace phxy;

SqIsland::SqIsland()
{
    reset();
}

void SqIsland::reset()
{
    islandId = SQ_NULL_INDEX;
    parentIsland = SQ_NULL_INDEX;

    headContact = SQ_NULL_INDEX;
    tailContact = SQ_NULL_INDEX;
    contactCount = 0;

    headJoint = SQ_NULL_INDEX;
    tailJoint = SQ_NULL_INDEX;
    jointCount = 0;

    constraintRemoveCount = 0;

    headBody = SQ_NULL_INDEX;
    tailBody = SQ_NULL_INDEX;
    bodyCount = 0;

    setIndex = SQ_NULL_INDEX;
    simIndex = SQ_NULL_INDEX;
}

SqIslandSim *SqIsland::getSim()
{
    SqSolverSet *set = world->getSloverSet(setIndex);
    return set->getIslandSim(simIndex);
}

void SqIsland::addContact(SqContact *contact)
{

    // printf("island %d add contact %d %d \n ", islandId, contact->contactId, contact->islandId);

    // 检查此contact是否已经加入到island中
    SQ_ASSERT(contact->islandId == SQ_NULL_INDEX);
    SQ_ASSERT(contact->islandPrev == SQ_NULL_INDEX);
    SQ_ASSERT(contact->islandNext == SQ_NULL_INDEX);
    if (headContact != SQ_NULL_INDEX)
    {
        contact->islandNext = headContact;
        SqContact *headContactObj = world->narrowPhase.getContact(headContact);
        headContactObj->islandPrev = contact->contactId;
    }

    headContact = contact->contactId;
    if (tailContact == SQ_NULL_INDEX)
    {
        tailContact = headContact;
    }

    contactCount += 1;
    contact->islandId = islandId;

    // b2ValidateIsland(world, islandId);
}

void SqIsland::removeContact(SqContact *contact)
{
    SQ_ASSERT(contact->islandId == islandId);

    // printf("island %d remove contact %d \n ", islandId, contact->contactId);

    if (contact->islandPrev != SQ_NULL_INDEX)
    {
        SqContact *prevContact = world->narrowPhase.getContact(contact->islandPrev);
        SQ_ASSERT(prevContact->islandNext == contact->contactId);
        prevContact->islandNext = contact->islandNext;
    }

    if (contact->islandNext != SQ_NULL_INDEX)
    {
        SqContact *nextContact = world->narrowPhase.getContact(contact->islandNext);
        SQ_ASSERT(nextContact->islandPrev == contact->contactId);
        nextContact->islandPrev = contact->islandPrev;
    }

    if (headContact == contact->contactId)
    {
        headContact = contact->islandNext;
    }

    if (tailContact == contact->contactId)
    {
        tailContact = contact->islandPrev;
    }

    SQ_ASSERT(contactCount > 0);
    contactCount -= 1;
    constraintRemoveCount += 1;

    contact->islandId = SQ_NULL_INDEX;
    contact->islandPrev = SQ_NULL_INDEX;
    contact->islandNext = SQ_NULL_INDEX;
}

void SqIsland::addJoint(SqJoint *joint)
{
    SQ_ASSERT(joint->islandId == SQ_NULL_INDEX);
    SQ_ASSERT(joint->islandPrev == SQ_NULL_INDEX);
    SQ_ASSERT(joint->islandNext == SQ_NULL_INDEX);

    if (this->headJoint != SQ_NULL_INDEX)
    {
        joint->islandNext = headJoint;
        SqJoint *headJoint = world->getJoint(this->headJoint);
        headJoint->islandPrev = joint->jointIndex;
    }

    this->headJoint = joint->jointIndex;
    if (this->tailJoint == SQ_NULL_INDEX)
    {
        this->tailJoint = this->headJoint;
    }

    this->jointCount += 1;
    joint->islandId = islandId;

    // b2ValidateIsland(world, islandId);
}

void SqIsland::removeJoint(SqJoint *joint)
{
    SQ_ASSERT(joint->islandId == islandId);

    if (joint->islandPrev != SQ_NULL_INDEX)
    {
        SqJoint *prevJoint = world->getJoint(joint->islandPrev);
        SQ_ASSERT(prevJoint->islandNext == joint->jointIndex);
        prevJoint->islandNext = joint->islandNext;
    }

    if (joint->islandNext != SQ_NULL_INDEX)
    {
        SqJoint *nextJoint = world->getJoint(joint->islandNext);
        SQ_ASSERT(nextJoint->islandPrev == joint->jointIndex);
        nextJoint->islandPrev = joint->islandPrev;
    }

    if (headJoint == joint->jointIndex)
    {
        headJoint = joint->islandNext;
    }

    if (tailJoint == joint->jointIndex)
    {
        tailJoint = joint->islandPrev;
    }

    SQ_ASSERT(jointCount > 0);
    jointCount -= 1;
    constraintRemoveCount += 1;

    joint->islandId = SQ_NULL_INDEX;
    joint->islandPrev = SQ_NULL_INDEX;
    joint->islandNext = SQ_NULL_INDEX;
}

void SqIsland::addBody(SqBody *body)
{
    body->islandId = islandId;
    if (tailBody != SQ_NULL_INDEX)
    {
        world->getBody(tailBody)->islandNext = body->bodyIndex;
    }
    body->islandPrev = tailBody;
    body->islandNext = SQ_NULL_INDEX;
    tailBody = body->bodyIndex;
    if (headBody == SQ_NULL_INDEX)
    {
        headBody = body->bodyIndex;
    }
    bodyCount += 1;
}

void SqIsland::removeBody(SqBody *body)
{
    if (body->islandId == SQ_NULL_INDEX)
    {
        SQ_ASSERT(body->islandPrev == SQ_NULL_INDEX);
        SQ_ASSERT(body->islandNext == SQ_NULL_INDEX);
        return;
    }

    SQ_ASSERT(body->islandId == islandId);
    SQ_ASSERT(bodyCount > 0);

    if (body->islandPrev != SQ_NULL_INDEX)
    {
        SqBody *prevBody = world->getBody(body->islandPrev);
        prevBody->islandNext = body->islandNext;
    }

    if (body->islandNext != SQ_NULL_INDEX)
    {
        SqBody *nextBody = world->getBody(body->islandNext);
        nextBody->islandPrev = body->islandPrev;
    }

    bodyCount -= 1;
    bool islandDestroyed = false;

    // 如果是根节点
    if (headBody == body->bodyIndex)
    {
        headBody = body->islandNext;

        // 表示Island中的没有body了，没有body那么表示Contact和joint都没有了，所以可以删除此Island
        if (headBody == SQ_NULL_INDEX)
        {
            SQ_ASSERT(tailBody == body->bodyIndex);
            SQ_ASSERT(bodyCount == 0);
            SQ_ASSERT(contactCount == 0);
            SQ_ASSERT(jointCount == 0);
            // printf("  world->removeIsland a %d \n", island->islandId);
            world->removeIsland(islandId);
            islandDestroyed = true;
        }
    }
    else if (tailBody == body->bodyIndex)
    {
        tailBody = body->islandPrev;
    }

    if (islandDestroyed == false)
    {
        // b2ValidateIsland(world, islandId);
    }

    body->islandId = SQ_NULL_INDEX;
    body->islandPrev = SQ_NULL_INDEX;
    body->islandNext = SQ_NULL_INDEX;
}

/**
 * 查找Island的根island
 * 使用路径压缩方法，可以快速找到根节点，这样就不会随着链表越深遍历时间越长了
 * https://en.wikipedia.org/wiki/Disjoint-set_data_structure
 */
SqIsland *SqIsland::findRootIsland()
{
    int rootId = islandId;
    SqIsland *rootIsland = this;
    while (rootIsland->parentIsland != SQ_NULL_INDEX)
    {
        SqIsland *parent = world->getIsland(rootIsland->parentIsland);
        if (parent->parentIsland != SQ_NULL_INDEX)
        {
            // 使用路径压缩（Path Compression） 减少迭代次数
            rootIsland->parentIsland = parent->parentIsland;
        }
        rootId = rootIsland->parentIsland;
        rootIsland = parent;
    }

    // 确保无论island层级有多深，所有的节点都指向根节点
    if (rootIsland != this)
    {
        parentIsland = rootId;
    }

    return rootIsland;
}

void SqIsland::mergeToParent()
{

    // printf("  SqIsland mergeToParent %d root %d \n",islandId,parentIsland);

    SQ_ASSERT(parentIsland != SQ_NULL_INDEX);
    SqIsland *rootIsland = world->getIsland(parentIsland);
    SQ_ASSERT(rootIsland->parentIsland == SQ_NULL_INDEX);

    int rootId = parentIsland;

    {
        // 将body合并到根Island
        int bodyId = headBody;
        while (bodyId != SQ_NULL_INDEX)
        {
            SqBody *body = world->getBody(bodyId);
            body->islandId = rootId;
            bodyId = body->islandNext;
        }
    }

    {
        // 将Contact合并到根Island
        int contactId = headContact;
        while (contactId != SQ_NULL_INDEX)
        {
            SqContact *contact = world->narrowPhase.getContact(contactId);
            contact->islandId = rootId;
            contactId = contact->islandNext;
        }
    }

    {
        // 将Joint合并到根Island
        int jointId = headJoint;
        while (jointId != SQ_NULL_INDEX)
        {
            SqJoint *joint = world->getJoint(jointId);
            joint->islandId = rootId;
            jointId = joint->islandNext;
        }
    }

    {

        // 将Body加入到root的Body链表中
        SQ_ASSERT(rootIsland->tailBody != SQ_NULL_INDEX);
        SqBody *tailBody = world->getBody(rootIsland->tailBody);
        SQ_ASSERT(tailBody->islandNext == SQ_NULL_INDEX);
        tailBody->islandNext = this->headBody; // 直接加入root island的尾部

        SQ_ASSERT(this->headBody != SQ_NULL_INDEX);
        SqBody *headBody = world->getBody(this->headBody);
        SQ_ASSERT(headBody->islandPrev == SQ_NULL_INDEX);
        headBody->islandPrev = rootIsland->tailBody;
        rootIsland->tailBody = this->tailBody;
        rootIsland->bodyCount += bodyCount;
    }

    // 将Contact加入到root island中，并构建Contact链表
    if (rootIsland->headContact == SQ_NULL_INDEX)
    {

        SQ_ASSERT(rootIsland->tailContact == SQ_NULL_INDEX && rootIsland->contactCount == 0);
        rootIsland->headContact = headContact;
        rootIsland->tailContact = tailContact;
        rootIsland->contactCount = contactCount;
    }
    else if (headContact != SQ_NULL_INDEX)
    {
        SQ_ASSERT(tailContact != SQ_NULL_INDEX && contactCount > 0);
        SQ_ASSERT(rootIsland->tailContact != SQ_NULL_INDEX && rootIsland->contactCount > 0);

        SqContact *tailContact = world->narrowPhase.getContact(rootIsland->tailContact);
        SQ_ASSERT(tailContact->islandNext == SQ_NULL_INDEX);
        tailContact->islandNext = headContact;

        SqContact *headContact = world->narrowPhase.getContact(this->headContact);
        SQ_ASSERT(headContact->islandPrev == SQ_NULL_INDEX);
        headContact->islandPrev = rootIsland->tailContact;

        rootIsland->tailContact = this->tailContact;
        rootIsland->contactCount += this->contactCount;
    }

    // 将Joint加入到root island中，并构造Joint链表
    if (rootIsland->headJoint == SQ_NULL_INDEX)
    {
        // Root island has no joints
        SQ_ASSERT(rootIsland->tailJoint == SQ_NULL_INDEX && rootIsland->jointCount == 0);
        rootIsland->headJoint = this->headJoint;
        rootIsland->tailJoint = this->tailJoint;
        rootIsland->jointCount = this->jointCount;
    }
    else if (this->headJoint != SQ_NULL_INDEX)
    {
        // Both islands have joints
        // printf("island merget %d %d \n", tailJoint, jointCount);

        SQ_ASSERT(this->tailJoint != SQ_NULL_INDEX && this->jointCount > 0);
        SQ_ASSERT(rootIsland->tailJoint != SQ_NULL_INDEX && rootIsland->jointCount > 0);

        SqJoint *tailJoint = world->getJoint(rootIsland->tailJoint);
        SQ_ASSERT(tailJoint->islandNext == SQ_NULL_INDEX);
        tailJoint->islandNext = this->headJoint;

        SqJoint *headJoint = world->getJoint(this->headJoint);
        SQ_ASSERT(headJoint->islandPrev == SQ_NULL_INDEX);
        headJoint->islandPrev = rootIsland->tailJoint;

        rootIsland->tailJoint = this->tailJoint;
        rootIsland->jointCount += this->jointCount;
    }

    // Track removed constraints
    rootIsland->constraintRemoveCount += this->constraintRemoveCount;

    // b2ValidateIsland(world, rootId);
}

/**
 * Island是通过Body之间的约束(Joint和Contact)关系来构建的
 * 比如A和B刚体直接创建了一个Joint，那么将A和B放到一个island中
 * 比如A和B刚体发生了碰撞了，那么将A和B放到一个island中
 * 但当A和B的约束没有了，即从碰撞到没有碰撞或删除了它们直接的joint，那么我们就需要将A和B从当前的Island中移除
 * 这个移除就是Island的拆分操作。
 *
 *
 * 注意：
 *   这里面会把当前island销毁的，所以调用split之后，这个island是不能被使用
 *
 */
void SqIsland::split()
{

    if (setIndex != sq_awakeSet)
    {
        return;
    }

    // 没有约束移除，则不需要执行分割操作
    if (constraintRemoveCount == 0)
    {
        return;
    }

    // printf("=====split %d \n", islandId);

    // No lock is needed because I ensure the allocator is not used while this task is active.
    int *stack = (int *)sqstd::StackTempArenaAllocator::getInstance()->allocateChunk(bodyCount * sizeof(int));
    int *bodyIds = (int *)sqstd::StackTempArenaAllocator::getInstance()->allocateChunk(bodyCount * sizeof(int));

    // Build array containing all body indices from base island. These
    // serve as seed bodies for the depth first search (DFS).
    int index = 0;
    int nextBody = headBody;
    while (nextBody != SQ_NULL_INDEX)
    {
        bodyIds[index++] = nextBody;
        SqBody *body = world->getBody(nextBody);
        body->isMarkedForIslandSplit = false;
        nextBody = body->islandNext;
    }
    SQ_ASSERT(index == bodyCount);

    int nextContactId = headContact;
    while (nextContactId != SQ_NULL_INDEX)
    {
        SqContact *contact = world->narrowPhase.getContact(nextContactId);
        contact->isMarkedForIslandSplit = false;
        nextContactId = contact->islandNext;
    }

    int nextJoint = headJoint;
    while (nextJoint != SQ_NULL_INDEX)
    {
        SqJoint *joint = world->getJoint(nextJoint);
        joint->isMarkedForIslandSplit = false;
        nextJoint = joint->islandNext;
    }
    for (int i = 0; i < bodyCount; ++i)
    {
        int seedIndex = bodyIds[i];
        SqBody *seed = world->getBody(seedIndex);

        SQ_ASSERT(seed->setIndex == setIndex);

        if (seed->isMarkedForIslandSplit == true)
        {
            continue;
        }

        int stackCount = 0;
        stack[stackCount++] = seedIndex;
        seed->isMarkedForIslandSplit = true;

        // 创建一个新的，将有有约束的都放到这个island中
        SqIsland *island = world->createIsland(setIndex);

        int islandId = island->islandId;

        // Perform a depth first search (DFS) on the constraint graph.
        while (stackCount > 0)
        {
            // Grab the next body off the stack and add it to the island.
            int bodyId = stack[--stackCount];
            SqBody *body = world->getBody(bodyId);
            SQ_ASSERT(body->setIndex == sq_awakeSet);
            SQ_ASSERT(body->isMarkedForIslandSplit == true);

            // Add body to island
            island->addBody(body);

            // 遍历这个body上所有的Contact
            int contactKey = body->headContactKey;
            while (contactKey != SQ_NULL_INDEX)
            {
                int contactId = contactKey >> 1;
                int edgeIndex = contactKey & 1;

                SqContact *contact = world->narrowPhase.getContact(contactId);
                SQ_ASSERT(contact->contactId == contactId);

                // Next key
                contactKey = contact->edges[edgeIndex].nextKey;

                // Has this contact already been added to this island?
                if (contact->isMarkedForIslandSplit)
                {
                    continue;
                }

                // Is this contact enabled and touching?
                if ((contact->flags & sq_contactTouchingFlag) == 0)
                {
                    continue;
                }

                contact->isMarkedForIslandSplit = true;

                int otherEdgeIndex = edgeIndex ^ 1;
                int otherBodyId = contact->edges[otherEdgeIndex].bodyIndex;
                SqBody *otherBody = world->getBody(otherBodyId);

                // Maybe add other body to stack
                if (otherBody->isMarkedForIslandSplit == false && otherBody->isEnabled())
                {
                    SQ_ASSERT(stackCount < bodyCount);
                    stack[stackCount++] = otherBodyId;
                    otherBody->isMarkedForIslandSplit = true;
                }

                // Add contact to island

                // 相当于删除Island
                contact->islandId = SQ_NULL_INDEX;
                contact->islandNext = SQ_NULL_INDEX;
                contact->islandPrev = SQ_NULL_INDEX;
                island->addContact(contact);
            }

            // Search all joints connect to this body.
            int jointKey = body->headJointKey;
            while (jointKey != SQ_NULL_INDEX)
            {
                int jointId = jointKey >> 1;
                int edgeIndex = jointKey & 1;

                SqJoint *joint = world->getJoint(jointId);
                SQ_ASSERT(joint->jointIndex == jointId);

                // Next key
                jointKey = joint->edges[edgeIndex].nextKey;

                // Has this joint already been added to this island?
                if (joint->isMarkedForIslandSplit)
                {
                    continue;
                }

                joint->isMarkedForIslandSplit = true;

                int otherEdgeIndex = edgeIndex ^ 1;
                int otherBodyId = joint->edges[otherEdgeIndex].bodyId;
                SqBody *otherBody = world->getBody(otherBodyId);

                // Don't simulate joints connected to disabled bodies.
                if (otherBody->setIndex == sq_disabledSet)
                {
                    continue;
                }

                // Maybe add other body to stack
                if (otherBody->isMarkedForIslandSplit == false && otherBody->setIndex == sq_awakeSet)
                {
                    SQ_ASSERT(stackCount < bodyCount);
                    stack[stackCount++] = otherBodyId;
                    otherBody->isMarkedForIslandSplit = true;
                }

                // Add joint to island
                island->addJoint(joint);
            }
        }

        // b2ValidateIsland(world, islandId);
    }

    // printf("  world->removeIsland b %d \n", this->islandId);
    world->removeIsland(this->islandId);
    sqstd::StackTempArenaAllocator::getInstance()->freeChunk(bodyIds);
    sqstd::StackTempArenaAllocator::getInstance()->freeChunk(stack);
}

/***
 * 所谓的Sleep其实就是将Body从Awake Set移到Sleep Set
 * 因为引擎只会模拟Awake Set，从Awake Set移除则就不会被模拟
 */
void SqIsland::sleep()
{
    // printf("SqIsland::sleep() setIndex %d \n",setIndex);

    SQ_ASSERT(setIndex == sq_awakeSet);

    // 有移除约束的Island会被拆分的，拆分肯定需要激活，那么就不需要进入sleep状态了
    if (constraintRemoveCount > 0)
    {
        return;
    }

    // printf("====SqIsland::sleep %d \n", islandId);

    // island is sleeping
    // - create new sleeping solver set
    // - move island to sleeping solver set
    // - identify non-touching contacts that should move to sleeping solver set or disabled set
    // - remove old island
    // - fix island

    // 创建一个Sleep Set，将Body转移到这个Sleep Set中
    SqSolverSet *sleepSet = world->createSloverSet();

    SqSolverSet *awakeSet = world->getSloverSet(sq_awakeSet);
    SQ_ASSERT(0 <= simIndex && simIndex < awakeSet->getIslandSimCount());
    sleepSet->bodySims.resize(bodyCount);
    sleepSet->contactSims.resize(contactCount);
    sleepSet->jointSims.resize(jointCount);

    {
        SqSolverSet *disabledSet = world->getSloverSet(sq_disabledSet);

        int bodyId = headBody;
        while (bodyId != SQ_NULL_INDEX)
        {
            SqBody *body = world->getBody(bodyId);
            SQ_ASSERT(body->isAwake());
            SQ_ASSERT(body->islandId == islandId);

            // Update the body move event to indicate this body fell asleep
            // It could happen the body is forced asleep before it ever moves.
            // if (body->bodyMoveIndex != SQ_NULL_INDEX)
            // {
            //     SqBodyMoveEvent *moveEvent = SqBodyMoveEventArray_Get(&world->bodyMoveEvents, body->bodyMoveIndex);
            //     SQ_ASSERT(moveEvent->bodyId.index1 - 1 == bodyId);
            //     SQ_ASSERT(moveEvent->bodyId.generation == body->generation);
            //     moveEvent->fellAsleep = true;
            //     body->bodyMoveIndex = SQ_NULL_INDEX;
            // }

            int awakeBodyIndex = body->simIndex;
            SqBodySim *awakeSim = awakeSet->bodySims.get(awakeBodyIndex);
            sleepSet->moveToBody(awakeSim);
            awakeSet->removeBody(awakeBodyIndex);

            /**
             * 遍历Body上所有的Contact，也将Contact移动Sleep中
             */
            int contactKey = body->headContactKey;
            while (contactKey != SQ_NULL_INDEX)
            {
                int contactId = contactKey >> 1;
                int edgeIndex = contactKey & 1;

                SqContact *contact = world->narrowPhase.getContact(contactId);

                SQ_ASSERT(contact->setIndex == sq_awakeSet || contact->setIndex == sq_disabledSet);
                contactKey = contact->edges[edgeIndex].nextKey;

                if (contact->setIndex == sq_disabledSet)
                {
                    // already moved to disabled set by another body in the island
                    continue;
                }

                // if (contact->colorIndex != SQ_NULL_INDEX)
                // {
                //     // contact is touching and will be moved separately
                //     SQ_ASSERT((contact->flags & sq_contactTouchingFlag) != 0);
                //     continue;
                // }

                if (contact->flags & sq_contactTouchingFlag)
                {
                    // 还在接触中的碰撞点不能立刻改为Sleep，他随后会Split到自己的Body的Island上，跟随Body一起Sleep
                    continue;
                }

                // the other body may still be awake, it still may go to sleep and then it will be responsible
                // for moving this contact to the disabled set.
                int otherEdgeIndex = edgeIndex ^ 1;
                int otherBodyId = contact->edges[otherEdgeIndex].bodyIndex;
                SqBody *otherBody = world->getBody(otherBodyId);
                if (otherBody->setIndex == sq_awakeSet)
                {
                    continue;
                }

                int localIndex = contact->simIndex;
                SqContactSim *contactSim = awakeSet->contactSims.get(localIndex);
                SQ_ASSERT(contactSim->manifold.pointCount == 0);
                SQ_ASSERT((contact->flags & sq_contactTouchingFlag) == 0);

                // 对于碰撞点，Sleep状态下是移到Disable Set中的
                disabledSet->moveToContact(contactSim);
                awakeSet->removeContact(localIndex);
            }

            bodyId = body->islandNext;
        }
    }

    {
        int contactId = headContact;
        while (contactId != SQ_NULL_INDEX)
        {
            SqContact *contact = world->narrowPhase.getContact(contactId);
            SQ_ASSERT(contact->setIndex == sq_awakeSet);
            SQ_ASSERT(contact->islandId == islandId);
            // int colorIndex = contact->colorIndex;
            // SQ_ASSERT(0 <= colorIndex && colorIndex < B2_GRAPH_COLOR_COUNT);

            // b2GraphColor *color = world->constraintGraph.colors + colorIndex;

            // Remove bodies from graph coloring associated with this constraint
            // if (colorIndex != B2_OVERFLOW_INDEX)
            // {
            //     // might clear a bit for a static body, but this has no effect
            //     b2ClearBit(&color->bodySet, contact->edges[0].bodyId);
            //     b2ClearBit(&color->bodySet, contact->edges[1].bodyId);
            // }

            // int localIndex = contact->localIndex;
            // SqContactSim *awakeContactSim = SqContactSimArray_Get(&color->contactSims, localIndex);

            // int sleepContactIndex = sleepSet->contactSims.count;
            // SqContactSim *sleepContactSim = SqContactSimArray_Add(&sleepSet->contactSims);
            // memcpy(sleepContactSim, awakeContactSim, sizeof(SqContactSim));

            // int movedLocalIndex = SqContactSimArray_RemoveSwap(&color->contactSims, localIndex);
            // if (movedLocalIndex != SQ_NULL_INDEX)
            // {
            //     // fix moved element
            //     SqContactSim *movedContactSim = color->contactSims.data + localIndex;
            //     SqContact *movedContact = SqContactArray_Get(&world->contacts, movedContactSim->contactId);
            //     SQ_ASSERT(movedContact->localIndex == movedLocalIndex);
            //     movedContact->localIndex = localIndex;
            // }

            // contact->setIndex = sleepSetId;
            // contact->colorIndex = SQ_NULL_INDEX;
            // contact->localIndex = sleepContactIndex;

            int oldSimIndex = contact->simIndex;
            SqContactSim *simSrc = awakeSet->contactSims.get(oldSimIndex);
            sleepSet->moveToContact(simSrc);
            awakeSet->removeContact(oldSimIndex);

            contactId = contact->islandNext;
        }
    }

    // move joints
    // this shuffles joints in the awake set
    {
        int jointId = headJoint;
        while (jointId != SQ_NULL_INDEX)
        {
            SqJoint *joint = world->getJoint(jointId);
            SQ_ASSERT(joint->setIndex == sq_awakeSet);
            SQ_ASSERT(joint->islandId == islandId);

            // int colorIndex = joint->colorIndex;
            // int localIndex = joint->localIndex;

            // SQ_ASSERT(0 <= colorIndex && colorIndex < B2_GRAPH_COLOR_COUNT);

            // b2GraphColor *color = world->constraintGraph.colors + colorIndex;

            // SqJointSim *awakeJointSim = SqJointSimArray_Get(&color->jointSims, localIndex);

            // if (colorIndex != B2_OVERFLOW_INDEX)
            // {
            //     // might clear a bit for a static body, but this has no effect
            //     b2ClearBit(&color->bodySet, joint->edges[0].bodyId);
            //     b2ClearBit(&color->bodySet, joint->edges[1].bodyId);
            // }

            // int sleepJointIndex = sleepSet->jointSims.count;
            // SqJointSim *sleepJointSim = SqJointSimArray_Add(&sleepSet->jointSims);
            // memcpy(sleepJointSim, awakeJointSim, sizeof(SqJointSim));

            // int movedIndex = SqJointSimArray_RemoveSwap(&color->jointSims, localIndex);
            // if (movedIndex != SQ_NULL_INDEX)
            // {
            //     // fix moved element
            //     SqJointSim *movedJointSim = color->jointSims.data + localIndex;
            //     int movedId = movedJointSim->jointId;
            //     SqJoint *movedJoint = SqJointArray_Get(&world->joints, movedId);
            //     SQ_ASSERT(movedJoint->localIndex == movedIndex);
            //     movedJoint->localIndex = localIndex;
            // }

            // joint->setIndex = sleepSetId;
            // joint->colorIndex = SQ_NULL_INDEX;
            // joint->localIndex = sleepJointIndex;

            int oldSimIndex = joint->simIndex;
            SqJointSim *simSrc = awakeSet->jointSims.get(oldSimIndex);
            sleepSet->moveToJoint(simSrc);
            awakeSet->removeJoint(oldSimIndex);

            jointId = joint->islandNext;
        }
    }

    // move island struct
    {

        int oldSimIndex = simIndex;

        SqIslandSim *islandSim = awakeSet->getIslandSim(simIndex);

        // printf(" SqIsland::sleep() setIndex %d simIndex %d awakeisland %d selfsetIndex %d islandId %d \n", awakeSet->setIndex, simIndex, islandSim->islandId, setIndex, islandId);

        sleepSet->moveToIsland(islandSim);

        SqIsland *island = world->getIsland(simIndex);
        awakeSet->removeIsland(oldSimIndex);
    }

    // b2ValidateSolverSets(world);
}
