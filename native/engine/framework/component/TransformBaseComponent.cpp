#include <vector>
#include "TransformBaseComponent.h"
#include "../../core/sqstd/Array.h"
#include "../../core/sqstd/StackTempArenaAllocator.h"
void TransformBaseComponent::invalidateChildren(TransformBit bit)
{

    // if (dirty)
    // {
    //     // 有可能设置了dirty,但外部一直没有调用getWorldTransform方法来改变dirty为false
    //     //  造成NodeFlag一直没有设置到，因为NodeFlag每帧都清除的
    //     if (!node->hasChangedFags())
    //     {
    //         node->setChangedFlags(node->changedFlags | bit);
    //     }
    //     return;
    // }

    // printf(" check a invalidateChildren %d %d \n", this->node->nativeId,bit);

    // 已经设置为dirty了，并且也标记了对应的bit，就没必要啦
    if (node->hasChangedFags() && (node->changedFlags & bit) == bit)
    {
        return;
    }

    SQ_ASSERT(bit);

    // if(this->node->nativeId == 29)
    //  if(this->node->testNum != 0)
    // printf("check b invalidateChildren %d %d \n", this->node->nativeId,bit);

    Node *cur = NULL;
    sqstd::Array<Node *> dirtyNodes(sqstd::StackTempArenaAllocator::getInstance());
    dirtyNodes.resize(100);
    dirtyNodes.push(this->node);

    // 使用传统的递归方式会因为方法调用太深和栈内存占用太多造成性能问题，
    // 所以使用while循环的方式实现递归
    while (dirtyNodes.getCount() > 0)
    {
        cur = *dirtyNodes.pop();
        if (!cur->hasChangedFags() || !((cur->changedFlags & bit) == bit))
        {
            //  if(this->node->nativeId == 29)
            // printf(" set invalidate to children %d %d\n",cur->nativeId,bit);
            cur->setChangedFlags(bit);
            const std::vector<Node *> &children = cur->getChildren();
            int size = children.size();
            if (size > 0)
            {
                for (int j = 0; j < size; j++)
                {
                    dirtyNodes.push(children[j]);
                }
            }
        }
    }
}
