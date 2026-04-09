#include "WebGL2Texture.h"
#include "WebGL2Command.h"
#include "../DeviceManager.h"

void WebGL2Texture::initialize(TextureInfo &info)
{
    gpuTexture.glInternalFmt = info.format;
    gpuTexture.width = info.width;
    gpuTexture.height = info.height;
    gpuTexture.type = info.type;
    gpuTexture.mipLevel = info.levelCount;
    gpuTexture.glFormat = info.format;
    gpuTexture.glType = info.format;
    WebGL2CmdFuncCreateTexture(static_cast<WebGL2Device *>(DeviceManager::getInstance()->device), gpuTexture);
}

void WebGL2Texture::resize(int width, int height)
{
    if (gpuTexture.width != width || gpuTexture.height != height)
    {
        gpuTexture.width = width;
        gpuTexture.height = height;
        WebGL2CmdFuncResizeTexture(static_cast<WebGL2Device *>(DeviceManager::getInstance()->device), gpuTexture);
    }
}

void WebGL2Texture::destroy()
{
    if (gpuTexture.glTexture)
    {
        WebGL2CmdFuncDestroyTexture(static_cast<WebGL2Device *>(DeviceManager::getInstance()->device), gpuTexture);
    }
}

int WebGL2Texture::getTexWidth()
{
    return gpuTexture.width;
}

int WebGL2Texture::getTexHeight()
{
    return gpuTexture.height;
}
