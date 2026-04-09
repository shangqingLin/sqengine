#ifndef _GFX_DRAW_INFO_H_
#define _GFX_DRAW_INFO_H_

namespace gfx
{
    struct InputDraw
    {

        // 索引绘制
        unsigned int indexCount{0};
        unsigned int firstIndex{0};
        gfx::Type glIndexType;

        // 非索引绘制
        unsigned int vertexCount{0};
        unsigned int vertexOffset{0};
        unsigned int firstVertex{0};

        // 多实例绘制
        unsigned int instanceCount{0};
        unsigned int firstInstance{0};
    };
}

#endif