#include "SqParticleSystemGrid.h"
#include "../../../engine/core/core.h"
#include "b2ParticleSetting.h"
#include "SqParticleSystem.h"
#include "../../../engine/core/sqstd/StackTempArenaAllocator.h"
#include <chrono>
#include <algorithm>
using namespace phxy;


//测试使用，比较std::sort和radixSortByTag 这两种排序算法哪种比较快

#ifndef SQ_PARTICLE_GRID_BENCHMARK
#define SQ_PARTICLE_GRID_BENCHMARK 0
#endif

#ifndef SQ_PARTICLE_GRID_COMPARE_STD_SORT
#define SQ_PARTICLE_GRID_COMPARE_STD_SORT 0
#endif

#if SQ_PARTICLE_GRID_BENCHMARK
namespace
{
	struct GridBenchCounter
	{
		double totalMs{0.0};
		unsigned int calls{0};
	};

	static GridBenchCounter g_updateBench;
	static GridBenchCounter g_updateNewPosBench;
#if SQ_PARTICLE_GRID_COMPARE_STD_SORT
	static GridBenchCounter g_updateStdBench;
	static GridBenchCounter g_updateNewPosStdBench;
	static sqstd::Array<GridCell> g_stdCompareBuffer;
#endif

	inline double benchNowMs()
	{
		using Clock = std::chrono::steady_clock;
		using Ms = std::chrono::duration<double, std::milli>;
		return std::chrono::duration_cast<Ms>(Clock::now().time_since_epoch()).count();
	}

	inline void benchRecord(const char *name, GridBenchCounter &counter, double elapsedMs)
	{
		counter.totalMs += elapsedMs;
		++counter.calls;
		if (counter.calls >= 1000)
		{
			printf("[ParticleGridBench] %s avg %.6f ms over %u calls\n", name, counter.totalMs / counter.calls, counter.calls);
			counter.totalMs = 0.0;
			counter.calls = 0;
		}
	}

#if SQ_PARTICLE_GRID_COMPARE_STD_SORT
	inline void benchStdSort(const char *name, GridBenchCounter &counter, const SqArray<GridCell> &source)
	{
		const int count = source.getCount();
		g_stdCompareBuffer.setTempAllocator(sqstd::StackTempArenaAllocator::getInstance());
		g_stdCompareBuffer.resize(count);
		GridCell *dst = g_stdCompareBuffer.begin();
		const GridCell *src = source.begin();
		for (int i = 0; i < count; ++i)
		{
			dst[i] = src[i];
		}

		const double t0 = benchNowMs();
		std::sort(g_stdCompareBuffer.begin(), g_stdCompareBuffer.begin() + count);
		benchRecord(name, counter, benchNowMs() - t0);
		g_stdCompareBuffer.freeData();
	}
#endif
}
#endif

//======================测试代码结束====================

/**
 * 我们这里需要构建一个二维网格将粒子放置到网格中实现粒子的碰撞检测等逻辑，我们需要计算这个位置对应到网格上哪个单元格。
 * 所以，我们需要构建一个映射关系：粒子位置->网格位置、或网格位置->粒子的位置。
 *
 * 如何实现：
 *
 * 1、单元格大小是：1 x 1
 *
 * 2、我们传统的思路：创建一个Map，将x和y当作key,value为grid的位置，这很明显我们无法构建这样的Map，因
 *  为粒子(x,y)是粒子真实游戏中的位置，这个位置是一个浮点数，有负值和正值。而且性能不好需要维护两个Map。
 *
 * 3、显然，我们需要将位置(x,y)合并为一个数字，这很符合哈希函数的特征，通过一对(x,y)计算出这个x，y对应的哈希值，而且这个哈希值中包含网格中位置的信息
 *  相当于使用坐标转换为一个哈希值（和哈希函数功能是一样的），用来标记粒子位置所对应的网格。
 */
uint32 phxy::computeTag(float x, float y)
{

	/**
	 * x和y + 2048 是将 [-2048,2047] 范围转换到[0,4095] 超出这个则循环反复，比如4096对应到0，这样就可以使用无符号整数来个表示网格单位。
	 *
	 */
	unsigned int tag = 0u;

	/**
	 * 处理超出 4095范围的情况,4095为占12位，大于4096占最少13位
	 *
	 * 4095 位模式为12位的：   			   1111 1111 1111
	 * 4096 位模式为13位的：  			 1 0000 0000 0000
	 * ~4096 为：1111 1111 1111 1111 1110 1111 1111 1111
	 *
	 */

	unsigned int d = y + 2048.f;

	// 处理超出4096
	d &= 4095u; // 取消高位的1
	tag |= d << 12u;

	d = x + 2048.f;

	d &= 4095u;
	tag |= d;
	return tag;
}

