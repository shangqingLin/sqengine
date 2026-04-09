#include "TextureBase.h"
#include "../core/base/config.h"

TextureBase::TextureBase(int id) : Asset(id),
                                   _gfxTexture(NULL),
                                   samplerObject(NULL)
{
    samplerObject = DeviceManager::getInstance()->device->getSampler(samplerInfo);
}

void TextureBase::setWrapMode(TextureWrapMode wrapS, TextureWrapMode wrapT, TextureWrapMode wrapR)
{
    samplerInfo.wrapS = wrapS;
    samplerInfo.wrapT = wrapT;
    samplerInfo.wrapR = wrapR;
    //对于相同的参数设置samplerObject是全局唯一的，即相同的纹理参数的所有的纹理都可以使用相同的samplerObject
    //在device内管理这些samplerObject
    samplerObject = DeviceManager::getInstance()->device->getSampler(samplerInfo);
}

void TextureBase::setFilters(TextureFilter minFilter, TextureFilter magFilter)
{
    samplerInfo.minFilter = minFilter;
    samplerInfo.magFilter = magFilter;
    samplerObject = DeviceManager::getInstance()->device->getSampler(samplerInfo);
}

void TextureBase::setSamplerFromJs(int hash)
{
    SamplerInfo unpackInfo;
    gfx::SamplerObject::unpackComputeHash(unpackInfo, hash);
    setFilters(unpackInfo.minFilter, unpackInfo.magFilter);
    setWrapMode(unpackInfo.wrapS, unpackInfo.wrapR, TextureWrapMode::CLAMP_EDGE);
}

void TextureBase::createGFXTexture(TextureInfo &info)
{
    destroyGFXTexture();
    _gfxTexture = DeviceManager::getInstance()->device->createTexture(info);
}

void TextureBase::destroyGFXTexture()
{
    if (_gfxTexture)
    {
        _gfxTexture->destroy();
        _gfxTexture = NULL;
    }
}

TextureBase::~TextureBase()
{
    destroyGFXTexture();
}

void TextureBase::updateData(const unsigned char *buffer, BufferTextureCopyRegion &region)
{
    SQ_ASSERT(region.texWidth > 0);
    SQ_ASSERT(region.texHeight > 0);

    DeviceManager::getInstance()->device->copyBuffersToTexture(buffer, _gfxTexture, region);
}

void TextureBase::updateData(const unsigned char *buffer)
{
    SQ_ASSERT(_gfxTexture);
    SQ_ASSERT(_gfxTexture->getTexWidth() > 0);
    SQ_ASSERT(_gfxTexture->getTexHeight() > 0);
    BufferTextureCopyRegion region;
    region.texWidth = _gfxTexture->getTexWidth();
    region.texHeight = _gfxTexture->getTexHeight();
    DeviceManager::getInstance()->device->copyBuffersToTexture(buffer, _gfxTexture, region);
}

void TextureBase::uploadData()
{
    DeviceManager::getInstance()->device->copyTexImagesToTexture(this->getId(), _gfxTexture);
}