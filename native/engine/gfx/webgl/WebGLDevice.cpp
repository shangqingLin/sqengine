#include "WebGLDevice.h"
#include "WebGLBufferObject.h"
#include "../DeviceManager.h"
#include "WebGLDescriptorSetLayout.h"
#include "WebGLDescriptorSet.h"
#include "WebGLShader.h"
#include "WebGLTexture.h"
#include "WebGLCommand.h"
#include "WebGLDeviceCaps.h"
#include "WebGLInputAssembler.h"
#include "WebGLRenderPass.h"
#include "WebGLSamplerObject.h"

using namespace gfx;

WebGLDevice::WebGLDevice()
{
   API = DeviceAPI::WEBGL;
   deviceCaps = new WebGLDeviceCaps();
   DeviceManager::getInstance()->device = this;
   commandBuffer = new WebGLCommandBuffer();
}

void WebGLDevice::initializeCapability(char *buffer)
{
   ArrayBuffer rBuffer;
   rBuffer.setExternalBuffer(buffer, 4);
   WebGLDeviceCaps *caps = dynamic_cast<WebGLDeviceCaps *>(deviceCaps);
   caps->supportFloatTexture = *rBuffer.popp<char>();
   caps->supportVertexShaderTexture = *rBuffer.popp<char>();
   caps->enableVAO = *rBuffer.popp<char>();
   caps->WEBGL_depth_texture = *rBuffer.popp<char>();
}

BufferObject *WebGLDevice::createBuffer(BufferInfo &info)
{
   WebGLBufferObject *buffer = new WebGLBufferObject();
   buffer->initialize(info);
   return buffer;
}

DescriptorSetLayout *WebGLDevice::createDescriptorSetLayout(DescriptorSetLayoutInfo &info)
{
   WebGLDescriptorSetLayout *layout = new WebGLDescriptorSetLayout();
   layout->initialize(info);
   return layout;
}

DescriptorSet *WebGLDevice::createDescriptorSet(DescriptorSetInfo &info)
{
   WebGLDescriptorSet *set = new WebGLDescriptorSet();
   set->initialize(info);
   return set;
}

Shader *WebGLDevice::createShader(ShaderInfo &info)
{
   WebGLShader *shader = new WebGLShader();
   shader->initialize(info);
   return shader;
}

gfx::Texture *WebGLDevice::createTexture(TextureInfo &info)
{
   WebGLTexture *texture = new WebGLTexture();
   texture->initialize(info);
   return texture;
}

void WebGLDevice::copyTexImagesToTexture(int imageAssetId, gfx::Texture *texture)
{
   WebGLTexture *webGLTexture = dynamic_cast<WebGLTexture *>(texture);
   WebGLCmdFuncCopyTexImagesToTexture(this, imageAssetId, webGLTexture->getGPUTexture());
}

void WebGLDevice::copyBuffersToTexture(const unsigned char *buffer, gfx::Texture *texture, BufferTextureCopyRegion &region)
{
   WebGLTexture *webGLTexture = dynamic_cast<WebGLTexture *>(texture);
   WebGCmdFuncCopyBuffersToTexture(this, buffer, webGLTexture->getGPUTexture(), region);
}

gfx::CommandBuffer *WebGLDevice::getCommandBuffer()
{
   return commandBuffer;
}

PipelineState *WebGLDevice::createPipelineState(IPipelineStateInfo &info)
{
   pipelineState.setPipelineInfo(info);
   return &pipelineState;
}

InputAssembler *WebGLDevice::createInputAssembler()
{
   return new WebGLInputAssembler();
}

gfx::RenderPass *WebGLDevice::createRenderPass(gfx::RenderPassDefine &renderPassDefine)
{
   gfx::WebGLRenderPass *renderPass = new gfx::WebGLRenderPass();
   renderPass->initialize(renderPassDefine);
   return renderPass;
}

SamplerObject *WebGLDevice::getSampler(SamplerInfo &info)
{
   unsigned int hash = gfx::SamplerObject::computeHash(info);
   if (samplers.find(hash) == samplers.end())
   {
      samplers[hash] = new gfx::WebGLSamplerObject(info, hash);
   }
   return samplers[hash];
}