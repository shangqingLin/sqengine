#ifndef _WEBGL2_COMMAND_H_
#define _WEBGL2_COMMAND_H_
#include "WebGL2Device.h"
#include "../../core/common/ArrayBuffer.h"
#include "WebGL2GPUObjectDefine.h"
#include "../../core/math/Rect.h"
#include "../../core/math/Color.h"
#include "WebGL2InputAssembler.h"
#include "WebGL2PipelineState.h"
#include "WebGL2RenderPass.h"

void WebGL2CmdFuncBeginRenderPass(WebGL2Device *device,gfx::WebGL2RenderPass*,Rect<float>& renderArea,Color& clearColors, float clearDepth, int clearStencil);
void WebGL2CmdFuncEndRenderPass();

/**
 * 创建Buffer Object
*/
void WebGL2CmdFuncCreateBuffer(WebGL2Device*, IGLGPUBuffer*);
void WebGL2CmdFuncResizeBuffer(WebGL2Device*, IGLGPUBuffer*,int size);
void WebGL2CmdFuncUpdateBuffer(WebGL2Device*, IGLGPUBuffer*, char*,int offset,int size);
void WebGL2CmdFuncCreateShader(WebGL2Device*, IGLGPUShader&);
void WebGL2CmdFuncDestroyShader(WebGL2Device*, IGLGPUShader&);
void WebGL2CmdFuncCreateTexture(WebGL2Device*, IGLGPUTexture&);
void WebGL2CmdFuncDestroyTexture (WebGL2Device*, IGLGPUTexture&);
void WebGL2CmdFuncResizeTexture(WebGL2Device*, IGLGPUTexture &gpuTexture);
void WebGL2CmdFuncCopyTexImagesToTexture (WebGL2Device*,int,IGLGPUTexture&);
void WebGL2CmdFuncCopyBuffersToTexture(WebGL2Device*,const unsigned char *buffer,IGLGPUTexture &gpuTexture,BufferTextureCopyRegion&);
void WebGL2CmdFuncBindStates(WebGL2Device*,WebGL2PipelineState*,std::vector<DescriptorSet*>&);
void WebGL2CmdFuncDraw(WebGL2Device*,const gfx::InputDraw&);
void WebGL2CmdFuncDestroyInputAssembler(WebGL2Device*,WebGL2InputAssembler*);
void WebGL2CmdFuncCreateFramebuffer (WebGL2Device* device, IGLGPUFrameBuffer& gpuFramebuffer);
void WebGL2CmdFuncDestroyFramebuffer (WebGL2Device* device, IGLGPUFrameBuffer& gpuFramebuffer);
#endif