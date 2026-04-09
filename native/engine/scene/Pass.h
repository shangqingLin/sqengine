#ifndef _PASS_H_
#define _PASS_H_

#include "../assets/EffectAsset.h"
#include "../gfx/gfx.h"
#include <unordered_map>
#include "PassUtils.h"
#include "../core/core.h"
#include "PassDefine.h"

typedef std::unordered_map<const UniformBlock *, sqstd::ByteBlockChunk *> BUFFER_VIEW_MAP;

/**
 * vk专业术语称为渲染通道（Render Pass）
 */
class Pass
{
private:
    PassInfo *info;
    BufferObject *uboBufferObj;
    sqstd::Byte *uboBuffer;
    sqstd::ByteBlock *uboBufferBlock;
    Shader *shader;
    DescriptorSet *descriptorSet;
    BUFFER_VIEW_MAP buffersViews;
    sqstd::hash_t _hash{0U};
    BlendState blendState;
    StencilState stencilState;
    RasterizerState rs;
    DepthState depthState;
    PrimitiveMode primitive;

    void doInit(PassInfo *info);

    /**
     * 将Uniform重置为在techniques->properties->value哪里指定的值，
     * 配置在value的值作为初始化值，允许没有在properties配置的情况
     */
    void resetUBOs();

    void createPass();
    void buildUniformBlocks(PassInfo *info);
    void updatePassHash();

public:
    friend class Material;

    Pass();
    ~Pass();

    inline BlendState &getBlendState() { return blendState; };
    inline StencilState &getStencilState() { return stencilState; };
    inline RasterizerState &getRasterizerState() { return rs; };
    inline DepthState &getDepthState() { return depthState; };
    inline PrimitiveMode &getPrimitive() { return primitive; };
    inline DescriptorSet *getDescriptorSet() const { return descriptorSet; };
    void initialize(PassInfo *info);
    void compile();
    Shader *getShaderVariant();
    void fillPipelineInfo(PassStates &overrideState);
    void resetPipelineInfo();
    void clear();
    void setUniform(const Uniform &, MaterialProperty &value);
    void setUniformArray(const Uniform &, std::vector<MaterialProperty> &values);
    void bindTexture(int binding, gfx::Texture *, int index = 0);
    void bindSampler(int binding, gfx::SamplerObject *, int index = 0);
    const sqstd::hash_t &getHash() { return _hash; };
    void copy(Pass &);
};

#endif