#pragma once

/**
 * 标记节点上的Transform组件改变的情况
 */
enum TransformBit
{
    /**
     * 无改变
     */
    NONE = 0,

    /**
     *  节点位置改变
     */
    POSITION = (1 << 0),

    /**
     * 节点旋转
     */
    ROTATION = (1 << 1),

    /**
     * 节点缩放
     */
    SCALE = (1 << 2),

    TRS = TransformBit::POSITION | TransformBit::ROTATION | TransformBit::SCALE

};