uint32 phxy::computeRelativeTag(uint32 tag, int x, int y)
{
	uint32 column = computeColumn(tag);
	uint32 row = computeRow(tag);

	/**
	 * 这里是无符号整数和有符号整数相加，那么有符号整数会转换为无符号整数之后再相加的
	 *
	 * 会出现：
	 * unsigned int(0) + int(-1) = 4294967295 的情况
	 * 这是因为-1会转换为补码的形式再与无符号数值相加的，-1转换为补码为32位最大的无符号数值：1111 1111 1111 1111 1111 1111 1111 1111
	 * 所以 0000 0000 0000 0000 000 0000 + 1111 1111 1111 1111 1111 1111 1111 1111 = 1111 1111 1111 1111 1111 1111 1111 1111
	 * 这样你就得不到正确的行列：
	 * unsigned int(0) + int(-1) 相加之后我们想得到4095的行列（循环反复）
	 *
	 * 所以我们需要：row & 4095u、column & 4095u
	 */

	column += x;
	row += y;

	unsigned int nTag = 0u;
	nTag |= (row & 4095u) << 12u;
	nTag |= column & 4095u;

	return nTag;

	// return tag + (y << yShift) + (x << xShift);
}

uint32 phxy::computeColumn(uint32 tag)
{
	return tag & 4095u;
	// return tag >> 8u & 4095u;
}

uint32 phxy::computeRow(uint32 tag)
{
	return tag >> 12u;
	// return tag >> yShift;
}

InsideBoundsEnumerator::InsideBoundsEnumerator(
	unsigned int lower, unsigned int upper, const GridCell *first, const GridCell *last)
{
	m_xLower = computeColumn(lower);
	m_xUpper = computeColumn(upper);
	m_yLower = computeRow(lower);
	m_yUpper = computeRow(upper);
	m_first = first;
	m_last = last;
	SQ_ASSERT(m_first <= m_last);
}

int InsideBoundsEnumerator::GetNext()
{
	while (m_first < m_last)
	{
		unsigned int xTag = computeColumn(m_first->tag);
#if ENABELD_ASSERT
		unsigned int yTag = computeRow(m_first->tag);
		SQ_ASSERT(yTag >= m_yLower);
		SQ_ASSERT(yTag <= m_yUpper);
#endif
		if (xTag >= m_xLower && xTag <= m_xUpper)
		{
			return (m_first++)->particleId;
		}
		m_first++;
	}
	return sq_invalidParticleIndex;
}

SqParticleSystemGrid::SqParticleSystemGrid(SqParticleSystem *system) : system(system)
{
}

/**
 * 目前测试此排序方法确实比std::sort快
 * 
 * 因为这里的数据形态非常适合 radix sort。

	核心原因就几条：

	1. key 是固定宽度整数 `tag`  
	`std::sort` 要反复做比较（`O(n log n)`），radix 按字节分桶是 `O(4n)`，规模一大差距明显。

	2. 排序很高频  
	你这里每步都在排，`n log n` 的额外比较次数会被放大，radix 的线性成本更稳定。

	3. 内存访问模式更友好  
	radix 主要是顺序扫描/顺序写入，cache 命中通常更好；`std::sort` 的访问和分支更不规则。

	4. 分支更少  
	`std::sort` 依赖大量 `<` 判断和分区逻辑；radix 基本是位运算+计数，分支预测压力小。

	另外你现在的实测结论最关键：  
	同场景下“跑得更快”就说明在你当前粒子数量、Wasm 引擎、帧节奏下，这个 tradeoff 是成立的。理论只是辅助，profile 才是最终标准。
 * 
 */
