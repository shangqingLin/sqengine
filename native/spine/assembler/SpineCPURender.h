#ifndef _SPINE_CPU_RENDER_H_
#define _SPINE_CPU_RENDER_H_
#include "SpineRender.h"
#include <engine/scene/graphics/VertexBuffer.h>
#include <engine/scene/graphics/IndexBuffer.h>
#include <spine/SkeletonClipping.h>

class SpineCPURender : public SpineRender
{
private:
    std::shared_ptr<Mesh> mesh;
    VertexBuffer *vertexBuffer;
    IndexBuffer *indexBuffer;    
    spSkeletonClipping *clipper;
public:
    SpineCPURender();    
    ~SpineCPURender();
    virtual void render(SpineComponent*,float dt);
};
#endif
 