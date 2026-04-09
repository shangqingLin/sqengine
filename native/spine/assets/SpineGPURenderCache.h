#ifndef _SPINE_GPU_RENDER_CACHE_H_
#define _SPINE_GPU_RENDER_CACHE_H_
#include <engine/assets/Texture2d.h>
#include <spine/Skin.h>
#include <unordered_map>
class SpineGPURenderCacheSkin
{
public:
    /**
     * 在GPU渲染默认中，使用纹理来存储Skin数据
     * 然后在顶点着色器读取这个纹理
     */
    Texture2d *texture = NULL;
    spSkin *skin = NULL;
    int *saveBoneMap = NULL;
    int saveBoneNum = 0;
    std::unordered_map<int,int> saveAttachmentMap;
    ~SpineGPURenderCacheSkin();
};

class SpineGPURenderCache
{
public:
    std::vector<SpineGPURenderCacheSkin*> skins;
    ~SpineGPURenderCache();
};

#endif
