#include "SqDynamicTree.h"
#include "../../common/SqMemory.h"
#include "../../common/SqCore.h"
#include "../../common/SqConfig.h"
#include "../../common/math/SqMath.h"
#include <float.h>
#include <stdio.h>

using namespace phxy;

#define SQ_TREE_STACK_SIZE 1024

enum SqRotateType
{
    sq_rotateNone,
    sq_rotateBF,
    sq_rotateBG,
    sq_rotateCD,
    sq_rotateCE
};

struct SqRebuildItem
{
    int nodeIndex;
    int childCount;

    // Leaf indices
    int startIndex;
    int splitIndex;
    int endIndex;
};

#define SQ_TREE_HEURISTIC 0

// Median split == 0, Surface area heuristic == 1
#define B2_TREE_HEURISTIC 0

#if B2_TREE_HEURISTIC == 0

// Median split heuristic
static int SqPartitionMid(int *indices, SqVec2 *centers, int count)
{
    // Handle trivial case
    if (count <= 2)
    {
        return count / 2;
    }

    SqVec2 lowerBound = centers[0];
    SqVec2 upperBound = centers[0];

    for (int i = 1; i < count; ++i)
    {
        lowerBound = SqVec2::Min(lowerBound, centers[i]);
        upperBound = SqVec2::Max(upperBound, centers[i]);
    }

    SqVec2 d = SqVec2::Sub(upperBound, lowerBound);
    SqVec2 c = {0.5f * (lowerBound.x + upperBound.x), 0.5f * (lowerBound.y + upperBound.y)};

    // Partition longest axis using the Hoare partition scheme
    // https://en.wikipedia.org/wiki/Quicksort
    // https://nicholasvadivelu.com/2021/01/11/array-partition/
    int i1 = 0, i2 = count;
    if (d.x > d.y)
    {
        float pivot = c.x;

        while (i1 < i2)
        {
            while (i1 < i2 && centers[i1].x < pivot)
            {
                i1 += 1;
            };

            while (i1 < i2 && centers[i2 - 1].x >= pivot)
            {
                i2 -= 1;
            };

            if (i1 < i2)
            {
                // Swap indices
                {
                    int temp = indices[i1];
                    indices[i1] = indices[i2 - 1];
                    indices[i2 - 1] = temp;
                }

                // Swap centers
                {
                    SqVec2 temp = centers[i1];
                    centers[i1] = centers[i2 - 1];
                    centers[i2 - 1] = temp;
                }

                i1 += 1;
                i2 -= 1;
            }
        }
    }
    else
    {
        float pivot = c.y;

        while (i1 < i2)
        {
            while (i1 < i2 && centers[i1].y < pivot)
            {
                i1 += 1;
            };

            while (i1 < i2 && centers[i2 - 1].y >= pivot)
            {
                i2 -= 1;
            };

            if (i1 < i2)
            {
                // Swap indices
                {
                    int temp = indices[i1];
                    indices[i1] = indices[i2 - 1];
                    indices[i2 - 1] = temp;
                }

                // Swap centers
                {
                    SqVec2 temp = centers[i1];
                    centers[i1] = centers[i2 - 1];
                    centers[i2 - 1] = temp;
                }

                i1 += 1;
                i2 -= 1;
            }
        }
    }
    SQ_ASSERT(i1 == i2);

    if (i1 > 0 && i1 < count)
    {
        return i1;
    }

    return count / 2;
}

#else

#define B2_BIN_COUNT 8

typedef struct b2TreeBin
{
    SqAABB aabb;
    int count;
} b2TreeBin;

typedef struct b2TreePlane
{
    SqAABB leftAABB;
    SqAABB rightAABB;
    int leftCount;
    int rightCount;
} b2TreePlane;

// "On Fast Construction of SAH-based Bounding Volume Hierarchies" by Ingo Wald
// Returns the left child count
static int SqPartitionSAH(int *indices, int *binIndices, SqAABB *boxes, int count)
{
    SQ_ASSERT(count > 0);

    b2TreeBin bins[B2_BIN_COUNT];
    b2TreePlane planes[B2_BIN_COUNT - 1];

    SqVec2 center = SqAABB::Center(boxes[0]);
    SqAABB centroidAABB;
    centroidAABB.lowerBound = center;
    centroidAABB.upperBound = center;

    for (int i = 1; i < count; ++i)
    {
        center = SqAABB::Center(boxes[i]);
        centroidAABB.lowerBound = SqVec2::Min(centroidAABB.lowerBound, center);
        centroidAABB.upperBound = SqVec2::Max(centroidAABB.upperBound, center);
    }

    SqVec2 d = SqVec2::Sub(centroidAABB.upperBound, centroidAABB.lowerBound);

    // Find longest axis
    int axisIndex;
    float invD;
    if (d.x > d.y)
    {
        axisIndex = 0;
        invD = d.x;
    }
    else
    {
        axisIndex = 1;
        invD = d.y;
    }

    invD = invD > 0.0f ? 1.0f / invD : 0.0f;

    // Initialize bin bounds and count
    for (int i = 0; i < B2_BIN_COUNT; ++i)
    {
        bins[i].aabb.lowerBound = (SqVec2){FLT_MAX, FLT_MAX};
        bins[i].aabb.upperBound = (SqVec2){-FLT_MAX, -FLT_MAX};
        bins[i].count = 0;
    }

    // Assign boxes to bins and compute bin boxes
    // TODO_ERIN optimize
    float binCount = B2_BIN_COUNT;
    float lowerBoundArray[2] = {centroidAABB.lowerBound.x, centroidAABB.lowerBound.y};
    float minC = lowerBoundArray[axisIndex];
    for (int i = 0; i < count; ++i)
    {
        SqVec2 c = SqAABB::Center(boxes[i]);
        float cArray[2] = {c.x, c.y};
        int binIndex = (int)(binCount * (cArray[axisIndex] - minC) * invD);
        binIndex = clampInt(binIndex, 0, B2_BIN_COUNT - 1);
        binIndices[i] = binIndex;
        bins[binIndex].count += 1;
        bins[binIndex].aabb = SqAABB::Union(bins[binIndex].aabb, boxes[i]);
    }

    int planeCount = B2_BIN_COUNT - 1;

    // Prepare all the left planes, candidates for left child
    planes[0].leftCount = bins[0].count;
    planes[0].leftAABB = bins[0].aabb;
    for (int i = 1; i < planeCount; ++i)
    {
        planes[i].leftCount = planes[i - 1].leftCount + bins[i].count;
        planes[i].leftAABB = SqAABB::Union(planes[i - 1].leftAABB, bins[i].aabb);
    }

    // Prepare all the right planes, candidates for right child
    planes[planeCount - 1].rightCount = bins[planeCount].count;
    planes[planeCount - 1].rightAABB = bins[planeCount].aabb;
    for (int i = planeCount - 2; i >= 0; --i)
    {
        planes[i].rightCount = planes[i + 1].rightCount + bins[i + 1].count;
        planes[i].rightAABB = SqAABB::Union(planes[i + 1].rightAABB, bins[i + 1].aabb);
    }

    // Find best split to minimize SAH
    float minCost = FLT_MAX;
    int bestPlane = 0;
    for (int i = 0; i < planeCount; ++i)
    {
        float leftArea = SqAABB::Perimeter(planes[i].leftAABB);
        float rightArea = SqAABB::Perimeter(planes[i].rightAABB);
        int leftCount = planes[i].leftCount;
        int rightCount = planes[i].rightCount;

        float cost = leftCount * leftArea + rightCount * rightArea;
        if (cost < minCost)
        {
            bestPlane = i;
            minCost = cost;
        }
    }

    // Partition node indices and boxes using the Hoare partition scheme
    // https://en.wikipedia.org/wiki/Quicksort
    // https://nicholasvadivelu.com/2021/01/11/array-partition/
    int i1 = 0, i2 = count;
    while (i1 < i2)
    {
        while (i1 < i2 && binIndices[i1] < bestPlane)
        {
            i1 += 1;
        };

        while (i1 < i2 && binIndices[i2 - 1] >= bestPlane)
        {
            i2 -= 1;
        };

        if (i1 < i2)
        {
            // Swap indices
            {
                int temp = indices[i1];
                indices[i1] = indices[i2 - 1];
                indices[i2 - 1] = temp;
            }

            // Swap boxes
            {
                SqAABB temp = boxes[i1];
                boxes[i1] = boxes[i2 - 1];
                boxes[i2 - 1] = temp;
            }

            i1 += 1;
            i2 -= 1;
        }
    }
    SQ_ASSERT(i1 == i2);

    if (i1 > 0 && i1 < count)
    {
        return i1;
    }
    else
    {
        return count / 2;
    }
}

