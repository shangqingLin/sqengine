#include "WebGLCommandBuffer.h"
#include "WebGLDevice.h"
#include "../DeviceManager.h"
#include "WebGLCommand.h"
#include "WebGLRenderPass.h"
#include "../../core/base/config.h"

void WebGLCommandBuffer::beginRenderPass(gfx::RenderPass *renderPass, Rect<float> &renderArea, Color &clearColors, float clearDepth, int clearStencil, bool forceRefresh)
{
    SQ_ASSERT(renderPass);

    if (forceRefresh)
    {
        renderPassSet.dirty = false;
        WebGLCmdFuncBeginRenderPass(
            static_cast<WebGLDevice *>(DeviceManager::getInstance()->device),
            (gfx::WebGLRenderPass *)renderPass,
            renderArea,
            clearColors,
            clearDepth,
            clearStencil);
    }
    else
    {
        renderPassSet.pass = (gfx::WebGLRenderPass *)renderPass;
        renderPassSet.renderArea = renderArea;
        renderPassSet.clearColors = clearColors;
        renderPassSet.clearDepth = clearDepth;
        renderPassSet.clearStencil = clearStencil;
        renderPassSet.dirty = true;
    }
}

void WebGLCommandBuffer::endRenderPass()
{
}

void WebGLCommandBuffer::bindPipelineState(PipelineState *pso)
{
    pipelineState = pso;
}

void WebGLCommandBuffer::bindDescriptorSet(int set, DescriptorSet *descriptorSet)
{
    int needSize = set + 1;
    if (curGPUDescriptorSets.size() < needSize)
    {
        curGPUDescriptorSets.resize(needSize);
    }
    curGPUDescriptorSets[set] = descriptorSet;
    // printf("+++++++++++++++++++++++++++end set %d d %p\n",set, curGPUDescriptorSets[set],descriptorSet);
}

void WebGLCommandBuffer::draw(const gfx::InputDraw &info)
{

    if (renderPassSet.dirty)
    {
        renderPassSet.dirty = false;
        WebGLCmdFuncBeginRenderPass(
            static_cast<WebGLDevice *>(DeviceManager::getInstance()->device),
            renderPassSet.pass,
            renderPassSet.renderArea,
            renderPassSet.clearColors,
            renderPassSet.clearDepth,
            renderPassSet.clearStencil);
    }
    bindStates();
    WebGLCmdFuncDraw(static_cast<WebGLDevice *>(DeviceManager::getInstance()->device), info);
}

void WebGLCommandBuffer::bindStates()
{
    WebGLCmdFuncBindStates(
        static_cast<WebGLDevice *>(DeviceManager::getInstance()->device),
        static_cast<WebGLPipelineState *>(pipelineState),
        curGPUDescriptorSets);
}

void WebGLCommandBuffer::setViewport(const Rect<float> &vp)
{
}

void WebGLCommandBuffer::setScissor(const Rect<float> &rect)
{
}