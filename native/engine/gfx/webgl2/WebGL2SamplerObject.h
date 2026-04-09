
#pragma once
#include "../base/SamplerObject.h"
#include "../webglbase/BaseWebGLGPUObjectDefine.h"

namespace gfx
{
    class WebGL2SamplerObject : public SamplerObject
    {
    public:
        IGLGPUSamplerObject gpuSampler;
        WebGL2SamplerObject(const SamplerInfo &, unsigned int hashKey);
        virtual ~WebGL2SamplerObject();
    };

}