#endif

SqDynamicTreeNode::SqDynamicTreeNode()
{
    reset();
}

void SqDynamicTreeNode::reset()
{
    height = 0;
    flags = sq_allocatedNode;
    children.child1 = SQ_NULL_INDEX;
    children.child2 = SQ_NULL_INDEX;
    parent = SQ_NULL_INDEX;
    categoryBits = SQ_DEFAULT_CATEGORY_BITS;
}

SqDynamicTree::SqDynamicTree()
{
    proxyCount = 0;
    root = SQ_NULL_INDEX;
    nodeCapacity = 16;
    nodeCount = 0;
    nodes = (SqDynamicTreeNode *)SqAlloc(nodeCapacity * sizeof(SqDynamicTreeNode));
    memset(nodes, 0, nodeCapacity * sizeof(SqDynamicTreeNode));
    freeList = 0;
    buildFreeList(0);

    leafIndices = NULL;
    leafBoxes = NULL;
    leafCenters = NULL;
    binIndices = NULL;
    rebuildCapacity = 0;
}

void SqDynamicTree::buildFreeList(int freeBeginIndex)
{
    // 标记 freeBeginIndex及其之后的节点都是为Free节点
    for (int i = freeBeginIndex; i < nodeCapacity - 1; ++i)
    {
        nodes[i].next = i + 1;
    }
    nodes[nodeCapacity - 1].next = SQ_NULL_INDEX;
    freeList = freeBeginIndex;
}

int SqDynamicTree::createNode()
{

    if (freeList == SQ_NULL_INDEX)
    { // 缓冲区里面没有节点，则重新分配一个

        SQ_ASSERT(nodeCount == nodeCapacity);

        SqDynamicTreeNode *oldNodes = nodes;
        int oldCapacity = nodeCapacity;

        nodeCapacity += oldCapacity >> 1; // 乘以2，每次2倍增长
        nodes = (SqDynamicTreeNode *)SqAlloc(nodeCapacity * sizeof(SqDynamicTreeNode));
        SQ_ASSERT(oldNodes != NULL);

        // 复制当前的数据到新内存中
        memcpy(nodes, oldNodes, nodeCount * sizeof(SqDynamicTreeNode));

        // 新分配的内存设置为0
        memset(nodes + nodeCount, 0, (nodeCapacity - nodeCount) * sizeof(SqDynamicTreeNode));

        SqFree(oldNodes, oldCapacity * sizeof(SqDynamicTreeNode));
        buildFreeList(nodeCount);
    }

    int nodeIndex = freeList;
    SqDynamicTreeNode *node = nodes + nodeIndex;
    freeList = node->next;
    node->reset();
    ++nodeCount;
    return nodeIndex;
}

void SqDynamicTree::freeNode(int nodeId)
{
    SQ_ASSERT(0 <= nodeId && nodeId < nodeCapacity);
    SQ_ASSERT(0 < nodeCount);
    nodes[nodeId].next = freeList;
    nodes[nodeId].flags = 0;
    freeList = nodeId;
    --nodeCount;
}

/**
 * 返回nodes数组中的下标，作为唯一的ID
 */
int SqDynamicTree::createProxy(const SqAABB &aabb, uint64_t categoryBits, void *userData)
{

#if ENABLE_ASSERTS
    float hugeValue = SqConfig::getInstance()->getHugValue();
    SQ_ASSERT(-hugeValue < aabb.lowerBound.x && aabb.lowerBound.x < hugeValue);
    SQ_ASSERT(-hugeValue < aabb.lowerBound.y && aabb.lowerBound.y < hugeValue);
    SQ_ASSERT(-hugeValue < aabb.upperBound.x && aabb.upperBound.x < hugeValue);
    SQ_ASSERT(-hugeValue < aabb.upperBound.y && aabb.upperBound.y < hugeValue);
#endif

    int proxyId = createNode();
    SqDynamicTreeNode *node = nodes + proxyId;

    // printf("createProxy %d userData %p\n", proxyId, userData);

    node->aabb = aabb;
    node->userData = userData;
    node->categoryBits = categoryBits;
    node->height = 0;
    node->flags = sq_allocatedNode | sq_leafNode;

    bool shouldRotate = true;
    insertLeaf(proxyId, shouldRotate);

    proxyCount += 1;

    return proxyId;
}

void SqDynamicTree::destroyProxy(int proxyId)
{
    SQ_ASSERT(0 <= proxyId && proxyId < nodeCapacity);
    SQ_ASSERT(SqDynamicTree::isLeaf(nodes + proxyId));

    removeLeaf(proxyId);
    freeNode(proxyId);
    SQ_ASSERT(proxyCount > 0);
    proxyCount -= 1;
}

/**
 * 移除叶节点。叶节点表示的是我们外部的指定的AABB，代表真实的用户数据
 */
void SqDynamicTree::removeLeaf(int leaf)
{
    if (leaf == root)
    {
        root = SQ_NULL_INDEX;
        return;
    }

    int parentIndex = nodes[leaf].parent;
    SqDynamicTreeNode &parent = this->nodes[parentIndex];
    int grandParent = parent.parent;

    // 查询保留哪个叶节点
    int sibling;
    if (parent.children.child1 == leaf) // 删除1，保留2
    {
        sibling = parent.children.child2;
    }
    else
    {
        sibling = parent.children.child1; // 删除2,保留1
    }

    /**
     *   grandParent
     *    /      \
     *   /        \
     *   node    parent
     *           /    \
     *          /      \
     *        sibling  leaf
     *
     * 当删除leaf之后，leaf和sibling共同的父节点也可以删除了
     * 因为这个父节点只有一个sibling子节点了，所以这个父节点就可以删除了
     * 然后将sibling节点合并到grandParent中
     */

    if (grandParent != SQ_NULL_INDEX)
    {
        // 将保留的叶节点合并到grandParent中

        // 这里查询parent节点位于grandParent哪个子节点中
        // 因为parent节点需要删除的，所以直接将sibling节点覆盖到parent占用的那个位置上就可以了
        if (nodes[grandParent].children.child1 == parentIndex)
        {
            nodes[grandParent].children.child1 = sibling;
        }
        else
        {
            nodes[grandParent].children.child2 = sibling;
        }
        nodes[sibling].parent = grandParent;
        freeNode(parentIndex);

        // 更新grandParent这个节点的AABB
        int index = grandParent;
        while (index != SQ_NULL_INDEX)
        {
            SqDynamicTreeNode *node = nodes + index;
            SqDynamicTreeNode *child1 = nodes + node->children.child1;
            SqDynamicTreeNode *child2 = nodes + node->children.child2;

            // Fast union using SSE
            //__m128 aabb1 = _mm_load_ps(&child1->aabb.lowerBound.x);
            //__m128 aabb2 = _mm_load_ps(&child2->aabb.lowerBound.x);
            //__m128 lower = _mm_min_ps(aabb1, aabb2);
            //__m128 upper = _mm_max_ps(aabb1, aabb2);
            //__m128 aabb = _mm_shuffle_ps(lower, upper, _MM_SHUFFLE(3, 2, 1, 0));
            //_mm_store_ps(&node->aabb.lowerBound.x, aabb);

            node->aabb = SqAABB::Union(child1->aabb, child2->aabb);
            node->categoryBits = child1->categoryBits | child2->categoryBits;
            node->height = 1 + max(child1->height, child2->height);

            index = node->parent;
        }
    }
    else
    {
        root = sibling;
        nodes[sibling].parent = SQ_NULL_INDEX;
        freeNode(parentIndex);
    }
}

