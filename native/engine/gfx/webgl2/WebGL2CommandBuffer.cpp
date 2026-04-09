#include "WebGL2CommandBuffer.h"
#include "WebGL2Device.h"
#include "../DeviceManager.h"
#include "WebGL2Command.h"
#include "WebGL2PipelineState.h"
#include "WebGL2RenderPass.h"

void WebGL2CommandBuffer::beginRenderPass(gfx::RenderPass *renderPass, Rect<float> &renderArea, Color &clearColors, float clearDepth, int clearStencil, bool forceRefresh)
{

    if (forceRefresh)
    {
        renderPassSet.dirty = false;
        WebGL2CmdFuncBeginRenderPass(
            static_cast<WebGL2Device *>(DeviceManager::getInstance()->device),
           (gfx::WebGL2RenderPass *)renderPass,
            renderArea,
            clearColors,
            clearDepth,
            clearStencil);
    }
    else
    {
        // 外部可能调用了beginRenderPass，但一直没有drawcall，那么就没必要执行beginRenderPass
        // 所以只有执行Draw操作才beginRenderPass

        renderPassSet.pass = (gfx::WebGL2RenderPass *)renderPass;
        renderPassSet.renderArea = renderArea;
        renderPassSet.clearColors = clearColors;
        renderPassSet.clearDepth = clearDepth;
        renderPassSet.clearStencil = clearStencil;
        renderPassSet.dirty = true;
    }
}

void WebGL2CommandBuffer::endRenderPass()
{
}

void WebGL2CommandBuffer::bindPipelineState(PipelineState *pso)
{
    pipelineState = pso;
}

void WebGL2CommandBuffer::bindDescriptorSet(int set, DescriptorSet *descriptorSet)
{
    int needSize = set + 1;
    if (curGPUDescriptorSets.size() < needSize)
    {
        curGPUDescriptorSets.resize(needSize);
    }
    curGPUDescriptorSets[set] = descriptorSet;
    // printf("  WebGL2CommandBuffer::bindDescriptorSet set %d d %p\n",set, curGPUDescriptorSets[set],descriptorSet);
}

void WebGL2CommandBuffer::draw(const gfx::InputDraw &info)
{
    if (renderPassSet.dirty)
    {
        renderPassSet.dirty = false;
        WebGL2CmdFuncBeginRenderPass(
            static_cast<WebGL2Device *>(DeviceManager::getInstance()->device),
            renderPassSet.pass,
            renderPassSet.renderArea,
            renderPassSet.clearColors,
            renderPassSet.clearDepth,
            renderPassSet.clearStencil);
    }
    bindStates();
    WebGL2CmdFuncDraw(static_cast<WebGL2Device *>(DeviceManager::getInstance()->device), info);
}

void WebGL2CommandBuffer::bindStates()
{
    WebGL2CmdFuncBindStates(
        static_cast<WebGL2Device *>(DeviceManager::getInstance()->device),
        static_cast<WebGL2PipelineState *>(pipelineState),
        curGPUDescriptorSets);
}

void WebGL2CommandBuffer::setViewport(const Rect<float> &vp)
{
}

void WebGL2CommandBuffer::setScissor(const Rect<float> &rect)
{
}