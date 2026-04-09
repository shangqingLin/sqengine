#include "WebGLPipelineState.h"
#include "WebGLShader.h"

void WebGLPipelineState::setPipelineInfo(IPipelineStateInfo& info){
    blendState = info.blendState;
    depthState = info.depthState;
    rasterizerState = info.rasterizerState;
    stencilState = info.stencilState;
    primitive = info.primitive;
    assembler = info.assembler;
    gpuShader = &static_cast<WebGLShader*>(info.shader)->getGPUShader();
}