void SqDynamicTree::insertLeaf(int leaf, bool shouldRotate)
{
    if (root == SQ_NULL_INDEX)
    {
        root = leaf;
        nodes[root].parent = SQ_NULL_INDEX;
        return;
    }
    // 1: 首先查找最优的插入的父节点，以保持尽量的平衡
    const SqAABB &leafAABB = nodes[leaf].aabb;
    int sibling = findBestSibling(leafAABB);
    // 2：创建一个新的父节点作为存储着插入的节点和sibling
    int oldParent = nodes[sibling].parent;
    int newParent = createNode();

    // warning: node pointer can change after allocation
    nodes[newParent].parent = oldParent;
    nodes[newParent].userData = nullptr;
    nodes[newParent].aabb = SqAABB::Union(leafAABB, nodes[sibling].aabb);
    nodes[newParent].categoryBits = nodes[leaf].categoryBits | nodes[sibling].categoryBits;
    nodes[newParent].height = nodes[sibling].height + 1;

    if (oldParent != SQ_NULL_INDEX)
    {
        // The sibling was not the root.
        if (nodes[oldParent].children.child1 == sibling)
        {
            nodes[oldParent].children.child1 = newParent;
        }
        else
        {
            nodes[oldParent].children.child2 = newParent;
        }

        nodes[newParent].children.child1 = sibling;
        nodes[newParent].children.child2 = leaf;
        nodes[sibling].parent = newParent;
        nodes[leaf].parent = newParent;
    }
    else
    {
        // The sibling was the root.
        nodes[newParent].children.child1 = sibling;
        nodes[newParent].children.child2 = leaf;
        nodes[sibling].parent = newParent;
        nodes[leaf].parent = newParent;
        root = newParent;
    }

    // Stage 3: walk back up the tree fixing heights and AABBs
    int index = nodes[leaf].parent;
    while (index != SQ_NULL_INDEX)
    {
        int child1 = nodes[index].children.child1;
        int child2 = nodes[index].children.child2;

        SQ_ASSERT(child1 != SQ_NULL_INDEX);
        SQ_ASSERT(child2 != SQ_NULL_INDEX);

        nodes[index].aabb = SqAABB::Union(nodes[child1].aabb, nodes[child2].aabb);
        nodes[index].categoryBits = nodes[child1].categoryBits | nodes[child2].categoryBits;
        nodes[index].height = 1 + max(nodes[child1].height, nodes[child2].height);
        nodes[index].flags |= (nodes[child1].flags | nodes[child2].flags) & sq_enlargedNode;

        if (shouldRotate)
        {
            rotateNodes(index);
        }

        index = nodes[index].parent;
    }
}

/**
 * 表面积启发式算法是一种强大的评估指标，可用于指导BVH的构建过程。
 */
int SqDynamicTree::findBestSibling(const SqAABB &boxD)
{
    SqVec2 centerD = SqAABB::Center(boxD);
    float areaD = SqAABB::Perimeter(boxD);

    int rootIndex = root;

    const SqAABB &rootBox = nodes[rootIndex].aabb;

    // Area of current node
    float areaBase = SqAABB::Perimeter(rootBox);

    // Area of inflated node
    float directCost = SqAABB::Perimeter(SqAABB::Union(rootBox, boxD));
    float inheritedCost = 0.0f;

    int bestSibling = rootIndex;
    float bestCost = directCost;

    // Descend the tree from root, following a single greedy path.
    int index = rootIndex;
    while (nodes[index].height > 0)
    {

        int child1 = nodes[index].children.child1;
        int child2 = nodes[index].children.child2;

        SQ_ASSERT(child1 != SQ_NULL_INDEX);

        // 如果nodes[index]与新叶子组成兄弟，则为此兄弟创建新的父节点的成本
        float cost = directCost + inheritedCost;

        // Sometimes there are multiple identical costs within tolerance.
        // This breaks the ties using the centroid distance.
        if (cost < bestCost)
        {
            bestSibling = index;
            bestCost = cost;
        }

        // Inheritance cost seen by children
        inheritedCost += directCost - areaBase;

        bool leaf1 = nodes[child1].height == 0;
        bool leaf2 = nodes[child2].height == 0;

        // 计算当将节点插入到child1时的成本
        float lowerCost1 = FLT_MAX;
        const SqAABB &box1 = nodes[child1].aabb;
        float directCost1 = SqAABB::Perimeter(SqAABB::Union(box1, boxD));
        float area1 = 0.0f;
        if (leaf1)
        { // 如果是叶节点，则使用这种方式计算cost

            // Cost of creating new node and increasing area of node P
            float cost1 = directCost1 + inheritedCost;

            // Need this here due to while condition above
            if (cost1 < bestCost)
            {
                bestSibling = child1;
                bestCost = cost1;
            }
        }
        else
        {
            // 如果是内部节点，使用下面的方式计算cost
            area1 = SqAABB::Perimeter(box1);

            // Lower bound cost of inserting under child 1. The minimum accounts for two possibilities:
            // 1. Child1 could be the sibling with cost1 = inheritedCost + directCost1
            // 2. A descendent of child1 could be the sibling with the lower bound cost of
            //       cost1 = inheritedCost + (directCost1 - area1) + areaD
            // This minimum here leads to the minimum of these two costs.
            lowerCost1 = inheritedCost + directCost1 + min(areaD - area1, 0.0f);
        }

        // 计算如果将节点插入到child2时的成本
        float lowerCost2 = FLT_MAX;
        const SqAABB &box2 = nodes[child2].aabb;
        float directCost2 = SqAABB::Perimeter(SqAABB::Union(box2, boxD));
        float area2 = 0.0f;
        if (leaf2)
        {
            float cost2 = directCost2 + inheritedCost;

            if (cost2 < bestCost)
            {
                bestSibling = child2;
                bestCost = cost2;
            }
        }
        else
        {
            area2 = SqAABB::Perimeter(box2);
            lowerCost2 = inheritedCost + directCost2 + min(areaD - area2, 0.0f);
        }

        if (leaf1 && leaf2)
        {
            break;
        }

        // Can the cost possibly be decreased?
        if (bestCost <= lowerCost1 && bestCost <= lowerCost2)
        {
            break;
        }

        if (lowerCost1 == lowerCost2 && leaf1 == false)
        {
            SQ_ASSERT(lowerCost1 < FLT_MAX);
            SQ_ASSERT(lowerCost2 < FLT_MAX);

            // No clear choice based on lower bound surface area. This can happen when both
            // children fully contain D. Fall back to node distance.
            SqVec2 d1 = SqVec2::Sub(SqAABB::Center(box1), centerD);
            SqVec2 d2 = SqVec2::Sub(SqAABB::Center(box2), centerD);
            lowerCost1 = SqVec2::LengthSquared(d1);
            lowerCost2 = SqVec2::LengthSquared(d2);
        }

        // Descend
        if (lowerCost1 < lowerCost2 && leaf1 == false)
        {
            index = child1;
            areaBase = area1;
            directCost = directCost1;
        }
        else
        {
            index = child2;
            areaBase = area2;
            directCost = directCost2;
        }

        SQ_ASSERT(nodes[index].height > 0);
    }

    return bestSibling;
}

