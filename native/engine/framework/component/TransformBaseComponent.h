#pragma once

#include "../../scene/NodeEnum.h"

/**
 * 由于2D和3D是分开两个Transform组件（为了减少内存和一些计算，2D可以简化的，所以分开了）
 * 有一些公共逻辑写到这里来
 */

#include "Component.h"
class TransformBaseComponent : public Component
{
public:
    friend class Node;
    
    /**
     * 递归地告诉这个节点下的子节点父节点的位置变化了
     */
    void invalidateChildren(TransformBit);
};
