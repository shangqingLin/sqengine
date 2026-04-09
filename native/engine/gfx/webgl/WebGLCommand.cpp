#include "WebGLCommand.h"
#include "WebGL.h"
#include "../base/define.h"
#include <limits.h>
#include <bindings/jswrapper/wasm/wasm/WebGLCommandBridge.h>
#include "WebGLBufferObject.h"
#include "WebGLTexture.h"
#include "../../core/core.h"
#include "../webglbase/BaseWebGLCommand.h"
#include "./WebGLDeviceCaps.h"
#include "./WebGLSamplerObject.h"

static void bindTexture(WebGLDevice *device, int texUnit, GLAPI::GLenum glTarget, GLAPI::GLTexture texture)
{
    GLGPUStateCache &cache = device->renderStateCache;

    if (cache.texUintBind.getCount() < texUnit + 1)
    {
        cache.texUintBind.resize(texUnit + 1);
        cache.texUintBindTarget.resize(texUnit + 1);
        for (size_t i = cache.texUintBind.getCount(); i < cache.texUintBind.getCapacity(); i++)
        {
            cache.texUintBind.push(-1);
            cache.texUintBindTarget.push(-1);
        }
    }

    if (cache.texUintBind[texUnit] != texture || cache.texUintBindTarget[texUnit] != glTarget)
    {
        cache.texUintBind[texUnit] = texture;
        cache.texUintBindTarget[texUnit] = glTarget;
        GLAPI::WebGL &gl = device->getGLContext();
        gl.activeTexture(texUnit);
        gl.bindTexture(glTarget, texture);
    }
}

void WebGLCmdFuncBeginRenderPass(WebGLDevice *device, gfx::WebGLRenderPass *renderPass, Rect<float> &renderArea, Color &clearColors, float clearDepth, int clearStencil)
{

    GLAPI::WebGL &gl = device->getGLContext();
    GLGPUStateCache &cache = device->renderStateCache;
    const IGLGPUFrameBuffer &gpuFramebuffer = renderPass->getFrameBuffer();

    // 如果是默认缓冲区，则gpuFramebuffer.glFramebuffer = 0，直接可以切换回默认帧缓冲区
    if (cache.glFramebuffer != gpuFramebuffer.glFramebuffer)
    {

        /**
         * 如果一个Texture在纹理单元激活中，但此刻又作为当前帧缓冲区的渲染目标使用的话，
         * 会报错 Feedback loop formed between Framebuffer and active Texture。所以此时需要取消绑定
         */
        for (int i = 0; i < renderPass->getFrameBuffer().colorTextures.size(); ++i)
        {
            const IGLGPUTexture &c = renderPass->getFrameBuffer().colorTextures[i];

            for (int n = 0; n < cache.texUintBind.getCount(); ++n)
            {
                if (cache.texUintBind[n] == c.glTexture)
                {
                    bindTexture(device, n, cache.texUintBindTarget[n], 0);
                    break;
                }
            }
            // printf("user frame %d Texture %d \n", gpuFramebuffer.glFramebuffer,c.glTexture);
        }

        gl.bindFramebuffer(gl.FRAMEBUFFER, gpuFramebuffer.glFramebuffer);
        cache.glFramebuffer = gpuFramebuffer.glFramebuffer;
    }

    if (cache.viewport.x != renderArea.x || cache.viewport.y != renderArea.y || cache.viewport.width != renderArea.width || cache.viewport.height != renderArea.height)
    {
        gl.viewport(renderArea.x, renderArea.y, renderArea.width, renderArea.height);

        cache.viewport.x = renderArea.x;
        cache.viewport.y = renderArea.y;
        cache.viewport.width = renderArea.width;
        cache.viewport.height = renderArea.height;
    }

    if (cache.scissor.x != renderArea.x || cache.scissor.y != renderArea.y || cache.scissor.width != renderArea.width || cache.scissor.height != renderArea.height)
    {
        gl.scissor(renderArea.x, renderArea.y, renderArea.width, renderArea.height);

        cache.scissor.x = renderArea.x;
        cache.scissor.y = renderArea.y;
        cache.scissor.width = renderArea.width;
        cache.scissor.height = renderArea.height;
    }

    GLAPI::GLbitfield clears = 0;
    for (int j = 0; j < renderPass->colorAttachments.size(); ++j)
    {
        gfx::ColorAttachment &colorAttachment = renderPass->colorAttachments[j];

        if (colorAttachment.format != Format::UNKNOWN)
        {
            switch (colorAttachment.op)
            {
            case gfx::AttachmentOp::LOAD:
                break;
            case gfx::AttachmentOp::CLEAR:
            {
                // 恢复由于在Blend操作中设置的颜色掩码使得某些颜色分量不能写入，这里恢复一下
                if (cache.bs.blendTarget.blendColorMask != toNumber(gfx::ColorMask::ALL))
                {
                    gl.colorMask(true, true, true, true);
                }
                gl.clearColor(clearColors.r, clearColors.g, clearColors.b, clearColors.a);
                clears |= gl.COLOR_BUFFER_BIT;
                break;
            }
            case gfx::AttachmentOp::DISCARD:
            {
                break;
            }
            }

            // 目前只需要设置一个就可以了
            break;
        }
    }

    if (renderPass->depthStencilAttachment.format != Format::UNKNOWN)
    {

        if (renderPass->depthStencilAttachment.format == Format::DEPTH || renderPass->depthStencilAttachment.format == Format::DEPTH_STENCIL)
        {
            switch (renderPass->depthStencilAttachment.depthOp)
            {
            case gfx::AttachmentOp::LOAD:
                break;
            case gfx::AttachmentOp::CLEAR:
            {
                // 恢复一下，要不clearDepth没有效果了
                if (!cache.ds.depthWrite)
                {
                    gl.depthMask(true);
                }

                gl.clearDepth(clearDepth);

                clears |= gl.DEPTH_BUFFER_BIT;
                break;
            }
            case gfx::AttachmentOp::DISCARD:
            {
                break;
            }
            }
        }

        if (renderPass->depthStencilAttachment.format == Format::DEPTH_STENCIL)
        {
            {
                switch (renderPass->depthStencilAttachment.stencilOp)
                {
                case gfx::AttachmentOp::LOAD:
                    break;
                case gfx::AttachmentOp::CLEAR:
                {

                    // 恢复一下，要不stencilMaskSeparate没有效果了
                    if (!cache.ss.stencilWriteMaskFront)
                    {
                        gl.stencilMaskSeparate(gl.FRONT, 0xffff);
                    }

                    if (!cache.ss.stencilWriteMaskBack)
                    {
                        gl.stencilMaskSeparate(gl.BACK, 0xffff);
                    }

                    gl.clearStencil(clearStencil);
                    clears |= gl.STENCIL_BUFFER_BIT;
                    break;
                }
                case gfx::AttachmentOp::DISCARD:
                {
                    break;
                }
                default:
                }
            }
        }
    }

    /**
     * clearDepth\clearStencil\clearColor之后还需要
     * 调用clear函数才能生效
     */
    if (clears)
    {
        gl.clear(clears);
    }

    // 恢复Blend操作中设置
    if (clears & gl.COLOR_BUFFER_BIT)
    {
        if (cache.bs.blendTarget.blendColorMask != toNumber(gfx::ColorMask::ALL))
        {
            unsigned int colorMask = cache.bs.blendTarget.blendColorMask;
            bool r = (colorMask & toNumber(gfx::ColorMask::R));
            bool g = (colorMask & toNumber(gfx::ColorMask::G));
            bool b = (colorMask & toNumber(gfx::ColorMask::B));
            bool a = (colorMask & toNumber(gfx::ColorMask::A));
            gl.colorMask(r, g, b, a);
        }
    }

    if ((clears & gl.DEPTH_BUFFER_BIT) && !cache.ds.depthWrite)
    {
        gl.depthMask(false);
    }

    if (clears & gl.STENCIL_BUFFER_BIT)
    {
        if (!cache.ss.stencilWriteMaskFront)
        {
            gl.stencilMaskSeparate(gl.FRONT, 0);
        }

        if (!cache.ss.stencilWriteMaskBack)
        {
            gl.stencilMaskSeparate(gl.BACK, 0);
        }
    }
}

