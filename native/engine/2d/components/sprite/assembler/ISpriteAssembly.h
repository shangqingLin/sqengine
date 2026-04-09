#pragma once

class SpriteComponent;
class ISpriteAssembly
{
public:
    virtual void requestRenderData(SpriteComponent*) = 0;   
    virtual void destroyRenderData(SpriteComponent*) = 0;
    virtual void fillBuffers(SpriteComponent *,int) = 0;
};


