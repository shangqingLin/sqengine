#pragma once
#include "../../../engine/framework/component/Component.h"
#include "../define.h"
#include "../../../engine/core/math/Vec2.h"
#include "../../../bindings/binding.h"
#include "../../phxy/sq-phxy.h"
#include <optional>

namespace physics2d
{

    struct RigidBodyDef
    {
        std::optional<phxy::SqBodyType> type;
        std::optional<Vec2> linearVelocity;
        std::optional<float> angularVelocity;
        std::optional<float> linearDamping;
        std::optional<float> angularDamping;
        std::optional<float> gravityScale;
        std::optional<float> sleepThreshold;

        std::optional<bool> lockLinearX;
        std::optional<bool> lockLinearY;
        std::optional<bool> loclAngularZ;

        std::optional<bool> enableSleep;
        std::optional<bool> isAwake;
        std::optional<bool> isBullet;
        std::optional<bool> enableSensorHits;
        std::optional<bool> isEnabled;
        std::optional<bool> allowFastRotation;
        std::optional<bool> enableContinuous;

        std::optional<bool> enableParticlePressure;
        std::optional<bool> enableParticleDamping;
    };

    enum class RigidbodySleepMode2D
    {
        NeverSleep = 1,
        StartAsleep = 2,
        StartAwake = 3,
    };

    class RigidBody2DComponent : public Component
    {

    private:
        bool ownerBody;
        std::optional<RigidBodyDef> def;
        int bodyId = -1;
        bridge::NativeToJsObject nativeToJs;
        RigidbodySleepMode2D sleepMode = RigidbodySleepMode2D::StartAwake;
        void createBody();
        virtual void onAwake();
        void onNodeTransformChange();

    protected:
        virtual void onEnable();
        virtual void onDisable();
        virtual void onStart();
    public:
        friend void processPhysicsRigidBody2DComponent(bridge::ComponentType type, char nodeOp, char op, ArrayBuffer &buffer, Node *node);

        RigidBody2DComponent();
        virtual ~RigidBody2DComponent();

        int getBody() { return bodyId; };

        void setSleepMode(RigidbodySleepMode2D);
        RigidbodySleepMode2D getSleepMode();

        bool isEnableContinuous();
        void setEnableContinuous(bool enable);

        void setBullet(bool);
        inline bool getBullet() { return false; };

        void setType(phxy::SqBodyType type);
        // inline RigidBody2DType getType() { return ; };

        void setLinearDamping(float damping);

        void setAngularDamping(float damping);

        void setLinearVelocity(const Vec2 &v) const;
        void setLinearVelocity(float x, float y) const;
        Vec2 getLinearVelocity();

        void setAngularVelocity(float v);
        float getAngularVelocity();

        void setFixedRotation(bool);
        void setFixedPosition(bool x, bool y);

        void ApplyForce(const Vec2 &force, const Vec2 &point, bool wake);
        void ApplyForceToCenter(const Vec2 &force, bool wake);
        void ApplyTorque(float torque, bool wake);
        void ApplyLinearImpulse(const Vec2 &impulse, const Vec2 &point, bool wake);
        void ApplyLinearImpulseToCenter(const Vec2 &impulse, bool wake);
        void ApplyAngularImpulse(float impulse, bool wake);
        void SetGravityScale(float scale);
        float GetGravityScale();
        void SetActive(bool active);
        void setMass(float mass);
        float getMass();
        void setRotationalInertia(float v);
        float getRotationalInertia();
        void setAwake(bool b);

        /**
         * 当粒子碰撞到Body时，是否应用粒子的压力到Body上
         * 默认为true
         */
        void setEnableParticlePressure(bool b);

        /**
         * 当粒子碰撞到Body时，是否应用阻力到Body上
         * 默认为true
         */
        void setEnablePartcileDamping(bool b);
    };
}