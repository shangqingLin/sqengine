#ifndef _SPINE_CACHE_H_
#define _SPINE_CACHE_H_
#include <unordered_map>
#include <engine/core/core.h>
#include <engine/core/sqstd/Byte.h>
#include <engine/assets/Texture2d.h>
#include "SpineGPURenderCache.h"

/**
 * 缓存Spine动画数据，避免每帧都计算，提升动画播放效率
 * 不过会比较耗内存
 */
class SpineCache
{

private:
    std::unordered_map<sqstd::hash_t,sqstd::Byte*> cacheMap;
    std::unordered_map<sqstd::hash_t,sqstd::Byte*> cacheIndicesMap;
    sqstd::hash_t getKey(int skin,int animationIndex,int frame,char type);
public:

    /**
     * 在GPU渲染Spine，使用纹理来存储Skin数据等数据，
     * 然后在顶点着色器中计算蒙皮动画。
     */
    SpineGPURenderCache* gpuCache = NULL;

    ~SpineCache();

    /**
     * 因为动画中，很多渲染下标数据都是一样的，不需要每帧都计算，相对来说节省了CPU，也不需要缓存每帧的index数据，相对来说节省了内存。
     * 即多个IndexBuffer之间共享一个index数据
     */
    void setCacheIndexBuffer(sqstd::hash_t key,sqstd::Byte* buffer);
    sqstd::Byte* getCacheIndexBuffer(sqstd::hash_t key);

    /**
     * 缓存动画上的某一帧的数据
     * @param skin 表示哪个皮肤
     * @param animationIndex 表示哪个
     * @param frame 表示第帧数
     * @param type 缓存的动画数据类型 1：表示顶点搜索数据 2：顶点数据
     */
    void addFrame(int skin,int animationIndex,int frame,char type,sqstd::Byte* buffer);
    sqstd::Byte* getFrame(int skin,int animationIndex,int frame,char type);
};
#endif