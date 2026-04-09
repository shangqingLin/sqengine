#pragma once
#include "stencil-define.h"
#include "../gfx/gfx.h"
#include <unordered_map>

/**
 * 用于实现UI的模板测试渲染流程。
 * 在Batch中使用
 */

namespace pipeline
{

    class Batch;
    
    /**
     * 使用栈的方式来构建模板测试流程
     */
    class BatchStencil
    {
    private:
        StencilStage currentStage = StencilStage::DISABLED;
        Batch *batch;
        std::unordered_map<int, StencilState> cacheState;

        // 多少个Stencil Test层级了
        int stackNum = 0;

        int getWriteMask();
        int getStencilRef();

    public:
        friend class Batch;
        BatchStencil(Batch *);

        const StencilState &getStateFromStage(StencilStage);

        /**
         * 开启模板测试，接下来的DrawCall会执行模板测试
         */
        void pushMask(StencilStage stage);

        bool enableStencilTest();
        
        void popMask();
        void reset();
    };
}