void SqDynamicTree::rotateNodes(int iA)
{
    SQ_ASSERT(iA != SQ_NULL_INDEX);
    SqDynamicTreeNode *A = nodes + iA;
    if (A->height < 2)
    {
        return;
    }

    int iB = A->children.child1;
    int iC = A->children.child2;
    SQ_ASSERT(0 <= iB && iB < nodeCapacity);
    SQ_ASSERT(0 <= iC && iC < nodeCapacity);

    SqDynamicTreeNode *B = nodes + iB;
    SqDynamicTreeNode *C = nodes + iC;

    if (B->height == 0)
    {
        // B is a leaf and C is internal
        SQ_ASSERT(C->height > 0);

        int iF = C->children.child1;
        int iG = C->children.child2;
        SqDynamicTreeNode *F = nodes + iF;
        SqDynamicTreeNode *G = nodes + iG;
        SQ_ASSERT(0 <= iF && iF < nodeCapacity);
        SQ_ASSERT(0 <= iG && iG < nodeCapacity);

        // Base cost
        float costBase = SqAABB::Perimeter(C->aabb);

        // Cost of swapping B and F
        SqAABB aabbBG = SqAABB::Union(B->aabb, G->aabb);
        float costBF = SqAABB::Perimeter(aabbBG);

        // Cost of swapping B and G
        SqAABB aabbBF = SqAABB::Union(B->aabb, F->aabb);
        float costBG = SqAABB::Perimeter(aabbBF);

        if (costBase < costBF && costBase < costBG)
        {
            // Rotation does not improve cost
            return;
        }

        if (costBF < costBG)
        {
            // Swap B and F
            A->children.child1 = iF;
            C->children.child1 = iB;

            B->parent = iC;
            F->parent = iA;

            C->aabb = aabbBG;

            C->height = 1 + max(B->height, G->height);
            A->height = 1 + max(C->height, F->height);
            C->categoryBits = B->categoryBits | G->categoryBits;
            A->categoryBits = C->categoryBits | F->categoryBits;
            C->flags |= (B->flags | G->flags) & sq_enlargedNode;
            A->flags |= (C->flags | F->flags) & sq_enlargedNode;
        }
        else
        {
            // Swap B and G
            A->children.child1 = iG;
            C->children.child2 = iB;

            B->parent = iC;
            G->parent = iA;

            C->aabb = aabbBF;

            C->height = 1 + max(B->height, F->height);
            A->height = 1 + max(C->height, G->height);
            C->categoryBits = B->categoryBits | F->categoryBits;
            A->categoryBits = C->categoryBits | G->categoryBits;
            C->flags |= (B->flags | F->flags) & sq_enlargedNode;
            A->flags |= (C->flags | G->flags) & sq_enlargedNode;
        }
    }
    else if (C->height == 0)
    {
        // C is a leaf and B is internal
        SQ_ASSERT(B->height > 0);

        int iD = B->children.child1;
        int iE = B->children.child2;
        SqDynamicTreeNode *D = nodes + iD;
        SqDynamicTreeNode *E = nodes + iE;
        SQ_ASSERT(0 <= iD && iD < nodeCapacity);
        SQ_ASSERT(0 <= iE && iE < nodeCapacity);

        // Base cost
        float costBase = SqAABB::Perimeter(B->aabb);

        // Cost of swapping C and D
        SqAABB aabbCE = SqAABB::Union(C->aabb, E->aabb);
        float costCD = SqAABB::Perimeter(aabbCE);

        // Cost of swapping C and E
        SqAABB aabbCD = SqAABB::Union(C->aabb, D->aabb);
        float costCE = SqAABB::Perimeter(aabbCD);

        if (costBase < costCD && costBase < costCE)
        {
            // Rotation does not improve cost
            return;
        }

        if (costCD < costCE)
        {
            // Swap C and D
            A->children.child2 = iD;
            B->children.child1 = iC;

            C->parent = iB;
            D->parent = iA;

            B->aabb = aabbCE;

            B->height = 1 + max(C->height, E->height);
            A->height = 1 + max(B->height, D->height);
            B->categoryBits = C->categoryBits | E->categoryBits;
            A->categoryBits = B->categoryBits | D->categoryBits;
            B->flags |= (C->flags | E->flags) & sq_enlargedNode;
            A->flags |= (B->flags | D->flags) & sq_enlargedNode;
        }
        else
        {
            // Swap C and E
            A->children.child2 = iE;
            B->children.child2 = iC;

            C->parent = iB;
            E->parent = iA;

            B->aabb = aabbCD;
            B->height = 1 + max(C->height, D->height);
            A->height = 1 + max(B->height, E->height);
            B->categoryBits = C->categoryBits | D->categoryBits;
            A->categoryBits = B->categoryBits | E->categoryBits;
            B->flags |= (C->flags | D->flags) & sq_enlargedNode;
            A->flags |= (B->flags | E->flags) & sq_enlargedNode;
        }
    }
    else
    {
        int iD = B->children.child1;
        int iE = B->children.child2;
        int iF = C->children.child1;
        int iG = C->children.child2;

        SqDynamicTreeNode *D = nodes + iD;
        SqDynamicTreeNode *E = nodes + iE;
        SqDynamicTreeNode *F = nodes + iF;
        SqDynamicTreeNode *G = nodes + iG;

        SQ_ASSERT(0 <= iD && iD < nodeCapacity);
        SQ_ASSERT(0 <= iE && iE < nodeCapacity);
        SQ_ASSERT(0 <= iF && iF < nodeCapacity);
        SQ_ASSERT(0 <= iG && iG < nodeCapacity);

        // Base cost
        float areaB = SqAABB::Perimeter(B->aabb);
        float areaC = SqAABB::Perimeter(C->aabb);
        float costBase = areaB + areaC;
        enum SqRotateType bestRotation = sq_rotateNone;
        float bestCost = costBase;

        // Cost of swapping B and F
        SqAABB aabbBG = SqAABB::Union(B->aabb, G->aabb);
        float costBF = areaB + SqAABB::Perimeter(aabbBG);
        if (costBF < bestCost)
        {
            bestRotation = sq_rotateBF;
            bestCost = costBF;
        }

        // Cost of swapping B and G
        SqAABB aabbBF = SqAABB::Union(B->aabb, F->aabb);
        float costBG = areaB + SqAABB::Perimeter(aabbBF);
        if (costBG < bestCost)
        {
            bestRotation = sq_rotateBG;
            bestCost = costBG;
        }

        // Cost of swapping C and D
        SqAABB aabbCE = SqAABB::Union(C->aabb, E->aabb);
        float costCD = areaC + SqAABB::Perimeter(aabbCE);
        if (costCD < bestCost)
        {
            bestRotation = sq_rotateCD;
            bestCost = costCD;
        }

        // Cost of swapping C and E
        SqAABB aabbCD = SqAABB::Union(C->aabb, D->aabb);
        float costCE = areaC + SqAABB::Perimeter(aabbCD);
        if (costCE < bestCost)
        {
            bestRotation = sq_rotateCE;
            // bestCost = costCE;
        }

        switch (bestRotation)
        {
        case sq_rotateNone:
            break;

        case sq_rotateBF:
            A->children.child1 = iF;
            C->children.child1 = iB;

            B->parent = iC;
            F->parent = iA;

            C->aabb = aabbBG;
            C->height = 1 + max(B->height, G->height);
            A->height = 1 + max(C->height, F->height);
            C->categoryBits = B->categoryBits | G->categoryBits;
            A->categoryBits = C->categoryBits | F->categoryBits;
            C->flags |= (B->flags | G->flags) & sq_enlargedNode;
            A->flags |= (C->flags | F->flags) & sq_enlargedNode;
            break;

        case sq_rotateBG:
            A->children.child1 = iG;
            C->children.child2 = iB;

            B->parent = iC;
            G->parent = iA;

            C->aabb = aabbBF;
            C->height = 1 + max(B->height, F->height);
            A->height = 1 + max(C->height, G->height);
            C->categoryBits = B->categoryBits | F->categoryBits;
            A->categoryBits = C->categoryBits | G->categoryBits;
            C->flags |= (B->flags | F->flags) & sq_enlargedNode;
            A->flags |= (C->flags | G->flags) & sq_enlargedNode;
            break;

        case sq_rotateCD:
            A->children.child2 = iD;
            B->children.child1 = iC;

            C->parent = iB;
            D->parent = iA;

            B->aabb = aabbCE;
            B->height = 1 + max(C->height, E->height);
            A->height = 1 + max(B->height, D->height);
            B->categoryBits = C->categoryBits | E->categoryBits;
            A->categoryBits = B->categoryBits | D->categoryBits;
            B->flags |= (C->flags | E->flags) & sq_enlargedNode;
            A->flags |= (B->flags | D->flags) & sq_enlargedNode;
            break;

        case sq_rotateCE:
            A->children.child2 = iE;
            B->children.child2 = iC;

            C->parent = iB;
            E->parent = iA;

            B->aabb = aabbCD;
            B->height = 1 + max(C->height, D->height);
            A->height = 1 + max(B->height, E->height);
            B->categoryBits = C->categoryBits | D->categoryBits;
            A->categoryBits = B->categoryBits | E->categoryBits;
            B->flags |= (C->flags | D->flags) & sq_enlargedNode;
            A->flags |= (B->flags | E->flags) & sq_enlargedNode;
            break;

        default:
            SQ_ASSERT(false);
            break;
        }
    }
}

