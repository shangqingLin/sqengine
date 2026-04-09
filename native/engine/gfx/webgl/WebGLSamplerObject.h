
#pragma once
#include "../base/SamplerObject.h"
#include "../webglbase/BaseWebGLGPUObjectDefine.h"

namespace gfx
{
    class WebGLSamplerObject : public SamplerObject
    {
    public:
        IGLGPUSamplerObject gpuSampler;
        WebGLSamplerObject(const SamplerInfo &, unsigned int hashKey);
    };

}