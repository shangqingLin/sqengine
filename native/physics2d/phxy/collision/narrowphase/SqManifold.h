#pragma once
#include "../../common/math/SqVec2.h"
#include <stdint.h>

namespace phxy
{
#define SQ_MAKE_ID(A, B) ((uint8_t)(A) << 8 | (uint8_t)(B))

    // A manifold point is a contact point belonging to a contact manifold.
    /// It holds details related to the geometry and dynamics of the contact points.
    /// Box2D uses speculative collision so some contact points may be separated.
    /// You may use the totalNormalImpulse to determine if there was an interaction during
    /// the time step.
    struct SqManifoldPoint
    {

        /// 这里的属性顺序不要打乱，因为js端直接读取

        /**
         * 这两个刚体的世界坐标系下的碰撞点，都在是坐标系下了，所以碰撞点肯定是在同一个位置上的
         * 所以只有一个
         */
        SqVec2 point;

        /**
         * anchorA和anchorB也是碰撞点，只不过是刚体本地坐标系下的碰撞点
         * 然后是anchor碰撞点从物体质心到碰撞点的向量。
         * 
         * 物理公式中需要使用这个计算这个碰撞点的速度，力矩也需要。
         * 
         * 随着刚体不断运动，他的碰撞点位置也是不断改变的，所以这个值是在不断改变的
         */
        SqVec2 anchorA;
        SqVec2 anchorB;

        /**
         * 碰撞点anchorA在碰撞法线上的投影距离。如果碰撞点有一点穿透则为负数
         */
        float separation{0.f};

        /**
         * 保存上一帧在法线方向上的冲量
         */
        float normalImpulse{0.f};

        /**
         * 保存上一帧在切线方向上的冲量
         */
        float tangentImpulse{0.f};

        /**
         * 法线方向上的累积冲量
         */
        /// The total normal impulse applied across sub-stepping and restitution. This is important
        /// to identify speculative contact points that had an interaction in the time step.
        float totalNormalImpulse{0.f};

        /// Relative normal velocity pre-solve. Used for hit events. If the normal impulse is
        /// zero then there was no hit. Negative means shapes are approaching.
        float normalVelocity{0.f};

        /**
         * 用于标记当前这个碰撞点
         * 使用SQ_MAKE_ID生成。
         *   A为是为0或1,表示在SqManifold中，0表示当前是第0个点，1表示为当前为第1个点
         *   B为
         */
        uint16_t id{0};

        /// Did this contact point exist the previous step?
        bool persisted{false};
    };

    struct SqManifold
    {
        /// 这里的属性顺序不要打乱，因为js端直接读取

        /**
         * 在世界坐标系下的碰撞法线，是一个单位向量。由ShapeA指向ShapeB
         */
        SqVec2 normal;

        /// The number of contacts points, will be 0, 1, or 2
        int pointCount{0};

        /// The manifold points, up to two are possible in 2D
        SqManifoldPoint points[2];

        /// Angular impulse applied for rolling resistance. N * m * s = kg * m^2 / s
        float rollingImpulse{0.f};
    };

}