#pragma once
#include "../SqStepContext.h"
#include "./SqJointDef.h"
#include "../SqSoftConstraint.h"

namespace phxy
{
    class SqJointSim;
    class SqWorld;

    struct SqJointEdge
    {
        int bodyId{SQ_NULL_INDEX};
        int prevKey{SQ_NULL_INDEX};
        int nextKey{SQ_NULL_INDEX};
    };

    typedef void SqJointDeleteCallback(void *userContext);

    class SqJoint
    {
    protected:
        SqJointType type;

        /**
         * 在SqWorld中的jointArray数组中的index
         */
        int jointIndex = SQ_NULL_INDEX;

        /**
         * SqJointSim在SqSolverSet中的索引
         */
        int simIndex = SQ_NULL_INDEX;

        /**
         * 当前这个Joint在World的哪个Set上
         */
        int setIndex = SQ_NULL_INDEX;

        // 在island中使用链表方式存储碰撞点
        int islandPrev{SQ_NULL_INDEX};
        int islandNext{SQ_NULL_INDEX};
        int islandId{SQ_NULL_INDEX}; // 这个碰撞点在哪个island中
        bool isMarkedForIslandSplit = false;

        bool collideConnected = false;
        bool enable = true;

        /**
         * 记录哪两个body
         */
        SqJointEdge edges[2];
        SqWorld *world;

        // 为了配合业务层的组件，组件内需要知道Joint是否被移除了所以加了这个回调函数
        SqJointDeleteCallback *deleteCallback = nullptr;
        void *deleteCallbackContext = nullptr;

        virtual void solvePrepare(const SqStepContext &context) = 0;

    public:
        friend class SqWorld;
        friend class SqIsland;
        friend class SqBody;
        friend class SqSolverSet;
        friend class SqWorldDraw;
        friend bool SqShouldBodiesCollide(SqWorld *world, SqBody *bodyA, SqBody *bodyB);
        virtual ~SqJoint() = default;

        inline SqJointType getType() const { return type; };
        virtual void prepare(const SqStepContext &context);
        virtual void warmStart(const SqStepContext &context) = 0;
        virtual void solve(const SqStepContext &, bool useBias) = 0;
        virtual void reset();

        inline void setDeleteCallback(SqJointDeleteCallback* deleteCallback, void *context)
        {
            this->deleteCallback = deleteCallback;
            this->deleteCallbackContext = context;
        }

        inline void setCollideConnected(bool c) { collideConnected = c; };
        bool isEnable();
        SqJointSim *getJointSim() const;
        const SqTransform& getLocalFrameATransform() const;
        void setLocalFrameATransform(const SqTransform &);
        void setConstraintHertz(float);
        void setConstraintDampingRatio(float);
        void setForceThreshold(float);
        void setTorqueThreshold(float);
        const SqTransform& getLocalFrameBTransform() const;
        void setLocalFrameBTransform(const SqTransform &);
        virtual void setEnableSim(bool b) { enable = b; };
        inline bool isEnableSim() { return enable; };
    };
}