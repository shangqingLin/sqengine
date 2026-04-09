#pragma once
#include "ISpriteAssembly.h"

/**
 * 径向填充。即按照不同的方向裁剪掉一部分
 */
class SpriteRadialAssembly : public ISpriteAssembly
{
private:
    void updateRenderData(SpriteComponent*);
public:
    static SpriteRadialAssembly *getInstance();
    virtual void requestRenderData(SpriteComponent *);
    virtual void destroyRenderData(SpriteComponent *);
    virtual void fillBuffers(SpriteComponent *sprite, int textureIndex);
};