/**
 * 节点发生了移动了（变换更改了），所以要更新节点树，将节点插入到适合的位置
 * 先删除，再重新跑一次拆入流程
 */
void SqDynamicTree::moveProxy(int proxyId, const SqAABB &aabb)
{
#if ENABLE_ASSERTS
    float hugeValue = SqConfig::getInstance()->getHugValue();
    SQ_ASSERT(SqAABB::isValid(aabb));
    SQ_ASSERT(aabb.upperBound.x - aabb.lowerBound.x < hugeValue);
    SQ_ASSERT(aabb.upperBound.y - aabb.lowerBound.y < hugeValue);
    SQ_ASSERT(0 <= proxyId && proxyId < nodeCapacity);
    SQ_ASSERT(isLeaf(nodes + proxyId));
#endif

    removeLeaf(proxyId);
    nodes[proxyId].aabb = aabb;
    insertLeaf(proxyId, false);
}

/**
 * 使用aabb去将proxyId节点进行扩展，如果aabb比proxyId小则不做任何更改
 * 此函数只用来扩大AABB区域
 */
void SqDynamicTree::enlargeProxy(int proxyId, const SqAABB &aabb)
{
#if ENABLE_ASSERTS
    float hugeValue = SqConfig::getInstance()->getHugValue();
    SQ_ASSERT(SqAABB::isValid(aabb));
    SQ_ASSERT(aabb.upperBound.x - aabb.lowerBound.x < hugeValue);
    SQ_ASSERT(aabb.upperBound.y - aabb.lowerBound.y < hugeValue);
    SQ_ASSERT(0 <= proxyId && proxyId < nodeCapacity);
    SQ_ASSERT(isLeaf(nodes + proxyId));
    // Caller must ensure this
    SQ_ASSERT(SqAABB::Contains(nodes[proxyId].aabb, aabb) == false);
#endif

    // 更新节点的aabb
    nodes[proxyId].aabb = aabb;

    // 父节点的AABB也需要更新
    int parentIndex = nodes[proxyId].parent;
    while (parentIndex != SQ_NULL_INDEX)
    {
        bool changed = SqAABB::enlargeAABB(&nodes[parentIndex].aabb, aabb);
        nodes[parentIndex].flags |= sq_enlargedNode;
        parentIndex = nodes[parentIndex].parent;

        if (changed == false)
        {
            break;
        }
    }

    // 由于上面的changed会跳出，造成再上面的父节点没有标记为sq_enlargedNode，所以继续标记一下
    while (parentIndex != SQ_NULL_INDEX)
    {
        if (nodes[parentIndex].flags & sq_enlargedNode)
        {
            // early out because this ancestor was previously ascended and marked as enlarged
            break;
        }

        nodes[parentIndex].flags |= sq_enlargedNode;
        parentIndex = nodes[parentIndex].parent;
    }
}

void SqDynamicTree::setCategoryBits(int proxyId, uint64_t categoryBits)
{
    SQ_ASSERT((nodes[proxyId].flags & sq_leafNode) == sq_leafNode);

    nodes[proxyId].categoryBits = categoryBits;

    // 父节点的也需要更新CategoryBits，否则父节点那里就过不了了
    int nodeIndex = nodes[proxyId].parent;
    while (nodeIndex != SQ_NULL_INDEX)
    {
        SqDynamicTreeNode *node = nodes + nodeIndex;
        int child1 = node->children.child1;
        SQ_ASSERT(child1 != SQ_NULL_INDEX);
        int child2 = node->children.child2;
        SQ_ASSERT(child2 != SQ_NULL_INDEX);
        node->categoryBits = nodes[child1].categoryBits | nodes[child2].categoryBits;

        nodeIndex = node->parent;
    }
}

uint64_t SqDynamicTree::getCategoryBits(int proxyId)
{
    SQ_ASSERT(0 <= proxyId && proxyId < nodeCapacity);
    return nodes[proxyId].categoryBits;
}

int SqDynamicTree::getHeight()
{
    if (root == SQ_NULL_INDEX)
    {
        return 0;
    }
    return nodes[root].height;
}

float SqDynamicTree::getAreaRatio()
{
    if (root == SQ_NULL_INDEX)
    {
        return 0.0f;
    }

    const SqDynamicTreeNode *root = nodes + this->root;
    float rootArea = SqAABB::Perimeter(root->aabb);

    float totalArea = 0.0f;
    for (int i = 0; i < nodeCapacity; ++i)
    {
        const SqDynamicTreeNode *node = nodes + i;
        if (SqDynamicTree::isAllocated(node) == false || SqDynamicTree::isLeaf(node) || i == this->root)
        {
            continue;
        }

        totalArea += SqAABB::Perimeter(node->aabb);
    }

    return totalArea / rootArea;
}

SqAABB SqDynamicTree::getRootBounds()
{
    if (root != SQ_NULL_INDEX)
    {
        return nodes[root].aabb;
    }
    SqAABB empty;
    return empty;
}

/**
 * 根据参数aabb查询在aabb区域内的叶节点，如果查询到则执行callback回调通知外部
 * @param context 外部自定义的数据，作为callbak的参数传递
 */
SqTreeStats SqDynamicTree::Query(const SqAABB &aabb, uint64_t maskBits, SqTreeQueryCallbackFcn *callback, void *context)
{
    SqTreeStats result = {0};

    if (nodeCount == 0)
    {
        return result;
    }

    // 遍历Tree
    int stack[SQ_TREE_STACK_SIZE];
    int stackCount = 0;
    stack[stackCount++] = root;

    while (stackCount > 0)
    {
        int nodeId = stack[--stackCount];
        if (nodeId == SQ_NULL_INDEX)
        {
            // todo huh?
            SQ_ASSERT(false);
            continue;
        }

        const SqDynamicTreeNode *node = nodes + nodeId;
        result.nodeVisits += 1;
        if ((node->categoryBits & maskBits) != 0 && SqAABB::Overlaps(node->aabb, aabb))
        {
            if (SqDynamicTree::isLeaf(node))
            {
                // callback to user code with proxy id
                bool proceed = callback(nodeId, node->userData, context);
                result.leafVisits += 1;

                if (proceed == false)
                {
                    return result;
                }
            }
            else
            {
                if (stackCount < SQ_TREE_STACK_SIZE - 1)
                {
                    stack[stackCount++] = node->children.child1;
                    stack[stackCount++] = node->children.child2;
                }
                else
                {
                    SQ_ASSERT(stackCount < SQ_TREE_STACK_SIZE - 1);
                }
            }
        }
    }

    return result;
}

