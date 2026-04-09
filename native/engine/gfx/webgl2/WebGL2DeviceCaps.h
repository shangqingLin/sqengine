#ifndef _WEB_GL2_DEVICE_CAPS_H_
#define _WEB_GL2_DEVICE_CAPS_H_
#include "../base/define_device_caps.h"

class WebGL2DeviceCaps : public DeviceCaps
{
public:
   virtual bool supperFormatTexture(Format format);
};




#endif