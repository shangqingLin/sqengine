#include "WebGLRenderPass.h"
#include "WebGLTexture.h"
#include "WebGLCommand.h"
#include "../DeviceManager.h"
using namespace gfx;

void WebGLRenderPass::initialize(RenderPassDefine &define)
{
    colorAttachments = define.colorAttachments;
    depthStencilAttachment = define.depthStencilAttachment;

    frameBuffer.colorTextures.reserve(define.colorTextures.size());
    for (int i = 0; i < define.colorTextures.size(); ++i)
    {
        WebGLTexture *texture = dynamic_cast<WebGLTexture *>(define.colorTextures[i]);
        frameBuffer.colorTextures.push_back(texture->getGPUTexture());
    }

    if (define.depthStencilTexture)
    {
        WebGLTexture *texture = dynamic_cast<WebGLTexture *>(define.depthStencilTexture);
        frameBuffer.depthStencilTexture = texture->getGPUTexture();
    }

    if (define.defaultRenderPass)
    {
        //默认缓冲区
        frameBuffer.glFramebuffer = 0;
    }
    else
    {
        WebGLCmdFuncCreateFramebuffer(static_cast<WebGLDevice *>(DeviceManager::getInstance()->device), frameBuffer);
    }
}

WebGLRenderPass::~WebGLRenderPass()
{
    WebGLCmdFuncDestroyFramebuffer(static_cast<WebGLDevice *>(DeviceManager::getInstance()->device), frameBuffer);
}