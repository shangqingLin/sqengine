#include "EffectAsset.h"
#include "../gfx/gfx.h"
#include "../scene/ProgramLib.h"
#include "../core/core.h"

IBlockInfo::~IBlockInfo()
{
    for (int i = 0; i < members.size(); ++i)
    {
        delete members[i];
    }
    members.clear();
}

IShaderDefine::~IShaderDefine()
{
    for (int i = 0; i < attributes.size(); ++i)
    {
        delete attributes[i];
    }
    attributes.clear();
    for (int i = 0; i < blocks.size(); ++i)
    {
        delete blocks[i];
    }
    blocks.clear();

    for (int i = 0; i < samplerTextures.size(); ++i)
    {
        delete samplerTextures[i];
    }
    samplerTextures.clear();
}

IEffectDefine::~IEffectDefine()
{
    for (int i = 0; i < shaders.size(); ++i)
    {
        delete shaders[i];
    }
    for (int i = 0; i < passes.size(); ++i)
    {
        delete passes[i];
    }
    shaders.clear();
    passes.clear();
}

//===========================================================

EffectAsset::EffectAsset(int id) : Asset(id) {}

EffectAsset::~EffectAsset()
{
    for (int i = 0; i < passes.size(); ++i)
    {
        ProgramLib::getInstnace()->remove(passes[i].effectAssetId, passes[i].shaderIndex);
    }
    passes.clear();
}

static void writePassState(PassInfo &pass, char *data)
{
    ArrayBuffer buffer;
    buffer.setExternalBuffer(data, 0);
    char primitive = *buffer.popp<char>();
    if (primitive != -1)
    {
        pass.state.primitive = PrimitiveMode(primitive);
    }
    unsigned int state = *buffer.popp<unsigned int>();
    if (state & (1 << 1))
    {
        unsigned int blendStateNum = *buffer.popp<unsigned int>();
        BlendState blendState;

        if (blendStateNum & (1 << 9))
        {
            if (blendStateNum & (1 << 1))
            {
                blendState.blendTarget.blend = (bool)(*buffer.popp<unsigned char>());
            }
            if (blendStateNum & (1 << 2))
            {
                blendState.blendTarget.blendSrc = gfx::BlendFactor(*buffer.popp<unsigned char>());
            }
            if (blendStateNum & (1 << 3))
            {
                blendState.blendTarget.blendSrcAlpha = gfx::BlendFactor(*buffer.popp<unsigned char>());
            }
            if (blendStateNum & (1 << 4))
            {
                blendState.blendTarget.blendDst = gfx::BlendFactor(*buffer.popp<unsigned char>());
            }
            if (blendStateNum & (1 << 5))
            {
                blendState.blendTarget.blendDstAlpha = gfx::BlendFactor(*buffer.popp<unsigned char>());
            }
            if (blendStateNum & (1 << 6))
            {
                blendState.blendTarget.blendEq = gfx::BlendOp(*buffer.popp<unsigned char>());
            }
            if (blendStateNum & (1 << 7))
            {
                blendState.blendTarget.blendAlphaEq = gfx::BlendOp(*buffer.popp<unsigned char>());
            }
             
            if (blendStateNum & (1 << 8))
            {
                blendState.blendTarget.blendColorMask = *buffer.popp<unsigned char>();
            }
        }
        else if (blendStateNum & (1 << 10))
        {
            SetBlendMode(BlendMode(*buffer.popp<unsigned char>()), blendState.blendTarget);
            blendState.blendTarget.blend = true;
        }
        pass.state.blendState = blendState;
    }
}

void EffectAsset::initialize(IEffectDefine *info)
{

    for (int i = 0; i < info->shaders.size(); ++i)
    {
        ProgramLib::getInstnace()->define(info->shaders[i], this, i);
    }

    passes.assign(info->passes.size(), PassInfo());
    for (int i = 0; i < info->passes.size(); ++i)
    {
        PassInfo &pass = passes[i];
        pass.shaderIndex = info->passes[i]->shaderIndex;
        pass.effectAssetId = getId();
        if (info->passes[i]->passStateBuffer)
        {
            writePassState(pass, info->passes[i]->passStateBuffer);
        }
    }
}