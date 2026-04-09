#ifndef _WEBGL_COMMAND_H_
#define _WEBGL_COMMAND_H_
#include "WebGLDevice.h"
#include "../../core/common/ArrayBuffer.h"
#include "WebGLGPUObjectDefine.h"
#include "../../core/math/Rect.h"
#include "../../core/math/Color.h"
#include "WebGLInputAssembler.h"
#include "WebGLRenderPass.h"

void WebGLCmdFuncBeginRenderPass(WebGLDevice *device,gfx::WebGLRenderPass* renderPass,Rect<float>& renderArea,Color& clearColors, float clearDepth, int clearStencil);
void WebGLCmdFuncEndRenderPass();

/**
 * 创建Buffer Object
*/
void WebGLCmdFuncCreateBuffer(WebGLDevice*, IGLGPUBuffer*);
void WebGLCmdFuncResizeBuffer(WebGLDevice*, IGLGPUBuffer*,int size);
void WebGLCmdFuncUpdateBuffer(WebGLDevice*, IGLGPUBuffer*, char*,int offset,int size);
void WebGLCmdFuncCreateShader(WebGLDevice*, IGLGPUShader&);
void WebGLCmdFuncDestroyShader(WebGLDevice*, IGLGPUShader&);
void WebGLCmdFuncCreateTexture(WebGLDevice*, IGLGPUTexture&);
void WebGLCmdFuncDestroyTexture (WebGLDevice*, IGLGPUTexture&);
void WebGLCmdFuncResizeTexture(WebGLDevice*, IGLGPUTexture &gpuTexture);
void WebGLCmdFuncCopyTexImagesToTexture (WebGLDevice*,int,IGLGPUTexture&);
void WebGCmdFuncCopyBuffersToTexture(WebGLDevice*,const unsigned char *buffer,IGLGPUTexture &gpuTexture,BufferTextureCopyRegion&);
void WebGLCmdFuncBindStates(WebGLDevice*,WebGLPipelineState*,std::vector<DescriptorSet*>&);
void WebGLCmdFuncDraw (WebGLDevice*,const gfx::InputDraw&);
void WebGLCmdFuncDestroyInputAssembler(WebGLDevice*,WebGLInputAssembler*);
void WebGLCmdFuncCreateFramebuffer (WebGLDevice* device, IGLGPUFrameBuffer& gpuFramebuffer);
void WebGLCmdFuncDestroyFramebuffer (WebGLDevice* device, IGLGPUFrameBuffer& gpuFramebuffer);
#endif