/**
 * 发射一条射线，检查这条射线与哪些节点发生了碰撞
 * 如果发生碰撞则回调callback,外部通过Callback得到检测结果
 */
SqTreeStats SqDynamicTree::RayCast(const SqRayCastInput *input, uint64_t maskBits, SqTreeRayCastCallbackFcn *callback, void *context)
{
    SqTreeStats result = {0};

    if (nodeCount == 0)
    {
        return result;
    }

    const SqVec2 &p1 = input->origin;
    const SqVec2 &d = input->translation;

    SqVec2 r = SqVec2::Normalize(d);

    // v is perpendicular to the segment.
    SqVec2 v = SqVec2::CrossSV(1.0f, r);
    SqVec2 abs_v = SqVec2::Abs(v);

    // Separating axis for segment (Gino, p80).
    // |dot(v, p1 - c)| > dot(|v|, h)

    float maxFraction = input->maxFraction;

    SqVec2 p2 = SqVec2::MulAdd(p1, maxFraction, d);

    // Build a bounding box for the segment.
    SqAABB segmentAABB = {SqVec2::Min(p1, p2), SqVec2::Max(p1, p2)};

    int stack[SQ_TREE_STACK_SIZE];
    int stackCount = 0;
    stack[stackCount++] = root;

    SqRayCastInput subInput = *input;

    while (stackCount > 0)
    {
        int nodeId = stack[--stackCount];
        if (nodeId == SQ_NULL_INDEX)
        {
            // todo is this possible?
            SQ_ASSERT(false);
            continue;
        }

        const SqDynamicTreeNode *node = nodes + nodeId;
        result.nodeVisits += 1;

        SqAABB nodeAABB = node->aabb;

        if ((node->categoryBits & maskBits) == 0 || SqAABB::Overlaps(nodeAABB, segmentAABB) == false)
        {
            continue;
        }

        // Separating axis for segment (Gino, p80).
        // |dot(v, p1 - c)| > dot(|v|, h)
        // radius extension is added to the node in this case
        SqVec2 c = SqAABB::Center(nodeAABB);
        SqVec2 h = SqAABB::Extents(nodeAABB);
        float term1 = abs(SqVec2::Dot(v, SqVec2::Sub(p1, c)));
        float term2 = SqVec2::Dot(abs_v, h);
        if (term2 < term1)
        {
            continue;
        }

        if (SqDynamicTree::isLeaf(node))
        {
            subInput.maxFraction = maxFraction;

            float value = callback(&subInput, nodeId, node->userData, context);
            result.leafVisits += 1;

            // The user may return -1 to indicate this shape should be skipped

            if (value == 0.0f)
            {
                // The client has terminated the ray cast.
                return result;
            }

            if (0.0f < value && value <= maxFraction)
            {
                // Update segment bounding box.
                maxFraction = value;
                p2 = SqVec2::MulAdd(p1, maxFraction, d);
                segmentAABB.lowerBound = SqVec2::Min(p1, p2);
                segmentAABB.upperBound = SqVec2::Max(p1, p2);
            }
        }
        else
        {
            if (stackCount < SQ_TREE_STACK_SIZE - 1)
            {
                SqVec2 c1 = SqAABB::Center(nodes[node->children.child1].aabb);
                SqVec2 c2 = SqAABB::Center(nodes[node->children.child2].aabb);
                if (SqVec2::DistanceSquared(c1, p1) < SqVec2::DistanceSquared(c2, p1))
                {
                    stack[stackCount++] = node->children.child2;
                    stack[stackCount++] = node->children.child1;
                }
                else
                {
                    stack[stackCount++] = node->children.child1;
                    stack[stackCount++] = node->children.child2;
                }
            }
            else
            {
                SQ_ASSERT(stackCount < SQ_TREE_STACK_SIZE - 1);
            }
        }
    }

    return result;
}

SqTreeStats SqDynamicTree::ShapeCast(const SqShapeCastInput *input, uint64_t maskBits, SqTreeShapeCastCallbackFcn *callback, void *context)
{
    SqTreeStats stats = {0};

    if (nodeCount == 0 || input->proxy.count == 0)
    {
        return stats;
    }

    // 顶点的AABB
    SqAABB originAABB = {input->proxy.points[0], input->proxy.points[0]};
    for (int i = 1; i < input->proxy.count; ++i)
    {
        originAABB.lowerBound = SqVec2::Min(originAABB.lowerBound, input->proxy.points[i]);
        originAABB.upperBound = SqVec2::Max(originAABB.upperBound, input->proxy.points[i]);
    }

    // 顶点加上直径的大小的AABB
    SqVec2 radius = {input->proxy.radius, input->proxy.radius};

    originAABB.lowerBound = SqVec2::Sub(originAABB.lowerBound, radius);
    originAABB.upperBound = SqVec2::Add(originAABB.upperBound, radius);

    SqVec2 p1 = SqAABB::Center(originAABB);
    SqVec2 extension = SqAABB::Extents(originAABB);

    // v is perpendicular to the segment.
    SqVec2 r = input->translation;
    SqVec2 v = SqVec2::CrossSV(1.0f, r);
    SqVec2 abs_v = SqVec2::Abs(v);

    // Separating axis for segment (Gino, p80).
    // |dot(v, p1 - c)| > dot(|v|, h)

    float maxFraction = input->maxFraction;

    // Build total box for the shape cast
    SqVec2 t = SqVec2::MulSV(maxFraction, input->translation);
    SqAABB totalAABB = {
        SqVec2::Min(originAABB.lowerBound, SqVec2::Add(originAABB.lowerBound, t)),
        SqVec2::Max(originAABB.upperBound, SqVec2::Add(originAABB.upperBound, t)),
    };

    SqShapeCastInput subInput = *input;

    int stack[SQ_TREE_STACK_SIZE];
    int stackCount = 0;
    stack[stackCount++] = root;

    while (stackCount > 0)
    {
        int nodeId = stack[--stackCount];
        if (nodeId == SQ_NULL_INDEX)
        {
            // todo is this possible?
            SQ_ASSERT(false);
            continue;
        }

        const SqDynamicTreeNode *node = nodes + nodeId;
        stats.nodeVisits += 1;

        if ((node->categoryBits & maskBits) == 0 || SqAABB::Overlaps(node->aabb, totalAABB) == false)
        {
            continue;
        }

        // Separating axis for segment (Gino, p80).
        // |dot(v, p1 - c)| > dot(|v|, h)
        // radius extension is added to the node in this case
        SqVec2 c = SqAABB::Center(node->aabb);
        SqVec2 h = SqVec2::Add(SqAABB::Extents(node->aabb), extension);
        float term1 = abs(SqVec2::Dot(v, SqVec2::Sub(p1, c)));
        float term2 = SqVec2::Dot(abs_v, h);
        if (term2 < term1)
        {
            continue;
        }

        if (SqDynamicTree::isLeaf(node))
        {
            subInput.maxFraction = maxFraction;

            float value = callback(&subInput, nodeId, node->userData, context);
            stats.leafVisits += 1;

            if (value == 0.0f)
            {
                // The client has terminated the ray cast.
                return stats;
            }

            if (0.0f < value && value < maxFraction)
            {
                // Update segment bounding box.
                maxFraction = value;
                t = SqVec2::MulSV(maxFraction, input->translation);
                totalAABB.lowerBound = SqVec2::Min(originAABB.lowerBound, SqVec2::Add(originAABB.lowerBound, t));
                totalAABB.upperBound = SqVec2::Max(originAABB.upperBound, SqVec2::Add(originAABB.upperBound, t));
            }
        }
        else
        {
            if (stackCount < SQ_TREE_STACK_SIZE - 1)
            {
                SqVec2 c1 = SqAABB::Center(nodes[node->children.child1].aabb);
                SqVec2 c2 = SqAABB::Center(nodes[node->children.child2].aabb);
                if (SqVec2::DistanceSquared(c1, p1) < SqVec2::DistanceSquared(c2, p1))
                {
                    stack[stackCount++] = node->children.child2;
                    stack[stackCount++] = node->children.child1;
                }
                else
                {
                    stack[stackCount++] = node->children.child1;
                    stack[stackCount++] = node->children.child2;
                }
            }
            else
            {
                SQ_ASSERT(stackCount < SQ_TREE_STACK_SIZE - 1);
            }
        }
    }

    return stats;
}

