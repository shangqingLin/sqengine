#include "SpineGPURenderCache.h"

SpineGPURenderCacheSkin::~SpineGPURenderCacheSkin()
{
    if (saveBoneMap)
    {
        delete saveBoneMap;
        saveBoneMap = NULL;
    }

    if(texture){
        delete texture;
        texture = NULL;
    }
}

SpineGPURenderCache::~SpineGPURenderCache()
{
    for (int i = 0; i < skins.size(); ++i)
    {
        delete skins[i];
    }
    skins.clear();
}