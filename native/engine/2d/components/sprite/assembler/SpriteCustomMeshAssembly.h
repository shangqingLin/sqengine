#pragma once
#include "SpriteSimpleAssembly.h"
#include "../SpriteComponent.h"

class SpriteCustomMeshAssembly : public ISpriteAssembly
{
private:
    void updateMesh(SpriteComponent *, const float *uvData);
    void updateTextureIndex(SpriteComponent *sprite, int textureIndex);
    void updateColor(SpriteComponent *sprite);
    void updateWorldVerts(SpriteComponent *sprite);
    void fillChunkIndices(SpriteComponent *sprite);

public:
    friend class SpriteComponent;
    virtual void requestRenderData(SpriteComponent *);
    virtual void destroyRenderData(SpriteComponent *);
    virtual void fillBuffers(SpriteComponent *sprite, int textureIndex);
    static SpriteCustomMeshAssembly *getInstance();
};