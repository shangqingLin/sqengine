#pragma once
#include "ISpriteAssembly.h"

/**
 * 横向或纵向填充。即在横向或纵向方向上裁剪，可以用来实习那进度条
 */
class SpriteBarFilledAssembly : public ISpriteAssembly
{
private:
    void updateRenderData(SpriteComponent *);
    void updateVertexData(SpriteComponent *, float, float);
    void updateUv(SpriteComponent *, float, float);
    void updateWorldVerts(SpriteComponent *sprite);
    void updateFillUVs(SpriteComponent *sprite);
    void updateColor(SpriteComponent *sprite);
    void updateTextureIndex(SpriteComponent *sprite, int textureIndex);
    void fillChunkIndices(SpriteComponent *sprite);

public:
    static SpriteBarFilledAssembly *getInstance();
    virtual void requestRenderData(SpriteComponent *);
    virtual void destroyRenderData(SpriteComponent *);
    virtual void fillBuffers(SpriteComponent *sprite, int textureIndex);
};
