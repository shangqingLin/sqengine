#include "SqParticleContact.h"
#include "b2ParticleSetting.h"
#include "SqParticle.h"
#include "SqParticleSystem.h"
#include "../common/SqCore.h"
#include "../common/math/SqMath.h"
#include "../common/math/SqVec2.h"

using namespace phxy;

void SqParticleContact::SetIndices(int a, int b)
{
    SQ_ASSERT(a <= sq_maxParticleIndex && b <= sq_maxParticleIndex);
    indexA = a;
    indexB = b;
}

bool SqParticleContact::operator==(const SqParticleContact &rhs) const
{
    return indexA == rhs.indexA && indexB == rhs.indexB && flags == rhs.flags && weight == rhs.weight && normal == rhs.normal;
}

// The reciprocal sqrt function differs between SIMD and non-SIMD, but they
// should create approximately equal results.
bool SqParticleContact::ApproximatelyEqual(const SqParticleContact &rhs) const
{
    static const float MAX_WEIGHT_DIFF = 0.01f; // Weight 0 ~ 1, so about 1%
    static const float MAX_NORMAL_DIFF = 0.01f; // Normal length = 1, so 1%
    return indexA == rhs.indexA && indexB == rhs.indexB && flags == rhs.flags &&
           abs(weight - rhs.weight) < MAX_WEIGHT_DIFF && SqVec2::Length(normal - rhs.normal) < MAX_NORMAL_DIFF;
}

bool SqParticleContact::contactListSortCompare(const SqParticleContact &a, const SqParticleContact &b)
{
    return a.indexA < b.indexB;
}

bool SqParticleContact::contactEquals(const SqParticleContact &a, const SqParticleContact &b)
{
    return (a.indexA == b.indexA && a.indexB == b.indexB) || (a.indexA == b.indexB && a.indexB == b.indexA);
}

bool SqParticleContact::ContactIsZombie(const SqParticleContact &contact)
{
    return (contact.GetFlags() & sq_zombieParticle) == sq_zombieParticle 
        || contact.system->findById(contact.indexA)->flags & sq_zombieParticle
        || contact.system->findById(contact.indexB)->flags & sq_zombieParticle;
}