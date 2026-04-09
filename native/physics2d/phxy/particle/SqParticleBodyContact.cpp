#include "SqParticleBodyContact.h"
#include "SqParticleSystem.h"

using namespace phxy;

bool SqParticleBodyContact::bodyContactListCompare(const SqParticleBodyContact &a, const SqParticleBodyContact &b)
{
    if (a.particleId != b.particleId)
        return a.particleId < b.particleId;
    return a.shape < b.shape;
}

bool SqParticleBodyContact::bodyContactListRemoveInvalid(const SqParticleBodyContact &a)
{
    return a.state == 0 || a.system->findById(a.particleId)->flags & sq_zombieParticle;
}

bool SqParticleBodyContact::bodyContactListEquals(const SqParticleBodyContact &a, const SqParticleBodyContact &b)
{
    // printf("compare a %d %p b %d %p \n",a.particleId,a.shape , b.particleId , b.shape);
    return a.particleId == b.particleId && a.shape == b.shape;
}
