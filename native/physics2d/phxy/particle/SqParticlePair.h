#pragma once

namespace phxy
{
    class SqParticlePair
    {
    public:
        /// Indices of the respective particles making pair.
        int indexA, indexB;

        /// The logical sum of the particle flags. See the b2ParticleFlag enum.
        unsigned int flags;

        /// The strength of cohesion among the particles.
        float strength;

        /// The initial distance of the particles.
        float distance;

        static bool IsPairInvalid(const SqParticlePair &pair);
    };

}