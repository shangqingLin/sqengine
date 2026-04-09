#ifndef _WEBGL_TEXTURE_H_
#define _WEBGL_TEXTURE_H_

#include "../base/Texture.h"
#include "WebGLGPUObjectDefine.h"

class WebGLTexture : public gfx::Texture
{
private:
    IGLGPUTexture gpuTexture;

public:
    virtual void initialize(TextureInfo &);
    virtual void destroy();
    virtual int getTexWidth();
    virtual int getTexHeight();
    virtual void resize(int width, int height);
    IGLGPUTexture &getGPUTexture() { return gpuTexture; };
};

#endif