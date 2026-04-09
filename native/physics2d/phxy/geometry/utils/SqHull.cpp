
#include "SqHull.h"
#include "../../common/math/SqMath.h"
#include "../../common/math/SqAABB.h"
#include "../../common/SqConfig.h"
#include "../../common/SqCore.h"
#include <float.h>
#include <stdio.h>

using namespace phxy;

// quickhull recursion
static SqHull b2RecurseHull(SqVec2 p1, SqVec2 p2, SqVec2 *ps, int count)
{
	SqHull hull;
	hull.count = 0;

	if (count == 0)
	{
		return hull;
	}

	// create an edge vector pointing from p1 to p2
	SqVec2 e = SqVec2::Normalize(SqVec2::Sub(p2, p1));

	// discard points left of e and find point furthest to the right of e
	SqVec2 rightPoints[SQ_MAX_POLYGON_VERTICES];
	int rightCount = 0;

	int bestIndex = 0;
	float bestDistance = SqVec2::Cross(SqVec2::Sub(ps[bestIndex], p1), e);
	if (bestDistance > 0.0f)
	{
		rightPoints[rightCount++] = ps[bestIndex];
	}

	for (int i = 1; i < count; ++i)
	{
		float distance = SqVec2::Cross(SqVec2::Sub(ps[i], p1), e);
		if (distance > bestDistance)
		{
			bestIndex = i;
			bestDistance = distance;
		}

		if (distance > 0.0f)
		{
			rightPoints[rightCount++] = ps[i];
		}
	}

	if (bestDistance < 2.0f * SqConfig::getInstance()->getLinearSlop())
	{
		return hull;
	}

	SqVec2 bestPoint = ps[bestIndex];

	// compute hull to the right of p1-bestPoint
	SqHull hull1 = b2RecurseHull(p1, bestPoint, rightPoints, rightCount);

	// compute hull to the right of bestPoint-p2
	SqHull hull2 = b2RecurseHull(bestPoint, p2, rightPoints, rightCount);

	// stitch together hulls
	for (int i = 0; i < hull1.count; ++i)
	{
		hull.points[hull.count++] = hull1.points[i];
	}

	hull.points[hull.count++] = bestPoint;

	for (int i = 0; i < hull2.count; ++i)
	{
		hull.points[hull.count++] = hull2.points[i];
	}

	SQ_ASSERT(hull.count < SQ_MAX_POLYGON_VERTICES);

	return hull;
}

// quickhull algorithm
// - merges vertices based on B2_LINEAR_SLOP
// - removes collinear points using B2_LINEAR_SLOP
// - returns an empty hull if it fails
SqHull phxy::SqComputeHull(const SqVec2 *points, int count)
{
	SqHull hull;
	hull.count = 0;

	if (count < 3 || count > SQ_MAX_POLYGON_VERTICES)
	{
		// check your data
		return hull;
	}

	count = min(count, SQ_MAX_POLYGON_VERTICES);

	SqAABB aabb = {{FLT_MAX, FLT_MAX}, {-FLT_MAX, -FLT_MAX}};

	// Perform aggressive point welding. First point always remains.
	// Also compute the bounding box for later.
	SqVec2 ps[SQ_MAX_POLYGON_VERTICES];
	int n = 0;
	const float linearSlop = SqConfig::getInstance()->getLinearSlop();
	const float tolSqr = 16.0f * linearSlop * linearSlop;
	for (int i = 0; i < count; ++i)
	{
		aabb.lowerBound = SqVec2::Min(aabb.lowerBound, points[i]);
		aabb.upperBound = SqVec2::Max(aabb.upperBound, points[i]);

		SqVec2 vi = points[i];

		bool unique = true;
		for (int j = 0; j < i; ++j)
		{
			SqVec2 vj = points[j];

			float distSqr = SqVec2::DistanceSquared(vi, vj);
			if (distSqr < tolSqr)
			{
				unique = false;
				break;
			}
		}

		if (unique)
		{
			ps[n++] = vi;
		}
	}

	if (n < 3)
	{
		// all points very close together, check your data and check your scale
		return hull;
	}

	// Find an extreme point as the first point on the hull
	SqVec2 c = SqAABB::Center(aabb);
	int f1 = 0;
	float dsq1 = SqVec2::DistanceSquared(c, ps[f1]);
	for (int i = 1; i < n; ++i)
	{
		float dsq = SqVec2::DistanceSquared(c, ps[i]);
		if (dsq > dsq1)
		{
			f1 = i;
			dsq1 = dsq;
		}
	}

	// remove p1 from working set
	SqVec2 p1 = ps[f1];
	ps[f1] = ps[n - 1];
	n = n - 1;

	int f2 = 0;
	float dsq2 = SqVec2::DistanceSquared(p1, ps[f2]);
	for (int i = 1; i < n; ++i)
	{
		float dsq = SqVec2::DistanceSquared(p1, ps[i]);
		if (dsq > dsq2)
		{
			f2 = i;
			dsq2 = dsq;
		}
	}

	// remove p2 from working set
	SqVec2 p2 = ps[f2];
	ps[f2] = ps[n - 1];
	n = n - 1;

	// split the points into points that are left and right of the line p1-p2.
	SqVec2 rightPoints[SQ_MAX_POLYGON_VERTICES - 2];
	int rightCount = 0;

	SqVec2 leftPoints[SQ_MAX_POLYGON_VERTICES - 2];
	int leftCount = 0;

	SqVec2 e = SqVec2::Normalize(SqVec2::Sub(p2, p1));

	for (int i = 0; i < n; ++i)
	{
		float d = SqVec2::Cross(SqVec2::Sub(ps[i], p1), e);

		// slop used here to skip points that are very close to the line p1-p2
		if (d >= 2.0f * linearSlop)
		{
			rightPoints[rightCount++] = ps[i];
		}
		else if (d <= -2.0f * linearSlop)
		{
			leftPoints[leftCount++] = ps[i];
		}
	}

	// compute hulls on right and left
	SqHull hull1 = b2RecurseHull(p1, p2, rightPoints, rightCount);
	SqHull hull2 = b2RecurseHull(p2, p1, leftPoints, leftCount);

	if (hull1.count == 0 && hull2.count == 0)
	{
		// all points collinear
		return hull;
	}

	// stitch hulls together, preserving CCW winding order
	hull.points[hull.count++] = p1;

	for (int i = 0; i < hull1.count; ++i)
	{
		hull.points[hull.count++] = hull1.points[i];
	}

	hull.points[hull.count++] = p2;

	for (int i = 0; i < hull2.count; ++i)
	{
		hull.points[hull.count++] = hull2.points[i];
	}

	SQ_ASSERT(hull.count <= SQ_MAX_POLYGON_VERTICES);

	// merge collinear
	bool searching = true;
	while (searching && hull.count > 2)
	{
		searching = false;

		for (int i = 0; i < hull.count; ++i)
		{
			int i1 = i;
			int i2 = (i + 1) % hull.count;
			int i3 = (i + 2) % hull.count;

			SqVec2 s1 = hull.points[i1];
			SqVec2 s2 = hull.points[i2];
			SqVec2 s3 = hull.points[i3];

			// unit edge vector for s1-s3
			SqVec2 r = SqVec2::Normalize(SqVec2::Sub(s3, s1));

			float distance = SqVec2::Cross(SqVec2::Sub(s2, s1), r);
			if (distance <= 2.0f * linearSlop)
			{
				// remove midpoint from hull
				for (int j = i2; j < hull.count - 1; ++j)
				{
					hull.points[j] = hull.points[j + 1];
				}
				hull.count -= 1;

				// continue searching for collinear points
				searching = true;

				break;
			}
		}
	}

	if (hull.count < 3)
	{
		// all points collinear, shouldn't be reached since this was validated above
		hull.count = 0;
	}

	return hull;
}

