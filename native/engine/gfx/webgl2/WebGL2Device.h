#ifndef _WEBGL_2_DEVICE_H_
#define _WEBGL_2_DEVICE_H_
#include "../base/Device.h"
#include "WebGL2.h"
#include "WebGL2GPUObjectDefine.h"
#include "WebGL2CommandBuffer.h"
#include "WebGL2PipelineState.h"

class WebGL2Device : public Device
{
private:
  WebGL2PipelineState pipelineState;
  WebGL2CommandBuffer *commandBuffer;
  GLAPI::WebGL2 gl;

public:
  GLGPUStateCache renderStateCache;
  WebGL2Device();
  void initializeCapability(char *buffer);
  inline GLAPI::WebGL2 &getGLContext() { return gl; };
  virtual BufferObject *createBuffer(BufferInfo &info);
  virtual gfx::Texture *createTexture(TextureInfo &);
  virtual DescriptorSetLayout *createDescriptorSetLayout(DescriptorSetLayoutInfo &info);
  virtual void copyBuffersToTexture(const unsigned char *buffer, gfx::Texture *texture, BufferTextureCopyRegion &);
  virtual DescriptorSet *createDescriptorSet(DescriptorSetInfo &info);
  virtual Shader *createShader(ShaderInfo &info);
  virtual void copyTexImagesToTexture(int imageAssetId, gfx::Texture *texture);
  virtual gfx::CommandBuffer *getCommandBuffer();
  virtual PipelineState *createPipelineState(IPipelineStateInfo &);
  virtual InputAssembler *createInputAssembler();
  virtual gfx::RenderPass *createRenderPass(gfx::RenderPassDefine &);
  virtual gfx::SamplerObject *getSampler(SamplerInfo &);
};

#endif