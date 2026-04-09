#pragma once
#include "BaseWebGLAPI.h"
#include "BaseWebGLGPUObjectDefine.h"

GLAPI::GLenum blendFractorToGLEnum(const GLAPI::BaseWebGLAPI &gl, gfx::BlendFactor);
GLAPI::GLenum blendOpToGLenum(const GLAPI::BaseWebGLAPI &gl, gfx::BlendOp);
void BaseWebGLCmdFuncCreateShader(GLAPI::BaseWebGLAPI &gl, GLGPUStateCache &cache, IGLGPUShader &gpuShader);
void BaseWebGLCmdFuncCreateFramebuffer(GLAPI::BaseWebGLAPI &gl, GLGPUStateCache &cache, IGLGPUFrameBuffer &gpuFramebuffer);
void BaseWebGLCmdFuncDestroyFramebuffer(GLAPI::BaseWebGLAPI &gl, GLGPUStateCache &cache, IGLGPUFrameBuffer &gpuFramebuffer);
void BaseWebGLCmdUpdateBufferObject(GLAPI::BaseWebGLAPI &gl, GLAPI::GLenum target, int nativeId);