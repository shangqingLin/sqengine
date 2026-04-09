#pragma once

#include "SqBodyDefine.h"
#include "sim/SqBodySim.h"
#include "../collision/narrowphase/SqContact.h"
#include "../collision/narrowphase/SqManifold.h"
#include "../common/SqTypeDefine.h"

namespace phxy
{
    class SqWorld;
    class SqSolverSet;
    class SqDebugDraw;
    class SqShape;
    class SqContact;
    class SqJoint;

    typedef void BeginContactCallback(const SqContactBeginEvent *, void *context);
    typedef void EndContactCallback(const SqContactEndEvent *, void *context);
    typedef void HitUpdateContactCallback(const SqContactHitUpdateEvent *, void *context);

    typedef void PreSolveCallback(const SqContact *const, const SqManifold *const manifold, void *context);
    typedef void PostSolveCallback(const SqContact *const, const SqManifold *const manifold, void *context);
    typedef void SensorBeginCallback(int otherBodyId, void *context);
    typedef void SensorEndCallback(int otherBodyId, void *context);

    class SqBody
    {
    private:
        /**
         * @type {SqBodyFlags}
         */
        int bodyFlag = 0;

        // Define
        SqBodyType type;
        void *userData;
        SqWorld *world;

        /**
         * 添加到当前Body上的Shape
         */
        SqShape *headShape;

        /**
         * 在world的bodyArray存储的Index位置
         */
        int bodyIndex;

        /**
         * SqBodySim在SqSolverSet中的索引
         */
        int simIndex;

        /**
         * 当前这个Body在World的哪个Set上
         */
        int setIndex;

        // 物理模拟相关的属性

        /**
         * 物体质量，是Body上所有的Shape加起来的质量
         */
        float mass{0.f};

        /**
         * 转动惯性。是Body上所有的Shape加起来的转动惯性
         */
        float inertia{0.f};

        // 只要Body移动速度小于等于这个阈值就会进入sleepTime计算
        float sleepThreshold;

        // 累计静止的时间，超出了这个时间Body会进入Sleep状态
        float sleepTime;

        /**
         * 存储当前的Body上的Contact
         */
        int headContactKey;
        int contactCount;

        /**
         * 在哪个SqIsland中
         * sq_dynamicBody和sq_kinematicBody都必须加入到island中
         * 在创建的时候就已经为body分配了island
         */
        int islandId;

        // 存储同一个island中的body
        int islandPrev;
        int islandNext;

        // 专门用于Split Island流程的
        bool isMarkedForIslandSplit;

        /**
         * 这个Body上添加的joint
         */
        int headJointKey;
        int jointCount;

        float scaleX = 1.0f;
        float scaleY = 1.0f;

        void createShapeProxy(SqShape *, bool forcePairCreation);
        void removeShapeProxy(SqShape *);
        void reset();
        void updateBodyMassData();
        void updateExtent(const SqVec2 &localCenter);
        bool wakeBody();
        void removeIsland();
        void createIslandForBody(int setIndex);
        void destroy();

        void addJointToLink(SqJoint *, int index);
        void removeJointFromLink(SqJoint *);
        void removeAllJoint();
        void addShape(SqShape *shape, bool updateBodyMass = true);
        void removeShape(SqShape *shape);

    public:
        friend class SqWorld;
        friend class SqBroadPhase;
        friend class SqContactManager;
        friend class SqNarrowPhase;
        friend class SqTimeOfImpactSolver;
        friend class SqIsland;
        friend class SqSolverSet;
        friend class SqBodySolver;
        friend class SqWorldDraw;
        friend class SqContactSolver;
        friend class SensorManager;
        friend bool SqShouldBodiesCollide(SqWorld *world, SqBody *bodyA, SqBody *bodyB);
        SqBody();

        SqBodySim *getBodySim();
        inline SqBodyType getType() { return type; };
        inline int getBodyFlag() { return bodyFlag; };
        inline void setBodyFlag(int flag) { bodyFlag = flag; };
        const SqTransform &getTransform();
        void setTransform(const SqVec2 &position, const SqRot &rotation);

        /**
         * 一定要非常谨慎，设置缩放是非常重度的操作，重新计算这个Body上所有的Shape的顶点
         */
        void setScale(float sx, float sy);
        inline void getScale(float& sx,float& sy) { sx = this->scaleX; sy = this->scaleY;  };

        inline void *getUserData() { return userData; };
        inline void setUserData(void *data) { userData = data; };