/**
 * 相当于清空当前的树从新构建，是一个比较重度的操作。
 *
 * BVH构建算法
 * https://www.sci.utah.edu/~wald/Publications/2007/ParallelBVHBuild/fastbuild.pdf
 *
 * 我自己的笔记里面有《BVH重建算法》。
 *
 */
int SqDynamicTree::rebuild(bool fullBuild)
{
    if (proxyCount == 0)
    {
        return 0;
    }

    // Ensure capacity for rebuild space
    if (proxyCount > rebuildCapacity)
    {
        int newCapacity = proxyCount + proxyCount / 2;

        SqFree(leafIndices, rebuildCapacity * sizeof(int));

        leafIndices = (int *)SqAlloc(newCapacity * sizeof(int));

#if B2_TREE_HEURISTIC == 0
        SqFree(leafCenters, rebuildCapacity * sizeof(SqVec2));
        leafCenters = (SqVec2 *)SqAlloc(newCapacity * sizeof(SqVec2));
#else
        SqFree(leafBoxes, rebuildCapacity * sizeof(SqAABB));
        leafBoxes = SqAlloc(newCapacity * sizeof(SqAABB));
        SqFree(binIndices, rebuildCapacity * sizeof(int));
        binIndices = SqAlloc(newCapacity * sizeof(int));
#endif
        rebuildCapacity = newCapacity;
    }

    int leafCount = 0;
    int stack[SQ_TREE_STACK_SIZE];
    int stackCount = 0;

    int nodeIndex = root;
    SqDynamicTreeNode *node = nodes + nodeIndex;

    // These are the nodes that get sorted to rebuild the tree.
    // I'm using indices because the node pool may grow during the build.
    int *leafIndices = this->leafIndices;

#if B2_TREE_HEURISTIC == 0
    SqVec2 *leafCenters = this->leafCenters;
#else
    SqAABB *leafBoxes = this->leafBoxes;
#endif

    // Gather all proxy nodes that have grown and all internal nodes that haven't grown. Both are
    // considered leaves in the tree rebuild.
    // Free all internal nodes that have grown.
    // todo use a node growth metric instead of simply enlarged to reduce rebuild size and frequency
    // this should be weighed against B2_AABB_MARGIN
    while (true)
    {
        if (node->height == 0 || ((node->flags & sq_enlargedNode) == 0 && fullBuild == false))
        {
            leafIndices[leafCount] = nodeIndex;
#if B2_TREE_HEURISTIC == 0
            leafCenters[leafCount] = SqAABB::Center(node->aabb);
#else
            leafBoxes[leafCount] = node->aabb;
#endif
            leafCount += 1;

            // Detach
            node->parent = SQ_NULL_INDEX;
        }
        else
        {
            int doomedNodeIndex = nodeIndex;

            // Handle children
            nodeIndex = node->children.child1;

            if (stackCount < SQ_TREE_STACK_SIZE)
            {
                stack[stackCount++] = node->children.child2;
            }
            else
            {
                SQ_ASSERT(stackCount < SQ_TREE_STACK_SIZE);
            }

            node = nodes + nodeIndex;

            // Remove doomed node
            freeNode(doomedNodeIndex);

            continue;
        }

        if (stackCount == 0)
        {
            break;
        }

        nodeIndex = stack[--stackCount];
        node = nodes + nodeIndex;
    }

#if B2_VALIDATE == 1
    for (int i = 0; i < nodeCapacity; ++i)
    {
        if (nodes[i].flags & sq_allocatedNode)
        {
            SQ_ASSERT((nodes[i].flags & sq_enlargedNode) == 0);
        }
    }
#endif

    SQ_ASSERT(leafCount <= proxyCount);

    root = buildTree(leafCount);
    Validate();
    return leafCount;
}

int SqDynamicTree::buildTree(int leafCount)
{
    if (leafCount == 1)
    {
        nodes[leafIndices[0]].parent = SQ_NULL_INDEX;
        return leafIndices[0];
    }

#if B2_TREE_HEURISTIC == 0
    SqVec2 *leafCenters = this->leafCenters;
#else
    SqAABB *leafBoxes = this->leafBoxes;
    int *binIndices = this->binIndices;
#endif

    // todo large stack item
    struct SqRebuildItem stack[SQ_TREE_STACK_SIZE];
    int top = 0;

    stack[0].nodeIndex = createNode();
    stack[0].childCount = -1;
    stack[0].startIndex = 0;
    stack[0].endIndex = leafCount;
#if B2_TREE_HEURISTIC == 0
    stack[0].splitIndex = SqPartitionMid(leafIndices, leafCenters, leafCount);
#else
    stack[0].splitIndex = SqPartitionSAH(leafIndices, binIndices, leafBoxes, leafCount);
#endif

    while (true)
    {
        struct SqRebuildItem *item = stack + top;

        item->childCount += 1;

        if (item->childCount == 2)
        {
            // This internal node has both children established

            if (top == 0)
            {
                // all done
                break;
            }

            struct SqRebuildItem *parentItem = stack + (top - 1);
            SqDynamicTreeNode *parentNode = nodes + parentItem->nodeIndex;

            if (parentItem->childCount == 0)
            {
                SQ_ASSERT(parentNode->children.child1 == SQ_NULL_INDEX);
                parentNode->children.child1 = item->nodeIndex;
            }
            else
            {
                SQ_ASSERT(parentItem->childCount == 1);
                SQ_ASSERT(parentNode->children.child2 == SQ_NULL_INDEX);
                parentNode->children.child2 = item->nodeIndex;
            }

            SqDynamicTreeNode *node = nodes + item->nodeIndex;

            SQ_ASSERT(node->parent == SQ_NULL_INDEX);
            node->parent = parentItem->nodeIndex;

            SQ_ASSERT(node->children.child1 != SQ_NULL_INDEX);
            SQ_ASSERT(node->children.child2 != SQ_NULL_INDEX);
            SqDynamicTreeNode *child1 = nodes + node->children.child1;
            SqDynamicTreeNode *child2 = nodes + node->children.child2;

            node->aabb = SqAABB::Union(child1->aabb, child2->aabb);
            node->height = 1 + max(child1->height, child2->height);
            node->categoryBits = child1->categoryBits | child2->categoryBits;

            // Pop stack
            top -= 1;
        }
        else
        {
            int startIndex, endIndex;
            if (item->childCount == 0)
            {
                startIndex = item->startIndex;
                endIndex = item->splitIndex;
            }
            else
            {
                SQ_ASSERT(item->childCount == 1);
                startIndex = item->splitIndex;
                endIndex = item->endIndex;
            }

            int count = endIndex - startIndex;

            if (count == 1)
            {
                int childIndex = leafIndices[startIndex];
                SqDynamicTreeNode *node = nodes + item->nodeIndex;

                if (item->childCount == 0)
                {
                    SQ_ASSERT(node->children.child1 == SQ_NULL_INDEX);
                    node->children.child1 = childIndex;
                }
                else
                {
                    SQ_ASSERT(item->childCount == 1);
                    SQ_ASSERT(node->children.child2 == SQ_NULL_INDEX);
                    node->children.child2 = childIndex;
                }

                SqDynamicTreeNode *childNode = nodes + childIndex;
                SQ_ASSERT(childNode->parent == SQ_NULL_INDEX);
                childNode->parent = item->nodeIndex;
            }
            else
            {
                SQ_ASSERT(count > 0);
                SQ_ASSERT(top < SQ_TREE_STACK_SIZE);

                top += 1;
                struct SqRebuildItem *newItem = stack + top;
                newItem->nodeIndex = createNode();
                newItem->childCount = -1;
                newItem->startIndex = startIndex;
                newItem->endIndex = endIndex;
#if B2_TREE_HEURISTIC == 0
                newItem->splitIndex = SqPartitionMid(leafIndices + startIndex, leafCenters + startIndex, count);
#else
                newItem->splitIndex =
                    SqPartitionSAH(leafIndices + startIndex, binIndices + startIndex, leafBoxes + startIndex, count);
#endif
                newItem->splitIndex += startIndex;
            }
        }
    }

    SqDynamicTreeNode *rootNode = nodes + stack[0].nodeIndex;
    SQ_ASSERT(rootNode->parent == SQ_NULL_INDEX);
    SQ_ASSERT(rootNode->children.child1 != SQ_NULL_INDEX);
    SQ_ASSERT(rootNode->children.child2 != SQ_NULL_INDEX);

    SqDynamicTreeNode *child1 = nodes + rootNode->children.child1;
    SqDynamicTreeNode *child2 = nodes + rootNode->children.child2;

    rootNode->aabb = SqAABB::Union(child1->aabb, child2->aabb);
    rootNode->height = 1 + max(child1->height, child2->height);
    rootNode->categoryBits = child1->categoryBits | child2->categoryBits;

    return stack[0].nodeIndex;
}

