#include "SqEdgeShape.h"
#include "SqCapsuleShape.h"
#include "../../common/math/SqMath.h"
#include "../../common/SqConfig.h"
#include "../../collision/narrowphase/shape-cast.h"
#include <stdio.h>

using namespace phxy;

SqEdgeShape::SqEdgeShape()
{
	type = sq_segmentShape;
	oneSided = false;
}

void SqEdgeShape::setEdge(const SqVec2 &point1, const SqVec2 &point2)
{
	this->point1 = point1;
	this->point2 = point2;
}

void SqEdgeShape::scale(float sx, float sy)
{
	point1.x *= sx;
	point1.y *= sy;
	point2.x *= sx;
	point2.y *= sy;
}

SqAABB SqEdgeShape::computeShapeAABB(const SqTransform &xf) const
{
	SqVec2 v1 = SqTransform::transformPoint(xf, point1);
	SqVec2 v2 = SqTransform::transformPoint(xf, point2);

	SqVec2 lower = SqVec2::Min(v1, v2);
	SqVec2 upper = SqVec2::Max(v1, v2);

	SqAABB aabb = {lower, upper};
	const float speculativeDistance = SqConfig::getInstance()->getSpeculativeDistance();
	aabb.lowerBound.x -= speculativeDistance;
	aabb.lowerBound.y -= speculativeDistance;
	aabb.upperBound.x += speculativeDistance;
	aabb.upperBound.y += speculativeDistance;
	return aabb;
}

SqMassData SqEdgeShape::computeShapeMass()
{
	return SqMassData();
}

SqShapeExtent SqEdgeShape::computeShapeExtent(const SqVec2 &localCenter)
{
	SqShapeExtent extent;
	extent.minExtent = 0.0f;
	SqVec2 c1 = SqVec2::Sub(point1, localCenter);
	SqVec2 c2 = SqVec2::Sub(point2, localCenter);
	extent.maxExtent = sqrtf(max(SqVec2::LengthSquared(c1), SqVec2::LengthSquared(c2)));
	return extent;
}

SqVec2 SqEdgeShape::getCentroid()
{
	return SqVec2::Lerp(point1, point2, 0.5f);
}

void SqEdgeShape::RayCastShapeImpl(SqCastOutput &output, const SqShapeCastInput &input, const SqTransform &transform) const
{
	SqShapeCastPairInput pairInput;
	SqVec2 points[2] = {point1, point2};
	pairInput.proxyA = sqMakeProxy(points, 2, 0.0f);
	pairInput.proxyB = input.proxy;
	pairInput.translationB = input.translation;
	pairInput.maxFraction = input.maxFraction;
	pairInput.canEncroach = input.canEncroach;
	sqShapeCast(output, &pairInput);
}

void SqEdgeShape::RayCastImpl(SqCastOutput &output, const SqRayCastInput &input, const SqTransform &transform) const
{
	if (oneSided)
	{
		// Skip left-side collision
		float offset = SqVec2::Cross(SqVec2::Sub(input.origin, this->point1), SqVec2::Sub(this->point2, this->point1));
		if (offset < 0.0f)
		{
			return;
		}
	}
	// Put the ray into the edge's frame of reference.
	const SqVec2 &p1 = input.origin;
	const SqVec2 &d = input.translation;

	const SqVec2 &v1 = this->point1;
	const SqVec2 &v2 = this->point2;
	SqVec2 e = SqVec2::Sub(v2, v1);

	// 判断线段的长度是否为0 （为0是一个不合法的线段）
	float length;
	SqVec2 eUnit = SqVec2::GetLengthAndNormalize(&length, e);
	if (length == 0.0f)
	{
		// printf(" length zero \n");
		return;
	}

	// 与线段垂直的法线，位于线段的右侧
	//  Normal points to the right, looking from v1 towards v2
	SqVec2 normal = SqVec2::RightPerp(eUnit);

	// Intersect ray with infinite segment using normal
	// Similar to intersecting a ray with an infinite plane
	// p = p1 + t * d
	// dot(normal, p - v1) = 0
	// dot(normal, p1 - v1) + t * dot(normal, d) = 0

	/**
	 * v1-p1构建一条从 p1指向v1 的向量。
	 * 然后求此向量上在线段法线上的投影，所以numerator为点p1到线段的有方向的垂直距离
	 *
	 *
	 * 1、numerator < 0 表明p1点在与法线同一侧
	 *
	 *   v1 -------->v2
	 * 	  ↗    |
	 * 	 /      |
	 *  /       |
	 *  p1.     | 法线
	 *
	 * 2、
	 *
	 *  numerator > 0 则表明p1在法线另外一侧，与法线相反的那一侧
	 *
	 *        p1.
	 * 		 /
	 *     v1 -------->v2
	 * 	     	|
	 * 	     	|
	 *        	|
	 *     		| 法线
	 *
	 */
	float numerator = SqVec2::Dot(normal, SqVec2::Sub(v1, p1));

	/**
	 * 求d在法线上的投影。即d在法线上长度
	 */
	float denominator = SqVec2::Dot(normal, d);

	// printf("origin %f %f normal %f %f d %f %f \n", p1.x, p1.y, normal.x, normal.y, d.x, d.y);
	if (denominator == 0.0f)
	{
		// 如果射线与normal的点积为0，则表示射线与Normal垂直，从而说明射线与线段平行，所以不会相交
		//  printf(" parallel \n");
		return;
	}

	/**
	 * numerator 是点p1投影在法线上的长度
	 * denominator 是d投影在法线上的长度
	 * 所以这两个都在法线这条直线上，然后计算numerator占用了denominator多少长度，从而确定t就是在denominator进度多少处于线段相交
	 *
	 *
	 * 1、相交的情况
	 * 	     p1
	 *       |  p1距离线段长度
	 * 		 | numerator        |     \
	 * 	     |                  |      \
	 *   v1 --------------------|-------\------>v2
	 *               |          |        \
	 * 	     		 |          |         \ d
	 *        		 |        denominator（为d在法线上的投影长度）
	 *     			 | 法线
	 *
	 *
	 * 2、不相交情况
	 *
	 *
	 *   	 p1
	 *       |  p1距离线段长度   |    \
	 * 		 | numerator        |     \
	 * 	     |                        d(d很短)
	 *   v1 --------------------------------->v2
	 *               |
	 * 	     		 |
	 *        		 |
	 *     			 | 法线
	 *
	 *  此时 numerator > denominator ，因此 t > 1，不相交
	 *  还有很多不相交的情况，可以一一举例子
	 */
	float t = numerator / denominator;
	if (t < 0.0f || input.maxFraction < t)
	{
		// t < 0 表示射线起点完全在线段法线的同一侧，肯定不会相交了
		// printf(" out of ray range t %f numerator %f denominator %f max %f \n", t, numerator, denominator, input.maxFraction);
		return;
	}

	// p就是为射线与线段相交的点，t为d在t进度处与线段相交
	SqVec2 p = SqVec2::MulAdd(p1, t, d);

	// Compute position of p along segment
	// p = v1 + s * e
	// s = dot(p - v1, e) / dot(e, e)

	float s = SqVec2::Dot(SqVec2::Sub(p, v1), eUnit);
	if (s < 0.0f || length < s)
	{
		// out of segment range
		// printf(" out of segment range \n");
		return;
	}

	if (numerator > 0.0f)
	{
		normal = SqVec2::Neg(normal);
	}

	output.fraction = t;
	output.point = p;
	output.normal = normal;
	output.hit = true;
}

