#include "collide-polygon-polygon.h"
#include "../../common/SqConfig.h"
#include <float.h>
#include "../../common/math/SqMath.h"
#include "../../common/SqCore.h"
#include "collision-algorithm.h"
#include "manifold-algprithm.h"
#include <stdio.h>

using namespace phxy;

SqManifold phxy::sqCollidePolygons(const SqPolygonShape *polygonA, SqTransform &xfA, const SqPolygonShape *polygonB, SqTransform &xfB)
{
	const SqVec2 &origin = polygonA->vertices[0];
	float linearSlop = SqConfig::getInstance()->getLinearSlop();
	float speculativeDistance = SqConfig::getInstance()->getSpeculativeDistance();

	/**
	 * 我们都知道要检测两个多边形，需要将确保两个多边形的坐标点必须在同一个坐标系下的才能进行算法运算，要么全部在世界坐标系中，要么两个多边形在同一个局部坐标系中。
	 * 这里不使用世界坐标系，而是使用polygonA局部坐标系。首先根据polygonA构建一个局部坐标系，然后将polygonB变换到这个坐标系下
	 */

	/**
	 * 首先构建一个基于polygonA的局部坐标系。
	 * 选取polygonA上任意一个顶点作为坐标系原点，这个坐标系在世界坐标系下表示的，所以原点必须在世界坐标系下
	 *
	 * 上面选取了polygonA的第0个顶点作为原点，即origin。所以先将origin变换到世界坐标系下，保留xfA的旋转和缩放，
	 * 构建了一个以origin为原点，xfA.q为旋转的sfA矩阵
	 */
	SqTransform sfA = {xfA.transformPoint(origin), xfA.q};

	// 将polygonB的矩阵xfB转换到sfA坐标系下表示
	SqTransform xf = SqTransform::InvMulTransforms(sfA, xfB);

	SqPolygonShape localPolyA;
	localPolyA.count = polygonA->count;
	localPolyA.radius = polygonA->radius;
	localPolyA.vertices[0].zero(); // 第0个为原点，所以这个顶点直接为（0，0）
	localPolyA.normals[0] = polygonA->normals[0];

	// 将polygonA多边形的顶点转换到sfA坐标系下。
	// 所以不转换到世界坐标系下的好处：不用将polygonA顶点和法线做矩阵乘法转换到世界。只需要简单的减法，法线不用计算
	for (int i = 1; i < localPolyA.count; ++i)
	{
		localPolyA.vertices[i] = SqVec2::Sub(polygonA->vertices[i], origin);
		localPolyA.normals[i] = polygonA->normals[i];

		// printf(" localPolyA %d %f %f \n",i,polygonA->normals[i].x,polygonA->normals[i].y);
	}

	SqPolygonShape localPolyB;
	localPolyB.count = polygonB->count;
	localPolyB.radius = polygonB->radius;

	// 将polygonB的顶点和法线转换到sfA表示的坐标系下。
	for (int i = 0; i < localPolyB.count; ++i)
	{
		localPolyB.vertices[i] = xf.transformPoint(polygonB->vertices[i]);
		localPolyB.normals[i] = xf.transformVector(polygonB->normals[i]);
		// printf(" localPolyB %d %f %f \n",i,polygonB->normals[i].x,polygonB->normals[i].y,localPolyB.normals[i].x,localPolyB.normals[i].y);
	}

	// 此时polygonA和polygonB都在同一个局部坐标系表示了

	// 下面使用SAT算法的原理找到两个多边形的距离。SAT算法要求需要检测两个多边形在各自上的投影，从而找到距离

	// SAT算法，计算localPolyB在localPolyA上的投影。得到两个多边形的最短距离
	int edgeA = 0;
	float separationA = phxy::sqFindMaxSeparation(&edgeA, &localPolyA, &localPolyB);

	
	// 计算localPolyA在localPolyB上的投影。得到两个多边形的最短距离
	int edgeB = 0;
	float separationB = phxy::sqFindMaxSeparation(&edgeB, &localPolyB, &localPolyA);

	float radius = localPolyA.radius + localPolyB.radius;

	// printf("======= %f %f %f\n",separationA,separationB,radius);

	if (separationA > speculativeDistance + radius || separationB > speculativeDistance + radius)
	{
		return SqManifold();
	}


	// Find incident edge
	bool flip;
	if (separationA >= separationB)
	{
		flip = false;

		const SqVec2& searchDirection = localPolyA.normals[edgeA];

		// Find the incident edge on polyB
		int count = localPolyB.count;
		const SqVec2 *normals = localPolyB.normals;
		edgeB = 0;
		float minDot = FLT_MAX;
		for (int i = 0; i < count; ++i)
		{
			float dot = SqVec2::Dot(searchDirection, normals[i]);
			if (dot < minDot)
			{
				minDot = dot;
				edgeB = i;
			}
		}
	}
	else
	{
		flip = true;

		const SqVec2& searchDirection = localPolyB.normals[edgeB];

		// Find the incident edge on polyA
		int count = localPolyA.count;
		const SqVec2 *normals = localPolyA.normals;
		edgeA = 0;
		float minDot = FLT_MAX;
		for (int i = 0; i < count; ++i)
		{
			float dot = SqVec2::Dot(searchDirection, normals[i]);
			if (dot < minDot)
			{
				minDot = dot;
				edgeA = i;
			}
		}
	}


	//上面只是使用SAT算法进行碰撞检查，并且选出两个多边形上发生碰撞的边，
	//这样接下来的算法我们只需要对这两边进行处理即可，不需要对整个多边形处理


	SqManifold manifold;

	
	// Using slop here to ensure vertex-vertex normal vectors can be safely normalized
	// todo this means edge clipping needs to handle slightly non-overlapping edges.
	if (separationA > 0.1f * linearSlop || separationB > 0.1f * linearSlop)
	{
		//到这里意味着两个多边形的刚好擦边接触，没有发生重叠
#if 1
		// Edges are disjoint. Find closest points between reference edge and incident edge
		// Reference edge on polygon A
		int i11 = edgeA;
		int i12 = edgeA + 1 < localPolyA.count ? edgeA + 1 : 0;
		int i21 = edgeB;
		int i22 = edgeB + 1 < localPolyB.count ? edgeB + 1 : 0;

		const SqVec2& v11 = localPolyA.vertices[i11];
		const SqVec2& v12 = localPolyA.vertices[i12];
		const SqVec2& v21 = localPolyB.vertices[i21];
		const SqVec2& v22 = localPolyB.vertices[i22];

		SqSegmentDistanceResult result = sqSegmentDistance(v11, v12, v21, v22);
		
		SQ_ASSERT(result.distanceSquared > 0.0f);

		float distance = sqrtf(result.distanceSquared);
		float separation = distance - radius;

		if (separation > speculativeDistance)
		{
			// This can happen in the vertex-vertex case
			return manifold;
		}

		// Attempt to clip edges
		manifold = sqClipPolygons(&localPolyA, &localPolyB, edgeA, edgeB, flip);

		float minSeparation = FLT_MAX;
		for (int i = 0; i < manifold.pointCount; ++i)
		{
			minSeparation = min(minSeparation, manifold.points[i].separation);
		}

		// Does vertex-vertex have substantially larger separation?
		if (separation + 0.1f * linearSlop < minSeparation)
		{
			if (result.fraction1 == 0.0f && result.fraction2 == 0.0f)
			{
				// v11 - v21
				SqVec2 normal = SqVec2::Sub(v21, v11);
				float invDistance = 1.0f / distance;
				normal.x *= invDistance;
				normal.y *= invDistance;

				SqVec2 c1 = SqVec2::MulAdd(v11, localPolyA.radius, normal);
				SqVec2 c2 = SqVec2::MulAdd(v21, -localPolyB.radius, normal);

				manifold.normal = normal;
				manifold.points[0].anchorA = SqVec2::Lerp(c1, c2, 0.5f);
				manifold.points[0].separation = separation;
				manifold.points[0].id = SQ_MAKE_ID(i11, i21);
				manifold.pointCount = 1;
			}
			else if (result.fraction1 == 0.0f && result.fraction2 == 1.0f)
			{
				// v11 - v22
				SqVec2 normal = SqVec2::Sub(v22, v11);
				float invDistance = 1.0f / distance;
				normal.x *= invDistance;
				normal.y *= invDistance;

				SqVec2 c1 = SqVec2::MulAdd(v11, localPolyA.radius, normal);
				SqVec2 c2 = SqVec2::MulAdd(v22, -localPolyB.radius, normal);

				manifold.normal = normal;
				manifold.points[0].anchorA = SqVec2::Lerp(c1, c2, 0.5f);
				manifold.points[0].separation = separation;
				manifold.points[0].id = SQ_MAKE_ID(i11, i22);
				manifold.pointCount = 1;
			}
			else if (result.fraction1 == 1.0f && result.fraction2 == 0.0f)
			{
				// v12 - v21
				SqVec2 normal = SqVec2::Sub(v21, v12);
				float invDistance = 1.0f / distance;
				normal.x *= invDistance;
				normal.y *= invDistance;

				SqVec2 c1 = SqVec2::MulAdd(v12, localPolyA.radius, normal);
				SqVec2 c2 = SqVec2::MulAdd(v21, -localPolyB.radius, normal);

				manifold.normal = normal;
				manifold.points[0].anchorA = SqVec2::Lerp(c1, c2, 0.5f);
				manifold.points[0].separation = separation;
				manifold.points[0].id = SQ_MAKE_ID(i12, i21);
				manifold.pointCount = 1;
			}
			else if (result.fraction1 == 1.0f && result.fraction2 == 1.0f)
			{
				// v12 - v22
				SqVec2 normal = SqVec2::Sub(v22, v12);
				float invDistance = 1.0f / distance;
				normal.x *= invDistance;
				normal.y *= invDistance;

				SqVec2 c1 = SqVec2::MulAdd(v12, localPolyA.radius, normal);
				SqVec2 c2 = SqVec2::MulAdd(v22, -localPolyB.radius, normal);

				manifold.normal = normal;
				manifold.points[0].anchorA = SqVec2::Lerp(c1, c2, 0.5f);
				manifold.points[0].separation = separation;
				manifold.points[0].id = SQ_MAKE_ID(i12, i22);
				manifold.pointCount = 1;
			}
		}
#else
		// Polygons are disjoint. Find closest points between reference edge and incident edge
		// Reference edge on polygon A
		int i11 = edgeA;
		int i12 = edgeA + 1 < localPolyA.count ? edgeA + 1 : 0;
		int i21 = edgeB;
		int i22 = edgeB + 1 < localPolyB.count ? edgeB + 1 : 0;

		SqVec2 v11 = localPolyA.vertices[i11];
		SqVec2 v12 = localPolyA.vertices[i12];
		SqVec2 v21 = localPolyB.vertices[i21];
		SqVec2 v22 = localPolyB.vertices[i22];

		b2SegmentDistanceResult result = b2SegmentDistance(v11, v12, v21, v22);

		if (result.fraction1 == 0.0f && result.fraction2 == 0.0f)
		{
			// v11 - v21
			SqVec2 normal = SqVec2::Sub(v21, v11);
			SQ_ASSERT(result.distanceSquared > 0.0f);
			float distance = sqrtf(result.distanceSquared);
			if (distance > B2_SPECULATIVE_DISTANCE + radius)
			{
				return manifold;
			}
			float invDistance = 1.0f / distance;
			normal.x *= invDistance;
			normal.y *= invDistance;

			SqVec2 c1 = SqVec2::MulAdd(v11, localPolyA.radius, normal);
			SqVec2 c2 = SqVec2::MulAdd(v21, -localPolyB.radius, normal);

			manifold.normal = normal;
			manifold.points[0].anchorA = SqVec2::Lerp(c1, c2, 0.5f);
			manifold.points[0].separation = distance - radius;
			manifold.points[0].id = SQ_MAKE_ID(i11, i21);
			manifold.pointCount = 1;
		}
		else if (result.fraction1 == 0.0f && result.fraction2 == 1.0f)
		{
			// v11 - v22
			SqVec2 normal = SqVec2::Sub(v22, v11);
			SQ_ASSERT(result.distanceSquared > 0.0f);
			float distance = sqrtf(result.distanceSquared);
			if (distance > B2_SPECULATIVE_DISTANCE + radius)
			{
				return manifold;
			}
			float invDistance = 1.0f / distance;
			normal.x *= invDistance;
			normal.y *= invDistance;

			SqVec2 c1 = SqVec2::MulAdd(v11, localPolyA.radius, normal);
			SqVec2 c2 = SqVec2::MulAdd(v22, -localPolyB.radius, normal);

			manifold.normal = normal;
			manifold.points[0].anchorA = SqVec2::Lerp(c1, c2, 0.5f);
			manifold.points[0].separation = distance - radius;
			manifold.points[0].id = SQ_MAKE_ID(i11, i22);
			manifold.pointCount = 1;
		}
		else if (result.fraction1 == 1.0f && result.fraction2 == 0.0f)
		{
			// v12 - v21
			SqVec2 normal = SqVec2::Sub(v21, v12);
			SQ_ASSERT(result.distanceSquared > 0.0f);
			float distance = sqrtf(result.distanceSquared);
			if (distance > B2_SPECULATIVE_DISTANCE + radius)
			{
				return manifold;
			}
			float invDistance = 1.0f / distance;
			normal.x *= invDistance;
			normal.y *= invDistance;

			SqVec2 c1 = SqVec2::MulAdd(v12, localPolyA.radius, normal);
			SqVec2 c2 = SqVec2::MulAdd(v21, -localPolyB.radius, normal);

			manifold.normal = normal;
			manifold.points[0].anchorA = SqVec2::Lerp(c1, c2, 0.5f);
			manifold.points[0].separation = distance - radius;
			manifold.points[0].id = SQ_MAKE_ID(i12, i21);
			manifold.pointCount = 1;
		}
		else if (result.fraction1 == 1.0f && result.fraction2 == 1.0f)
		{
			// v12 - v22
			SqVec2 normal = SqVec2::Sub(v22, v12);
			SQ_ASSERT(result.distanceSquared > 0.0f);
			float distance = sqrtf(result.distanceSquared);
			if (distance > speculativeDistance + radius)
			{
				return manifold;
			}
			float invDistance = 1.0f / distance;
			normal.x *= invDistance;
			normal.y *= invDistance;

			SqVec2 c1 = SqVec2::MulAdd(v12, localPolyA.radius, normal);
			SqVec2 c2 = SqVec2::MulAdd(v22, -localPolyB.radius, normal);

			manifold.normal = normal;
			manifold.points[0].anchorA = SqVec2::Lerp(c1, c2, 0.5f);
			manifold.points[0].separation = distance - radius;
			manifold.points[0].id = SQ_MAKE_ID(i12, i22);
			manifold.pointCount = 1;
		}
		else
		{
			// Edge region
			manifold = sqClipPolygons(&localPolyA, &localPolyB, edgeA, edgeB, flip);
		}
#endif
	}
	else
	{

		// 到这里意味着两个多边形发生了重叠了，因为他们的距离为负数
		manifold = sqClipPolygons(&localPolyA, &localPolyB, edgeA, edgeB, flip);
	}

	// Convert manifold to world space
	if (manifold.pointCount > 0)
	{
		manifold.normal = xfA.transformVector(manifold.normal);
		for (int i = 0; i < manifold.pointCount; ++i)
		{
			SqManifoldPoint *mp = manifold.points + i;

			// anchor points relative to shape origin in world space
			mp->anchorA = xfA.transformVector(SqVec2::Add(mp->anchorA, origin));
			mp->anchorB = SqVec2::Add(mp->anchorA, SqVec2::Sub(xfA.p, xfB.p));
			mp->point = SqVec2::Add(xfA.p, mp->anchorA);
		}
	}

	return manifold;
}
