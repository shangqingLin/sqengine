#include "gjk-distance.h"
#include "../../common/SqCore.h"
#include "../../common/math/SqMath.h"
#include "../../common/SqConfig.h"
#include "../../common/SqTypeDefine.h"
#include <stdio.h>
#include <float.h>
#include <engine/core/simd/simd.h>

using namespace phxy;

static inline SqVec2 sqWeight2(float a1, SqVec2 w1, float a2, SqVec2 w2)
{
	return SqVec2{
			a1 * w1.x + a2 * w2.x,
			a1 * w1.y + a2 * w2.y
		};
}

static inline SqVec2 sqWeight3(float a1, SqVec2 w1, float a2, SqVec2 w2, float a3, SqVec2 w3)
{
	return SqVec2{
			a1 * w1.x + a2 * w2.x + a3 * w3.x, 
			a1 * w1.y + a2 * w2.y + a3 * w3.y
		};
}

int phxy::sqFindSupport(const phxy::SqShapeProxy *proxy, const SqVec2 &direction)
{
	/**
	 * 将proxy形状上的点投影到direction上
	 * 并找到投影最大的那个顶点，即找到单纯形上的顶点
	 */

	const SqVec2 *points = proxy->points;
	int count = proxy->count;

	int bestIndex = 0;
	float bestValue = SqVec2::Dot(points[0], direction);
	for (int i = 1; i < count; ++i)
	{
		float value = SqVec2::Dot(points[i], direction);
		if (value > bestValue)
		{
			bestIndex = i;
			bestValue = value;
		}
	}

	return bestIndex;
}

/**
 * 单纯形中只有两个顶点，那么计算原点到线段的最近点，并返回下一次搜索的方向
 */
static SqVec2 SqSolveSimplex2(SqSimplex *s)
{
	const SqVec2 &w1 = s->v1.w;		  // 单纯形中的第一个顶点
	const SqVec2 &w2 = s->v2.w;		  // 单纯形中的第二个顶点
	SqVec2 e12 = SqVec2::Sub(w2, w1); // 由上面w1和w2顶点组成的线段 ， 由w1->w2

	/**
	 * 下面将计算原点到线段e12上的距离。通过重心坐标来计算
	 */

	// 计算原点在w1那一侧的权重u
	// u(权重) = ( (0,0) - w1).e12 / ||e12||
	// 由于我们只需要判断w1位于e21的哪个区域，只需要知道权重u的符号即可，所以省略除以||e12||
	float d12_2 = -SqVec2::Dot(w1, e12);
	if (d12_2 <= 0.0f)
	{
		// d12_2 < 0 表示原点位于线段之外，并且在w1的那一侧
		// 所以原点到线段最近的点为w1，直接得到结果，则返回
		// 所以w2顶点将不会对单存形有所贡献了，则可以删除w2
		s->v1.a = 1.0f;
		s->count = 1; // 删除了w2，因此只剩下w1这个顶点了

		// 因此，还是拿组成w1的两个suppor point组成的向量作为下一次查询的方向
		return SqVec2::Neg(w1);
	}

	// 计算原点在w2那一侧的权重v
	// v(权重) = (w1 - (0,0)).e12 / ||e12||
	float d12_1 = SqVec2::Dot(w2, e12);
	if (d12_1 <= 0.0f)
	{
		// 如果权重v<=0，则表示原点在线段之外，并且在w2的那一侧

		// a1 <= 0, so we clamp it to 0
		s->v2.a = 1.0f;
		s->count = 1;
		s->v1 = s->v2;

		// 因此，还是拿组成w2的两个suppor point组成的向量作为下一次查询的方向
		return SqVec2::Neg(w2);
	}

	// 如果执行到这里，则表示原点在线段e12的区域内
	// 那么我们可以通过重心坐标，得到原点到线性e12的最近点

	// 上面d12_1和d12_2都还没有除以e12的模长，这里除以模长就可以得出重心坐标了
	float inv_d12 = 1.0f / (d12_1 + d12_2);
	s->v1.a = d12_1 * inv_d12; // 重心坐标u
	s->v2.a = d12_2 * inv_d12; // 重心坐标v
	s->count = 2;
	return SqVec2::CrossSV(SqVec2::Cross(SqVec2::Add(w1, w2), e12), e12);
}

