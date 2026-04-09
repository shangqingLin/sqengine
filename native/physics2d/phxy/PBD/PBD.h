#pragma once
#include "../common/SqTypeDefine.h"
#include "../common/math/SqVec2.h"
#include "../common/math/SqTransform.h"

namespace phxy
{
    // 拉伸约束定义：一个点与另一个点形成一条边
    struct PBDStretchDef
    {
        int i1 = -1;
        int i2 = -1;
    };

    // 弯曲约束定义：三点角约束（i2 为中间点）
    struct PBDBendDef
    {
        int i1 = -1;
        int i2 = -1;
        int i3 = -1;
    };

    struct PBDStretch
    {
        int i1, i2;
        float invMass1, invMass2;
        float L;
        float lambda;
        float spring;
        float damper;
    };

    struct PBDBend
    {
        int i1, i2, i3;
        float invMass1, invMass2, invMass3;
        float invEffectiveMass;
        float lambda;
        float L1, L2;
        float alpha1, alpha2;
        float spring;
        float damper;
    };

    /**
     * 距离约束的算法
     */
    enum class PBDStretchingModel
    {
        // 使用PBD距离约束
        pbdStretchingModel,

        // 使用XPBD距离约束
        xpbdStretchingModel
    };

    /**
     * 实现弯曲约束有4种算法
     * 1、角度约束，即Angle
     * 2、距离约束
     * 3、Hehgiht Based
     * 4、Triangle Based 这是最高级的一种算法
     */
    enum class PBDBendingModel
    {
        springAngleBendingModel = 0,
        pbdAngleBendingModel,
        xpbdAngleBendingModel,
        pbdDistanceBendingModel,
        pbdHeightBendingModel,
        pbdTriangleBendingModel
    };

    struct PBDTuning
    {
        PBDTuning()
        {
            stretchingModel = PBDStretchingModel::pbdStretchingModel;
            bendingModel = PBDBendingModel::pbdAngleBendingModel;
            damping = 0.0f;
            stretchStiffness = 1.0f;
            stretchHertz = 1.0f;
            stretchDamping = 0.0f;
            bendStiffness = 0.5f;
            bendHertz = 1.0f;
            bendDamping = 0.0f;
            isometric = false;
            fixedEffectiveMass = false;
            warmStart = false;
        }

        /**
         * 距离约束
         */
        PBDStretchingModel stretchingModel;

        /**
         *
         * 弯曲约束
         */
        PBDBendingModel bendingModel;

        /**
         * 设置阻尼
         */
        float damping;

        /**
         * 拉伸刚度，越大拉伸时越不容易变长
         */
        float stretchStiffness;
        float stretchHertz;

        /**
         * 拉伸阻尼，越大阻尼越大，减少振荡
         */
        float stretchDamping;

        /**
         * 弯曲刚度，控制橡皮筋折弯的难度
         */
        float bendStiffness;
        float bendHertz;

        /**
         * 弯曲阻尼，防止橡皮筋甩动过度
         */
        float bendDamping;

        /**
         * 是否弯曲约束中，是否使用等距模式，确保橡皮筋形变合理
         */
        bool isometric;
        bool fixedEffectiveMass;
        bool warmStart;
    };

    struct PBDDef
    {

        /**
         * PBD中的粒子的坐标点。（必须提供）
         */
        SqVec2 *vertices = nullptr;

        /**
         * 每个粒子的初始速度，可以不提供
         */
        SqVec2 *velocities = nullptr;

        /**
         * vertices数组中元素的个数
         */
        int count = 0;

        /**
         * 每个粒子的质量，质量为0的点为静态点，不会被模拟的
         */
        float *masses = nullptr;

        /**
         * 定义距离约束。即你可以定义哪些点与哪些是链接的。设置任意的拓扑结构
         * 为空时默认按绳子链式 (i, i+1) 自动生成
         */
        const PBDStretchDef *stretchConstraints = nullptr;
        int stretchCount = 0;

        // 可选：自定义弯曲三元组；为空时：
        // 1) 若已提供自定义拉伸边且 autoGenerateBendConstraints=true，则从 1-ring 邻居自动生成
        // 2) 否则按绳子链式 (i, i+1, i+2) 自动生成
        const PBDBendDef *bendConstraints = nullptr;
        int bendCount = 0;
        bool autoGenerateBendConstraints = true;