void WebGLCmdFuncEndRenderPass()
{
}

void bindVertexBuffer(WebGLDevice *device, IGLGPUBuffer *gpuBuffer)
{
    GLAPI::WebGL &gl = device->getGLContext();
    GLGPUStateCache &cache = device->renderStateCache;

    // if(gpuBuffer->attrs){
    //     printf("--------------\n");
    //     for(int i = 0; i < gpuBuffer->attrs->size(); ++i){
    //         printf("==== attr name %s \n",(*gpuBuffer->attrs)[i].name.c_str());
    //     }
    // }

    SQ_ASSERT(gpuBuffer->glBuffer);
    if (gpuBuffer->glBuffer != cache.glArrayBuffer)
    {
        cache.glArrayBuffer = gpuBuffer->glBuffer;
        gl.bindBuffer(gl.ARRAY_BUFFER, gpuBuffer->glBuffer);
    }
}

void bindIndexBuffer(WebGLDevice *device, GLAPI::GLBuffer glBuffer)
{
    GLAPI::WebGL &gl = device->getGLContext();
    GLGPUStateCache &cache = device->renderStateCache;
    SQ_ASSERT(glBuffer);
    if (glBuffer != cache.glElementArrayBuffer)
    {
        cache.glElementArrayBuffer = glBuffer;
        gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, glBuffer);
    }
}

void WebGLCmdFuncCreateBuffer(WebGLDevice *device, IGLGPUBuffer *gpuBuffer)
{
    GLAPI::WebGL &gl = device->getGLContext();
    GLGPUStateCache &cache = device->renderStateCache;
    if (gpuBuffer->type == BufferType::VERTEX)
    {
        gpuBuffer->glTarget = gl.ARRAY_BUFFER;
        gpuBuffer->glBuffer = gl.createBuffer();
        if (gpuBuffer->size > 0)
        {

            if (cache.glVAO)
            {
                cache.glVAO = 0;
                gl.bindVertexArrayOES(0);
            }
            bindVertexBuffer(device, gpuBuffer);
            gl.bufferData(gl.ARRAY_BUFFER, gpuBuffer->size, gpuBuffer->memUsage == BufferUsage::DYNAMIC_DRAW ? gl.DYNAMIC_DRAW : gl.STATIC_DRAW);
        }
    }
    else if (gpuBuffer->type == BufferType::INDEX)
    {
        if (cache.glVAO)
        {
            cache.glVAO = 0;
            gl.bindVertexArrayOES(0);
        }
        gpuBuffer->glTarget = gl.ELEMENT_ARRAY_BUFFER;
        gpuBuffer->glBuffer = gl.createBuffer();
        if (gpuBuffer->size > 0)
        {
            bindIndexBuffer(device, gpuBuffer->glBuffer);
            gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, gpuBuffer->size, gpuBuffer->memUsage == BufferUsage::DYNAMIC_DRAW ? gl.DYNAMIC_DRAW : gl.STATIC_DRAW);
        }
    }
    else if (gpuBuffer->type == BufferType::UNIFORM)
    {
        // 有BufferView表示不是Buffer Object，他只映射其中一段内存而已
        if (gpuBuffer->bufferView)
        {
            return;
        }

        // WebGL1.0不支持UBO，这里直接将数据填充到应用端的内存中，通过调用传统的glUniform*来进行设值
        gpuBuffer->buffer = new sqstd::Byte();
        if (gpuBuffer->size > 0)
            gpuBuffer->buffer->resize(gpuBuffer->size);
    }
}

void WebGLCmdFuncResizeBuffer(WebGLDevice *device, IGLGPUBuffer *gpuBuffer, int size)
{
    GLAPI::WebGL &gl = device->getGLContext();
    gpuBuffer->size = size;
    if (gpuBuffer->type == BufferType::VERTEX)
    {
        GLGPUStateCache &cache = device->renderStateCache;
        if (cache.glVAO)
        {
            cache.glVAO = 0;
            gl.bindVertexArrayOES(0);
        }

        bindVertexBuffer(device, gpuBuffer);
        gl.bufferData(gl.ARRAY_BUFFER, gpuBuffer->size, gpuBuffer->memUsage == BufferUsage::DYNAMIC_DRAW ? gl.DYNAMIC_DRAW : gl.STATIC_DRAW);
    }
    else if (gpuBuffer->type == BufferType::INDEX)
    {

        GLGPUStateCache &cache = device->renderStateCache;
        if (cache.glVAO)
        {
            cache.glVAO = 0;
            gl.bindVertexArrayOES(0);
        }
        bindIndexBuffer(device, gpuBuffer->glBuffer);
        gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, gpuBuffer->size, gpuBuffer->memUsage == BufferUsage::DYNAMIC_DRAW ? gl.DYNAMIC_DRAW : gl.STATIC_DRAW);
    }
    else if (gpuBuffer->type == BufferType::UNIFORM)
    {
        if (gpuBuffer->buffer)
        {
            delete gpuBuffer->buffer;
        }
        gpuBuffer->buffer = new sqstd::Byte();
        gpuBuffer->buffer->resize(gpuBuffer->size);
    }
}

void WebGLCmdFuncUpdateBuffer(WebGLDevice *device, IGLGPUBuffer *gpuBuffer, char *data, int offset, int size)
{
    GLAPI::WebGL &gl = device->getGLContext();
    if (gpuBuffer->type == BufferType::UNIFORM)
    {

        gpuBuffer->buffer->clearData();
        if (offset > 0)
        {
            gpuBuffer->buffer->appendEmpty(offset);
        }
        gpuBuffer->buffer->append(data, size);
        // std::string datas;
        // charBufferToFloatString(gpuBuffer->buffer->m_pBuffer,16,datas);
        // printf("WebGLCmdFuncUpdateBuffer %s\n",datas.c_str());
    }
    else
    {
        GLGPUStateCache &cache = device->renderStateCache;
        if (cache.glVAO)
        {
            cache.glVAO = 0;
            gl.bindVertexArrayOES(0);
        }

        if (gpuBuffer->type == BufferType::INDEX)
        {
            bindIndexBuffer(device, gpuBuffer->glBuffer);
        }
        else
        {
            bindVertexBuffer(device, gpuBuffer);
        }
        // printf(">>>>>>>>>>>>>>>>>>>> %d %d\n",gpuBuffer->glTarget,gpuBuffer->glBuffer);
        gl.bufferSubData(gpuBuffer->glTarget, offset, data, size);
    }
}