#if B2_VALIDATE
// Compute the height of a sub-tree.
static int b2ComputeHeight(const SqDynamicTree *tree, int nodeId)
{
    SQ_ASSERT(0 <= nodeId && nodeId < tree->nodeCapacity);
    SqDynamicTreeNode *node = tree->nodes + nodeId;

    if (SqDynamicTree::isLeaf(node))
    {
        return 0;
    }

    int height1 = b2ComputeHeight(tree, node->children.child1);
    int height2 = b2ComputeHeight(tree, node->children.child2);
    return 1 + max(height1, height2);
}

static void b2ValidateStructure(const SqDynamicTree *tree, int index)
{
    if (index == SQ_NULL_INDEX)
    {
        return;
    }

    if (index == tree->root)
    {
        SQ_ASSERT(tree->nodes[index].parent == SQ_NULL_INDEX);
    }

    const SqDynamicTreeNode *node = tree->nodes + index;

    SQ_ASSERT(node->flags == 0 || (node->flags & sq_allocatedNode) != 0);

    if (b2IsLeaf(node))
    {
        SQ_ASSERT(node->height == 0);
        return;
    }

    int child1 = node->children.child1;
    int child2 = node->children.child2;

    SQ_ASSERT(0 <= child1 && child1 < tree->nodeCapacity);
    SQ_ASSERT(0 <= child2 && child2 < tree->nodeCapacity);

    SQ_ASSERT(tree->nodes[child1].parent == index);
    SQ_ASSERT(tree->nodes[child2].parent == index);

    if ((tree->nodes[child1].flags | tree->nodes[child2].flags) & sq_enlargedNode)
    {
        SQ_ASSERT(node->flags & sq_enlargedNode);
    }

    b2ValidateStructure(tree, child1);
    b2ValidateStructure(tree, child2);
}

static void b2ValidateMetrics(const SqDynamicTree *tree, int index)
{
    if (index == SQ_NULL_INDEX)
    {
        return;
    }

    const SqDynamicTreeNode *node = tree->nodes + index;

    if (SqDynamicTree::isLeaf(node))
    {
        SQ_ASSERT(node->height == 0);
        return;
    }

    int child1 = node->children.child1;
    int child2 = node->children.child2;

    SQ_ASSERT(0 <= child1 && child1 < tree->nodeCapacity);
    SQ_ASSERT(0 <= child2 && child2 < tree->nodeCapacity);

    int height1 = tree->nodes[child1].height;
    int height2 = tree->nodes[child2].height;
    int height = 1 + max(height1, height2);
    SQ_ASSERT(node->height == height);

    // b2AABB aabb = b2AABB_Union(tree->nodes[child1].aabb, tree->nodes[child2].aabb);

    SQ_ASSERT(SqAABB::Contains(node->aabb, tree->nodes[child1].aabb));
    SQ_ASSERT(SqAABB::Contains(node->aabb, tree->nodes[child2].aabb));

    // SQ_ASSERT(aabb.lowerBound.x == node->aabb.lowerBound.x);
    // SQ_ASSERT(aabb.lowerBound.y == node->aabb.lowerBound.y);
    // SQ_ASSERT(aabb.upperBound.x == node->aabb.upperBound.x);
    // SQ_ASSERT(aabb.upperBound.y == node->aabb.upperBound.y);

    uint64_t categoryBits = tree->nodes[child1].categoryBits | tree->nodes[child2].categoryBits;
    SQ_ASSERT(node->categoryBits == categoryBits);

    b2ValidateMetrics(tree, child1);
    b2ValidateMetrics(tree, child2);
}
#endif

void SqDynamicTree::Validate()
{
#if B2_VALIDATE
    if (root == SQ_NULL_INDEX)
    {
        return;
    }

    b2ValidateStructure(this, root);
    b2ValidateMetrics(this, root);

    int freeCount = 0;
    int freeIndex = freeList;
    while (freeIndex != SQ_NULL_INDEX)
    {
        SQ_ASSERT(0 <= freeIndex && freeIndex < nodeCapacity);
        freeIndex = nodes[freeIndex].next;
        ++freeCount;
    }

    int height = getHeight();
    int computedHeight = b2ComputeHeight(this, root);
    SQ_ASSERT(height == computedHeight);
    SQ_ASSERT(nodeCount + freeCount == nodeCapacity);
#endif
}

SqDynamicTree::~SqDynamicTree()
{
    SqFree(nodes, nodeCapacity * sizeof(SqDynamicTreeNode));
    nodes = NULL;

    SqFree(leafIndices, rebuildCapacity * sizeof(int32_t));
    SqFree(leafBoxes, rebuildCapacity * sizeof(SqAABB));
    SqFree(leafCenters, rebuildCapacity * sizeof(SqVec2));
    SqFree(binIndices, rebuildCapacity * sizeof(int32_t));
}

bool SqDynamicTree::isLeaf(const SqDynamicTreeNode *node)
{
    return node->flags & sq_leafNode;
}

bool SqDynamicTree::isAllocated(const SqDynamicTreeNode *node)
{
    return node->flags & sq_allocatedNode;
}

const SqAABB &SqDynamicTree::getAABB(int proxyId)
{
    SQ_ASSERT(0 <= proxyId && proxyId < nodeCapacity);
    return nodes[proxyId].aabb;
}

void *SqDynamicTree::getUserData(int proxyId)
{
    SQ_ASSERT(0 <= proxyId && proxyId < nodeCapacity);
    return nodes[proxyId].userData;
}