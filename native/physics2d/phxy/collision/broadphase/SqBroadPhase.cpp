#include "SqBroadPhase.h"
#include "../../common/SqConfig.h"
#include "../../dynamics/SqWorld.h"
#include <stdio.h>
#include "SqBroadPhaseFilter.h"

using namespace phxy;

bool phxy::pairQueryCallback(int proxyId, void *userData, void *context)
{

    SqQueryPairContext *queryContext = (SqQueryPairContext *)context;
    SqBroadPhase *broadPhase = &queryContext->world->getBroadPhase();

    int proxyKey = SQ_PROXY_KEY(proxyId, queryContext->queryTreeType);
    int queryProxyKey = queryContext->queryProxyKey;

    // printf("pairQueryCallback %d %d \n", proxyKey, queryProxyKey);

    // 查询到自己，则直接忽略
    if (proxyKey == queryProxyKey)
    {
        return true;
    }

    SqBodyType queryProxyType = SQ_PROXY_TYPE(queryProxyKey);

    /**
     * 去重。
     * 要保证这一对的节点只能生成一对结果而不是生成两对；
     *
     */

    if (queryProxyType == sq_dynamicBody)
    {

        /**
         *
         * 如果A和B都发生了移动，即都在moveArray中，那么在findPairsTask方法中都会拿A和B去查询Tree。
         * 即如果A和B重叠，则拿A去查询Tree会得到结果B，拿B去查询Tree会得到结果A，为了避免生成两对AB pair，则拿proxyKey比较大的哪一方生成结果就行。
         */
        if (queryContext->queryTreeType == sq_dynamicBody && proxyKey < queryProxyKey)
        {
            // 如果proxyKey是小的哪一方，则直接忽略，在queryProxyKey那一边生成pair
            bool moved = broadPhase->proxyHasMove(proxyKey);
            if (moved)
            {
                return true;
            }
        }
    }
    else
    {
        // 也可能拿sq_staticBody和sq_kinematicBody的Body去查询（即queryProxyType==sq_staticBody | sq_kinematicBody），
        // 但只能去查询sq_dynamicBody tree,所以查询到的proxyKey肯定是sq_dynamicBody类型，所以可能在moveArray中，则由proxyKey那边生成对即可
        SQ_ASSERT(queryContext->queryTreeType == sq_dynamicBody);
        bool moved = broadPhase->proxyHasMove(proxyKey);
        if (moved)
        {
            return true;
        }
    }

    // 确保key小的在A，大的在B。方便后续阶段，如下面为此生成的Hash值
    SqShape *shapeA;
    SqShape *shapeB;
    if (proxyKey < queryProxyKey)
    {
        shapeA = (SqShape *)userData;
        shapeB = queryContext->queryShape;
    }
    else
    {
        shapeA = queryContext->queryShape;
        shapeB = (SqShape *)userData;
    }

    // 同一个Body上的Shape不发生碰撞
    if (shapeA->getBody() == shapeB->getBody())
    {
        return true;
    }

    SqWorld *world = queryContext->world;
    if (!SqBroadShapeFilterCollide(world, shapeA, shapeB))
    {
        return true;
    }

    SqBroadPhaseCustomFilterFcn *customFilterFcn = queryContext->world->customBroadPhaseFilterFcn;
    if (customFilterFcn != NULL)
    {
        bool shouldCollide = customFilterFcn(shapeA, shapeB, queryContext->world->customBroadPhaseFilterContext);
        if (shouldCollide == false)
        {
            return true;
        }
    }

    if (shapeA->isSensor() || shapeB->isSensor())
    {
        SensorManager &sensor = queryContext->world->sensor;
        if (shapeA->isSensor())
        {
            sensor.addOverlapSensor(shapeA->getSensorIndex(), shapeB);
        }

        if (shapeB->isSensor())
        {
            sensor.addOverlapSensor(shapeB->getSensorIndex(), shapeA);
        }
        return true;
    }

    // 检查这对碰撞是否之前已经有了，则不需要重复创建
    if (queryContext->world->narrowPhase.hasContact(shapeA, shapeB))
    {
        // contact exists
        return true;
    }

    int pairIndex = ++broadPhase->movePairIndex; // b2AtomicFetchAddInt(&broadPhase->movePairIndex, 1);

    SqMovePair *pair;
    if (pairIndex < broadPhase->movePairCapacity)
    {
        pair = broadPhase->movePairs + pairIndex;
        pair->heap = false;
    }
    else
    {
        pair = (SqMovePair *)SqAlloc(sizeof(SqMovePair));
        pair->heap = true;
    }

    pair->shapeA = shapeA;
    pair->shapeB = shapeB;
    pair->next = queryContext->moveResult->pairList;
    queryContext->moveResult->pairList = pair;

    // 返回true表示tree应该继续往下查询
    return true;
}

