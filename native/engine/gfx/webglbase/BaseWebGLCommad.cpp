#include "BaseWebGLCommand.h"
#include <bindings/jswrapper/wasm/wasm/WebGLCommandBridge.h>
#include "../../core/core.h"

void BaseWebGLCmdFuncCreateShader(GLAPI::BaseWebGLAPI &gl, GLGPUStateCache &cache, IGLGPUShader &gpuShader)
{

    // printf(" BaseWebGLCmdFuncCreateShader create shader %d %d %u\n",gpuShader.effectAssetId,gpuShader.shaderIndex,gpuShader.macroFlags);

    bridge::GPUShaderInfo bridgeShaderInfo;
    gpuShader.glProgram = gl.createProgram();
    bridge::WebGLCommandBridge::getInstance()->createShader(
        gpuShader.glProgram,
        gpuShader.effectAssetId,
        gpuShader.macroFlags,
        gpuShader.shaderIndex,
        bridgeShaderInfo);

    if (!bridgeShaderInfo.activeUniform.empty())
    {
        gpuShader.glActiveUniforms.assign(bridgeShaderInfo.activeUniform.size(), IGLGPUUniform());

        int uniformTotalSize = 0;
        for (int i = 0; i < bridgeShaderInfo.activeUniform.size(); ++i)
        {
            bridge::ActiveGPUUniform &bridgeUniform = bridgeShaderInfo.activeUniform[i];

            SQ_ASSERT(bridgeUniform.blockIndex <= gpuShader.blocks->size() - 1);

            // printf("==============uniform %d %d %d %d \n", gpuShader.blocks->size() ,uniform.blockIndex, uniform.memberIndex, uniform.location);

            const UniformBlock &block = *(*gpuShader.blocks)[bridgeUniform.blockIndex];

            int offset = 0;
            for (int n = 0; n < bridgeUniform.memberIndex; ++n)
            {
                offset += gfx::getTypeSize(block.members[n].type);
            }

            uniformTotalSize += gfx::getTypeSize(block.members[bridgeUniform.memberIndex].type);

            IGLGPUUniform &gpuUniform = gpuShader.glActiveUniforms[i];
            gpuUniform.blockIndex = bridgeUniform.blockIndex;
            gpuUniform.memberIndex = bridgeUniform.memberIndex;
            gpuUniform.offset = offset;
            gpuUniform.location = bridgeUniform.location;
        }

        // 每个Uniform需要缓存，如果Uniform一直不变则不需要一直调用API去设置
        sqstd::Byte *buffer = new sqstd::Byte();
        buffer->resize(uniformTotalSize);
        buffer->clearData(0);
        sqstd::ByteBlock *byteBlock = new sqstd::ByteBlock(buffer, true);
        gpuShader.uniformBuffer = byteBlock;

        // printf("+++++++++++++++++++++++shader \n");

        for (int i = 0; i < gpuShader.glActiveUniforms.size(); ++i)
        {
            IGLGPUUniform &gpuUniform = gpuShader.glActiveUniforms[i];
            const UniformBlock &block = *(*gpuShader.blocks)[gpuUniform.blockIndex];

            // printf("uniform %s \n",block.members[gpuUniform.memberIndex].name.c_str());

            int size = gfx::getTypeSize(block.members[gpuUniform.memberIndex].type);
            gpuUniform.cache = byteBlock->allocateChunk(size);
        }
    }

    if (!bridgeShaderInfo.activeSampler.empty())
    {

        int samplerCount = bridgeShaderInfo.activeSampler.size();
        gpuShader.glSamplerTextures.assign(samplerCount, IGLGPUUniformSamplerTexture());

        gl.useProgram(gpuShader.glProgram);

        // printf("=====================fuck samplerCount %d program %d \n", samplerCount,gpuShader.glProgram);

        // sampler使用哪个纹理单元，在创建Shader的时候就可以设置的
        // 因为在同一个drawcall中不可能同时激活两个shader，所以同一时刻一个纹理单元只会被一个shader使用，不会冲突
        int texUnit = 0;
        for (int i = 0; i < samplerCount; ++i)
        {
            IGLGPUUniformSamplerTexture &gpuSampler = gpuShader.glSamplerTextures[i];
            bridge::ActiveGPUUniform &info = bridgeShaderInfo.activeSampler[i];
            gpuSampler.index = info.memberIndex;
            gpuSampler.location = info.location;

            UniformSamplerTexture &textureInfo = *(*gpuShader.samplerTextures)[gpuSampler.index];

            // printf("add Texture texUnit %d size %d name %s index %d \n",
            //        texUnit,
            //        textureInfo.count,
            //        textureInfo.name.c_str(),
            //        gpuSampler.index);

            // 设置过一次就不需要再设置，因为后续不会改变
            if (textureInfo.count > 1)
            {
                gpuSampler.texUnits.reserve(textureInfo.count);
                for (int m = 0; m < textureInfo.count; ++m)
                {
                    gpuSampler.texUnits.push_back(texUnit);
                    ++texUnit;
                }
                gl.uniform1iv(gpuSampler.location, (int)textureInfo.count, (char *)(&gpuSampler.texUnits[0]));
            }
            else
            {
                gpuSampler.texUnits.push_back(texUnit);
                gl.uniform1i(gpuSampler.location, texUnit);
                ++texUnit;
            }
        }
        if (cache.glProgram)
        {
            gl.useProgram(cache.glProgram);
        }

        gpuShader.numTexUnitUse = texUnit;
    }

    {
        int attributeNum = bridgeShaderInfo.activeAttributes.size();

        // printf("parse attribute num: %d  shader %p \n", attributeNum,&gpuShader);

        if (attributeNum > 0)
        {
            gpuShader.glActiveAttributes.assign(attributeNum, IGLGPUAttribute());

            int stride = 0;
            for (int i = 0; i < attributeNum; ++i)
            {
                IGLGPUAttribute &gpuAtrribute = gpuShader.glActiveAttributes[i];
                bridge::ActiveAttribute &info = bridgeShaderInfo.activeAttributes[i];
                Attribute &attribute = (*gpuShader.attributes)[info.index];

                // printf("===============attribute %d %d %s %d %d\n", i, info.index, attribute.name.c_str(), info.location, gpuShader.glActiveAttributes.size());

                gpuAtrribute.attribute = &attribute;
                gpuAtrribute.location = info.location;
                gpuAtrribute.type = attribute.format;
                FormatInfo &formatInfo = FormatInfos[toNumber(attribute.format)];
                gpuAtrribute.count = formatInfo.count;
                stride += formatInfo.size;
            }

            int offset = 0;
            for (unsigned short i = 0; i < attributeNum; ++i)
            {
                IGLGPUAttribute &gpuAtrribute = gpuShader.glActiveAttributes[i];
                gpuAtrribute.offset = offset;
                gpuAtrribute.stride = stride;

                offset += FormatInfos[toNumber(gpuAtrribute.attribute->format)].size;
            }
        }
    }

    // printf("++++++++++++++++++++++++++++++++++++++++++++++++++++create shader end progam %d \n", gpuShader.glProgram);
}

