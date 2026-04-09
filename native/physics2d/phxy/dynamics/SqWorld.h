#pragma once

#include "../collision/broadphase/SqBroadPhase.h"
#include "../collision/narrowphase/SqNarrowPhase.h"
#include "../collision/SensorManager.h"

#include "../geometry/shape/SqShapeDefine.h"
#include "../common/SqConfig.h"
#include "../common/collection/SqArray.h"
#include "../common/collection/SqIdPool.h"

#include "SqStepContext.h"
#include "SqSolverSet.h"
#include "SqBody.h"
#include "SqIsland.h"
#include "./joint/SqJoint.h"
#include "./joint/SqJointDef.h"
#include "solver/SqSolver.h"
#include "SqSoftConstraint.h"
#include "SqConstraintGraph.h"

#include "SqWorldDraw.h"

namespace phxy
{

    enum SqSetType
    {
        sq_staticSet = 0,
        sq_disabledSet = 1,
        sq_awakeSet = 2,
        sq_firstSleepingSet = 3,
    };

    class SqParticleSystemWorld;
    class SqCapsuleShape;

    class SqWorld
    {
    private:
        /**
         * 使用这个变量标记当前的World正在Step中
         * 在Step中的World有很多操作是不能进行，我们可以使用这个做判断避免外部不正确设置
         */
        bool lock = false;

        SqVec2 gravity;
        SqBroadPhase broadPhase;
        SqNarrowPhase narrowPhase;
        SqSolver solver;
        SensorManager sensor;

        SqArray<SqBody> bodyArray;
        SqIdPool bodyIdPool;

        SqArray<SqIsland> islandArray;
        SqIdPool islandIdPool;

        SqArray<SqJoint *> jointArray;
        SqIdPool jointIdPool;

        SqArray<SqShape *> shapeArray;
        SqIdPool shapeIdPool;

        /**
         * 所有的事件都会在下一帧清除的，所以外部记得不要存储这里的数据
         */
        SqArray<SqContactBeginEvent> contactBeginEvents;
        SqArray<SqContactEndEvent> contactEndEvents;
        SqArray<SqContactHitUpdateEvent> contactHitEvents;
        SqArray<SqSensorBeginEvent> sensorBeginEvents;
        SqArray<SqSensorEndEvent> sensorEndEvents;

        SqParticleSystemWorld *particleSystem;

        SqConstraintGraph constraintGraph;
        SqFrictionCallback *frictionCallback;
        SqRestitutionCallback *restitutionCallback;

        SqWorldDraw worldDebugDraw;

        /**
         * 为了提升性能，会根据SqSetType的定义将不同的状态的Body放到不同的Set
         *
         * 比如：
         * 1、sq_staticSet静态的Body放到一个Set
         * 2、sq_disabledSet，即被禁用的Body放到一个Set
         * 3、sq_awakeSet当前正在模拟的Body放到一个Set
         *
         * solverSets中：
         * 第一个为：sq_staticSet 的 Set
         * 第二个为：sq_disabledSet 的 Set
         * 第三个为：sq_awakeSet 的 Set
         * 之后全部都是为Sleep状态的 Set
         */
        SqArray<SqSolverSet> solverSets;

        // 记录当前在缓冲池中的SqSolverSet
        SqIdPool sloverIdPool;

        SqSoftConstraint staticSoftness;
        SqSoftConstraint contactSoftness;
        float contactSpeed;
        float contactHertz;
        float contactDampingRatio;

        bool enableContinuous;
        bool enableSleep;
        bool enableWarmStart;

        /**
         * 在某些Joint内需要用到帧时间来计算即时的相关属性
         */
        float dt;

        SqBroadPhaseCustomFilterFcn *customBroadPhaseFilterFcn;
        void *customBroadPhaseFilterContext;

        BeforeSolveCustomFcn *beforeSoveCustomFunc;

        void initSloverSet(int setIndex);
        void moveSleepSetToAwakeSet(int sleepSet);
        void mergeSolverSets(int setId1, int setId2);
        void removeSloverSet(SqSolverSet *set);
        void removeSloverSet(int setIndex);
        SqSolverSet *createSloverSet();
        void narrowPhaseStep(SqStepContext &);
        void createJoint(SqJoint *, SqBaseJointDef &);

        SqIsland *createIsland(int setId);
        inline SqIsland *getIsland(int islandId) { return islandArray.get(islandId); };
        void removeIsland(int);
        void mergeAwakeIslands();
        void linkContactToIsland(SqContact *contact);
        void unlinkContactFromIsland(SqContact *contact);
        void linkJointToIsland(SqJoint *joint, bool mergeIslands);
        void unlinkJointFromIsland(SqJoint *joint);

