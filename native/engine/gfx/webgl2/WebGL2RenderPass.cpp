#include "WebGL2RenderPass.h"
#include "WebGL2Texture.h"
#include "WebGL2Command.h"
#include "../DeviceManager.h"

using namespace gfx;

void WebGL2RenderPass::initialize(RenderPassDefine &define)
{
    colorAttachments = define.colorAttachments;
    depthStencilAttachment = define.depthStencilAttachment;

    // printf("+===create depthStencilAttachment %d \n",define.depthStencilAttachment.format);

    frameBuffer.colorTextures.reserve(define.colorTextures.size());
    for (int i = 0; i < define.colorTextures.size(); ++i)
    {
        WebGL2Texture *texture = dynamic_cast<WebGL2Texture *>(define.colorTextures[i]);
        frameBuffer.colorTextures.push_back(texture->getGPUTexture());

        // printf("+++++++create texture %d \n",texture->getGPUTexture().glTexture);
        // TRACE();
    }

    if (define.depthStencilTexture)
    {
        WebGL2Texture *texture = dynamic_cast<WebGL2Texture *>(define.depthStencilTexture);
        frameBuffer.depthStencilTexture = texture->getGPUTexture();
    }

    if (define.defaultRenderPass)
    {
        // 默认缓冲区
        frameBuffer.glFramebuffer = 0;
    }
    else
    {
        WebGL2CmdFuncCreateFramebuffer(static_cast<WebGL2Device *>(DeviceManager::getInstance()->device), frameBuffer);
    }
}

WebGL2RenderPass::~WebGL2RenderPass()
{
    WebGL2CmdFuncDestroyFramebuffer(static_cast<WebGL2Device *>(DeviceManager::getInstance()->device), frameBuffer);
}