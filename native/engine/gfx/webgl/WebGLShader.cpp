#include "WebGLShader.h"
#include "WebGLCommand.h"
#include "../DeviceManager.h"
#include "WebGLDevice.h"

WebGLShader::WebGLShader()
{
    gpuShader = new IGLGPUShader();
}

WebGLShader::~WebGLShader()
{
    if (gpuShader->glProgram)
    {
        WebGLCmdFuncDestroyShader(static_cast<WebGLDevice *>(DeviceManager::getInstance()->device), *gpuShader);
    }

    if (gpuShader->uniformBuffer)
    {
        delete gpuShader->uniformBuffer;
    }

    delete gpuShader;
    gpuShader = NULL;
}

void WebGLShader::initialize(ShaderInfo &info)
{
    gpuShader->effectAssetId = info.assetId;
    gpuShader->macroFlags = info.macroFlags;
    gpuShader->shaderIndex = info.shaderIndex;
    gpuShader->attributes = &info.attributes;
    gpuShader->blocks = &info.blocks;
    gpuShader->samplerTextures = &info.samplerTextures;

    // todo
    WebGLCmdFuncCreateShader(static_cast<WebGLDevice *>(DeviceManager::getInstance()->device), *gpuShader);
}

IGLGPUShader &WebGLShader::getGPUShader()
{
    if (gpuShader->glProgram == 0)
    {
        WebGLCmdFuncCreateShader(static_cast<WebGLDevice *>(DeviceManager::getInstance()->device), *gpuShader);
    }
    return *gpuShader;
}
