#include "SqParticleTriad.h"

using namespace phxy;

bool SqParticleTriad::IsTriadInvalid(const SqParticleTriad &triad)
{
    return triad.indexA < 0 || triad.indexB < 0 || triad.indexC < 0;
}