void BaseWebGLCmdFuncCreateFramebuffer(GLAPI::BaseWebGLAPI &gl, GLGPUStateCache &cache, IGLGPUFrameBuffer &gpuFramebuffer)
{
    gpuFramebuffer.glFramebuffer = gl.createFramebuffer();
    gl.bindFramebuffer(gl.FRAMEBUFFER, gpuFramebuffer.glFramebuffer);

    // if(gpuFramebuffer.glFramebuffer == 6){
    //     TRACE();
    // }
    int colorAttachmentNum = gpuFramebuffer.colorTextures.size();
    for (int i = 0; i < colorAttachmentNum; ++i)
    {
        IGLGPUTexture &gpuTexture = gpuFramebuffer.colorTextures[i];
        if (gpuTexture.glTexture)
        {
            gl.framebufferTexture2D(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0 + i, gpuTexture.glTarget, gpuTexture.glTexture, 0);
        }
        else
        {
            gl.framebufferRenderbuffer(
                gl.FRAMEBUFFER,
                gl.COLOR_ATTACHMENT0 + i,
                gl.RENDERBUFFER,
                gpuTexture.glRenderbuffer);
        }
    }

    // 要支持MRT必須使用drawBuffers進行設置，對應到片元着色器中输出的顺序
    if (colorAttachmentNum > 1)
    {
        gl.drawBuffers(colorAttachmentNum);
    }

    if (gpuFramebuffer.depthStencilTexture.has_value())
    {

        IGLGPUTexture &texture = gpuFramebuffer.depthStencilTexture.value();
        GLAPI::GLenum glAttachment = texture.glInternalFmt == Format::DEPTH_STENCIL ? gl.DEPTH_STENCIL_ATTACHMENT : gl.DEPTH_ATTACHMENT;

        if (texture.glTexture)
        {
            gl.framebufferTexture2D(
                gl.FRAMEBUFFER,
                glAttachment,
                texture.glTarget,
                texture.glTexture,
                0); // level must be 0
        }
        else
        {
            gl.framebufferRenderbuffer(
                gl.FRAMEBUFFER,
                glAttachment,
                gl.RENDERBUFFER,
                texture.glRenderbuffer);
        }
    }

    // 就算cache.glFramebuffer=0也需要绑定一下，因为0代表的是默认的framebuffer
    gl.bindFramebuffer(gl.FRAMEBUFFER, cache.glFramebuffer);
}

