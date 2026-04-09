#ifndef GFX_WEBGL2_PIPELINE_STATE_H_
#define GFX_WEBGL2_PIPELINE_STATE_H_

#include "../base/PipelineState.h"
#include "WebGL2GPUObjectDefine.h"

class WebGL2PipelineState : public PipelineState
{
public:
    IGLGPUShader *gpuShader;
    void setPipelineInfo(IPipelineStateInfo &);
};

#endif