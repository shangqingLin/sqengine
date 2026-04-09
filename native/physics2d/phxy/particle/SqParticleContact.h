#pragma once
#include "../common/math/SqVec2.h"

namespace phxy
{

    class SqParticleSystem;
    class SqParticleContact
    {
    private:
        SqParticleSystem *system = nullptr;

        // 记录哪是哪两个粒子发生碰撞了
        int indexA, indexB;

        /// Weight of the contact. A value between 0.0f and 1.0f.
        /// 0.0f ==> particles are just barely touching
        /// 1.0f ==> particles are perfectly on top of each other
        float weight;

        /// 碰撞法线 。从A指向B的法线
        SqVec2 normal;

        /// The logical sum of the particle behaviors that have been set.
        /// See the b2ParticleFlag enum.
        unsigned int flags;

    public:
        friend class SqParticleSystem;
        int state = 0;
        
        void SetIndices(int a, int b);
        void SetWeight(float w) { weight = w; }
        void SetNormal(const SqVec2 &n) { normal = n; }
        void SetFlags(unsigned int f) { flags = f; }

        int GetIndexA() const { return indexA; }
        int GetIndexB() const { return indexB; }
        float GetWeight() const { return weight; }
        const SqVec2 &GetNormal() const { return normal; }
        unsigned int GetFlags() const { return flags; }

        bool operator==(const SqParticleContact &rhs) const;
        bool operator!=(const SqParticleContact &rhs) const { return !operator==(rhs); }
        bool ApproximatelyEqual(const SqParticleContact &rhs) const;

        static bool contactListSortCompare(const SqParticleContact &a, const SqParticleContact &b);
        static bool contactEquals(const SqParticleContact &a, const SqParticleContact &b);
        static bool ContactIsZombie(const SqParticleContact &contact);
    };
}