/**
 * 检测是否为凸包.
 * 这个验证算法要求传进来的点是：顺时针（Clockwise）顺序。
 * Convex Polygon Half-Space Test
 */
bool phxy::SqValidateHull(const SqHull *hull)
{
	if (hull->count < 3 || SQ_MAX_POLYGON_VERTICES < hull->count)
	{
		return false;
	}

	// printf("++++++++++++++++++++++++fuck %d \n",hull->count);
	// for (int i = 0; i < hull->count; i += 2)
	// {
	// 	printf("point %f %f \n", hull->points[i], hull[i + 1]);
	// }

	// 对每条边，检查所有点是否在同一侧。
	for (int i = 0; i < hull->count; ++i)
	{
		// i1和i2相邻的点为一条边，下面计算向量
		int i1 = i;
		int i2 = i < hull->count - 1 ? i1 + 1 : 0;
		const SqVec2 &p = hull->points[i1];

		// 构建向量并归一化
		SqVec2 e = SqVec2::Normalize(SqVec2::Sub(hull->points[i2], p));

		// 遍历多边形所有点
		for (int j = 0; j < hull->count; ++j)
		{
			// skip points that subtend the current edge
			if (j == i1 || j == i2)
			{
				continue;
			}

			/**
			 * 计算叉积
			 * 1、>0: 表示点在边左侧
			 * 2、<0：点在边右侧
			 * 3、=0：共线
			 *
			 * 因此，如果多边形顶点是 顺时针，则所有其他点必须 distance < 0
			 * 否则说明：
			 *		有点在边外侧
			 *		多边形不是凸的
			 *		或者顶点顺序错
			 */
			float distance = SqVec2::Cross(SqVec2::Sub(hull->points[j], p), e);
			if (distance >= 0.0f)
			{
				printf("++++++++++++++++ distance error %d \n", distance);
				return false;
			}
		}
	}

	// 检查是否有存在冗余共线点
	const float linearSlop = SqConfig::getInstance()->getLinearSlop();
	for (int i = 0; i < hull->count; ++i)
	{
		int i1 = i;
		int i2 = (i + 1) % hull->count;
		int i3 = (i + 2) % hull->count;

		SqVec2 p1 = hull->points[i1];
		SqVec2 p2 = hull->points[i2];
		SqVec2 p3 = hull->points[i3];

		SqVec2 e = SqVec2::Normalize(SqVec2::Sub(p3, p1));

		float distance = SqVec2::Cross(SqVec2::Sub(p2, p1), e);
		if (distance <= linearSlop)
		{
			// p1-p2-p3 are collinear
			printf("++++++++++++++++ p1-p2-p3 are collinear error \n");
			return false;
		}
	}

	return true;
}
