#pragma once

#include "../common/math/SqVec2.h"
#include "../common/collection/SqArray.h"
#include "SqParticleColor.h"
#include "SqParticleGroup.h"
#include "SqParticleTriad.h"
#include "SqParticlePair.h"
#include "SqParticleBodyContact.h"
#include "SqParticleContact.h"

namespace phxy
{
    class SqParticleSystem;

    /**
     * 有些功能没有开启就没必要为此分配内存了
     * 等到需要开启的是才分配内存
     */
    template <typename T>
    struct LazyBuffer
    {
        LazyBuffer()
        {
            data = nullptr;
        }
        T *data;
    };

    class SqParticleSystemMemory
    {

    private:
        /**
         * 当前总共分配了多少内存。单位是一个粒子
         */
        int particleCapacity = 0;

        /**
         * 当前粒子系统中粒子的个数
         */
        int m_count = 0;

        SqParticleSystem *system;

        /**
         * 只要粒子发生Destroy、RotateBuffer操作，那么粒子在Buffer中的index是改变
         * 这是为了保持粒子的数据在内存是连续，即如果你中间删除一个粒子，位于这个粒子之后的粒子都要往前面移动，则后面的粒子的index全部改变了
         * 所以记得不要使用index作为一个粒子的唯一ID。PartilceSystem原来很多的API都是对外提供index来操作粒子，这是不对的
         * 所以我们需要使用b2Particle实例来唯一标记一个粒子
         */

        int *particleIdToBufferIndexMap = nullptr;

        /**
         * 记录每个粒子当前的位置
         */
        SqVec2 *m_positionBuffer = nullptr;

        /**
         * 用于存储每个粒子的权重值，因为粒子是没有体积或质量单位，仅由粒子间的相对分布决定的一个量。
         * 这个权重值用来表示粒子与周边的粒子的拥挤程度，模拟局部密度或拥挤程度。
         * 这个值在粒子与粒子、粒子与刚体发生碰撞时计算，并最终在ComputeWeight统计
         *
         * 1、可以用于渲染粒子。粒子拿到这个权重值来决定粒子颜色在边缘与其他粒子的深浅程度
         * 2、用于接下计算粒子的 ComputeDepth(), SolveStaticPressure() 、 SolvePressure()计算这些力是需要用到权重
         *
         * 每个粒子的权重计算公式为：
         * 1-distance/diameter(直径)
         * 其中distance是两个碰撞的粒子此时的距离，是两个的粒子质心点处的距离
         * 当权重值为0表示不与任何粒子发射碰撞，为1时表示两个粒子完全重合了
         */
        float *m_weightBuffer = nullptr;

        /**
         * 记录每个粒子的颜色。用于渲染粒子
         */
        LazyBuffer<SqParticleColor> m_colorBuffer;

        /**
         * 记录每个粒子的受力
         */
        LazyBuffer<SqVec2> m_forceBuffer;

        /// When any particles have the flag b2_staticPressureParticle,
        /// m_staticPressureBuffer is first allocated and used in
        /// SolveStaticPressure() and SolvePressure().  It will be reallocated on
        /// subsequent CreateParticle() calls.
        LazyBuffer<float> m_staticPressureBuffer;

        /// m_accumulationBuffer is used in many functions as a temporary buffer
        /// for scalar values.
        float *m_accumulationBuffer = nullptr;

        /// When any particles have the flag b2_tensileParticle,
        /// m_accumulation2Buffer is first allocated and used in SolveTensile()
        /// as a temporary buffer for vector values.  It will be reallocated on
        /// subsequent CreateParticle() calls.
        LazyBuffer<SqVec2> m_accumulation2Buffer;

        /// When any particle groups have the flag b2_solidParticleGroup,
        /// m_depthBuffer is first allocated and populated in ComputeDepth() and
        /// used in SolveSolid(). It will be reallocated on subsequent
        /// CreateParticle() calls.
        LazyBuffer<float> m_depthBuffer;

        /**
         * 记录每个粒子所在的粒子组。粒子可以不同一定设置到粒子组中的
         */
        LazyBuffer<SqParticleGroup *> m_groupBuffer;

        /**
         * 每一个粒子还可以设置一个用户数据
         */
        LazyBuffer<void *> m_userDataBuffer;

        /**
         *
         * 存储每个粒子寿命
         * 这里记录不是粒子生命的时长，是帧数。虽然我们在SetParticleLifetime指定的是时间秒,但内部会被转换为帧数，所以为int类型
         */
        LazyBuffer<int> m_expirationTimeBuffer;
        LazyBuffer<int> m_sortExpirationTimeBuffer;

        // LazyBuffer<int> m_lastBodyContactStepBuffer;

        // 记录这个粒子与多少个Box2D中的刚体发生碰撞了
        // LazyBuffer<int> m_bodyContactCountBuffer;
        // LazyBuffer<int> m_consecutiveContactStepsBuffer;

        // b2GrowableBuffer<int> m_stuckParticleBuffer;
        // b2GrowableBuffer<GridCell> m_proxyBuffer;

        // 记录粒子与粒子的碰撞对
        SqArray<SqParticleContact> m_contactBuffer;

        // 记录粒子与Body的碰撞对
        SqArray<SqParticleBodyContact> m_bodyContactBuffer;

        SqArray<SqParticlePair> m_pairBuffer;
        SqArray<SqParticleTriad> m_triadBuffer;

        template <typename T>
        void freeBuffer(T **b);

        template <typename T>
        T *reallocateBuffer(T *oldBuffer, int oldCapacity, int newCapacity);

        template <typename T>
        void reallocateLazyBuffer(LazyBuffer<T> *buffer, int oldCapacity, int newCapacity);

        template <typename T>
        T *requestBuffer(T *buffer);

        void requestUserDataBuffer();
        void requestLifeTimeBuffer();
        void requestGroupBuffer();
        void requestColorBuffer();
        void requestDepthBuffer();
        void requestAccumulation2Buffer();
        void requestStaticPressureBuffer();
        void resetStaticPressureBuffer();
        void requestForceBuffer();
        void resetForceBuffer();
        void destroyLifeTimeBuffer();
        void resetAccumulation2Buffer();    
        void resetAccumulationBuffer();
    public:
        friend class SqParticleSystem;
        friend class SqParticle;

        SqParticleSystemMemory(SqParticleSystem *);
        ~SqParticleSystemMemory();

        int addParticle(int);
        int findParticle(int index) const;
        void removeParticle(int *);
        void resize(int particleNum);
        inline int getCapacity() { return particleCapacity; };
    };
}