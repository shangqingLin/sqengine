#include "WebGL2Device.h"
#include "../DeviceManager.h"
#include "WebGL2BufferObject.h"
#include "WebGL2DescriptorSet.h"
#include "WebGL2DescriptorSetLayout.h"
#include "WebGL2Shader.h"
#include "WebGL2Texture.h"
#include "WebGL2Command.h"
#include "WebGL2InputAssembler.h"
#include "WebGL2DeviceCaps.h"
#include "WebGL2SamplerObject.h"
using namespace gfx;

WebGL2Device::WebGL2Device()
{
    API = DeviceAPI::WEBGL2;
    deviceCaps = new WebGL2DeviceCaps();
    DeviceManager::getInstance()->device = this;
    commandBuffer = new WebGL2CommandBuffer;
}

void WebGL2Device::initializeCapability(char *buffer)
{
    // deviceCaps->enableVAO = false;
}

BufferObject *WebGL2Device::createBuffer(BufferInfo &info)
{
    WebGL2BufferObject *buffer = new WebGL2BufferObject();
    buffer->initialize(info);
    return buffer;
}

DescriptorSetLayout *WebGL2Device::createDescriptorSetLayout(DescriptorSetLayoutInfo &info)
{
    WebGL2DescriptorSetLayout *layout = new WebGL2DescriptorSetLayout();
    layout->initialize(info);
    return layout;
}

DescriptorSet *WebGL2Device::createDescriptorSet(DescriptorSetInfo &info)
{
    DescriptorSet *set = new DescriptorSet();
    set->initialize(info);
    return set;
}

Shader *WebGL2Device::createShader(ShaderInfo &info)
{
    WebGL2Shader *shader = new WebGL2Shader();
    shader->initialize(info);
    return shader;
}

gfx::Texture *WebGL2Device::createTexture(TextureInfo &info)
{
    WebGL2Texture *texture = new WebGL2Texture();
    texture->initialize(info);
    return texture;
}

void WebGL2Device::copyTexImagesToTexture(int imageAssetId, gfx::Texture *texture)
{
    WebGL2Texture *webGLTexture = dynamic_cast<WebGL2Texture *>(texture);
    WebGL2CmdFuncCopyTexImagesToTexture(this, imageAssetId, webGLTexture->getGPUTexture());
}

void WebGL2Device::copyBuffersToTexture(const unsigned char *buffer, gfx::Texture *texture, BufferTextureCopyRegion &region)
{
    WebGL2Texture *webGLTexture = dynamic_cast<WebGL2Texture *>(texture);
    WebGL2CmdFuncCopyBuffersToTexture(this, buffer, webGLTexture->getGPUTexture(), region);
}

gfx::CommandBuffer *WebGL2Device::getCommandBuffer()
{
    return commandBuffer;
}

PipelineState *WebGL2Device::createPipelineState(IPipelineStateInfo &info)
{
    pipelineState.setPipelineInfo(info);
    return &pipelineState;
}

InputAssembler *WebGL2Device::createInputAssembler()
{
    return new WebGL2InputAssembler();
}

gfx::RenderPass *WebGL2Device::createRenderPass(gfx::RenderPassDefine &define)
{
    gfx::WebGL2RenderPass *renderPass = new gfx::WebGL2RenderPass();
    renderPass->initialize(define);
    return renderPass;
}

SamplerObject *WebGL2Device::getSampler(SamplerInfo &info)
{
    // 对于相同的参数设置samplerObject是全局唯一的，即相同的纹理参数的所有的纹理都可以使用相同的samplerObject
    // 这样方便做渲染管线换成，避免了在渲染过程中频繁切换SamplerObject
    unsigned int hash = SamplerObject::computeHash(info);
    if (samplers.find(hash) == samplers.end())
    {
        samplers[hash] = new gfx::WebGL2SamplerObject(info, hash);
    }
    return samplers[hash];
}