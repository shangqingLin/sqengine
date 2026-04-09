#include "./WebGLSamplerObject.h"

using namespace gfx;

WebGLSamplerObject::WebGLSamplerObject(const SamplerInfo &info, unsigned int hashKey)
{
    hash = hashKey;
    gpuSampler.hash = hashKey;
}