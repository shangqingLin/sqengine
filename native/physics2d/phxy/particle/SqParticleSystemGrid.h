#pragma once
#include "b2ParticleSetting.h"
#include "../common/collection/SqArray.h"
#include "../common/math/SqAABB.h"
#include "sq_world_callbacks.h"
#include "../../../engine/core/sqstd/Array.h"


/**
 * 粒子系统将整个空间划分为网格，这个tag就是标记是网格上的哪个单元的index，index唯一标记单元格。
 * 根据当前粒子的位置，为此位置生成一个唯一的tag，即标记每个位置。
 * 随着粒子位置的变化，他所处的tag也会变化，所以这里重新根据位置计算粒子所在的网格上哪个单元格。
 *
 * 构建的网格的原点在左下角，范围是[0,4095]
 * tag是按从左到右、从上到下的顺序。
 */

namespace phxy
{
    struct GridCell;
    class SqParticleSystem;
    uint32 computeTag(float x, float y);
    uint32 computeRelativeTag(uint32 tag, int x, int y);
    uint32 computeRow(uint32 tag);
    uint32 computeColumn(uint32 tag);

    /// InsideBoundsEnumerator enumerates all particles inside the given bounds.
    class InsideBoundsEnumerator
    {
    public:
        /// Construct an enumerator with bounds of tags and a range of proxies.
        InsideBoundsEnumerator(unsigned int lower, unsigned int upper, const GridCell *first, const GridCell *last);

        /// Get index of the next particle. Returns sq_invalidParticleIndex if
        /// there are no more particles.
        int GetNext();

    private:
        /// The lower and upper bound of x component in the tag.
        unsigned int m_xLower, m_xUpper;
        /// The lower and upper bound of y component in the tag.
        unsigned int m_yLower, m_yUpper;
        /// The range of proxies.
        const GridCell *m_first;
        const GridCell *m_last;
    };

    /**
     * 记录哪个粒子所在的格子
     */
    struct GridCell
    {
        /**
         * 粒子在Buffer中的索引
         */
        int particleId;

        // 粒子所在格子
        unsigned int tag;

        SqParticleSystem *system;

        friend inline bool operator<(const GridCell &a, const GridCell &b)
        {
            return a.tag < b.tag;
        }
        friend inline bool operator<(unsigned int a, const GridCell &b)
        {
            return a < b.tag;
        }
        friend inline bool operator<(const GridCell &a, unsigned int b)
        {
            return a.tag < b;
        }
    };

    class SqParticleSystemGrid
    {
    private:
        SqArray<GridCell> cells;
        sqstd::Array<GridCell> sortScratch;
        SqParticleSystem *system;
        static bool IsProxyInvalid(const GridCell &proxy);
        void radixSortByTag();

    public:
        friend class SqParticleSystem;
        SqParticleSystemGrid(SqParticleSystem *);
        inline const SqArray<GridCell> &getCells() const { return cells; };

        /**
         * 发射射线检测粒子
         */
        void rayCast(b2RayCastCallback *callback, const SqVec2 &point1, const SqVec2 &point2);

        /**
         * 查询网格中之前区域AABB内有哪些粒子
         */
        void queryAABB(b2QueryCallback *callback, const SqAABB &aabb);

        void addCell(int particleId);

        InsideBoundsEnumerator GetInsideBoundsEnumerator(const SqAABB &aabb) const;

        /**
         * 从Grid中删除被销毁的粒子
         */
        void removeInvaildParticle();

        /**
         * 帧循环
         */
        void update();

        void updateAndSortNewPos();
    };

}
