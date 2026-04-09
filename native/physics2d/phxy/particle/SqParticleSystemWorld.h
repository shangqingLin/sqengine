#pragma once
#include "SqParticleSystem.h"
#include "sq_world_callbacks.h"
#include <unordered_map>
#include <variant>

namespace phxy
{

    typedef std::unordered_map<phxy::SqParticleSystem *, void *> ParticleSystemWorldContantListenerRegisterMap;

    class SqParticleSystemWorld
    {
    private:
        SqParticleSystem *m_particleSystemList;
        SqWorld *world;
        b2DestructionListener *m_destructionListener;
        // b2ContactFilter *m_contactFilter;
        b2ContactListener *m_contactListener;
        ParticleSystemWorldContantListenerRegisterMap contantListenerRegister;

        /**
         * 每帧需要执行多少次的粒子迭代
         */
        int particleIterations = 1;

    public:
        friend class SqParticleSystem;
        friend class SqParticleGroup;
        friend class b2ParticleSystemContactListenerImp;
        SqParticleSystemWorld(SqWorld *world);

        void step(SqStepContext &context);
        SqParticleSystem *CreateParticleSystem(const b2ParticleSystemDef *def);
        SqParticleSystem *GetParticleSystemList();
        void DestroyParticleSystem(SqParticleSystem *p);
        /**
         * 查询所有的粒子系统与aabb发生碰撞的粒子
         */
        void QueryAABB(b2QueryCallback *callback, const SqAABB &aabb) const;
        void registerContactListener(SqParticleSystem *p, void *userData);
        void removeContantListener(SqParticleSystem *p);
    };
}