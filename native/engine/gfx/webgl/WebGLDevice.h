#ifndef _WEBGL_DEVICE_H_
#define _WEBGL_DEVICE_H_
#include "../base/Device.h"
#include "WebGLGPUObjectDefine.h"
#include "WebGLCommandBuffer.h"
#include "WebGLPipelineState.h"
#include "WebGL.h"

class WebGLDevice : public Device
{
private:
    WebGLCommandBuffer *commandBuffer;

    // 整个应用只需要一个state就够了，每次绘制都重新设置state中的数据
    WebGLPipelineState pipelineState;

    GLAPI::WebGL gl;

public:
    GLGPUStateCache renderStateCache;

    WebGLDevice();
    void initializeCapability(char *buffer);
    inline GLAPI::WebGL &getGLContext() { return gl; };
    virtual BufferObject *createBuffer(BufferInfo &info);
    virtual gfx::Texture *createTexture(TextureInfo &);
    virtual void copyTexImagesToTexture(int imageAssetId, gfx::Texture *texture);
    virtual void copyBuffersToTexture(const unsigned char *buffer, gfx::Texture *texture, BufferTextureCopyRegion &);
    virtual DescriptorSetLayout *createDescriptorSetLayout(DescriptorSetLayoutInfo &info);
    virtual DescriptorSet *createDescriptorSet(DescriptorSetInfo &info);
    virtual Shader *createShader(ShaderInfo &info);
    virtual gfx::CommandBuffer *getCommandBuffer();
    virtual PipelineState *createPipelineState(IPipelineStateInfo &);
    virtual InputAssembler *createInputAssembler();
    virtual gfx::RenderPass *createRenderPass(gfx::RenderPassDefine &);
    virtual gfx::SamplerObject *getSampler(SamplerInfo &);
};
#endif
