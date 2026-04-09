#ifndef _TEXTURE_2D_H_
#define _TEXTURE_2D_H_

#include "TextureBase.h"
#include "../gfx/base/define.h"

struct ITexture2DCreateInfo
{
    int width = 0;
    int height = 0;

    // Format枚举值
    // 不用枚举因为需要支持与JS交互
    int format;
    int mipmapLevel = 1;

    // JS将Sampler合并为一个数字
    int samplerHash = 0;

    std::optional<SamplerInfo> sampler;
};

class Texture2d : public TextureBase
{

protected:
    int width;
    int height;

public:
    Texture2d(int id);
    Texture2d();
    // virtual void setKeyUrl(char *url);
    void resize(int width, int height);
    inline int getWidth() { return width; };
    inline int getHeight() { return height; };
    virtual void create(ITexture2DCreateInfo *);
};

#endif