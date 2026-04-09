#include "WebGLInputAssembler.h"
#include "WebGLCommand.h"
#include "../DeviceManager.h"

WebGLInputAssembler::~WebGLInputAssembler()
{
    WebGLCmdFuncDestroyInputAssembler(static_cast<WebGLDevice *>(DeviceManager::getInstance()->device), this);
}