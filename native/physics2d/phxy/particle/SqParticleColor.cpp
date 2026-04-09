#include "SqParticleColor.h"
using namespace phxy;

#define B2PARTICLECOLOR_BITS_PER_COMPONENT (sizeof(unsigned char) << 3)
// Maximum value of a SqParticleColor component.
#define B2PARTICLECOLOR_MAX_VALUE \
    ((1U << B2PARTICLECOLOR_BITS_PER_COMPONENT) - 1)

/// Number of bits used to store each SqParticleColor component.
const unsigned char SqParticleColor::k_bitsPerComponent = B2PARTICLECOLOR_BITS_PER_COMPONENT;
const float SqParticleColor::k_maxValue = (float)B2PARTICLECOLOR_MAX_VALUE;
const float SqParticleColor::k_inverseMaxValue = 1.0f / (float)B2PARTICLECOLOR_MAX_VALUE;

SqParticleColor::SqParticleColor(const SqColor &color)
{
    Set(color);
}

SqColor SqParticleColor::GetColor() const
{
    return SqColor(k_inverseMaxValue * r,
                   k_inverseMaxValue * g,
                   k_inverseMaxValue * b);
}

void SqParticleColor::Set(const SqColor &color)
{
    Set((unsigned char)(k_maxValue * color.r),
        (unsigned char)(k_maxValue * color.g),
        (unsigned char)(k_maxValue * color.b),
        B2PARTICLECOLOR_MAX_VALUE);
}