#ifndef _DEVICE_H_
#define _DEVICE_H_
#include "BufferObject.h"
#include "define.h"
#include "InputAssembler.h"
#include "DescriptorSetLayout.h"
#include "DescriptorSet.h"
#include "Shader.h"
#include "CommandBuffer.h"
#include "define_device_caps.h"
#include "RenderPass.h"
#include "SamplerObject.h"
#include <unordered_map>
enum class DeviceAPI
{
    UNKNOWN,
    GLES2,
    GLES3,
    VULKAN,
    WEBGL,
    WEBGL2,
    WEBGPU
};

/**
 * 代表了绘制的图形接口
 *
 */
class Device
{

protected:

    /**
     * 对于同一个Samapler设置，所有的Texture都使用一个即可
     */
    std::unordered_map<unsigned int, gfx::SamplerObject *> samplers;

public:
    DeviceCaps *deviceCaps;
    DeviceAPI API;

    Device();
    virtual ~Device();

    /**
     * 创建缓冲。
     */
    virtual BufferObject *createBuffer(BufferInfo &info) = 0;

    virtual gfx::Texture *createTexture(TextureInfo &) = 0;

    virtual void copyTexImagesToTexture(int imageAssetId, gfx::Texture *texture) = 0;
    virtual void copyBuffersToTexture(const unsigned char *buffer, gfx::Texture *texture, BufferTextureCopyRegion &) = 0;
    virtual DescriptorSetLayout *createDescriptorSetLayout(DescriptorSetLayoutInfo &info) = 0;

    virtual DescriptorSet *createDescriptorSet(DescriptorSetInfo &info) = 0;

    virtual Shader *createShader(ShaderInfo &info) = 0;

    virtual gfx::CommandBuffer *getCommandBuffer() = 0;

    virtual PipelineState *createPipelineState(IPipelineStateInfo &) = 0;

    virtual InputAssembler *createInputAssembler() = 0;

    virtual gfx::RenderPass *createRenderPass(gfx::RenderPassDefine &) = 0;

    virtual gfx::SamplerObject *getSampler(SamplerInfo &) = 0;
};

#endif