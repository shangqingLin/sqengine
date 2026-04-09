#include "Pass.h"
#include "../gfx/gfx.h"
#include "../core/common/ArrayBuffer.h"
#include "../core/common/BufferView.h"
#include "ProgramLib.h"
#include "../rendering/buildin-block-define.h"

Pass::Pass() : descriptorSet(NULL),
               uboBuffer(NULL),
               uboBufferObj(NULL),
               uboBufferBlock(NULL),
               info(NULL),
               shader(NULL),
               primitive(PrimitiveMode::TRIANGLE_LIST)
{
}

Pass::~Pass()
{

    delete descriptorSet;
    descriptorSet = NULL;

    delete uboBuffer;
    uboBuffer = NULL;

    delete uboBufferObj;
    uboBufferObj = NULL;

    delete uboBufferBlock;
    uboBufferBlock = nullptr;

    // BUFFER_VIEW_MAP::iterator it = buffersViews.begin();
    // while (it != buffersViews.end())
    // {
    //     delete it->second;
    //     ++it;
    // }
    buffersViews.clear();
}

void Pass::initialize(PassInfo *info)
{
    this->info = info;
    doInit(info);
}

void Pass::doInit(PassInfo *info)
{
    // printf("======================doInit pass %d \n", info->effectAssetId);
    // fillPipelineInfo(*info);
    if (info->state.primitive.has_value())
        primitive = info->state.primitive.value();
    DescriptorSetLayout *layout = ProgramLib::getInstnace()->getDescriptorSetLayout(info->effectAssetId, info->shaderIndex);

    // printf(" Pass::doInit effectId %d %d %p\n", info->effectAssetId, info->shaderIndex,layout);

    DescriptorSetInfo setInfo;
    setInfo.layout = layout;
    descriptorSet = DeviceManager::getInstance()->device->createDescriptorSet(setInfo);
    buildUniformBlocks(info);
    fillPipelineInfo(info->state);

    compile();
}

/**
 * 为Uniform分配内存，创建UBO缓冲区
 */
void Pass::buildUniformBlocks(PassInfo *info)
{
    std::vector<const UniformBlock *> &blocks = ProgramLib::getInstnace()->getTemplateInfo(info->effectAssetId, info->shaderIndex)->shaderInfo.blocks;
    int blockNum = blocks.size();
    if (blockNum == 0)
        return;

    std::vector<const UniformBlock *> materialBlock;
    materialBlock.reserve(blockNum);
    for (int i = 0; i < blockNum; ++i)
    {
        const UniformBlock *block = blocks[i];
        if (block->set == toNumber(pipeline::SetIndex::MATERIAL))
        {
            materialBlock.push_back(block);
        }
    }

    blockNum = materialBlock.size();
    if (blockNum == 0)
        return;

    int totalSize = PassUtils::getUniformSize(materialBlock);

    // 整个Shader所有的Uniform使用一个大的缓存区存储数据
    // 每个Block从这个大的缓冲中读取一段数据作为Block的存储
    BufferInfo bufferInfo;
    bufferInfo.type = BufferType::UNIFORM;
    bufferInfo.usage = BufferUsage::STATIC_DRAW;
    bufferInfo.size = totalSize;
    uboBufferObj = DeviceManager::getInstance()->device->createBuffer(bufferInfo);
    uboBuffer = new sqstd::Byte();
    uboBuffer->resize(totalSize);

    uboBufferBlock = new sqstd::ByteBlock(uboBuffer);
    bufferInfo.size = 0;
    for (int i = 0; i < blockNum; ++i)
    {
        const UniformBlock *block = materialBlock[i];
        int blockSize = PassUtils::getUniformSize(*block);
        sqstd::ByteBlockChunk *chunk = uboBufferBlock->allocateChunk(blockSize);

        buffersViews[block] = chunk;
        bufferInfo.bufferView = chunk;
        BufferObject *buffer = DeviceManager::getInstance()->device->createBuffer(bufferInfo);
        descriptorSet->bindBuffer(materialBlock[i]->binding, buffer);
    }
}

void Pass::fillPipelineInfo(PassStates &overrideState)
{
    if (overrideState.blendState.has_value())
    {
        blendState = overrideState.blendState.value();
    }

    if (overrideState.depthState.has_value())
    {
        depthState = overrideState.depthState.value();
    }

    if (overrideState.rasterizerState.has_value())
    {
        rs = overrideState.rasterizerState.value();
    }

    if (overrideState.stencilState.has_value())
    {
        stencilState = overrideState.stencilState.value();
    }
    // updatePassHash();
}