void SqEdgeShape::CollideMoverImpl(SqPlaneResult &result, SqCapsuleShape *mover) const
{
	SqDistanceInput distanceInput;

	SqVec2 points[2] = {this->point1, this->point2};
	distanceInput.proxyA = sqMakeProxy(points, 2, 0.0f);

	points[0] = mover->center1;
	points[1] = mover->center2;
	distanceInput.proxyB = sqMakeProxy(points, 2, mover->radius);
	distanceInput.useRadii = false;
	distanceInput.transformLocaBVertex = false;

	float totalRadius = mover->radius;

	SqSimplexCache cache = {0};
	SqDistanceOutput distanceOutput = sqShapeDistance(&distanceInput, &cache, NULL, 0);

	if (distanceOutput.distance <= totalRadius)
	{
		SqPlane plane = {distanceOutput.normal, totalRadius - distanceOutput.distance};
		result.plane = plane;
		result.point = distanceOutput.pointA;
		result.hit = true;
	}
}

void SqEdgeShape::computeDistance(const SqVec2 &point, const SqTransform &xf, float &outDistance, SqVec2 &outNormal) const
{
	// 计算点到线段的最短距离，注意不是点到直线的距离。因此需要处理如下：
	// 如果point点在线段区域内，则求解点到直线上的垂直距离，distance为距离，normal为那条垂直线，指向p点的向量
	// 如果point点在线段两个端点之外，则求p点与两个端点距离最小的那个端点。distance距离，normal为point点到最近的端点的向量，这条向量指向p点

	SqVec2 v1 = xf.transformPoint(point1);
	SqVec2 v2 = xf.transformPoint(point2);

	// printf("computeDistance check point %f %f edgePoint v1(%f %f) v2(%f %f) \n", point.x, point.y, v1.x, v1.y, v2.x, v2.y);

	SqVec2 d = point - v1;

	// 线段向量
	SqVec2 s = v2 - v1;

	// point在线段上的投影长度和方向
	float ds = SqVec2::Dot(d, s);

	// ds>0,则表示d向量和ds向量在同一侧，则表示point点在线段两个端点之内或在v2之外
	if (ds > 0)
	{

		float s2 = SqVec2::LengthSquared(s);
		if (ds > s2) // 判断是否在v2之外
		{
			// 证明p点还是在线段之外，只不过是离v2点最近，所以直接计算point到v2的距离即可
			d = point - v2;
			// printf(" out v2 \n");
		}
		else
		{
			// p在线段之内。使用向量法求得这条垂直线
			d -= ds / s2 * s;
			// printf(" inner edge \n");
		}
	}

	// else
	// {
	// 	printf(" out v1 \n");
	// }

	float d1 = SqVec2::Length(d);
	outDistance = d1;

	// 归一化后返回点到线段的那条垂线
	outNormal = d1 > 0 ? 1 / d1 * d : SqVec2();
}