void WebGLCmdFuncCreateShader(WebGLDevice *device, IGLGPUShader &gpuShader)
{
    BaseWebGLCmdFuncCreateShader(device->getGLContext(), device->renderStateCache, gpuShader);
}

void WebGLCmdFuncDestroyShader(WebGLDevice *device, IGLGPUShader &gpuShader)
{
    if (gpuShader.glProgram)
    {
        GLAPI::WebGL &gl = device->getGLContext();
        if (device->renderStateCache.glProgram == gpuShader.glProgram)
        {
            device->renderStateCache.glProgram = -1;
        }
        gl.deleteProgram(gpuShader.glProgram);
        gpuShader.glProgram = NULL;
    }
}

void applyUniform(
    WebGLDevice *device,
    WebGLPipelineState *pilelineState,
    std::vector<DescriptorSet *> &descriptorSets)
{
    IGLGPUShader &gpuShader = *pilelineState->gpuShader;
    int uniformNum = gpuShader.glActiveUniforms.size();
    if (uniformNum == 0)
        return;

    GLAPI::WebGL &gl = device->getGLContext();
    std::vector<const UniformBlock *> &blocks = *gpuShader.blocks;

    // printf("applyUniform uniformNum %d %d \n", uniformNum, blocks.size());

    for (int i = 0; i < uniformNum; ++i)
    {

        IGLGPUUniform &gpuUniform = gpuShader.glActiveUniforms[i];

        // printf("===== blsize %d index %d \n",blocks.size(), gpuUniform.blockIndex);

        SQ_ASSERT(gpuUniform.blockIndex <= blocks.size() - 1);

        const UniformBlock *block = blocks[gpuUniform.blockIndex];

        // printf("applyUniform block membersNumber: %d memberIndex %d\n", block->members.size(), gpuUniform.memberIndex);

        SQ_ASSERT(gpuUniform.memberIndex <= block->members.size() - 1);

        const Uniform &uniform = block->members[gpuUniform.memberIndex];

        SQ_ASSERT(block->set <= descriptorSets.size() - 1);

        DescriptorSet *descriptSet = descriptorSets[block->set];

        SQ_ASSERT(descriptSet);

        // printf("applyUniform descriptSet: %p blockbinding: %d blockset: %d uninformName: %s\n", descriptSet, block->binding, block->set, uniform.name.c_str());

        BufferObject *bufferObject = descriptSet->getBindingValue<BufferObject>(block->binding);

        SQ_ASSERT(bufferObject);

        WebGLBufferObject *bufferData = dynamic_cast<WebGLBufferObject *>(bufferObject);

        sqstd::Byte *buffer = nullptr;

        if (bufferData->bufferData.bufferView)
        {
            buffer = &bufferData->bufferData.bufferView->buffer;
        }
        else
        {
            buffer = bufferData->bufferData.buffer;
        }

        SQ_ASSERT(buffer);

        sqstd::Byte &dataCache = gpuUniform.cache->buffer;

        switch (uniform.type)
        {
        case gfx::Type::BOOL:
        case gfx::Type::INT:
        {
            int value = buffer->get<int>(gpuUniform.offset);
            if (gpuUniform.dataInit == false || dataCache.get<int>(0) != value)
            {
                gpuUniform.dataInit = true;
                dataCache.setValue(0, value);
                gl.uniform1i(gpuUniform.location, value);
            }
            break;
        }
        case gfx::Type::BOOL2:
        case gfx::Type::INT2:
        {
            int value1 = buffer->get<int>(gpuUniform.offset);
            int value2 = buffer->get<int>(gpuUniform.offset + 4);
            if (gpuUniform.dataInit == false || value1 != dataCache.get<int>(0) || dataCache.get<int>(4) != value2)
            {
                gpuUniform.dataInit = true;
                dataCache.setValue(0, value1);
                dataCache.setValue(4, value2);
                gl.uniform2i(
                    gpuUniform.location,
                    value1,
                    value2);
            }
            break;
        }
        case gfx::Type::BOOL3:
        case gfx::Type::INT3:
        {
            int value1 = buffer->get<int>(gpuUniform.offset);
            int value2 = buffer->get<int>(gpuUniform.offset + 4);
            int value3 = buffer->get<int>(gpuUniform.offset + 8);
            if (gpuUniform.dataInit == false || dataCache.get<int>(0) != value1 || dataCache.get<int>(4) != value2 || dataCache.get<int>(8) != value3)
            {
                gpuUniform.dataInit = true;
                dataCache.setValue(0, value1);
                dataCache.setValue(4, value2);
                dataCache.setValue(8, value3);
                gl.uniform3i(
                    gpuUniform.location,
                    value1,
                    value2,
                    value3);
            }
            break;
        }
        case gfx::Type::BOOL4:
        case gfx::Type::INT4:

        {
            int value1 = buffer->get<int>(gpuUniform.offset);
            int value2 = buffer->get<int>(gpuUniform.offset + 4);
            int value3 = buffer->get<int>(gpuUniform.offset + 8);
            int value4 = buffer->get<int>(gpuUniform.offset + 12);
            if (gpuUniform.dataInit == false || dataCache.get<int>(0) != value1 || dataCache.get<int>(4) != value2 || dataCache.get<int>(8) != value3 || dataCache.get<int>(12) != value4)
            {
                gpuUniform.dataInit = true;
                dataCache.setValue(0, value1);
                dataCache.setValue(4, value2);
                dataCache.setValue(8, value3);
                dataCache.setValue(12, value4);
                gl.uniform4i(
                    gpuUniform.location,
                    value1,
                    value2,
                    value3,
                    value4);
            }
            break;
        }

        case gfx::Type::FLOAT:
        {
            float value = buffer->get<float>(gpuUniform.offset);
            if (gpuUniform.dataInit == false || dataCache.get<float>(0) != value)
            {
                // printf("===== %f \n",buffer->get<float>(gpuUniform.offset));
                gpuUniform.dataInit = true;
                dataCache.setValue(0, value);
                gl.uniform1f(gpuUniform.location, value);
            }
            break;
        }
        case gfx::Type::FLOAT2:
        {
            float value = buffer->get<float>(gpuUniform.offset);
            float value2 = buffer->get<float>(gpuUniform.offset + 4);
            if (gpuUniform.dataInit == false || dataCache.get<float>(0) != value || dataCache.get<float>(4) != value2)
            {
                // printf("===== %f \n",buffer->get<float>(gpuUniform.offset));
                gpuUniform.dataInit = true;
                dataCache.setValue(0, value);
                dataCache.setValue(4, value2);
                gl.uniform2f(gpuUniform.location, value, value2);
            }
            break;
        }

        case gfx::Type::FLOAT3:

        {
            float value = buffer->get<float>(gpuUniform.offset);
            float value2 = buffer->get<float>(gpuUniform.offset + 4);
            float value3 = buffer->get<float>(gpuUniform.offset + 8);
            if (gpuUniform.dataInit == false || dataCache.get<float>(0) != value || dataCache.get<float>(4) != value2 || dataCache.get<float>(8) != value3)
            {
                // printf("===== %f \n",buffer->get<float>(gpuUniform.offset));
                gpuUniform.dataInit = true;
                dataCache.setValue(0, value);
                dataCache.setValue(4, value2);
                dataCache.setValue(8, value3);
                gl.uniform3f(gpuUniform.location, value, value2, value3);
            }
            break;
        }
        case gfx::Type::FLOAT4:
        {
            float value = buffer->get<float>(gpuUniform.offset);
            float value2 = buffer->get<float>(gpuUniform.offset + 4);
            float value3 = buffer->get<float>(gpuUniform.offset + 8);
            float value4 = buffer->get<float>(gpuUniform.offset + 12);
            if (gpuUniform.dataInit == false || dataCache.get<float>(0) != value || dataCache.get<float>(4) != value2 || dataCache.get<float>(8) != value3 || dataCache.get<float>(12) != value4)
            {
                // printf("===== %f \n",buffer->get<float>(gpuUniform.offset));
                gpuUniform.dataInit = true;
                dataCache.setValue(0, value);
                dataCache.setValue(4, value2);
                dataCache.setValue(8, value3);
                dataCache.setValue(12, value4);
                gl.uniform4f(gpuUniform.location, value, value2, value3, value4);
            }
            break;
        }
        case gfx::Type::MAT2:
        {
            bool dirty = gpuUniform.dataInit == false;
            gpuUniform.dataInit = true;

            int index = 0;
            for (int i = 0; i < 4; ++i)
            {
                float v = buffer->get<float>(gpuUniform.offset + index);
                float cV = dataCache.get<float>(index);
                if (v != cV)
                {
                    dirty = true;
                    dataCache.setValue(index, v);
                }
                index += 4;
            }

            if (dirty)
                gl.uniformMatrix2fv(gpuUniform.location, dataCache.getBuffer());
            break;
        }
        case gfx::Type::MAT3:
        {
            bool dirty = gpuUniform.dataInit == false;
            gpuUniform.dataInit = true;
            int index = 0;
            for (int i = 0; i < 9; ++i)
            {
                float v = buffer->get<float>(gpuUniform.offset + index);
                float cV = dataCache.get<float>(index);
                if (v != cV)
                {
                    dirty = true;
                    dataCache.setValue(index, v);
                }
                index += 4;
            }
            if (dirty)
            {
                gl.uniformMatrix3fv(gpuUniform.location, dataCache.getBuffer());
            }
            break;
        }
        case gfx::Type::MAT4:
        {
            bool dirty = gpuUniform.dataInit == false;
            gpuUniform.dataInit = true;
            int index = 0;

            // printf("fuck %s %p %p\n",uniform.name.c_str(), &gpuUniform,&dataCache);
            for (int i = 0; i < 16; ++i)
            {
                float v = buffer->get<float>(gpuUniform.offset + index);
                float cV = dataCache.get<float>(index);
                if (v != cV)
                {
                    // printf("%d %f %f %d \n", i, dataCache.get<float>(index), v, dataCache.getBuffSize());
                    dirty = true;
                    dataCache.setValue(index, v);

                    // printf("%d %p %f \n", i, dataCache.getBuffer(), dataCache.get<float>(index));
                }
                index += 4;
            }

            if (dirty)
            {
                if (bufferData->bufferData.sysToJsByCopy)
                {
                    gl.uniformMatrix4f(gpuUniform.location, dataCache.getBuffer());
                }
                else
                {
                    gl.uniformMatrix4fv(gpuUniform.location, buffer->getBuffer() + gpuUniform.offset);
                }
            }
            break;
        }
        }
    }
}