void SqParticleSystemGrid::radixSortByTag()
{
	const int count = cells.getCount();
	if (count < 2)
	{
		return;
	}

	// Reuse a persistent scratch buffer to avoid per-step allocations.
	sortScratch.setTempAllocator(sqstd::StackTempArenaAllocator::getInstance());
	sortScratch.resize(count);

	GridCell *src = cells.begin();
	GridCell *dst = sortScratch.begin();

	unsigned int hist0[256] = {};
	unsigned int hist1[256] = {};
	unsigned int hist2[256] = {};
	unsigned int hist3[256] = {};

	for (int i = 0; i < count; ++i)
	{
		const unsigned int tag = src[i].tag;
		++hist0[tag & 0xFFu];
		++hist1[(tag >> 8u) & 0xFFu];
		++hist2[(tag >> 16u) & 0xFFu];
		++hist3[(tag >> 24u) & 0xFFu];
	}

	unsigned int offsets[256];
	const unsigned int *hists[4] = {hist0, hist1, hist2, hist3};
	for (int pass = 0; pass < 4; ++pass)
	{
		const unsigned int *hist = hists[pass];
		unsigned int sum = 0;
		for (int i = 0; i < 256; ++i)
		{
			offsets[i] = sum;
			sum += hist[i];
		}

		const unsigned int shift = static_cast<unsigned int>(pass * 8);
		for (int i = 0; i < count; ++i)
		{
			const unsigned int key = (src[i].tag >> shift) & 0xFFu;
			dst[offsets[key]++] = src[i];
		}

		GridCell *tmp = src;
		src = dst;
		dst = tmp;
	}

	// Even number of passes keeps output in `cells`. Keep fallback for safety.
	if (src != cells.begin())
	{
		GridCell *out = cells.begin();
		for (int i = 0; i < count; ++i)
		{
			out[i] = src[i];
		}
	}
	sortScratch.freeData();
}

void SqParticleSystemGrid::addCell(int particleId)
{
	GridCell *cell = cells.Add();
	cell->particleId = particleId;
	cell->system = system;
}

void SqParticleSystemGrid::queryAABB(b2QueryCallback *callback, const SqAABB &aabb)
{
	if (cells.getCount() == 0)
	{
		return;
	}
	const GridCell *beginProxy = cells.begin();
	const GridCell *endProxy = cells.end();
	const GridCell *firstProxy = std::lower_bound(beginProxy, endProxy, computeTag(system->m_inverseDiameter * aabb.lowerBound.x, system->m_inverseDiameter * aabb.lowerBound.y));
	const GridCell *lastProxy = std::upper_bound(firstProxy, endProxy, computeTag(system->m_inverseDiameter * aabb.upperBound.x, system->m_inverseDiameter * aabb.upperBound.y));
	for (const GridCell *proxy = firstProxy; proxy < lastProxy; ++proxy)
	{
		SqParticle *particle = system->findById(proxy->particleId);
		const SqVec2 &p = particle->getPosition();
		if (aabb.lowerBound.x < p.x && p.x < aabb.upperBound.x &&
			aabb.lowerBound.y < p.y && p.y < aabb.upperBound.y)
		{
			if (!callback->ReportParticle(system, particle->getId()))
			{
				break;
			}
		}
	}
}

void SqParticleSystemGrid::rayCast(b2RayCastCallback *callback, const SqVec2 &point1, const SqVec2 &point2)
{
	if (cells.getCount() == 0)
	{
		return;
	}
	SqAABB aabb;
	aabb.lowerBound = SqVec2::Min(point1, point2);
	aabb.upperBound = SqVec2::Max(point1, point2);
	float fraction = 1;
	// solving the following equation:
	// ((1-t)*point1+t*point2-position)^2=diameter^2
	// where t is a potential fraction
	SqVec2 v = point2 - point1;
	float v2 = SqVec2::Dot(v, v);
	InsideBoundsEnumerator enumerator = GetInsideBoundsEnumerator(aabb);
	int i;
	while ((i = enumerator.GetNext()) >= 0)
	{
		SqParticle *particle = system->findById(i);
		SqVec2 p = point1 - particle->getPosition();
		float pv = SqVec2::Dot(p, v);
		float p2 = SqVec2::Dot(p, p);
		float determinant = pv * pv - v2 * (p2 - system->m_squaredDiameter);
		if (determinant >= 0)
		{
			float sqrtDeterminant = sqrt(determinant);
			// find a solution between 0 and fraction
			float t = (-pv - sqrtDeterminant) / v2;
			if (t > fraction)
			{
				continue;
			}
			if (t < 0)
			{
				t = (-pv + sqrtDeterminant) / v2;
				if (t < 0 || t > fraction)
				{
					continue;
				}
			}

			SqVec2 n = p + t * v;
			n = SqVec2::Normalize(n);
			float f = callback->ReportParticle(system, i, point1 + t * v, n, t);
			fraction = min(fraction, f);
			if (fraction <= 0)
			{
				break;
			}
		}
	}
}

/**
 * 查找指定的aabb区域内有哪些粒子
 */
