#pragma once

#include "SqDynamicTree.h"
#include "../../dynamics/SqBodyDefine.h"
#include "../../geometry/shape/SqShape.h"
#include "../../common/collection/SqHashSet.h"
#include "../../common/collection/SqArray.h"

// 将ID和TYPE这两个数组合成为一个数字
// 将ID往前移动2位，后面的两位存储TYPE
#define SQ_PROXY_KEY(ID, TYPE) (((ID) << 2) | (TYPE))

// SQ_PROXY_KEY合成的数字中提取Type
#define SQ_PROXY_TYPE(KEY) ((SqBodyType)((KEY) & 3))

// SQ_PROXY_KEY合成的数字中提取ID
#define SQ_PROXY_ID(KEY) ((KEY) >> 2)

namespace phxy
{
    class SqWorld;
    struct SqMovePair
    {
        SqShape *shapeA{nullptr};
        SqShape *shapeB{nullptr};
        SqMovePair *next{nullptr};
        bool heap{false};
    };

    struct SqMoveResult
    {
        SqMovePair *pairList{nullptr};
    };

    struct SqQueryPairContext
    {
        SqWorld *world{nullptr};
        SqMoveResult *moveResult{nullptr};
        SqBodyType queryTreeType;
        int queryProxyKey{SQ_NULL_INDEX};
        SqShape *queryShape{nullptr};
    };

    class SqBroadPhase
    {
    private:
        SqWorld *world;

        /**
         * 当节点的Transform在外部改动时记录一下，以便告诉Tree需要更新哪些节点
         * 这样就可以只处理变换更改的节点，不全部处理，这样提升性能
         */
        SqArray<int> moveArray;

        /**
         * 为了避免moveArray记录相同的数据，使用一个哈希表去重
         */
        SqHashSet moveSet;

        SqDynamicTree trees[SqBodyType::sq_bodyTypeCount];

        // 下面的变量用于存储查询的结果
        SqMoveResult *moveResults = nullptr;
        SqMovePair *movePairs = nullptr;
        int movePairCapacity = 0;
        int movePairIndex = SQ_NULL_INDEX;

        void findPairsTask(SqBroadPhase *bp, int startIndex, int endIndex);

    public:
        friend class SqContactManager;
        friend class SqWorld;
        friend bool pairQueryCallback(int proxyId, void *userData, void *context);

        SqBroadPhase(SqWorld *);
        void proxyMove(int proxyKey);
        bool proxyHasMove(int proxyKey);
        void removeProxyMove(int proxyKey);
        void rebuildTrees();
        bool testOverlap(int proxyKeyA, int proxyKeyB);
        int createShapeProxy(SqBodyType proxyType, SqShape *, const SqAABB &aabb, uint64_t categoryBits, bool forcePairCreation);
        void destroyShapeProxy(SqShape *shape);
        void updateProxy(int proxyKey, const SqAABB &aabb);
        void elargeProxy(int proxyKey, const SqAABB &aabb);
        void updatePairs();
        inline SqDynamicTree &getTree(int index) { return trees[index]; };
    };
}