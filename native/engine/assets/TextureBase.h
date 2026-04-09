#ifndef _TEXTURE_BASE_H_
#define _TEXTURE_BASE_H_

#include "Asset.h"
#include "../gfx/gfx.h"

class TextureBase : public Asset
{
protected:
    gfx::Texture *_gfxTexture;
    SamplerInfo samplerInfo;
    gfx::SamplerObject *samplerObject;
    void createGFXTexture(TextureInfo &info);
    void destroyGFXTexture();

public:
    TextureBase(int id);
    virtual ~TextureBase();
    void updateData(const unsigned char *buffer, BufferTextureCopyRegion &);
    void updateData(const unsigned char *buffer);
    void uploadData();
    virtual void setWrapMode(TextureWrapMode wrapS, TextureWrapMode wrapT, TextureWrapMode wrapR);
    virtual void setFilters(TextureFilter minFilter, TextureFilter magFilter);
    void setSamplerFromJs(int hash);
    inline gfx::Texture *getGFXTexture() { return _gfxTexture; };
    inline gfx::SamplerObject *getGFXSampler() { return samplerObject; };
};

#endif