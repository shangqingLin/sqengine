#include "./WebGL2SamplerObject.h"
#include "WebGL2Device.h"
#include "../DeviceManager.h"

using namespace gfx;

WebGL2SamplerObject::WebGL2SamplerObject(const SamplerInfo &info, unsigned int hashKey)
{
    hash = hashKey;
    gpuSampler.hash = hashKey;
}

WebGL2SamplerObject::~WebGL2SamplerObject()
{
    if (gpuSampler.sampler != 0)
    {

        WebGL2Device *device = static_cast<WebGL2Device *>(DeviceManager::getInstance()->device);
        device->getGLContext().deleteSampler(gpuSampler.sampler);
        gpuSampler.sampler = 0;
    }
}