void Pass::resetPipelineInfo()
{
    blendState = BlendState();
    depthState = DepthState();
    rs = RasterizerState();
    stencilState = StencilState();
}

void Pass::clear()
{
    resetPipelineInfo();
    info = nullptr;
    uboBufferObj = nullptr;
    uboBuffer = nullptr;
    uboBufferBlock = nullptr;
    shader = nullptr;
    descriptorSet = nullptr;
}

void Pass::compile()
{
    shader = ProgramLib::getInstnace()->getGFXShader(DeviceManager::getInstance()->device, info->defines, info->effectAssetId, info->shaderIndex);
}

Shader *Pass::getShaderVariant()
{
    if (!shader)
    {
        compile();
    }
    return shader;
}

void Pass::resetUBOs()
{
    std::vector<const UniformBlock *> &blocks = ProgramLib::getInstnace()->getTemplateInfo(info->effectAssetId, info->shaderIndex)->shaderInfo.blocks;
    int blockNum = blocks.size();
    if (blockNum == 0)
        return;

    std::vector<const UniformBlock *> materialBlock;
    materialBlock.reserve(blockNum);
    for (int i = 0; i < blockNum; ++i)
    {
        const UniformBlock *block = blocks[i];
        if (block->set == toNumber(pipeline::SetIndex::MATERIAL))
        {
            materialBlock.push_back(block);
        }
    }

    blockNum = materialBlock.size();
    if (blockNum == 0)
        return;

    for (int i = 0; i < blockNum; ++i)
    {
        const UniformBlock *block = materialBlock[i];
        int offset = 0;
        for (int n = 0; n < block->members.size(); ++n)
        {
            Uniform &u = (Uniform &)block->members[n];
            //    BufferView* bufferView = buffersViews[block->binding];
            if (u.type < gfx::Type::FLOAT)
            {
                // bufferView->setValue();
            }
            else
            {
            }
        }
    }
    // this->info->properties;
}

void Pass::setUniformArray(const Uniform &uniform, std::vector<MaterialProperty> &values)
{
    int stride = getTypeSize(uniform.type) >> 2;
    auto writeIt = PassUtils::type2writer.find(uniform.type);
    SQ_ASSERT(writeIt != PassUtils::type2writer.end());
    int offset = uniform.offset;
    auto bufferChunk = buffersViews[uniform.block];
    for (int i = 0; i < values.size(); ++i)
    {
        writeIt->second(bufferChunk, values[i], offset);
        offset += stride;
    }
}

void Pass::setUniform(const Uniform &uniform, MaterialProperty &value)
{
    // printf("setUniform type %d \n", uniform.type);
    auto writeIt = PassUtils::type2writer.find(uniform.type);
    SQ_ASSERT(writeIt != PassUtils::type2writer.end());
    auto bufferChunk = buffersViews[uniform.block];
    writeIt->second(bufferChunk, value, uniform.offset);
}

void Pass::bindTexture(int binding, gfx::Texture *texture, int index)
{
    // printf("pass bind texture %d %p binding %d index %d \n", this->info->effectAssetId, descriptorSet,binding,index);
    descriptorSet->bindTexture(binding, texture, index);
}

void Pass::bindSampler(int binding, gfx::SamplerObject *sampler, int index)
{
    descriptorSet->bindSampler(binding, sampler, index);
}

void Pass::updatePassHash()
{
    _hash = 666;
    // sqstd::hash_combine(hashValue, pass->_primitive);
    // sqstd::hash_combine(hashValue, pass->_dynamicStates);
    sqstd::hash_combine(_hash, blendState.serializeHash());
    sqstd::hash_combine(_hash, stencilState.serializeHash());
    sqstd::hash_combine(_hash, rs.serializeHash());
}

void Pass::copy(Pass &pass)
{
    descriptorSet = pass.descriptorSet;
    blendState = pass.blendState;
    depthState = pass.depthState;
    stencilState = pass.stencilState;
    primitive = pass.primitive;
    rs = pass.rs;
    shader = pass.getShaderVariant();
    info = pass.info;
}