SqBroadPhase::SqBroadPhase(SqWorld *world) : world(world), moveSet(16)
{
}

void SqBroadPhase::rebuildTrees()
{
    // 比较重度，可以考虑放到多线程中
    trees[sq_dynamicBody].rebuild(false);
    trees[sq_kinematicBody].rebuild(false);
}

/**
 * 将Shape添加到Tree中，返回代表这个Shape的ID
 */
int SqBroadPhase::createShapeProxy(SqBodyType proxyType, SqShape *shape, const SqAABB &aabb, uint64_t categoryBits, bool forcePairCreation)
{
    SQ_ASSERT(0 <= proxyType && proxyType < sq_bodyTypeCount);

    // printf("create shape proxy %d %p %llu \n",proxyType,shape,categoryBits);

    int proxyId = trees[proxyType].createProxy(aabb, categoryBits, shape);

    // proxyId和proxyType 合成一个数组作为为一个Key
    int proxyKey = SQ_PROXY_KEY(proxyId, proxyType);
    shape->proxyKey = proxyKey;
    shape->tree = &trees[proxyType];
    // printf("create shape proxy %d shape: %p proxyKey:%d category %llu \n",proxyType,shape,shape->proxyKey,categoryBits);

    if (proxyType != sq_staticBody || forcePairCreation)
    {
        proxyMove(proxyKey);
    }
    return proxyKey;
}

void SqBroadPhase::destroyShapeProxy(SqShape *shape)
{
    if (shape->proxyKey != SQ_NULL_INDEX)
    {
        // ???????
        SQ_ASSERT(moveArray.getCount() == moveSet.getCount());

        removeProxyMove(shape->proxyKey);

        SqBodyType proxyType = SQ_PROXY_TYPE(shape->proxyKey);
        int proxyId = SQ_PROXY_ID(shape->proxyKey);

        SQ_ASSERT(0 <= proxyType && proxyType <= sq_bodyTypeCount);
        trees[proxyType].destroyProxy(proxyId);
        shape->proxyKey = SQ_NULL_INDEX;
        shape->tree = nullptr;
    }
}

/**
 * 会涉及到节点树的更改，比较重度的操作
 */
void SqBroadPhase::updateProxy(int proxyKey, const SqAABB &aabb)
{
    SqBodyType proxyType = SQ_PROXY_TYPE(proxyKey);
    int proxyId = SQ_PROXY_ID(proxyKey);
    trees[proxyType].moveProxy(proxyId, aabb);
    proxyMove(proxyKey);

    // printf("updateProxy proxyKey %d aabb p(%f %f %f %f)\n", proxyKey, aabb.lowerBound.x, aabb.lowerBound.y, aabb.upperBound.x, aabb.upperBound.y);
}

/**
 * 只是用来扩大AABB,没有涉及到任何的节点树的更改，所以这个操作比较高效
 */
void SqBroadPhase::elargeProxy(int proxyKey, const SqAABB &aabb)
{
    SQ_ASSERT(proxyKey);
    int typeIndex = SQ_PROXY_TYPE(proxyKey);
    int proxyId = SQ_PROXY_ID(proxyKey);
    SQ_ASSERT(typeIndex != SqBodyType::sq_staticBody);
    trees[typeIndex].enlargeProxy(proxyId, aabb);
    proxyMove(proxyKey);
}

void SqBroadPhase::proxyMove(int proxyKey)
{
    if (!moveSet.addKey(proxyKey + 1))
    {
        moveArray.push(proxyKey);
    }
}

bool SqBroadPhase::proxyHasMove(int proxyKey)
{
    return moveSet.containsKey(proxyKey + 1);
}

/**
 * 从moveArray中移除
 */
void SqBroadPhase::removeProxyMove(int proxyKey)
{
    bool found = moveSet.removeKey(proxyKey + 1);

    if (found)
    {
        // Purge from move buffer. Linear search.
        // todo if I can iterate the move set then I don't need the moveArray
        int count = moveArray.getCount();
        for (int i = 0; i < count; ++i)
        {
            if (*moveArray.get(i) == proxyKey)
            {
                moveArray.removeSwap(i);
                break;
            }
        }
    }
}

/**
 * 判断当前这两个节点是否重叠
 */
