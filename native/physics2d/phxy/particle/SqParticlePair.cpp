#include "SqParticlePair.h"

using namespace phxy;

bool SqParticlePair::IsPairInvalid(const SqParticlePair &pair)
{
    return pair.indexA < 0 || pair.indexB < 0;
}

