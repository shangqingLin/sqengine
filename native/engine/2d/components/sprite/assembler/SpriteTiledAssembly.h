#pragma once
#include "ISpriteAssembly.h"

/**
 * 当渲染区域大于资源的大小，则平铺重复的方式渲染
 */
class SpriteTiledAssembly : public ISpriteAssembly
{
private:
public:
    static SpriteTiledAssembly *getInstance();
    virtual void requestRenderData(SpriteComponent *);
    virtual void destroyRenderData(SpriteComponent *);
    virtual void fillBuffers(SpriteComponent *sprite, int textureIndex);
};