/**
 * 单纯形有三个点，那么计算原点到三角形的最近点
 *
 */
static SqVec2 SqSolveSimplex3(SqSimplex *s)
{
	const SqVec2 &w1 = s->v1.w;
	const SqVec2 &w2 = s->v2.w;
	const SqVec2 &w3 = s->v3.w;

	// Edge12
	// [1      1     ][a1] = [1]
	// [w1.e12 w2.e12][a2] = [0]
	// a3 = 0
	SqVec2 e12 = SqVec2::Sub(w2, w1); // 三角形第一条边
	float w1e12 = SqVec2::Dot(w1, e12);
	float w2e12 = SqVec2::Dot(w2, e12);
	float d12_1 = w2e12;
	float d12_2 = -w1e12;

	// Edge13
	// [1      1     ][a1] = [1]
	// [w1.e13 w3.e13][a3] = [0]
	// a2 = 0
	SqVec2 e13 = SqVec2::Sub(w3, w1); // 三角形第二条边
	float w1e13 = SqVec2::Dot(w1, e13);
	float w3e13 = SqVec2::Dot(w3, e13);
	float d13_1 = w3e13;
	float d13_2 = -w1e13;

	// Edge23
	// [1      1     ][a2] = [1]
	// [w2.e23 w3.e23][a3] = [0]
	// a1 = 0
	SqVec2 e23 = SqVec2::Sub(w3, w2); // 三角形第三条边
	float w2e23 = SqVec2::Dot(w2, e23);
	float w3e23 = SqVec2::Dot(w3, e23);
	float d23_1 = w3e23;
	float d23_2 = -w2e23;

	// 三角形面积 = cross(边1，边2) / 2

	// 三角形的面积（只是没有除以2而已）
	float n123 = SqVec2::Cross(e12, e13);

	// 原点与三角形的边组成的三角形的面积
	float d123_1 = n123 * SqVec2::Cross(w2, w3);
	float d123_2 = n123 * SqVec2::Cross(w3, w1);
	float d123_3 = n123 * SqVec2::Cross(w1, w2);

	// w1 region
	if (d12_2 <= 0.0f && d13_2 <= 0.0f)
	{
		s->v1.a = 1.0f;
		s->count = 1;
		return SqVec2::Neg(w1);
	}

	// e12
	if (d12_1 > 0.0f && d12_2 > 0.0f && d123_3 <= 0.0f)
	{
		float inv_d12 = 1.0f / (d12_1 + d12_2);
		s->v1.a = d12_1 * inv_d12;
		s->v2.a = d12_2 * inv_d12;
		s->count = 2;
		return SqVec2::CrossSV(SqVec2::Cross(SqVec2::Add(w1, w2), e12), e12);
	}

	// e13
	if (d13_1 > 0.0f && d13_2 > 0.0f && d123_2 <= 0.0f)
	{
		float inv_d13 = 1.0f / (d13_1 + d13_2);
		s->v1.a = d13_1 * inv_d13;
		s->v3.a = d13_2 * inv_d13;
		s->count = 2;
		s->v2 = s->v3;
		return SqVec2::CrossSV(SqVec2::Cross(SqVec2::Add(w1, w3), e13), e13);
	}

	// w2 region
	if (d12_1 <= 0.0f && d23_2 <= 0.0f)
	{
		s->v2.a = 1.0f;
		s->count = 1;
		s->v1 = s->v2;
		return SqVec2::Neg(w2);
	}

	// w3 region
	if (d13_1 <= 0.0f && d23_1 <= 0.0f)
	{
		s->v3.a = 1.0f;
		s->count = 1;
		s->v1 = s->v3;
		return SqVec2::Neg(w3);
	}

	// e23
	if (d23_1 > 0.0f && d23_2 > 0.0f && d123_1 <= 0.0f)
	{
		float inv_d23 = 1.0f / (d23_1 + d23_2);
		s->v2.a = d23_1 * inv_d23;
		s->v3.a = d23_2 * inv_d23;
		s->count = 2;
		s->v1 = s->v3;
		return SqVec2::CrossSV(SqVec2::Cross(SqVec2::Add(w2, w3), e23), e23);
	}

	// 原点在单纯形之内
	float inv_d123 = 1.0f / (d123_1 + d123_2 + d123_3);
	s->v1.a = d123_1 * inv_d123;
	s->v2.a = d123_2 * inv_d123;
	s->v3.a = d123_3 * inv_d123;
	s->count = 3;

	// No search direction
	return SqVec2();
}

