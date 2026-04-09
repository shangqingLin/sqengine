#pragma once
#include "ParticlePhysicsRender.h"
#include "../../../engine/assets/Texture2d.h"
#include "../../../engine/scene/graphics/VertexBuffer.h"
#include "../../../engine/scene/graphics/Mesh.h"

namespace physics2d
{
    class ParticlePhysics2DComponent;
    
    class ParticlePhysicsTextureBlur : public ParticlePhysicsRender
    {
    private:
        VertexBuffer *posBuffer;
        VertexBuffer *weightBuffer;
        Mesh *mesh;
        int particleCount = 0;

    public:
        ParticlePhysicsTextureBlur(ParticlePhysics2DComponent *component);
        virtual ~ParticlePhysicsTextureBlur();
        virtual void onUpdateMaterial() {};
        virtual void update();
        virtual void allocate(int particleNum);
    };
}