        SqVec2 gravity;
        PBDTuning tuning;
    };

    struct SqPBDRayCastOut : public SqCastOutput
    {
        int pointAIndex{-1};
        int pointBIndex{-1};
    };

    struct PBDParticle
    {

        /**
         * 存储每个点初始化时的位置，即RopeDef中的vertices指定的位置，模拟过程中这个不变。
         */
        SqVec2 m_bindPositions;

        /**
         * 顶点当前帧的位置
         */
        SqVec2 m_ps;

        /**
         * 顶点上一帧的位置
         */
        SqVec2 m_p0s;

        /**
         * 顶点速度
         */
        SqVec2 m_vs;

        /**
         * 质量
         * 注意：没有质量的点是不会被模拟的，相当于静态的点，只有有质量的点才是动态的点
         */
        float m_invMasses;

        void *userData{nullptr};
    };

    class PBD
    {
    public:
        PBD();
        ~PBD();

        ///
        void Create(const PBDDef &def);

        ///
        void SetTuning(const PBDTuning &tuning);

        void Step(float timeStep, int iterations);

        /**
         * 将整个PBD重置到初始状态，所有点的位置都回到初始位置，速度为0
         */
        void Reset();

        /**
         * 设置指定点的线速度
         * @param pointIndex 点的索引。就是你创建时点在数组中的顺序，0表示第一个点，1表示第二个点，以此类推。
         * PBD类中出现的pointIndex的都是指这个点的索引。
         */
        void setLinearVelocity(int pointIndex, const SqVec2 &);

        /**
         * 将点移动一定的偏移量
         */
        void move(int pointIndex, const SqVec2 &);

        // 设置指定的点的位置
        void position(int pointIndex, const SqVec2 &);

        /**
         * 设置指定的点的质量
         * 如果质量设置为0，则点为静态的点，不会被模拟的
         * @param mass
         */
        void setMass(int pointIndex, float mass);

        inline float getMass(int pointIndex) const
        {
            return particles[pointIndex].m_invMasses > 0.f ? 1.0f / particles[pointIndex].m_invMasses : 0.f;
        }

        /**
         * 获取所有点的当前位置数据，返回一个Vec2数组，数组长度为创建时指定的点的数量
         */
        inline const SqVec2 &getPointPosition(int pointIndex) const { return particles[pointIndex].m_ps; };

        inline int getPointCount() const { return m_count; };

        /**
         * 获取当前所有点的速度
         */
        inline const SqVec2 &getVelocity(int pointIndex) const { return particles[pointIndex].m_vs; };

        inline void setTransform(const SqTransform &transform) { this->transform = transform; };

        /**
         * 进行线段的射线检测
         */
        void rayCast(SqPBDRayCastOut &output, const SqRayCastInput &input) const;

        inline void setUserData(int pointIndex, void *data) { particles[pointIndex].userData = data; };
        inline void *getUserData(int pointIndex) { return particles[pointIndex].userData; };
        inline void setPaused(bool paused)
        {
            this->paused = paused;
        }

#ifdef PHYSICS_DEBUG
        void Draw(phxy::SqDebugDraw *draw) const;
#endif

    private:
        // 释放并重置所有内部缓存
        void clear();
        // 质量变化后，同步约束里的质量参数与弯曲预计算量
        void syncConstraintMassesAndPrecompute();
        // 索引保护：避免非法 pointIndex 访问
        bool isValidPointIndex(int index) const;

        void SolveStretch_PBD();
        void SolveStretch_XPBD(float dt);
        void SolveBend_PBD_Angle();
        void SolveBend_XPBD_Angle(float dt);
        void SolveBend_PBD_Distance();
        void SolveBend_PBD_Height();
        void SolveBend_PBD_Triangle();
        void ApplyBendForces(float dt);
        void updateTuning();

        int m_count;
        int m_stretchCount;
        int m_bendCount;

        bool paused = false;

        SqTransform transform;

        PBDParticle *particles;

        PBDStretch *m_stretchConstraints;
        PBDBend *m_bendConstraints;

        SqVec2 m_gravity;

        PBDTuning m_tuning;
    };
};
