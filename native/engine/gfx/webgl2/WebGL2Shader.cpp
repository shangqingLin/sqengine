#include "WebGL2Shader.h"
#include "WebGL2Command.h"
#include "../DeviceManager.h"
#include "WebGL2Device.h"

WebGL2Shader::WebGL2Shader()
{
    gpuShader = new IGLGPUShader();
}

WebGL2Shader::~WebGL2Shader()
{
    if (gpuShader->glProgram)
    {
        WebGL2CmdFuncDestroyShader(static_cast<WebGL2Device *>(DeviceManager::getInstance()->device), *gpuShader);
    }

    if (gpuShader->uniformBuffer)
    {
        delete gpuShader->uniformBuffer;
    }

    delete gpuShader;
    gpuShader = NULL;
}

void WebGL2Shader::initialize(ShaderInfo &info)
{
    gpuShader->effectAssetId = info.assetId;
    gpuShader->shaderIndex = info.shaderIndex;
    gpuShader->attributes = &info.attributes;
    gpuShader->macroFlags = info.macroFlags;
    gpuShader->blocks = &info.blocks;
    gpuShader->samplerTextures = &info.samplerTextures;

    // todo
    WebGL2CmdFuncCreateShader(static_cast<WebGL2Device *>(DeviceManager::getInstance()->device), *gpuShader);
}

IGLGPUShader &WebGL2Shader::getGPUShader()
{
    if (gpuShader->glProgram == 0)
    {
        WebGL2CmdFuncCreateShader(static_cast<WebGL2Device *>(DeviceManager::getInstance()->device), *gpuShader);
    }
    return *gpuShader;
}
