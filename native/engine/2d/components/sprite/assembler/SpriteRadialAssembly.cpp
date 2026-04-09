#include "SpriteRadialAssembly.h"

static SpriteRadialAssembly *_inst = nullptr;
SpriteRadialAssembly *SpriteRadialAssembly::getInstance()
{
    if (!_inst)
    {
        _inst = new SpriteRadialAssembly();
    }
    return _inst;
}

void SpriteRadialAssembly::requestRenderData(SpriteComponent *)
{
}

void SpriteRadialAssembly::destroyRenderData(SpriteComponent *)
{
}

void SpriteRadialAssembly::updateRenderData(SpriteComponent *)
{

}

void SpriteRadialAssembly::fillBuffers(SpriteComponent *sprite, int textureIndex)
{
    
}