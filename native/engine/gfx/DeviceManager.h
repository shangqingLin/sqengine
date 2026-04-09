#ifndef _DEVICE_MANAGER_H_
#define _DEVICE_MANAGER_H_

#include "./base/Device.h"

class DeviceManager
{
public:
  Device *device;
  inline Device *getDevice() { return device; };
  DeviceManager();
  static DeviceManager *getInstance();
};

#endif