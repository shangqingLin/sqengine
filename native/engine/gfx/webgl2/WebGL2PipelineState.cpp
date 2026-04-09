#include "WebGL2PipelineState.h"
#include "WebGL2Shader.h"

void WebGL2PipelineState::setPipelineInfo(IPipelineStateInfo& info){
    blendState = info.blendState;
    depthState = info.depthState;
    rasterizerState = info.rasterizerState;
    stencilState = info.stencilState;
    primitive = info.primitive;
    assembler = info.assembler;
    gpuShader = &static_cast<WebGL2Shader*>(info.shader)->getGPUShader();
}
