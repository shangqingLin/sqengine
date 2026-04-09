#ifndef _WEB_GL_DEVICE_CAPS_H_
#define _WEB_GL_DEVICE_CAPS_H_
#include "../base/define_device_caps.h"

class WebGLDeviceCaps : public DeviceCaps
{
public:

   /**
    * 在WebGL1.0中，如果支持WEBGL_depth_texture扩展，则FBO可以自动将深度值写入到纹理,不需要额外创建RBO
    * 但是如果不支持，我们需要创建RBO。
    */
   bool WEBGL_depth_texture = false;
   virtual bool supperFormatTexture(Format format);
};




#endif