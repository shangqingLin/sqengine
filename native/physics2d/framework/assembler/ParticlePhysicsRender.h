#pragma once

namespace physics2d
{
    class ParticlePhysics2DComponent;
    class ParticlePhysicsRender
    {
    protected:
        ParticlePhysics2DComponent *component;

    public:
        ParticlePhysicsRender(ParticlePhysics2DComponent *component) : component(component) {};
        virtual ~ParticlePhysicsRender() = default;
        virtual void update() = 0;
        virtual void allocate(int particleNum) = 0;
        virtual void onUpdateMaterial() = 0;
    };

}