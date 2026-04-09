#include "WebGLTexture.h"
#include "WebGLCommand.h"
#include "../DeviceManager.h"
#include "../../core/base/config.h"

void WebGLTexture::initialize(TextureInfo &info)
{
    SQ_ASSERT(info.format != Format::UNKNOWN);
    SQ_ASSERT(info.width > 0);
    SQ_ASSERT(info.height > 0);
    
    gpuTexture.glInternalFmt = info.format;
    gpuTexture.width = info.width;
    gpuTexture.height = info.height;
    gpuTexture.type = info.type;
    gpuTexture.mipLevel = info.levelCount;
    gpuTexture.glFormat = info.format;
    gpuTexture.glType = info.format;
    WebGLCmdFuncCreateTexture(static_cast<WebGLDevice *>(DeviceManager::getInstance()->device), gpuTexture);
}

void WebGLTexture::resize(int width, int height)
{
    if (gpuTexture.width != width || gpuTexture.height != height)
    {
        gpuTexture.width = width;
        gpuTexture.height = height;
        WebGLCmdFuncResizeTexture(static_cast<WebGLDevice *>(DeviceManager::getInstance()->device), gpuTexture);
    }
}

void WebGLTexture::destroy()
{
    if (gpuTexture.glTexture)
    {
        WebGLCmdFuncDestroyTexture(static_cast<WebGLDevice *>(DeviceManager::getInstance()->device), gpuTexture);
    }
}

int WebGLTexture::getTexWidth()
{
    return gpuTexture.width;
}

int WebGLTexture::getTexHeight()
{
    return gpuTexture.height;
}