static void applyTextureParameter(WebGLDevice *device, IGLGPUTexture &gpuTexture, gfx::WebGLSamplerObject *sampler)
{
    GLAPI::WebGL &gl = device->getGLContext();
    GLAPI::GLenum glWrapS;
    GLAPI::GLenum glWrapT;
    GLAPI::GLenum glMinFilter;
    GLAPI::GLenum glMagFilter;

    switch (sampler->getWrapS())
    {
    case TextureWrapMode::REPEAT:
        glWrapS = gl.REPEAT;
        break;
    case TextureWrapMode::MIRROR_REPEAT:
        glWrapS = gl.MIRRORED_REPEAT;
        break;
    case TextureWrapMode::CLAMP_EDGE:
        glWrapS = gl.CLAMP_TO_EDGE;
        break;
    }

    switch (sampler->getWrapT())
    {
    case TextureWrapMode::REPEAT:
        glWrapT = gl.REPEAT;
        break;
    case TextureWrapMode::MIRROR_REPEAT:
        glWrapT = gl.MIRRORED_REPEAT;
        break;
    case TextureWrapMode::CLAMP_EDGE:
        glWrapT = gl.CLAMP_TO_EDGE;
        break;
    }

    switch (sampler->getMinFilter())
    {
    case TextureFilter::LINEAR:
        glMinFilter = gl.LINEAR;
        break;
    case TextureFilter::NEAREST:
        glMinFilter = gl.NEAREST;
        break;
    case TextureFilter::NEAREST_MIPMAP_NEAREST:
        glMinFilter = gl.NEAREST_MIPMAP_NEAREST;
        break;
    }

    switch (sampler->getMagFilter())
    {
    case TextureFilter::LINEAR:
        glMagFilter = gl.LINEAR;
        break;
    case TextureFilter::NEAREST:
        glMagFilter = gl.NEAREST;
        break;
    }

    // printf("================min filter %d %d %d %d\n",glWrapS,glWrapT,glMagFilter,glMinFilter);

    gl.texParameteri(gpuTexture.glTarget, gl.TEXTURE_WRAP_S, glWrapS);
    gl.texParameteri(gpuTexture.glTarget, gl.TEXTURE_WRAP_T, glWrapT);
    gl.texParameteri(gpuTexture.glTarget, gl.TEXTURE_MIN_FILTER, glMinFilter);
    gl.texParameteri(gpuTexture.glTarget, gl.TEXTURE_MAG_FILTER, glMagFilter);
}