static SqSimplex sqMakeSimplexFromCache(const SqSimplexCache *cache, const phxy::SqShapeProxy *proxyA, const phxy::SqShapeProxy *proxyB)
{
	SQ_ASSERT(cache->count <= 3);
	SqSimplex s;

	// Copy data from cache.
	s.count = cache->count;

	SqSimplexVertex *vertices[] = {&s.v1, &s.v2, &s.v3};
	for (int i = 0; i < s.count; ++i)
	{
		SqSimplexVertex *v = vertices[i];
		v->indexA = cache->indexA[i];
		v->indexB = cache->indexB[i];
		v->wA = proxyA->points[v->indexA];
		v->wB = proxyB->points[v->indexB];
		v->w = SqVec2::Sub(v->wA, v->wB);

		// invalid
		v->a = -1.0f;
	}

	// 表示为新创建的单纯形
	// 则找两个多边形的第一个顶点作为support point (其实首个点可以为任意点)
	if (s.count == 0)
	{
		SqSimplexVertex *v = vertices[0];
		v->indexA = 0;
		v->indexB = 0;
		v->wA = proxyA->points[0];
		v->wB = proxyB->points[0];
		v->w = SqVec2::Sub(v->wA, v->wB);
		v->a = 1.0f;
		s.count = 1;
	}

	return s;
}

static void sqMakeSimplexCache(SqSimplexCache *cache, const SqSimplex *simplex)
{
	cache->count = (uint16_t)simplex->count;
	const SqSimplexVertex *vertices[] = {&simplex->v1, &simplex->v2, &simplex->v3};
	for (int i = 0; i < simplex->count; ++i)
	{
		cache->indexA[i] = (uint8_t)vertices[i]->indexA;
		cache->indexB[i] = (uint8_t)vertices[i]->indexB;
	}
}

/**
 *
 * @param s: 当前的 Simplex，包含 1~3 个顶点
 * @param a: 输出，形状 A 上的最近点
 * @param b: 输出，形状 B 上的最近点
 *
 */
static void sqComputeSimplexWitnessPoints(SqVec2 *a, SqVec2 *b, const SqSimplex *s)
{
	switch (s->count)
	{
	case 1:
		// 单纯形只有一个点，那么组成这个点两个support point就是作为最近点了
		*a = s->v1.wA;
		*b = s->v1.wB;
		break;

	case 2:
		*a = sqWeight2(s->v1.a, s->v1.wA, s->v2.a, s->v2.wA);
		*b = sqWeight2(s->v1.a, s->v1.wB, s->v2.a, s->v2.wB);
		break;

	case 3:
		*a = sqWeight3(s->v1.a, s->v1.wA, s->v2.a, s->v2.wA, s->v3.a, s->v3.wA);
		// todo why are these not equal?
		//*b = sqWeight3(s->v1.a, s->v1.wB, s->v2.a, s->v2.wB, s->v3.a, s->v3.wB);
		*b = *a;
		break;

	default:
		a->zero();
		b->zero();
		SQ_ASSERT(false);
		break;
	}
}

