#include "SamplerObject.h"

using namespace gfx;

unsigned int SamplerObject::computeHash(const SamplerInfo &info)
{
    unsigned int hash = toNumber(info.minFilter);
    hash |= (toNumber(info.magFilter) << 4);
    hash |= (toNumber(info.wrapS) << 8);
    hash |= (toNumber(info.wrapT) << 12);
    hash |= (toNumber(info.wrapR) << 16);
    return hash;
}

void SamplerObject::unpackComputeHash(SamplerInfo &info, unsigned int hash)
{
    info.minFilter = TextureFilter(hash & 15);
    info.magFilter = TextureFilter((hash >> 4) & 15);
    info.wrapS = TextureWrapMode((hash >> 8) & 15);
    info.wrapT = TextureWrapMode((hash >> 12) & 15);
    info.wrapR = TextureWrapMode((hash >> 16) & 15);
}

TextureWrapMode SamplerObject::getWrapS()
{
    // 15 = 4个位，每个为1
    return TextureWrapMode((hash >> 8) & 15);
}

TextureWrapMode SamplerObject::getWrapT()
{
    return TextureWrapMode((hash >> 12) & 15);
}

TextureFilter SamplerObject::getMinFilter()
{
    return TextureFilter(hash & 15);
}

TextureFilter SamplerObject::getMagFilter()
{
    return TextureFilter((hash >> 4) & 15);
}