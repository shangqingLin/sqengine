#ifndef _GFX_TEXTURE_H_
#define _GFX_TEXTURE_H_
#include "define.h"

namespace gfx
{
    class Texture
    {
    public:
        virtual void initialize(TextureInfo &) = 0;
        virtual void destroy() = 0;
        virtual void resize(int width, int height) = 0;
        virtual int getTexWidth() = 0;
        virtual int getTexHeight() = 0;
        virtual ~Texture(){};
    };
}

#endif