/**
 *
 * 使用GJK算法来计算两个凸多边形的距离
 * https://box2d.org/files/ErinCatto_GJK_GDC2010.pdf
 * I spent time optimizing this and could find no further significant gains 3/30/2025
 */
SqDistanceOutput phxy::sqShapeDistance(const SqDistanceInput *input, SqSimplexCache *cache, SqSimplex *simplexes, int simplexCapacity)
{
	SQ_ASSERT(input->proxyA.count > 0 && input->proxyB.count > 0);
	SQ_ASSERT(input->proxyA.radius >= 0.0f);
	SQ_ASSERT(input->proxyB.radius >= 0.0f);

	SqDistanceOutput output;

	const phxy::SqShapeProxy *proxyA = &input->proxyA;

	// Get proxyB in frame A to avoid further transforms in the main loop.
	// This is still a performance gain at 8 points.
	phxy::SqShapeProxy localProxyB;

	{
		localProxyB.count = input->proxyB.count;
		localProxyB.radius = input->proxyB.radius;

		// 外部可能已经将transformB转换到transformA下了，就不需要了再运算了，提升性能
		if (input->transformLocaBVertex)
		{
			SqTransform transform = SqTransform::InvMulTransforms(input->transformA, input->transformB);
			for (int i = 0; i < localProxyB.count; ++i)
			{
				localProxyB.points[i] = SqTransform::transformPoint(transform, input->proxyB.points[i]);
			}
		}
		else
		{
			for (int i = 0; i < localProxyB.count; ++i)
			{
				localProxyB.points[i] = input->proxyB.points[i];
			}
		}
	}

	// Initialize the simplex.
	SqSimplex simplex = sqMakeSimplexFromCache(cache, proxyA, &localProxyB);

	int simplexIndex = 0;
	if (simplexes != NULL && simplexIndex < simplexCapacity)
	{
		simplexes[simplexIndex] = simplex;
		simplexIndex += 1;
	}

	// Get simplex vertices as an array.
	SqSimplexVertex *vertices[] = {&simplex.v1, &simplex.v2, &simplex.v3};

	SqVec2 nonUnitNormal;

	// These store the vertices of the last simplex so that we can check for duplicates and prevent cycling.
	int saveA[3], saveB[3];

	// 最大迭代20次
	const int maxIterations = 20;
	int iteration = 0;
	while (iteration < maxIterations)
	{
		// Copy simplex so we can identify duplicates.
		int saveCount = simplex.count;
		for (int i = 0; i < saveCount; ++i)
		{
			saveA[i] = vertices[i]->indexA;
			saveB[i] = vertices[i]->indexB;
		}

		SqVec2 d;
		switch (simplex.count)
		{
		case 1:
			// 单纯形只有一个顶点的时候，取两个support point的向量作为首次的方向
			d = SqVec2::Neg(simplex.v1.w);
			break;

		case 2:
			d = SqSolveSimplex2(&simplex);
			break;

		case 3:
			d = SqSolveSimplex3(&simplex);
			break;

		default:
			SQ_ASSERT(false);
		}

		if (simplex.count == 3)
		{
			// 这种情况一般是两个多边形发生了重叠或完全接触，其距离为0，即gjk算法中单纯形包含了原点
			// 直接 output.distance = 0
			SqVec2 localPointA, localPointB;
			sqComputeSimplexWitnessPoints(&localPointA, &localPointB, &simplex);
			output.pointA = SqTransform::transformPoint(input->transformA, localPointA);
			output.pointB = SqTransform::transformPoint(input->transformA, localPointB);

			// printf("dist %d nomral %f %f pointA (%f %f) pointB (%f %f) \n", output.distance, 
			// 	output.normal.x, output.normal.y,
			// 	output.pointA.x,output.pointA.y,
			// 	output.pointB.x,output.pointB.y
			// );
			return output;
		}

#ifndef NDEBUG
		if (simplexes != NULL && simplexIndex < simplexCapacity)
		{
			simplexes[simplexIndex] = simplex;
			simplexIndex += 1;
		}
#endif

		// Ensure the search direction is numerically fit.
		if (SqVec2::Dot(d, d) < FLT_EPSILON * FLT_EPSILON)
		{
			// This is unlikely but could lead to bad cycling.
			// The branch predictor seems to make this check have low cost.

			// The origin is probably contained by a line segment
			// or triangle. Thus the shapes are overlapped.

			// Must return overlap due to invalid normal.
			SqVec2 localPointA, localPointB;
			sqComputeSimplexWitnessPoints(&localPointA, &localPointB, &simplex);
			output.pointA = SqTransform::transformPoint(input->transformA, localPointA);
			output.pointB = SqTransform::transformPoint(input->transformA, localPointB);
			return output;
		}

		// Save the normal
		nonUnitNormal = d;

		// 计算Minkowski差的集合组成的多边形
		// support = support(a, d) - support(b, -d)
		SqSimplexVertex *vertex = vertices[simplex.count];
		vertex->indexA = sqFindSupport(proxyA, d);
		vertex->wA = proxyA->points[vertex->indexA];

		// 在d的反方向再找一个support point
		vertex->indexB = sqFindSupport(&localProxyB, SqVec2::Neg(d));
		vertex->wB = localProxyB.points[vertex->indexB];

		// 单纯形上的边
		vertex->w = SqVec2::Sub(vertex->wA, vertex->wB);

		// Iteration count is equated to the number of support point calls.
		++iteration;

		bool duplicate = false;
		for (int i = 0; i < saveCount; ++i)
		{
			// 查找新找出来的support point是否与在当前单纯形中了，避免重复
			// 如果发生了重复，那么表明两个多边形的顶点都已经全部遍历过一次了
			if (vertex->indexA == saveA[i] && vertex->indexB == saveB[i])
			{
				duplicate = true;
				break;
			}
		}

		// If we found a duplicate support point we must exit to avoid cycling.
		if (duplicate)
		{
			break;
		}

		// 增加了一个单纯形顶点
		simplex.count += 1;
	}

#ifndef NDEBUG
	if (simplexes != NULL && simplexIndex < simplexCapacity)
	{
		simplexes[simplexIndex] = simplex;
		simplexIndex += 1;
	}
#endif

	// printf("iteration %d \n", iteration);

	// Prepare output
	SqVec2 normal = SqVec2::Normalize(nonUnitNormal);
	// SQ_ASSERT(b2IsNormalized(normal));
	normal = SqTransform::transformVector(input->transformA, normal);

	SqVec2 localPointA, localPointB;
	sqComputeSimplexWitnessPoints(&localPointA, &localPointB, &simplex);
	output.normal = normal;
	output.distance = SqVec2::Distance(localPointA, localPointB);
	output.pointA = SqTransform::transformPoint(input->transformA, localPointA);
	output.pointB = SqTransform::transformPoint(input->transformA, localPointB);
	output.iterations = iteration;
	output.simplexCount = simplexIndex;

	// Cache the simplex
	sqMakeSimplexCache(cache, &simplex);

	// Apply radii if requested
	if (input->useRadii && output.distance > 0.1f * SqConfig::getInstance()->getLinearSlop())
	{
		float radiusA = input->proxyA.radius;
		float radiusB = input->proxyB.radius;
		output.distance = max(0.0f, output.distance - radiusA - radiusB);

		// Keep closest points on perimeter even if overlapped, this way the points move smoothly.
		output.pointA = SqVec2::MulAdd(output.pointA, radiusA, normal);
		output.pointB = SqVec2::MulSub(output.pointB, radiusB, normal);
	}

	return output;
}
