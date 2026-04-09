#ifndef _ALGORITHM_MAXRECT_PACKER_NODE_H_
#define _ALGORITHM_MAXRECT_PACKER_NODE_H_

namespace algorithm
{
    class MaxRectPacker;
    struct MaxRectPackerNode
    {
        int x = 0;
        int y = 0;
        int width = 0;
        int height = 0;
        bool rotation = false;
        void *userData = nullptr;
        MaxRectPacker *packer = nullptr;
    };
}

#endif