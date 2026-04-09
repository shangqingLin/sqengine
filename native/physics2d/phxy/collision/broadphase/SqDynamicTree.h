#pragma once

#include <stdint.h>
#include "../../common/math/SqAABB.h"
#include "../../common/SqTypeDefine.h"

namespace phxy
{
    enum SQTreeNodeFlags
    {
        sq_allocatedNode = 0x0001,

        //记录一下当前的节点AABB是否发生更改
        sq_enlargedNode = 0x0002,

        //标记节点是否为叶节点
        sq_leafNode = 0x0004,
    };

    struct SqTreeStats
    {
        /**
         * 记录了当前查询了多少个节点
         */
        int nodeVisits{0};

        /**
         * 记录了当前查询了多少个叶节点，只记录叶节点，而nodeVisits是所有都记录
         */
        int leafVisits{0};
    };

    typedef bool SqTreeQueryCallbackFcn(int proxyId, void *userData, void *context);

    struct SqDynamicTreeNode
    {

        SqAABB aabb;

        // 使用链表的方式在Tree中存储Node
        union
        {
            // 这个节点的父节点
            int32_t parent;

            // 用于freeList
            int32_t next;
        };

        // 如果是叶节点，则userData有数据，存储外部的数据
        // 如果不是叶节点，则child1和child2有数据，指向他的两个子节点
        union
        {
            // Children (internal node)
            struct
            {
                int32_t child1, child2;
            } children;

            /// User data (leaf node)
            void *userData;
        }; // 8

        uint16_t flags{0};
        uint64_t categoryBits{0};

        /**
         * 记录这个节点在树的高度
         */
        uint16_t height{0};

        SqDynamicTreeNode();
        
        // 无法调用构造函数，因为是通过alloc分配的内存，所以分配出来之后需要调用这个reset初始化
        void reset();
    };

    class SqDynamicTree
    {
    private:
        /**
         * nodes数组当前的大小。单位是Node
         */
        int nodeCapacity;

        /**
         * 当前树中有多少个正在使用的节点
         */
        int nodeCount;

        /**
         * 当前插入了多少个数据节点（即叶节点）
         */
        int proxyCount;

        /**
         * 整条树的根节点
         */
        int root;

        /**
         * 存储所有的节点
         */
        SqDynamicTreeNode *nodes;

        /**
         *  这个也是使用链表的方式存储的
         */
        int freeList;

        int rebuildCapacity;
        int *leafIndices;
        SqAABB *leafBoxes;
        SqVec2 *leafCenters;

        /// Bins for sorting during rebuild
        int *binIndices;
        void buildFreeList(int freeBeginIndex);
        int createNode();
        void freeNode(int nodeId);
        void removeLeaf(int leaf);
        void insertLeaf(int leaf, bool shouldRotate);
        int findBestSibling(const SqAABB &boxD);
        void rotateNodes(int iA);

        int buildTree(int leafCount);
        void Validate();

    public:
        SqDynamicTree();
        ~SqDynamicTree();

        int createProxy(const SqAABB &aabb, uint64_t categoryBits, void *userData);
        void destroyProxy(int proxyId);
        void enlargeProxy(int proxyId, const SqAABB &aabb);
        void moveProxy(int proxyId, const SqAABB &aabb);
        void setCategoryBits(int proxyId, uint64_t categoryBits);
        uint64_t getCategoryBits(int proxyId);
        int getHeight();
        float getAreaRatio();
        SqAABB getRootBounds();
        int rebuild(bool fullBuild);
        const SqAABB &getAABB(int proxyId);
        void *getUserData(int proxyId);

        SqTreeStats Query(const SqAABB &aabb, uint64_t maskBits, SqTreeQueryCallbackFcn *callback, void *context);
        SqTreeStats RayCast(const SqRayCastInput *input, uint64_t maskBits, SqTreeRayCastCallbackFcn *callback, void *context);
        SqTreeStats ShapeCast(const SqShapeCastInput *input, uint64_t maskBits, SqTreeShapeCastCallbackFcn *callback, void *context);
        static bool isLeaf(const SqDynamicTreeNode *node);
        static bool isAllocated(const SqDynamicTreeNode *node);
    };

}