        void enableBeginContactEvent(bool);
        inline bool isEnableBeginContactEvent() { return bodyFlag & SqBodyFlags::sq_enableContactBeginEvent; };

        void enableEndContactEvent(bool);
        inline bool isEnableEndContactEvent() { return bodyFlag & SqBodyFlags::sq_enableContactEndEvent; };

        void enableHitUpdateContactEvent(bool);
        inline bool isEnableHitUpdateContactEvent() { return bodyFlag & SqBodyFlags::sq_enableContactHitEvent; };

        void enableSensorBeginEvent(bool);
        inline bool isEnableSensorBeginEvent() { return bodyFlag & SqBodyFlags::sq_enableSensorBeginEvent; };

        void enableSensorEndEvent(bool);
        inline bool isEnableSensorEndEvent() { return bodyFlag & SqBodyFlags::sq_enableSensorEndEvent; };

        inline SqShape *getShapes() { return headShape; };
        inline int getBodyId() { return bodyIndex; };

        void updateShapeProxy(bool checkSmallMove = false);

        void setMotionLocks(SqBodyMotionLocks locks);
        SqBodyMotionLocks getMotionLocks();

        void setMassData(const SqMassData &massData);
        SqMassData getMassData();
        float getMass();

        bool isStatic();
        bool isSleep();
        bool isAwake();
        void setAwake(bool awake);

        bool isEnableContinuous();
        void setEnableContinuous(bool enable);

        void enable();
        void disable();
        bool isEnabled();

        void removeAllContacts(bool wakeBodies);
        void removeContact(SqContact *, bool wakeBodies);

        void setType(SqBodyType type);

        /**
         * 设置刚体线性运动中的阻尼。对质点线性运动产生阻尼逐渐减少刚体线性速度
         * 单位是：线速度/每秒 linearDamping/s
         */
        void setLinearDamping(float linearDamping);
        float getLinearDamping();

        /**
         * 设置刚体转动运动中的阻尼。逐渐减少角速度
         * 单位是：角速度/每秒 angularDamping/s
         */
        void setAngularDamping(float angularDamping);
        float getAngularDamping();

        /**
         * 应用线性运动的冲量。改变线性运动的速度，如果point不在质心，则还会产生角速度
         */
        void applyLinearImpulse(const SqVec2 &impulse, const SqVec2 &point, bool wake);
        void applyLinearImpulseToCenter(const SqVec2 &impulse, bool wake);

        /**
         * 应用一个角冲量，改变角速度
         *
         */
        void applyAngularImpulse(float impulse, bool wake);

        /**
         * gravityScale*世界重力 = 当前这个刚体所受到的重力
         */
        void setGravityScale(float gravityScale);
        float getGravityScale();

        /**
         * 在刚体指定的点上应用一个力。产生线性运动和角运动
         * @param force 力的大小和方向
         * @param point 刚体上shape上的本地坐标
         * @param wake 刚体当前可能处于Sleep状态,处于Sleep状态不能应用力
         */
        void applyForce(const SqVec2 &force, const SqVec2 &point, bool wake);

        /**
         * 只往质点上应用一个力，所以只能产生线性运动的线性速度，不会产生角速度
         */
        void applyForceToCenter(const SqVec2 &force, bool wake);

        /**
         * 应用一个力矩，产生角速度
         */
        void applyTorque(float torque, bool wake);

        void setLinearVelocity(const SqVec2 &linearVelocity);
        const SqVec2 &getLinearVelocity();
        SqVec2 getLinearVelocityFromWorldPoint(const SqVec2 &worldPoint);
        void setAngularVelocity(float angularVelocity);
        float getAngularVelocity();

        void setBullet(bool flag);
        bool isBullet();

        bool isEnableSleep();
        void setEnableSleep(bool e);
        inline float getSleepThreshold() { return sleepThreshold; };
        inline void setSleepThreshold(float t) { sleepThreshold = t; };
        SqVec2 getLocalCenterOfMass();
        SqVec2 getWorldCenterOfMass();

        /**
         * 当粒子碰撞到Body时，是否应用粒子的压力到Body上
         * 默认为true
         */
        void setEnableParticlePressure(bool b);
        inline bool isEnableParticlePressure() { return bodyFlag & SqBodyFlags::sq_enableParticlePressure; };

        /**
         * 当粒子碰撞到Body时，是否应用阻力到Body上
         * 默认为true
         */
        void setEnablePartcileDamping(bool b);
        inline bool isEnableParticleDamping() { return bodyFlag & SqBodyFlags::sq_enableParticleDamping; };
    };
}
