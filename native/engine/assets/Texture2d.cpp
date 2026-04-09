#include "Texture2d.h"
#include "../gfx/gfx.h"

Texture2d::Texture2d(int id) : width(0),
                               height(0),
                               TextureBase(id)
{
}

Texture2d::Texture2d() : width(0),
                         height(0),
                         TextureBase(-1)
{
}

void Texture2d::resize(int width, int height)
{
    this->width = width;
    this->height = height;
    _gfxTexture->resize(width, height);
}

// #include "../gfx/webgl2/WebGL2Texture.h"
void Texture2d::create(ITexture2DCreateInfo *info)
{
    TextureInfo textureInfo;
    textureInfo.format = Format(info->format);
    textureInfo.width = info->width;
    textureInfo.height = info->height;
    textureInfo.levelCount = info->mipmapLevel <= 0 ? 1 : info->mipmapLevel;
    textureInfo.type = TextureType::TEX2D;
    if (info->samplerHash)
    {
        SamplerInfo unpackInfo;
        gfx::SamplerObject::unpackComputeHash(unpackInfo, info->samplerHash);
        setFilters(unpackInfo.minFilter, unpackInfo.magFilter);
        setWrapMode(unpackInfo.wrapS, unpackInfo.wrapR, TextureWrapMode::CLAMP_EDGE);
        // printf("Set Texture %d %d %d %d \n", toNumber(min), toNumber(max), toNumber(wrapS), toNumber(wrapT));
    }
    else if (info->sampler.has_value())
    {
        setFilters(info->sampler.value().minFilter, info->sampler.value().magFilter);
        setWrapMode(info->sampler.value().wrapS, info->sampler.value().wrapR, info->sampler.value().wrapT);
    }
    width = info->width;
    height = info->height;
    createGFXTexture(textureInfo);


    // printf("create texture glTexture %d \n", dynamic_cast<WebGL2Texture *>(_gfxTexture)->getGPUTexture().glTexture);
    // TRACE();
}


// void Texture2d::setKeyUrl(char *url)
// {
//     Asset::setKeyUrl(url);
//     printf("create texture %s glTexture %d \n", getKeyUrl().c_str(), dynamic_cast<WebGL2Texture *>(_gfxTexture)->getGPUTexture().glTexture);
// }