void applySamplerTextures(
    WebGLDevice *device,
    WebGLPipelineState *pilelineState,
    std::vector<DescriptorSet *> &descriptorSets)
{
    IGLGPUShader &gpuShader = *pilelineState->gpuShader;
    int samplerNum = gpuShader.glSamplerTextures.size();
    GLAPI::WebGL &gl = device->getGLContext();

    // printf("------textures samplerNum %d \n",samplerNum);

    if (samplerNum == 0)
        return;

    GLGPUStateCache &cache = device->renderStateCache;

    int texUnit = 0;

    for (int i = 0; i < samplerNum; ++i)
    {

        IGLGPUUniformSamplerTexture &gpuSampler = gpuShader.glSamplerTextures[i];

        SQ_ASSERT(gpuSampler.index <= gpuShader.samplerTextures->size() - 1);
        const UniformSamplerTexture &textureInfo = *(*gpuShader.samplerTextures)[gpuSampler.index];

        SQ_ASSERT(textureInfo.set <= descriptorSets.size() - 1);
        DescriptorSet *descriptSet = descriptorSets[textureInfo.set];

        int numTexture = textureInfo.count <= 0 ? 1 : textureInfo.count;

        // printf("textures info %d %d descriptSet: %p binding %d name: %s \n",textureInfo.set,numTexture,descriptSet,textureInfo.binding,textureInfo.name.c_str());

        // 支持纹理数组
        for (int c = 0; c < numTexture; ++c)
        {

            //  printf("_______________check texture %d %d %d\n",textureInfo.binding,c,descriptSet->hasValue(textureInfo.binding, c));
            if (!descriptSet->hasValue(textureInfo.binding, c))
            {

#if ENABLE_ERROR_CHECK
                if (c == 0)
                {
                    std::string erroMsg = "Active Texture No value ";
                    erroMsg.append(textureInfo.name);
                    ERROR(true, erroMsg.c_str());
                }
#endif
                break;
            }

            // printf("????????????????? %p \n",descriptSet->getBindingValue<gfx::Texture>(textureInfo.binding, c));

            WebGLTexture *texture = dynamic_cast<WebGLTexture *>(descriptSet->getBindingValue<gfx::Texture>(textureInfo.binding, c));
            IGLGPUTexture &gpuTexture = texture->getGPUTexture();
            gfx::WebGLSamplerObject *sampler = static_cast<gfx::WebGLSamplerObject *>(descriptSet->getBindingSamplerObject(textureInfo.binding, c));
            texUnit = gpuSampler.texUnits[c];
            bindTexture(device, texUnit, gpuTexture.glTarget, gpuTexture.glTexture);
            if (gpuTexture.cacheBindSampler == nullptr || (gpuTexture.cacheBindSampler->hash != sampler->getHash()))
            {
                gpuTexture.cacheBindSampler = &sampler->gpuSampler;
                gpuTexture.cacheBindSampler->hash = sampler->getHash();
                applyTextureParameter(device, gpuTexture, sampler);
            }
        }
    }
}

void applyAttributes2(WebGLDevice *device, WebGLPipelineState *pilelineState)
{
    WebGLInputAssembler *gpuInputAssembler = static_cast<WebGLInputAssembler *>(pilelineState->assembler);
    IGLGPUShader &gpuShader = *pilelineState->gpuShader;
    GLAPI::WebGL &gl = device->getGLContext();

    for (int i = 0; i < gpuShader.glActiveAttributes.size(); ++i)
    {
        const IGLGPUAttribute &gpuAttribute = gpuShader.glActiveAttributes[i];
        const Attribute *assetAttribute = nullptr;

        // printf("for find %d %s,%s\n", c, iGpuAttribute.attribute->name.c_str(), attr.name.c_str());

        for (int v = 0; v < gpuInputAssembler->vertexBuffers.size(); ++v)
        {
            WebGLBufferObject *bufferObj = dynamic_cast<WebGLBufferObject *>(gpuInputAssembler->vertexBuffers[v]);
            const std::vector<Attribute> &attributes = gpuInputAssembler->attrs[v];

            for (int a = 0; a < attributes.size(); ++a)
            {
                if (gpuAttribute.attribute->name == attributes[a].name)
                {
                    assetAttribute = &attributes[a];
                    bindVertexBuffer(device, &bufferObj->bufferData);
                    break;
                }
            }

            if (assetAttribute)
                break;
        }

        ERR_FAIL_NULL_V_MSG(assetAttribute, gpuAttribute.attribute->name.c_str());

        if (assetAttribute)
        {
            gl.enableVertexAttribArray(gpuAttribute.location);
            // printf("_____________ %d %d %d %d %d\n",gpuAttribute.location,gpuAttribute.count,gpuAttribute.type,gpuAttribute.stride,gpuAttribute.offset);

            if (assetAttribute->custom.has_value())
            {
                // 外部自定义的设置，OpenGL允许着色器中声明的变量类型和设置的类型不一致

                gl.vertexAttribPointer(
                    gpuAttribute.location,
                    (GLAPI::GLint)assetAttribute->custom->count,
                    toNumber(assetAttribute->custom->type),
                    assetAttribute->custom->stride,
                    (GLAPI::GLsizei)assetAttribute->custom->offset);

                if (assetAttribute->custom->instanceStride > 0)
                {
                    gl.vertexAttribDivisor(gpuAttribute.location, assetAttribute->custom->instanceStride);
                }
            }
            else
            {
                gl.vertexAttribPointer(
                    gpuAttribute.location,
                    (GLAPI::GLint)gpuAttribute.count,
                    toNumber(gpuAttribute.type),
                    gpuAttribute.stride,
                    (GLAPI::GLsizei)gpuAttribute.offset);
            }
        }
    }

    if (gpuInputAssembler->indexBuffer)
    {
        WebGLBufferObject *indexObj = dynamic_cast<WebGLBufferObject *>(gpuInputAssembler->indexBuffer);
        bindIndexBuffer(device, indexObj->bufferData.glBuffer);
    }
}

void applyAttributes(WebGLDevice *device, WebGLPipelineState *pilelineState)
{

    WebGLInputAssembler *gpuInputAssembler = static_cast<WebGLInputAssembler *>(pilelineState->assembler);
    GLGPUStateCache &cache = device->renderStateCache;
    GLAPI::WebGL &gl = device->getGLContext();
    if (device->deviceCaps->enableVAO)
    {
        IGLGPUShader &gpuShader = *pilelineState->gpuShader;
        std::unordered_map<GLAPI::GLProgram, GLAPI::GLVAO>::iterator it = gpuInputAssembler->shaderToVAOMap.find(gpuShader.glProgram);

        if (it == gpuInputAssembler->shaderToVAOMap.end())
        {
            GLAPI::GLVAO vao = gl.createVertexArrayOES();
            gpuInputAssembler->shaderToVAOMap[gpuShader.glProgram] = vao;
            it = gpuInputAssembler->shaderToVAOMap.find(gpuShader.glProgram);
            gpuInputAssembler->dirty = false;

            gl.bindVertexArrayOES(vao);

            // 对VAO设置时，必要要求重新绑定所有的缓冲区，就算当前这个缓冲区在激活的状态也需要重新绑定
            // 否则这个缓冲区无法设置到vao中
            cache.glArrayBuffer = 0;
            cache.glElementArrayBuffer = 0;
            applyAttributes2(device, pilelineState);

            // 这里一定一定记得要解绑啊，否则后续的OpenGL命令不小心绑到到这个VAO上了
            // 造成当前这个VAO绑定的数据错乱
            gl.bindVertexArrayOES(0);
            cache.glVAO = 0;

            gl.bindBuffer(gl.ARRAY_BUFFER, 0);
            cache.glArrayBuffer = 0;

            gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, 0);
            cache.glElementArrayBuffer = 0;
        }

        if (cache.glVAO != it->second)
        {
            gl.bindVertexArrayOES(it->second);
            cache.glVAO = it->second;
        }
    }
    else
    {
        applyAttributes2(device, pilelineState);
    }
}

