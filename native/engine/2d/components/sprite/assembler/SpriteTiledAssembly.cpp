#include "SpriteTiledAssembly.h"

static SpriteTiledAssembly *instance = new SpriteTiledAssembly;
SpriteTiledAssembly *SpriteTiledAssembly::getInstance()
{
    return instance;
}

void SpriteTiledAssembly::requestRenderData(SpriteComponent *)
{
}

void SpriteTiledAssembly::destroyRenderData(SpriteComponent *)
{
}

void SpriteTiledAssembly::fillBuffers(SpriteComponent *sprite, int textureIndex)
{
}