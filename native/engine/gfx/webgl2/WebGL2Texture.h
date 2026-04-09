#ifndef _WEBGL2_TEXTURE_H_
#define _WEBGL2_TEXTURE_H_

#include "../base/Texture.h"
#include "WebGL2GPUObjectDefine.h"

class WebGL2Texture : public gfx::Texture
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