void applyState(WebGLDevice *device, WebGLPipelineState *pilelineState)
{
    GLAPI::WebGL &gl = device->getGLContext();
    GLGPUStateCache &cache = device->renderStateCache;
    if (pilelineState->rasterizerState)
    {
        RasterizerState &rs = *pilelineState->rasterizerState;
        if (cache.rs.cullMode != rs.cullMode)
        {
            switch (rs.cullMode)
            {
            case CullMode::NONE:
            {
                gl.disable(gl.CULL_FACE);
                break;
            }
            case CullMode::FRONT:
            {
                gl.enable(gl.CULL_FACE);
                gl.cullFace(gl.FRONT);
                break;
            }
            case CullMode::BACK:
            {
                gl.enable(gl.CULL_FACE);
                gl.cullFace(gl.BACK);
                break;
            }
            }
            cache.rs.cullMode = rs.cullMode;
        }

        if (cache.rs.isFrontFaceCCW != rs.isFrontFaceCCW)
        {
            gl.frontFace(rs.isFrontFaceCCW ? gl.CCW : gl.CW);
            cache.rs.isFrontFaceCCW = rs.isFrontFaceCCW;
        }

        if ((cache.rs.depthBias != rs.depthBias) || (cache.rs.depthBiasSlop != rs.depthBiasSlop))
        {
            gl.polygonOffset(rs.depthBias, rs.depthBiasSlop);
            cache.rs.depthBias = rs.depthBias;
            cache.rs.depthBiasSlop = rs.depthBiasSlop;
        }

        if (cache.rs.lineWidth != rs.lineWidth)
        {
            gl.lineWidth(rs.lineWidth);
            cache.rs.lineWidth = rs.lineWidth;
        }
    }

    if (pilelineState->depthState)
    {
        DepthState &ds = *pilelineState->depthState;
        if (cache.ds.depthTest != ds.depthTest)
        {
            if (ds.depthTest)
            {
                gl.enable(gl.DEPTH_TEST);
            }
            else
            {
                gl.disable(gl.DEPTH_TEST);
            }
            cache.ds.depthTest = ds.depthTest;
        }

        if (ds.depthTest)
        {
            if (cache.ds.depthWrite != ds.depthWrite)
            {
                GLAPI::GLboolean b = (GLAPI::GLboolean)ds.depthWrite;
                gl.depthMask(b);
                cache.ds.depthWrite = ds.depthWrite;
            }

            if (cache.ds.depthFunc != ds.depthFunc)
            {
                GLAPI::GLenum func = (GLAPI::GLenum)ds.depthFunc;
                gl.depthFunc(func);
                cache.ds.depthFunc = ds.depthFunc;
            }
        }
    }

    if (pilelineState->stencilState)
    {
        StencilState &ss = *pilelineState->stencilState;

        bool enable = ss.stencilTestFront || ss.stencilTestBack;
        if ((cache.ss.stencilTestFront != ss.stencilTestFront) || (cache.ss.stencilTestBack != ss.stencilTestBack))
        {
            if (enable)
            {
                gl.enable(gl.STENCIL_TEST);
            }
            else
            {
                gl.disable(gl.STENCIL_TEST);
            }
            cache.ss.stencilTestFront = ss.stencilTestFront;
            cache.ss.stencilTestBack = ss.stencilTestBack;
        }

        if (enable)
        {
            if ((cache.ss.stencilFuncFront != ss.stencilFuncFront) || (cache.ss.stencilRefFront != ss.stencilRefFront) || (cache.ss.stencilReadMaskFront != ss.stencilReadMaskFront))
            {
                gl.stencilFuncSeparate(
                    gl.FRONT,
                    toNumber(ss.stencilFuncFront),
                    ss.stencilRefFront,
                    ss.stencilReadMaskFront);

                cache.ss.stencilFuncFront = ss.stencilFuncFront;
                cache.ss.stencilRefFront = ss.stencilRefFront;
                cache.ss.stencilReadMaskFront = ss.stencilReadMaskFront;
            }

            if ((cache.ss.stencilFailOpFront != ss.stencilFailOpFront) || (cache.ss.stencilZFailOpFront != ss.stencilZFailOpFront) || (cache.ss.stencilPassOpFront != ss.stencilPassOpFront))
            {
                gl.stencilOpSeparate(
                    gl.FRONT,
                    toNumber(ss.stencilFailOpFront),
                    toNumber(ss.stencilZFailOpFront),
                    toNumber(ss.stencilPassOpFront));

                cache.ss.stencilFailOpFront = ss.stencilFailOpFront;
                cache.ss.stencilZFailOpFront = ss.stencilZFailOpFront;
                cache.ss.stencilPassOpFront = ss.stencilPassOpFront;
            }

            if (cache.ss.stencilWriteMaskFront != ss.stencilWriteMaskFront)
            {
                gl.stencilMaskSeparate(gl.FRONT, ss.stencilWriteMaskFront);
                cache.ss.stencilWriteMaskFront = ss.stencilWriteMaskFront;
            }

            // back
            if ((cache.ss.stencilFuncBack != ss.stencilFuncBack) || (cache.ss.stencilRefBack != ss.stencilRefBack) || (cache.ss.stencilReadMaskBack != ss.stencilReadMaskBack))
            {
                gl.stencilFuncSeparate(
                    gl.BACK,
                    toNumber(ss.stencilFuncBack),
                    ss.stencilRefBack,
                    ss.stencilReadMaskBack);

                cache.ss.stencilFuncBack = ss.stencilFuncBack;
                cache.ss.stencilRefBack = ss.stencilRefBack;
                cache.ss.stencilReadMaskBack = ss.stencilReadMaskBack;
            }

            if ((cache.ss.stencilFailOpBack != ss.stencilFailOpBack) || (cache.ss.stencilZFailOpBack != ss.stencilZFailOpBack) || (cache.ss.stencilPassOpBack != ss.stencilPassOpBack))
            {
                gl.stencilOpSeparate(
                    gl.BACK,
                    toNumber(ss.stencilFailOpBack),
                    toNumber(ss.stencilZFailOpBack),
                    toNumber(ss.stencilPassOpBack));

                cache.ss.stencilFailOpBack = ss.stencilFailOpBack;
                cache.ss.stencilZFailOpBack = ss.stencilZFailOpBack;
                cache.ss.stencilPassOpBack = ss.stencilPassOpBack;
            }

            if (cache.ss.stencilWriteMaskBack != ss.stencilWriteMaskBack)
            {
                gl.stencilMaskSeparate(gl.BACK, ss.stencilWriteMaskBack);
                cache.ss.stencilWriteMaskBack = ss.stencilWriteMaskBack;
            }
        }
    }

    if (pilelineState->blendState)
    {
        BlendState &bs = *pilelineState->blendState;
        BlendTarget &cacheTarget = cache.bs.blendTarget;
        BlendTarget &bsTarget = bs.blendTarget;

        if (cacheTarget.blend != bsTarget.blend)
        {
            if (bsTarget.blend)
            {
                gl.enable(gl.BLEND);
            }
            else
            {
                gl.disable(gl.BLEND);
            }
            cacheTarget.blend = bsTarget.blend;
        }

        if (bsTarget.blend)
        {
            if ((cache.bs.blendColor.r != bs.blendColor.r) || (cache.bs.blendColor.g != bs.blendColor.g) || (cache.bs.blendColor.b != bs.blendColor.b) || (cache.bs.blendColor.a != bs.blendColor.a))
            {
                gl.blendColor(bs.blendColor.r, bs.blendColor.g, bs.blendColor.b, bs.blendColor.a);

                cache.bs.blendColor.r = bs.blendColor.r;
                cache.bs.blendColor.g = bs.blendColor.g;
                cache.bs.blendColor.b = bs.blendColor.b;
                cache.bs.blendColor.a = bs.blendColor.a;
            }

            if ((cacheTarget.blendEq != bsTarget.blendEq) || (cacheTarget.blendAlphaEq != bsTarget.blendAlphaEq))
            {
                gl.blendEquationSeparate(blendOpToGLenum(gl, bsTarget.blendEq), blendOpToGLenum(gl, bsTarget.blendAlphaEq));
                cacheTarget.blendEq = bsTarget.blendEq;
                cacheTarget.blendAlphaEq = bsTarget.blendAlphaEq;
            }

            if ((cacheTarget.blendSrc != bsTarget.blendSrc) || (cacheTarget.blendDst != bsTarget.blendDst) || (cacheTarget.blendSrcAlpha != bsTarget.blendSrcAlpha) || (cacheTarget.blendDstAlpha != bsTarget.blendDstAlpha))
            {

                gl.blendFuncSeparate(
                    blendFractorToGLEnum(gl, bsTarget.blendSrc),
                    blendFractorToGLEnum(gl, bsTarget.blendDst),
                    blendFractorToGLEnum(gl, bsTarget.blendSrcAlpha),
                    blendFractorToGLEnum(gl, bsTarget.blendDstAlpha));

                cacheTarget.blendSrc = bsTarget.blendSrc;
                cacheTarget.blendDst = bsTarget.blendDst;
                cacheTarget.blendSrcAlpha = bsTarget.blendSrcAlpha;
                cacheTarget.blendDstAlpha = bsTarget.blendDstAlpha;
            }
        }

        if (cacheTarget.blendColorMask != bsTarget.blendColorMask)
        {
            unsigned int mask = bsTarget.blendColorMask;
            gl.colorMask(
                (mask & toNumber(gfx::ColorMask::R)),
                (mask & toNumber(gfx::ColorMask::G)),
                (mask & toNumber(gfx::ColorMask::B)),
                (mask & toNumber(gfx::ColorMask::A)));
            cacheTarget.blendColorMask = bsTarget.blendColorMask;
        }
    }
}