InsideBoundsEnumerator SqParticleSystemGrid::GetInsideBoundsEnumerator(const SqAABB &aabb) const
{
	// 将位置和大小表示的aabb区域转换为网格区域的表示的区域
	unsigned int lowerTag = computeTag(system->m_inverseDiameter * aabb.lowerBound.x - 1, system->m_inverseDiameter * aabb.lowerBound.y - 1);
	unsigned int upperTag = computeTag(system->m_inverseDiameter * aabb.upperBound.x + 1, system->m_inverseDiameter * aabb.upperBound.y + 1);

	const GridCell *beginProxy = cells.begin();
	const GridCell *endProxy = cells.end();
	const GridCell *firstProxy = std::lower_bound(beginProxy, endProxy, lowerTag);
	const GridCell *lastProxy = std::upper_bound(firstProxy, endProxy, upperTag);

	// printf("query low %u %u upperTag %u %u \n", computeColumn(lowerTag), computeRow(lowerTag), computeColumn(upperTag), computeRow(upperTag));

	return InsideBoundsEnumerator(lowerTag, upperTag, firstProxy, lastProxy);
}

bool SqParticleSystemGrid::IsProxyInvalid(const GridCell &proxy)
{
	return proxy.system->findById(proxy.particleId)->flags & sq_zombieParticle;
};

void SqParticleSystemGrid::removeInvaildParticle()
{
	cells.removeIf(SqParticleSystemGrid::IsProxyInvalid);
}

void SqParticleSystemGrid::update()
{
#if SQ_PARTICLE_GRID_BENCHMARK
	const double t0 = benchNowMs();
#endif

	// 计算每个粒子所在网格上哪个单元格
	const GridCell *endProxy = cells.end();
	for (GridCell *proxy = cells.begin(); proxy < endProxy; ++proxy)
	{
		SqParticle *particle = system->findById(proxy->particleId);
		const SqVec2 &p = particle->getPosition();

		/**
		 * 单元格的大小是1x1,但是我们物理实际中是以米为单位，这意味着位置并不是1x1的单元，即位置 p 为 (1,0)和(2,0) 并不一定是相邻的
		 * 我们应该构建一个以粒子直径为大小的格子，即如果粒子直径为0.1x0.1,但tag计算是1x1的大小，所以需要将位置0.1转换为1个单元格大小，即 ： 0.1 = 1
		 * 所以需要将位置除以直径，比如 p.x = 0.5,  0.5/0.1 = 5,即这个这个位置x轴上处于第5个格子上
		 */
		proxy->tag = computeTag(system->m_inverseDiameter * p.x, system->m_inverseDiameter * p.y);

		// printf("particle tag %u %u\n", computeColumn(proxy->tag), computeRow(proxy->tag));
	}

	// 根据粒子所处的单元格即tag值对粒子进行从小到大对cells排序。即按照二维网格的顺序进行排序
#if SQ_PARTICLE_GRID_BENCHMARK && SQ_PARTICLE_GRID_COMPARE_STD_SORT
	benchStdSort("update_stdSort", g_updateStdBench, cells);
	const double sortT0 = benchNowMs();
	radixSortByTag();
	benchRecord("update_radixSort", g_updateBench, benchNowMs() - sortT0);
#else
	radixSortByTag();
#if SQ_PARTICLE_GRID_BENCHMARK
	benchRecord("update", g_updateBench, benchNowMs() - t0);
#endif
#endif
}

void SqParticleSystemGrid::updateAndSortNewPos()
{
#if SQ_PARTICLE_GRID_BENCHMARK
	const double t0 = benchNowMs();
#endif
	const GridCell *endProxy = cells.end();
	for (GridCell *proxy = cells.begin(); proxy < endProxy; ++proxy)
	{
		SqParticle *particle = system->findById(proxy->particleId);
		proxy->tag = computeTag(system->m_inverseDiameter * particle->newPosition.x, system->m_inverseDiameter * particle->newPosition.y);
	}

#if SQ_PARTICLE_GRID_BENCHMARK && SQ_PARTICLE_GRID_COMPARE_STD_SORT
	benchStdSort("updateAndSortNewPos_stdSort", g_updateNewPosStdBench, cells);
	const double sortT0 = benchNowMs();
	radixSortByTag();
	benchRecord("updateAndSortNewPos_radixSort", g_updateNewPosBench, benchNowMs() - sortT0);
#else
	radixSortByTag();
#if SQ_PARTICLE_GRID_BENCHMARK
	benchRecord("updateAndSortNewPos", g_updateNewPosBench, benchNowMs() - t0);
#endif
#endif
}
