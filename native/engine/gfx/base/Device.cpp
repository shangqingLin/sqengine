#include "Device.h"

 Device::Device():deviceCaps(NULL){
 }

 Device::~Device(){
    delete deviceCaps;
    deviceCaps = NULL;
 }