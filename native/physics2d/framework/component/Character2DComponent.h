#pragma once
#include "../../../engine/framework/component/Component.h"
#include "../../../engine/core/core.h"
#include "../../phxy/sq-phxy.h"

namespace physics2d
{
    enum class CharacterState : int
    {
        JUMP = 1 << 1,
        MOVE_LEFT = 1 << 2,
        MOVE_RIGHT = 1 << 3
    };
    SQ_ENUM_CONVERSION_OPERATOR(CharacterState)

    struct ShapeUserData
    {
        float maxPush;
        bool clipVelocity;
    };
    class Character2DComponent : public Component
    {
    private:
        static constexpr int m_planeCapacity = 8;
        bool m_onGround;

        //存储当前的速度
        phxy::SqVec2 m_velocity;

        // 移动时可以达到的最大速度
        float m_speed = 6.f;

        // 当速度小于m_minSpeed设置的速度时，则停止移动、控制停止条件
        // 或清空上一帧的速度
        float m_minSpeed = 0.01f;
        float m_stopSpeed = 1.0f;

        //地面移动时的摩擦力
        float m_friction = 8.f;
        float m_gravity = 20.0f;
        float m_jumpSpeed = 10.0f;
        float m_airSteer = 0.2f;

        /**
         * 如果当前角色的速度小于m_speed，则会慢慢加速度
         * m_accelerate就是控制多快能够加到m_speed的速度
         */
        float m_accelerate = 2.0f;

        /**
         * 距离地面多高的地方会自动走过去
         * 否则会阻挡走不过去
         */
        float pogoHeight = 2.f;
        float m_pogoHertz = 5.0f;
        float m_pogoDampingRatio = 1.f;
        int m_planeCount = 0;
        float m_pogoVelocity = 0.f;

        ShapeUserData m_friendlyShape;
        ShapeUserData m_elevatorShape;
        phxy::SqCollisionPlane m_planes[m_planeCapacity] = {};
        phxy::SqTransform m_transform;
        phxy::SqWorld* world;
        int state;

        phxy::SqCapsuleShape m_capsule;
        phxy::SqShapeFilter groundFilter;
        void update(float dt);
        void syncPhysicsToTransform();

    public:
        friend bool PlaneResultFcn(phxy::SqShape* shape, const phxy::SqPlaneResult *planeResult, void *context);
        Character2DComponent();
        virtual ~Character2DComponent();
        virtual void onEnable();
        virtual void onDisable();
        void moveLeft(bool);
        void moveRight(bool);

        /**
         * 执行跳跃
         **/
        void jump();
        void setFilter(unsigned int categoryBits, unsigned int maskBits);
    };
}