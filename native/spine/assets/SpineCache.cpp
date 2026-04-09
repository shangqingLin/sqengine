#include "SpineCache.h"
#include <engine/core/base/config.h>

sqstd::hash_t SpineCache::getKey(int skin, int animationIndex, int frame, char type)
{
    sqstd::hash_t key = 888;
    sqstd::hash_combine(key, skin);
    sqstd::hash_combine(key, animationIndex);
    sqstd::hash_combine(key, frame);
    sqstd::hash_combine(key, type);
    return key;
}

void SpineCache::addFrame(int skin, int animationIndex, int frame, char type, sqstd::Byte *buffer)
{
    sqstd::hash_t key = getKey(skin, animationIndex, frame, type);
#if ENABLE_ASSERTS
    std::unordered_map<sqstd::hash_t, sqstd::Byte *>::iterator it = cacheMap.find(key);
    SQ_ASSERT(it == cacheMap.end());
#endif
    cacheMap[key] = buffer;
}

sqstd::Byte *SpineCache::getFrame(int skin, int animationIndex, int frame, char type)
{
    sqstd::hash_t key = getKey(skin, animationIndex, frame, type);
    std::unordered_map<sqstd::hash_t, sqstd::Byte *>::iterator it = cacheMap.find(key);
    return it == cacheMap.end() ? NULL : it->second;
}

void SpineCache::setCacheIndexBuffer(sqstd::hash_t key, sqstd::Byte *buffer)
{
#if ENABLE_ASSERTS
    std::unordered_map<sqstd::hash_t, sqstd::Byte *>::iterator it = cacheIndicesMap.find(key);
    SQ_ASSERT(it == cacheIndicesMap.end());
#endif
    cacheIndicesMap[key] = buffer;
}

sqstd::Byte *SpineCache::getCacheIndexBuffer(sqstd::hash_t key)
{
    std::unordered_map<sqstd::hash_t, sqstd::Byte *>::iterator it = cacheIndicesMap.find(key);
    return it == cacheIndicesMap.end() ? NULL : it->second;
}

SpineCache::~SpineCache()
{
    if(gpuCache){
        delete gpuCache;
        gpuCache = NULL;
    }
    
    std::unordered_map<sqstd::hash_t, sqstd::Byte *>::iterator it = cacheMap.begin();
    while (it != cacheMap.end())
    {
        delete it->second;
        ++it;
    }
    cacheMap.clear();

    std::unordered_map<sqstd::hash_t, sqstd::Byte *>::iterator it2 = cacheIndicesMap.begin();
    while (it2 != cacheIndicesMap.end())
    {
        delete it2->second;
        ++it2;
    }
    cacheIndicesMap.clear();
}