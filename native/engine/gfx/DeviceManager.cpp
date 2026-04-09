#include "DeviceManager.h"


static DeviceManager *ins = new DeviceManager();

DeviceManager::DeviceManager():device(NULL){}

DeviceManager* DeviceManager::getInstance(){
    return ins;
}