void WebGLCmdFuncBindStates(
    WebGLDevice *device,
    WebGLPipelineState *pilelineState,
    std::vector<DescriptorSet *> &descriptorSets)
{
    GLGPUStateCache &cache = device->renderStateCache;
    cache.primitive = pilelineState->primitive;
    IGLGPUShader &gpuShader = *pilelineState->gpuShader;
    GLAPI::GLProgram &program = gpuShader.glProgram;
    if (cache.glProgram != program)
    {
        GLAPI::WebGL &gl = device->getGLContext();
        gl.useProgram(program);
        cache.glProgram = program;
    }

    applyState(device, pilelineState);
    applyUniform(device, pilelineState, descriptorSets);
    applySamplerTextures(device, pilelineState, descriptorSets);
    applyAttributes(device, pilelineState);
}

void WebGLCmdFuncCreateTexture(WebGLDevice *device, IGLGPUTexture &gpuTexture)
{
    GLAPI::WebGL &gl = device->getGLContext();
    GLGPUStateCache &cache = device->renderStateCache;
    switch (gpuTexture.type)
    {
    case TextureType::TEX2D:
    {

        // 如果是这种类型的格式，表示意图是将FBO的深度缓冲区或模板缓冲区存储到一张纹理中
        // 在WebGL1.0中，如果支持WEBGL_depth_texture扩展，则FBO可以自动将深度值写入到纹理,不需要额外创建RBO
        // 但是如果不支持，我们需要创建RBO。
        if ((gpuTexture.glFormat == Format::DEPTH || gpuTexture.glFormat == Format::DEPTH_STENCIL) &&
            !dynamic_cast<WebGLDeviceCaps *>(device->deviceCaps)->WEBGL_depth_texture)
        {
            gpuTexture.glRenderbuffer = gl.createRenderbuffer();
            if (gpuTexture.width > 0 && gpuTexture.height > 0)
            {
                gl.bindRenderbuffer(gl.RENDERBUFFER, gpuTexture.glRenderbuffer);
                cache.glRenderbuffer = gpuTexture.glRenderbuffer;
                gl.renderbufferStorage(gl.RENDERBUFFER, toNumber(gpuTexture.glFormat), gpuTexture.width, gpuTexture.height);
            }
            return;
        }

        gpuTexture.glTexture = gl.createTexture();
        gpuTexture.glTarget = gl.TEXTURE_2D;

        // applyTextureParameter(device, gpuTexture);

        int w = gpuTexture.width;
        int h = gpuTexture.height;
        if (w <= 0 || h <= 0)
        {
            return;
        }

        bindTexture(device, 0, gpuTexture.glTarget, gpuTexture.glTexture);

        // printf("WebGLCmdFuncCreateTexture %d %d\n", gpuTexture.glTexture, gpuTexture.glType);

        // if (FormatInfos[gpuTexture.format].isCompressed) {
        //      for (let i = 0; i < gpuTexture.mipLevel; ++i) {
        //             const imgSize = FormatSize(gpuTexture.format, w, h, 1);
        //             const view: Uint8Array = new Uint8Array(imgSize);
        //             gl.compressedTexImage2D(gl.TEXTURE_2D, i, gpuTexture.glInternalFmt, w, h, 0, view);
        //             w = Math.max(1, w >> 1);
        //             h = Math.max(1, h >> 1);
        //     }
        // } else {

        for (int i = 0; i < gpuTexture.mipLevel; ++i)
        {
            gl.texImage2D(gl.TEXTURE_2D, i, toNumber(gpuTexture.glInternalFmt), w, h, 0, toNumber(gpuTexture.glFormat), toNumber(gpuTexture.glType), NULL);
            // w = max(1, w >> 1);
            // h = max(1, h >> 1);
        }
        // }
    }
    break;
    }
}

void WebGLCmdFuncDestroyTexture(WebGLDevice *device, IGLGPUTexture &gpuTexture)
{
    GLAPI::WebGL &gl = device->getGLContext();
    GLGPUStateCache &cache = device->renderStateCache;
    if (gpuTexture.glTexture)
    {

        // 清空绑定到的纹理单位
        for (int i = 0; i < cache.texUintBind.getCount(); ++i)
        {
            if (cache.texUintBind[i] == gpuTexture.glTexture)
            {
                bindTexture(device, i, gpuTexture.glTarget, 0);
                break;
            }
        }
        gl.deleteTexture(gpuTexture.glTexture);
        gpuTexture.glTexture = 0;
    }

    if (gpuTexture.glRenderbuffer)
    {
        GLAPI::GLRenderbuffer glRenderbuffer = cache.glRenderbuffer;
        gl.deleteRenderbuffer(gpuTexture.glRenderbuffer);
        if (glRenderbuffer == gpuTexture.glRenderbuffer)
        {
            gl.bindRenderbuffer(gl.RENDERBUFFER, NULL);
            glRenderbuffer = NULL;
        }
        gpuTexture.glRenderbuffer = NULL;
    }
}

