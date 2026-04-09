#pragma once
#include "ISpriteAssembly.h"
#include "../SpriteComponent.h"

class SpriteSimpleAssembly : public ISpriteAssembly
{
protected:
    void updateVertexData(SpriteComponent *sprite);
    void updateUVs(SpriteComponent *sprite);
    void updateTextureIndex(SpriteComponent *sprite, int);
    void updateColor(SpriteComponent *sprite);
    void updateWorldVerts(SpriteComponent *sprite);
    void fillChunkIndices(SpriteComponent *sprite);

public:
    virtual void requestRenderData(SpriteComponent *);
    virtual void destroyRenderData(SpriteComponent *);
    virtual void fillBuffers(SpriteComponent *sprite, int textureIndex);
    static SpriteSimpleAssembly *getInstance();
};