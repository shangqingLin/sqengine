#pragma once

#include "../common/math/SqVec2.h"
#include "../geometry/shape/SqShape.h"

namespace phxy
{

    class SqParticleSystem;

    /**
     * 粒子系统与Box2D刚体发生碰撞时，记录碰撞信息
     */
    class SqParticleBodyContact
    {
    public:
        SqParticleSystem* system = nullptr;

        /**
         * 与哪个粒子发生碰撞
         */
        int particleId;

        /**
         * 与哪个形状发生碰撞
         */
        SqShape *shape;

        /**
         * 与哪个刚体发生碰撞
         */
        int bodyId;

        /// Weight of the contact. A value between 0.0f and 1.0f.
        float weight;

        /**
         * 碰撞法线，从粒子指向刚体的法线
         */
        /// The normalized direction from the particle to the body.
        SqVec2 normal;

        /// The effective mass used in calculating force.
        float mass;

        // 1 表示刚开始接触 2 表示已经接触目前还在接触 0：表示End Contact
        unsigned char state = 0;

        static bool bodyContactListCompare(const SqParticleBodyContact &a, const SqParticleBodyContact &b);
        static bool bodyContactListRemoveInvalid(const SqParticleBodyContact &a);
        static bool bodyContactListEquals(const SqParticleBodyContact &a, const SqParticleBodyContact &b);
    };
}