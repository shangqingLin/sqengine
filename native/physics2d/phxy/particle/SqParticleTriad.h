#pragma once
#include "../common/math/SqVec2.h"

namespace phxy
{
    class SqParticleSystem;
    class SqParticleTriad
    {
    public:
        SqParticleSystem* system = nullptr;
        
        /// 哪三个粒子组成的
        int indexA, indexB, indexC;

        /// The logical sum of the particle flags. See the b2ParticleFlag enum.
        int flags;

        /// The strength of cohesion among the particles.
        float strength;

        /// Values used for calculation.
        SqVec2 pa, pb, pc;
        float ka, kb, kc, s;

        static bool IsTriadInvalid(const SqParticleTriad &triad);
    };

}