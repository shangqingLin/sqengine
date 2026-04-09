#pragma once
#include "./sq_world_callbacks.h"

namespace phxy
{
    class SqParticleSystemWorld;
    class b2ParticleSystemContactListenerImp : public b2ContactListener
    {
    private:
        SqParticleSystemWorld *world;

    public:
        b2ParticleSystemContactListenerImp(SqParticleSystemWorld *world);
        virtual void BeginBodyContact(SqParticleSystem *particleSystem, SqParticleBodyContact *particleBodyContact);
        virtual void EndBodyContact(SqParticleSystem *particleSystem, SqParticleBodyContact *particleBodyContact);
        virtual void UpdateBodyContact(SqParticleSystem *particleSystem, SqParticleBodyContact *particleBodyContact);
        virtual void BeginParticleContact(SqParticleSystem *particleSystem, SqParticleContact *particleContact);
        virtual void EndParticleContact(SqParticleSystem *particleSystem, int indexA, int indexB);
    };
}