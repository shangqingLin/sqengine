#pragma once
#include "ISpriteAssembly.h"

/**
 * 9宫格渲染方式
 */
class SpriteSlicedAssembly : public ISpriteAssembly
{
private:
    void updateVertexData(SpriteComponent *sprite);
    void updateTextureIndex(SpriteComponent *sprite, int);
    void updateColor(SpriteComponent *sprite);
    void updateWorldVerts(SpriteComponent *sprite);
    void updateUVs(SpriteComponent *sprite);
    void fillChunkIndices(SpriteComponent *sprite);

public:
    static SpriteSlicedAssembly *getInstance();
    virtual void requestRenderData(SpriteComponent *);
    virtual void destroyRenderData(SpriteComponent *);
    virtual void fillBuffers(SpriteComponent *sprite, int textureIndex);
};