bool SqBroadPhase::testOverlap(int proxyKeyA, int proxyKeyB)
{
    int typeIndexA = SQ_PROXY_TYPE(proxyKeyA);
    int proxyIdA = SQ_PROXY_ID(proxyKeyA);
    int typeIndexB = SQ_PROXY_TYPE(proxyKeyB);
    int proxyIdB = SQ_PROXY_ID(proxyKeyB);
    const SqAABB &aabbA = trees[typeIndexA].getAABB(proxyIdA);
    const SqAABB &aabbB = trees[typeIndexB].getAABB(proxyIdB);
    return SqAABB::Overlaps(aabbA, aabbB);
}

void SqBroadPhase::findPairsTask(SqBroadPhase *bp, int startIndex, int endIndex)
{
    SqQueryPairContext queryContext;
    queryContext.world = bp->world;

    // 拿发生移动的节点去查询Tree,遍历moveArray数组
    for (int i = startIndex; i < endIndex; ++i)
    {
        // Initialize move result for this moved proxy
        queryContext.moveResult = bp->moveResults + i;
        queryContext.moveResult->pairList = NULL;

        int proxyKey = *moveArray.get(i);
        if (proxyKey == SQ_NULL_INDEX)
        {
            // proxy was destroyed after it moved
            continue;
        }

        SqBodyType proxyType = SQ_PROXY_TYPE(proxyKey);

        int proxyId = SQ_PROXY_ID(proxyKey);
        queryContext.queryProxyKey = proxyKey;

        SqDynamicTree *baseTree = bp->trees + proxyType;
        const SqAABB &fatAABB = baseTree->getAABB(proxyId);
        queryContext.queryShape = (SqShape *)baseTree->getUserData(proxyId);

        // printf("findPairsTask proxyKey %d aabb (%f %f %f %f)\n", proxyKey, fatAABB.lowerBound.x, fatAABB.lowerBound.y, fatAABB.upperBound.x, fatAABB.upperBound.y);

        SqTreeStats stats = {0};
        if (proxyType == sq_dynamicBody)
        {
            queryContext.queryTreeType = sq_kinematicBody;
            SqTreeStats statsKinematic = bp->trees[sq_kinematicBody].Query(fatAABB, SQ_DEFAULT_MASK_BITS, pairQueryCallback, &queryContext);
            stats.nodeVisits += statsKinematic.nodeVisits;
            stats.leafVisits += statsKinematic.leafVisits;

            queryContext.queryTreeType = sq_staticBody;
            SqTreeStats statsStatic = bp->trees[sq_staticBody].Query(fatAABB, SQ_DEFAULT_MASK_BITS, pairQueryCallback, &queryContext);
            stats.nodeVisits += statsStatic.nodeVisits;
            stats.leafVisits += statsStatic.leafVisits;
        }

        queryContext.queryTreeType = sq_dynamicBody;
        SqTreeStats statsDynamic = bp->trees[sq_dynamicBody].Query(fatAABB, SQ_DEFAULT_MASK_BITS, pairQueryCallback, &queryContext);
        stats.nodeVisits += statsDynamic.nodeVisits;
        stats.leafVisits += statsDynamic.leafVisits;
    }
}

void SqBroadPhase::updatePairs()
{
    int moveCount = moveArray.getCount();
    SQ_ASSERT(moveCount == (int)moveSet.getCount());

    if (moveCount == 0)
    {
        return;
    }

    moveResults = (SqMoveResult *)sqstd::StackTempArenaAllocator::getInstance()->allocateChunk(moveCount * sizeof(SqMoveResult));
    movePairCapacity = 16 * moveCount;
    movePairs = (SqMovePair *)sqstd::StackTempArenaAllocator::getInstance()->allocateChunk(movePairCapacity * sizeof(SqMovePair));

    // printf("====move moveCount %d \n", moveCount);

    findPairsTask(this, 0, moveCount);

    for (int i = 0; i < moveCount; ++i)
    {
        SqMoveResult *result = moveResults + i;
        SqMovePair *pair = result->pairList;

        while (pair != NULL)
        {

            SqShape *shapeA = pair->shapeA;
            SqShape *shapeB = pair->shapeB;
            world->narrowPhase.createContact(shapeA, shapeB);
            if (pair->heap)
            {
                SqMovePair *temp = pair;
                pair = pair->next;
                SqFree(temp, sizeof(SqMovePair));
            }
            else
            {
                pair = pair->next;
            }
        }
    }

    // 内部使用到moveArray数据，所以必须在moveArray清理之前调用
    world->sensor.updateSensors();

    moveArray.clear();
    moveSet.clear();
    sqstd::StackTempArenaAllocator::getInstance()->freeChunk(movePairs);
    movePairs = NULL;
    sqstd::StackTempArenaAllocator::getInstance()->freeChunk(moveResults);
    moveResults = NULL;
}