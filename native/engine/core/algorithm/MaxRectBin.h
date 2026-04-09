#ifndef _ALGORITHM_MAX_RECT_H_
#define _ALGORITHM_MAX_RECT_H_
#include <vector>
#include "../math/Rect.h"
#include "MaxRectPackerNode.h"

namespace algorithm
{


    enum class MAX_RECT_PACKING_LOGIC
    {
        MAX_AREA,
        MAX_EDGE
    };

    struct MaxRectPackerOptions
    {
        bool allowRotation = true;

        MAX_RECT_PACKING_LOGIC logic = MAX_RECT_PACKING_LOGIC::MAX_EDGE;
    };

    class MaxRectBin
    {
    private:
        int width;
        int height;
        MaxRectPackerOptions *option;

        /**
         * 当前的空闲区域
         */
        std::vector<Rect<int>> freeRects;

        /**
         * 当前已经被使用的区域
         */
        std::vector<MaxRectPackerNode*> useRects;

        bool place(int width, int height, MaxRectPackerNode &result);
        bool splitNode(Rect<int> &, Rect<int> &);
        bool findNode(int width, int height, bool allowRotation, MaxRectPackerNode &bestNode);

    public:
        MaxRectBin(int maxWidth, int maxHeight, MaxRectPackerOptions *option);
        ~MaxRectBin();
        /**
         * 如果添加不进去返回为空
         */
        MaxRectPackerNode* add(int width, int height);

        /**
         * 经过一系列的删除、添加等操作，可能会造成很多空间碎片
         * 这里重新将当前在使用的空间重新插入一次，并且是按照一定的方式插入，整理空间，避免过多空间浪费
         * 会比较耗时
         */
        void repack();
        void pruneFreeList();
        void remove( MaxRectPackerNode *node);
    };
}

#endif