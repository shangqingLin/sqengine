#ifndef _ALGORITHM_MAXRECT_PACKER_H_
#define _ALGORITHM_MAXRECT_PACKER_H_
#include <optional>
#include "MaxRectBin.h"

/**
 * https://raw.githubusercontent.com/rougier/freetype-gl/master/doc/RectangleBinPack.pdf
 * https://github.com/soimy/maxrects-packer
 *
 * 将很多的小矩形区域合并到一个很大的矩形区域中。并能够充分利用空间不浪费空间的算法
 * 1、一般用于图集打包中，将散图打包到一个大图集中，并且充分利用图集中空间
 */

namespace algorithm
{

    class MaxRectPacker
    {
    private:
        // 空间的大小
        int width;
        int height;
        algorithm::MaxRectPackerOptions option;
        algorithm::MaxRectBin bin;

    public:
        MaxRectPacker(int width, int height, algorithm::MaxRectPackerOptions &);
        MaxRectPacker(int width, int height);

        /**
         * 返回null表示无法添加了
         */
        algorithm::MaxRectPackerNode *add(int width, int height);
        void pruneFreeList();
        void remove(algorithm::MaxRectPackerNode *node);
        void repack();
    };

}

#endif