    public:
        friend bool pairQueryCallback(int proxyId, void *userData, void *context);
        friend class SqWorldDraw;
        friend class SqBody;
        friend class SqBroadPhase;
        friend class SqNarrowPhase;
        friend class SqContactManager;
        friend class SqSolver;
        friend class SqContactSolver;
        friend class SqBodySolver;
        friend class SqTimeOfImpactSolver;
        friend class SqJoint;
        friend class SqDistanceJoint;
        friend class SqJointSolver;
        friend class SqSolverSet;
        friend class SqIsland;
        friend class SqRevoluteJoint;
        friend class SqContactSolverPGS_Soft;
        friend class SqParticleSystemWorld;
        friend class SqParticleSystem;
        friend class SqShape;
        friend class SensorManager;
        SqWorld();

// 一帧一帧地调试使用
#if 1
        bool _enableNextStep = false;
        bool _triggerNextStep = false;
        inline void enableNextStep(bool b) { _enableNextStep = b; };
        inline void nextStep() { _triggerNextStep = true; };
#endif

        inline void setParticleSystem(SqParticleSystemWorld *particleSystem) { this->particleSystem = particleSystem; };
        inline SqBroadPhaseCustomFilterFcn *getSqBroadPhaseCustomFilterFcn() { return customBroadPhaseFilterFcn; };
        inline void *getSqBroadPhaseCustomFilterContext() { return customBroadPhaseFilterContext; };

        SqSolverSet *getSloverSet(int index) const;
        inline const SqArray<SqSolverSet> &getSolverSets() { return solverSets; };
        inline SqBroadPhase &getBroadPhase() { return broadPhase; };

        inline const SqArray<SqContactBeginEvent> &getBeginContact() const { return contactBeginEvents; };
        inline const SqArray<SqContactEndEvent> &getEndContact() const { return contactEndEvents; };
        inline const SqArray<SqContactHitUpdateEvent> &getHitUpdateContact() const { return contactHitEvents; };
        inline const SqArray<SqSensorBeginEvent> &getSensorBeginContact() const { return sensorBeginEvents; };
        inline const SqArray<SqSensorEndEvent> &getSensorEndContact() const { return sensorEndEvents; };

        int createBody(const SqBodyDef *def);
        inline SqBody *getBody(int bodyIndex) { return bodyArray.get(bodyIndex); };
        void removeBody(int bodyId);
        bool bodyIsDestroy(int bodyId);

        SqShape *createShape(int bodyId, const SqShapeDef &def);
        void destroyShape(SqShape *shape);

        SqJoint *createRevoluteJoint(SqRevoluteJointDef &def);
        SqJoint *createWheelJoint(SqWheelJointDef &def);
        SqJoint *createWeldJoint(SqWeldJointDef &def);
        SqJoint *createMouseJoint(SqMouseJointDef &def);
        SqJoint *createDistanceJoint(SqDistanceJointDef &def);
        SqJoint *createMotorJoint(SqMotorJointDef &def);
        SqJoint *createPrimaticJoint(SqPrismaticJointDef &def);
        void destroyJoint(SqJoint *, bool wakeBodies);
        SqJoint *getJoint(int jointIndex);

        inline void setGravity(const SqVec2 &gravity) { this->gravity = gravity; };
        inline const SqVec2 &getGravity() { return gravity; };
        inline bool isEnableWarmdStart() { return enableWarmStart; };
        void debugDraw(SqDebugDraw *);
        void beforeStep();
        void step(float dt, int subStepCount);
        void setBroadPhaseCustomFilter(SqBroadPhaseCustomFilterFcn *fuc, void *context);
        void setLengthUnitsPerMeter(float lengthUnits);
        float getLengthUnitsPerMeter(void);

        void collideMover(SqCapsuleShape *mover, SqShapeFilter filter, SqPlaneResultFcn *callback, void *context);
        float castMover(const SqCapsuleShape *mover, SqVec2 translation, SqShapeFilter filter);
        SqTreeStats castRay(SqVec2 origin, SqVec2 translation, SqShapeFilter filter, SqCastResultFcn *fcn, void *context);
        SqTreeStats castShape(const SqShapeProxy *proxy, SqVec2 translation, SqShapeFilter filter, SqCastResultFcn *fcn, void *context);
        SqTreeStats overlapAABB(const SqAABB &aabb, const SqShapeFilter &filter, SqOverlapResultFcn *fcn, void *context);
        SqTreeStats overlapShape(const SqShapeProxy *proxy, const SqShapeFilter &filter, SqOverlapResultFcn *fcn, void *context);
    };
}