void WebGLCmdFuncResizeTexture(WebGLDevice *device, IGLGPUTexture &gpuTexture)
{
    GLAPI::WebGL &gl = device->getGLContext();
    GLGPUStateCache &cache = device->renderStateCache;

    if (gpuTexture.glRenderbuffer)
    {
        if (gpuTexture.glRenderbuffer != cache.glRenderbuffer)
        {
            gl.bindRenderbuffer(gl.RENDERBUFFER, gpuTexture.glRenderbuffer);
            cache.glRenderbuffer = gpuTexture.glRenderbuffer;
        }
        gl.renderbufferStorage(gl.RENDERBUFFER, toNumber(gpuTexture.glFormat), gpuTexture.width, gpuTexture.height);
        return;
    }

    bindTexture(device, 0, gpuTexture.glTarget, gpuTexture.glTexture);

    if (gpuTexture.glTarget == gl.TEXTURE_2D)
    {
        for (int i = 0; i < gpuTexture.mipLevel; ++i)
        {
            gl.texImage2D(gl.TEXTURE_2D, i, toNumber(gpuTexture.glInternalFmt),
                          gpuTexture.width, gpuTexture.height, 0, toNumber(gpuTexture.glFormat), toNumber(gpuTexture.glType), NULL);
        }
    }
}

void WebGLCmdFuncCopyTexImagesToTexture(WebGLDevice *device, int imgAssetId, IGLGPUTexture &gpuTexture)
{
    GLAPI::WebGL &gl = device->getGLContext();
    GLGPUStateCache &cache = device->renderStateCache;

    bindTexture(device, 0, gpuTexture.glTarget, gpuTexture.glTexture);

    if (gpuTexture.glTarget == gl.TEXTURE_2D)
    {

        gl.texSubImage2D(
            gl.TEXTURE_2D,
            0,
            0,
            0,
            toNumber(gpuTexture.glFormat),
            toNumber(gpuTexture.glType),
            imgAssetId);
    }
}

void WebGCmdFuncCopyBuffersToTexture(WebGLDevice *device, const unsigned char *buffer, IGLGPUTexture &gpuTexture, BufferTextureCopyRegion &region)
{
    GLAPI::WebGL &gl = device->getGLContext();
    GLGPUStateCache &cache = device->renderStateCache;
    bindTexture(device, 0, gpuTexture.glTarget, gpuTexture.glTexture);

    // 如果外部传递进来的偏移不是4字节对齐的，那么确保偏移量x和y是4字节对齐，因为纹理存储都是按照4字节对齐的
    int alignmentWidth, alignmentHeight;
    formatAlignment(gpuTexture.glFormat, alignmentWidth, alignmentHeight);

    int offsetX = region.texOffsetX == 0 ? 0 : alignTo(region.texOffsetX, alignmentWidth);
    int offsetY = region.texOffsetY == 0 ? 0 : alignTo(region.texOffsetY, alignmentHeight);

    // 确保宽高也是4字节对齐的情况下的大小
    int destWidth = region.texWidth <= alignmentWidth ? region.texWidth : alignTo(region.texWidth, alignmentWidth);
    int destHeight = region.texHeight <= alignmentHeight ? region.texHeight : alignTo(region.texHeight, alignmentHeight);
    destWidth = (region.texWidth + offsetX) == gpuTexture.width ? region.texWidth : destWidth;
    destHeight = (region.texHeight + offsetY) == gpuTexture.height ? region.texHeight : destHeight;

    int texSize = formatTexSize(gpuTexture.glFormat, destWidth, destHeight, 1);

    // printf("WebGCmdFuncCopyBuffersToTexture %d %d %d %d %d %d %d\n", offsetX,offsetY,destWidth,destHeight,texSize,gpuTexture.glFormat, gpuTexture.glType);

    if (gpuTexture.glTarget == gl.TEXTURE_2D)
    {
        gl.texSubImage2D(
            gl.TEXTURE_2D,
            0,
            offsetX,
            offsetY,
            destWidth,
            destHeight,
            toNumber(gpuTexture.glFormat),
            toNumber(gpuTexture.glType),
            (unsigned char *)buffer,
            texSize,
            region.buffCopyToNative);
    }
}

void WebGLCmdFuncDraw(WebGLDevice *device, const gfx::InputDraw &drawInfo)
{
    GLAPI::WebGL &gl = device->getGLContext();
    GLAPI::GLenum type;
    int iStride;
    switch (drawInfo.glIndexType)
    {
    case gfx::Type::UINT2:
        type = gl.UNSIGNED_SHORT;
        iStride = 2;
        break;
    default:
        break;
    }

    // printf("WebGLCmdFuncDraw %d %d\n",drawInfo.instanceCount,drawInfo.indexCount);

    // 转换为字节单位表示
    int offset = drawInfo.firstIndex * iStride;
    if (drawInfo.instanceCount > 0)
    {

        gl.drawElementsInstancedANGLE(
            toNumber(device->renderStateCache.primitive),
            drawInfo.indexCount,
            type,
            offset,
            drawInfo.instanceCount);
        // printf("drawElementsInstancedANGLE\n");
    }
    else if (drawInfo.indexCount > 0)
    {
        gl.drawElements(toNumber(device->renderStateCache.primitive), drawInfo.indexCount, type, offset);
        // printf("drawElements\n");
    }
    else if (drawInfo.vertexCount > 0)
    {
    }

    // 注意这里啊，当前的DrawCall VAO使用之后必须解绑
    // 避免有下一个DrawCall的状态绑定到当前的这个VAO上
    GLGPUStateCache &cache = device->renderStateCache;
    if (cache.glVAO)
    {
        gl.bindVertexArrayOES(0);
        cache.glVAO = 0;
    }
}

void WebGLCmdFuncDestroyInputAssembler(WebGLDevice *device, WebGLInputAssembler *assembler)
{
    GLAPI::WebGL &gl = device->getGLContext();
    GLGPUStateCache &cache = device->renderStateCache;

    std::unordered_map<GLAPI::GLProgram, GLAPI::GLVAO>::iterator it = assembler->shaderToVAOMap.begin();
    while (it != assembler->shaderToVAOMap.end())
    {
        gl.deleteVertexArrayOES(it->second);
        if (cache.glVAO == it->second)
        {
            gl.bindVertexArrayOES(0);
            cache.glVAO = 0;
        }
        ++it;
    }
}

void WebGLCmdFuncCreateFramebuffer(WebGLDevice *device, IGLGPUFrameBuffer &gpuFramebuffer)
{
    BaseWebGLCmdFuncCreateFramebuffer(device->getGLContext(), device->renderStateCache, gpuFramebuffer);
}

void WebGLCmdFuncDestroyFramebuffer(WebGLDevice *device, IGLGPUFrameBuffer &gpuFramebuffer)
{
    BaseWebGLCmdFuncDestroyFramebuffer(device->getGLContext(), device->renderStateCache, gpuFramebuffer);
}