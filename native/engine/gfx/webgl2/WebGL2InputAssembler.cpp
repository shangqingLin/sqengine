#include "WebGL2InputAssembler.h"
#include "WebGL2Command.h"
#include "../DeviceManager.h"
WebGL2InputAssembler::~WebGL2InputAssembler()
{
    WebGL2CmdFuncDestroyInputAssembler(static_cast<WebGL2Device *>(DeviceManager::getInstance()->device), this);
}