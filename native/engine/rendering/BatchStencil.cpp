#include "BatchStencil.h"
#include "../core/base/config.h"
#include "Batch.h"

using namespace pipeline;

/**
 *  客户端设置：ENTER_LEVEL或ENTER_LEVEL_INVERTED进入模板测试流程
 *
 * 1、开始执行：CEAR\CLEAR_INVERTED
 *    这个阶段将整个模板缓冲区设置为0
 * 2、当前还是执行：ENTER_LEVEL\ENTER_LEVEL_INVERTED
 *    绘制当前节点从而在屏幕上定义一个区域，这个区域设置非0值，假设为A值
 *    经过此步骤之后，整个缓冲区变成在这个节点区域内的值为A，区域之外的值为0的缓冲区
 *
 * 3、子节点执行：ENABLED
 *    然后绘制子节点内容，等于A值的内容保留下来，非A值的丢弃
 *
 * 4、结束执行：DISABLED
 */

BatchStencil::BatchStencil(Batch *batch) : batch(batch)
{
}

const StencilState &BatchStencil::getStateFromStage(StencilStage stage)
{
    int key = toNumber(stage) << 16 | stackNum;
    std::unordered_map<int, StencilState>::iterator it = cacheState.find(key);
    if (it != cacheState.end())
    {
        return it->second;
    }

    StencilState state;

    if (stage == StencilStage::DISABLED)
    {
        state.stencilTestFront = state.stencilTestBack = false;
        state.stencilFuncFront = state.stencilFuncBack = ComparisonFunc::ALWAYS;
        state.stencilFailOpFront = state.stencilFailOpBack = StencilOp::KEEP;
        state.stencilZFailOpFront = state.stencilZFailOpBack = StencilOp::KEEP;
        state.stencilPassOpFront = state.stencilPassOpBack = StencilOp::KEEP;
        state.stencilReadMaskFront = state.stencilReadMaskBack = state.stencilReadMaskFront = state.stencilWriteMaskBack = 0xffff;
        state.stencilRefFront = state.stencilRefBack = 1;
    }
    else
    {
        state.stencilTestFront = state.stencilTestBack = true;

        if (stage == StencilStage::ENABLED)
        {
            // 子节点渲染使用这个模板测试的设置，判断是否是父节点mask区域内的像素
            state.stencilFuncFront = state.stencilFuncBack = ComparisonFunc::EQUAL;
            state.stencilFailOpFront = state.stencilFailOpBack = StencilOp::KEEP;
            state.stencilZFailOpFront = state.stencilZFailOpBack = StencilOp::KEEP;
            state.stencilPassOpFront = state.stencilPassOpBack = StencilOp::KEEP;
            state.stencilWriteMaskFront = state.stencilWriteMaskBack = getWriteMask();

            // 实现多层级stencil test的关键之处
            state.stencilReadMaskFront = state.stencilReadMaskBack = state.stencilRefFront = state.stencilRefBack = getStencilRef();
        }
        else if (stage == StencilStage::CLEAR)
        {
            // 全屏设置为0
            // 设置永远不通过，即永远失败，然后失败的时候将缓冲区值设置为0
            state.stencilFuncFront = state.stencilFuncBack = ComparisonFunc::NEVER;
            state.stencilFailOpFront = state.stencilFailOpBack = StencilOp::ZERO;
            state.stencilZFailOpFront = state.stencilZFailOpBack = StencilOp::KEEP;
            state.stencilPassOpFront = state.stencilPassOpBack = StencilOp::KEEP;
            state.stencilReadMaskFront = state.stencilReadMaskBack = state.stencilRefFront = state.stencilRefBack = state.stencilWriteMaskFront = state.stencilWriteMaskBack = getWriteMask();
        }
        else if (stage == StencilStage::CLEAR_INVERTED)
        {
            // 全屏设置为ref值
            // 设置永远不通过，即永远失败，然后失败的时候将缓冲区值设置为ref值
            state.stencilFuncFront = state.stencilFuncBack = ComparisonFunc::NEVER;
            state.stencilFailOpFront = state.stencilFailOpBack = StencilOp::REPLACE;
            state.stencilZFailOpFront = state.stencilZFailOpBack = StencilOp::KEEP;
            state.stencilPassOpFront = state.stencilPassOpBack = StencilOp::KEEP;
            state.stencilReadMaskFront = state.stencilReadMaskBack = state.stencilRefFront = 
                state.stencilRefBack = state.stencilWriteMaskFront = state.stencilWriteMaskBack = getWriteMask();
        }
        else if (stage == StencilStage::ENTER_LEVEL)
        {
            // 全屏清除，得到一个缓冲区为0的区域，然后绘制一个可视区域，这个区域就是子节点的内容区域
            // 将此可视区域的模板值设置为ref值,
            // 然后StencilStage::ENABLED阶段就与这些值进行比较相等的话就通过测试显示此像素
            state.stencilFuncFront = state.stencilFuncBack = ComparisonFunc::NEVER;
            state.stencilFailOpFront = state.stencilFailOpBack = StencilOp::REPLACE;
            state.stencilZFailOpFront = state.stencilZFailOpBack = StencilOp::KEEP;
            state.stencilPassOpFront = state.stencilPassOpBack = StencilOp::KEEP;
            state.stencilReadMaskFront = state.stencilReadMaskBack = state.stencilRefFront = state.stencilRefBack =     
                state.stencilWriteMaskFront = state.stencilWriteMaskBack = getWriteMask();
        }
        else if (stage == StencilStage::ENTER_LEVEL_INVERTED)
        {
            // 全屏清除之后，得到的是一个ref值的缓冲区，然后绘制一个不可视区域，这个区域内的模板值为0
            // 然后StencilStage::ENABLED阶段就与这些值进行比较相等的话就通过测试显示此像素，不通过则丢弃。
            // 由于不可视区域的值0，所以StencilStage::ENABLED阶段判断不相等则不通过就丢弃了像素
            state.stencilFuncFront = state.stencilFuncBack = ComparisonFunc::NEVER;
            state.stencilFailOpFront = state.stencilFailOpBack = StencilOp::ZERO;
            state.stencilZFailOpFront = state.stencilZFailOpBack = StencilOp::KEEP;
            state.stencilPassOpFront = state.stencilPassOpBack = StencilOp::KEEP;
            state.stencilReadMaskFront = state.stencilReadMaskBack = state.stencilRefFront = state.stencilRefBack = 
                state.stencilWriteMaskFront = state.stencilWriteMaskBack = getWriteMask();
        }
    }

    cacheState[key] = state;
    return cacheState[key];
}

int BatchStencil::getStencilRef()
{
    // 为了支持多层级的stencil test需要这个值
    int result = 0;
    for (int i = 0; i < stackNum; ++i)
    {
        result += (0x00000001 << i);
    }
    return result;
}

int BatchStencil::getWriteMask()
{
    SQ_ASSERT(stackNum <= 32 && stackNum > 0);
    
    // 可以看到支持的最深层级为32
    return 1 << (stackNum - 1);
}

void BatchStencil::pushMask(StencilStage stage)
{
    SQ_ASSERT(currentStage != stage);
    SQ_ASSERT(stage == StencilStage::ENTER_LEVEL || stage == StencilStage::ENTER_LEVEL_INVERTED);
    currentStage = StencilStage::ENABLED;
    ++stackNum;
}

void BatchStencil::popMask()
{
    if (stackNum <= 0)
        return;
    --stackNum;
    if (stackNum == 0)
    {
        currentStage = StencilStage::DISABLED;
    }
}

bool BatchStencil::enableStencilTest()
{
    return stackNum > 0;
}

void BatchStencil::reset()
{
    stackNum = 0;
    currentStage = StencilStage::DISABLED;
}