#pragma once
#include "../../../engine/2d/components/Transform2DComponent.h"
#include "../../../engine/framework/component/Component.h"
#include "../../../bindings/define.h"
#include "../../phxy/sq-phxy.h"
#include <optional>

namespace physics2d
{

    typedef void BeginContactCallback(const phxy::SqContactBeginEvent *event);
    typedef void EndContactCallback(const phxy::SqContactEndEvent *);
    typedef void HitUpdateContactCallback(const phxy::SqContactHitUpdateEvent *);

    typedef void PreSolveCallback(const phxy::SqContact *const, const phxy::SqManifold *const manifold);
    typedef void PostSolveCallback(const phxy::SqContact *const, const phxy::SqManifold *const manifold);
    typedef void SensorBeginCallback(Node *);
    typedef void SensorEndCallback(Node *);

    typedef void ParticleBeginCallback(int nodeId, int particleId);
    typedef void ParticleEndCallback(int nodeId, int particleId);

    /**
     * 碰撞组件
     */
    class Collision2DComponent : public Component
    {      
    protected:
        bool ownerBody;
        int bodyId = -1;

        // js端为每个Component Class生成的一个key
        // 因为一个Collision2DComponent可以添加多个，所以要区分一下
        int classKey = 0;

        phxy::SqShape *shape = nullptr;

        std::optional<phxy::SqShapeDef> baseDef;
        std::optional<Vec2> offset;

        int jsContactListener = 0;
        std::optional<std::function<physics2d::BeginContactCallback>> beginContactCall;
        std::optional<std::function<physics2d::EndContactCallback>> endContactCall;
        std::optional<std::function<physics2d::HitUpdateContactCallback>> hitUpdateCall;
        std::optional<std::function<physics2d::PreSolveCallback>> prevSolveContactCall;
        std::optional<std::function<physics2d::PostSolveCallback>> postSolveContactCall;
        std::optional<std::function<physics2d::SensorBeginCallback>> sensorBeginCallback;
        std::optional<std::function<physics2d::SensorEndCallback>> sensorEndCallback;

        virtual void onAwake();

        virtual void onNodeTransformChange();

        void createBody();

        void copyBaseDef(phxy::SqShapeDef &def);

        void onBeginContact(const phxy::SqContactBeginEvent *event);
        void onEndContact(const phxy::SqContactEndEvent *event);
        void onHitUpdateContact(const phxy::SqContactHitUpdateEvent *event);
        void onSensorBegin(const phxy::SqSensorBeginEvent *event);
        void onSensorEnd(const phxy::SqSensorEndEvent *event);

        void onParticleBegin(Node *contactNode, int particleId);
        void onParticleEnd(Node *contactNode, int particleId);

    public:
        friend class RigidBody2DComponent;
        friend class ParticlePhysics2DComponent;
        friend class PhysicsWorld;
        friend void preSolver(const phxy::SqContact *const, const phxy::SqManifold *const manifold, void *context);

        Collision2DComponent();
        virtual ~Collision2DComponent();

        void setBeginContactCallback(std::function<physics2d::BeginContactCallback> beginContactCallback);
        void offBeginContactCallback();

        void setEndContactCallback(std::function<physics2d::EndContactCallback> endContactCallback);
        void offEndContactCallback();

        void setHitUpdateContactCallback(std::function<physics2d::HitUpdateContactCallback> callback);

        void setPreSolveCallback(std::function<physics2d::PreSolveCallback> preSolveCallback);
        void setPostSolveCallback(std::function<physics2d::PostSolveCallback> endSolveallback);

        void setSensorBeginCallback(std::function<SensorBeginCallback>);
        void setSensorEndCallback(std::function<SensorEndCallback>);

        void setBridgeHasContactListener(bool b, char type, int classKey);
        int getBody();
        virtual void onEnable();
        virtual void onDisable();
        virtual void setOffset(float x, float y);
        void setDensity(float);
        void setFriction(float friction);
        void setRestitution(float restitution);
        void setTangentSpeed(float speed);
        void setSensor(bool);
        /**
         * 如果当前的为传感器，则设置传感器的碰撞检测方式
         * 如果这里设置为true则只要是AABB盒子发生碰撞即可以认为是发生碰撞了，而不需要真实的碰撞，有利于提升性能
         * 因为省略了精确检测的步骤
         */
        void setSensorAABB(bool);
        void setFilter(unsigned int categoryBits, unsigned int maskBits);
        void getFilter(unsigned int &categoryBits, unsigned int &maskBits);
        void setCustomColor(uint32_t color);
        float getMass();
        inline phxy::SqShape *getShape() { return shape; }
    };
}