void BaseWebGLCmdFuncDestroyFramebuffer(GLAPI::BaseWebGLAPI &gl, GLGPUStateCache &cache, IGLGPUFrameBuffer &gpuFramebuffer)
{
    if (gpuFramebuffer.glFramebuffer)
    {
        gl.deleteFramebuffer(gpuFramebuffer.glFramebuffer);
        if (cache.glFramebuffer == gpuFramebuffer.glFramebuffer)
        {
            gl.bindFramebuffer(gl.FRAMEBUFFER, NULL);
            cache.glFramebuffer = NULL;
        }
        gpuFramebuffer.glFramebuffer = NULL;
    }
}

GLAPI::GLenum blendFractorToGLEnum(const GLAPI::BaseWebGLAPI &gl, gfx::BlendFactor factor)
{
    switch (factor)
    {
    case gfx::BlendFactor::ONE:
        return gl.ONE;
    case gfx::BlendFactor::ZERO:
        return gl.ZERO;
    case gfx::BlendFactor::SRC_ALPHA:
        return gl.SRC_ALPHA;
    case gfx::BlendFactor::DST_ALPHA:
        return gl.DST_ALPHA;
    case gfx::BlendFactor::SRC_ALPHA_SATURATE:
        return gl.SRC_ALPHA_SATURATE;
    case gfx::BlendFactor::CONSTANT_ALPHA:
        return gl.CONSTANT_ALPHA;
    case gfx::BlendFactor::CONSTANT_COLOR:
        return gl.CONSTANT_COLOR;
    case gfx::BlendFactor::DST_COLOR:
        return gl.DST_COLOR;
    case gfx::BlendFactor::ONE_MINUS_CONSTANT_ALPHA:
        return gl.ONE_MINUS_CONSTANT_ALPHA;
    case gfx::BlendFactor::ONE_MINUS_CONSTANT_COLOR:
        return gl.ONE_MINUS_CONSTANT_COLOR;
    case gfx::BlendFactor::ONE_MINUS_DST_ALPHA:
        return gl.ONE_MINUS_DST_ALPHA;
    case gfx::BlendFactor::ONE_MINUS_DST_COLOR:
        return gl.ONE_MINUS_DST_COLOR;
    case gfx::BlendFactor::ONE_MINUS_SRC_ALPHA:
        return gl.ONE_MINUS_SRC_ALPHA;
    case gfx::BlendFactor::ONE_MINUS_SRC_COLOR:
        return gl.ONE_MINUS_SRC_COLOR;
    case gfx::BlendFactor::SRC_COLOR:
        return gl.SRC_COLOR;
    default:
        SQ_ASSERT(false);
    }
    return 0;
}

GLAPI::GLenum blendOpToGLenum(const GLAPI::BaseWebGLAPI &gl, gfx::BlendOp op)
{
    switch (op)
    {
    case gfx::BlendOp::ADD:
        return gl.FUNC_ADD;
    case gfx::BlendOp::MAX:
        return gl.MAX;
    case gfx::BlendOp::MIN:
        return gl.MIN;
    case gfx::BlendOp::SUB:
        return gl.FUNC_SUBTRACT;
    case gfx::BlendOp::REV_SUB:
        return gl.FUNC_REVERSE_SUBTRACT;
    default:
        SQ_ASSERT(false);
    }
    return 0;
}

void BaseWebGLCmdUpdateBufferObject(GLAPI::BaseWebGLAPI &gl, GLAPI::GLenum target, int nativeId)
{
    gl.bufferDataFromJs(target, nativeId);
}