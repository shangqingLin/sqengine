#ifndef GFX_WEBGL_PIPELINE_STATE_H_
#define GFX_WEBGL_PIPELINE_STATE_H_

#include "../base/PipelineState.h"
#include "WebGLGPUObjectDefine.h"

class WebGLPipelineState : public PipelineState
{
public:
    IGLGPUShader *gpuShader;
    void setPipelineInfo(IPipelineStateInfo &);
};

#endif