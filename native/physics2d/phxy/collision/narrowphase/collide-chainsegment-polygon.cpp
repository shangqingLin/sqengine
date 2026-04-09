#include "collide-chainsegment-polygon.h"
#include "../../common/SqConfig.h"
#include <float.h>
#include "../../common/math/SqMath.h"
#include "../../common/SqCore.h"
#include "collision-algorithm.h"
#include <stdio.h>

using namespace phxy;

struct SqChainSegmentParams
{
	SqVec2 edge1;
	SqVec2 normal0;
	SqVec2 normal2;
	bool convex1;
	bool convex2;
};

enum SqNormalType
{
	// 这意味着法线指向与凸顶点相对的非平滑方向，应当跳过该法线。
	sq_normalSkip,

	// 这意味着法线指向与凸顶点相对的平滑方向，应当用于碰撞检测。
	sq_normalAdmit,

	// 这意味着法线位于凹顶点区域，应当将其调整为该线段的法线。
	sq_normalSnap
};

// Evaluate Gauss map
// See https://box2d.org/posts/2020/06/ghost-collisions/
SqNormalType sqClassifyNormal(const SqChainSegmentParams &params, const SqVec2 &normal)
{
	const float sinTol = 0.01f;

	if (SqVec2::Dot(normal, params.edge1) <= 0.0f)
	{
		// Normal points towards the segment tail
		if (params.convex1)
		{
			if (SqVec2::Cross(normal, params.normal0) > sinTol)
			{
				return sq_normalSkip;
			}

			return sq_normalAdmit;
		}
		else
		{
			return sq_normalSnap;
		}
	}
	else
	{
		// Normal points towards segment head
		if (params.convex2)
		{
			if (SqVec2::Cross(params.normal2, normal) > sinTol)
			{
				return sq_normalSkip;
			}

			return sq_normalAdmit;
		}
		else
		{
			return sq_normalSnap;
		}
	}
}

SqManifold phxy::sqCollideChainSegmentAndPolygon(const SqChainEdgeShape *segmentA, SqTransform &xfA, const SqPolygonShape *polygonB,
												 SqTransform &xfB, SqSimplexCache *cache)
{
	SqManifold manifold;
	const float linearSlop = SqConfig::getInstance()->getLinearSlop();
	const float speculativeDistance = SqConfig::getInstance()->getSpeculativeDistance();

	// printf("transformA %f %f %f %f transformB %f %f %f %f \n",
	// 	xfA.p.x,xfA.p.y,xfA.q.c,xfA.q.s,
	// 	xfB.p.x,xfB.p.y,xfB.q.c,xfB.q.s
	// );

	/**
	 * 将polygonB转换到segmentA的本地坐标系中运算
	 */
	SqTransform xf = SqTransform::InvMulTransforms(xfA, xfB);
	SqVec2 centroidB = xf.transformPoint(polygonB->centroid);
	float radiusB = polygonB->radius;

	// printf("centroidB %f %f \n",centroidB.x,centroidB.y);

	const SqVec2 &p1 = segmentA->point1;
	const SqVec2 &p2 = segmentA->point2;

	// segmentA线段的向量表示
	SqVec2 edge1 = SqVec2::Normalize(SqVec2::Sub(p2, p1));

	// Normal points to the righ。即线段正面的法线
	SqVec2 normal1 = SqVec2::RightPerp(edge1);

	SqChainSegmentParams smoothParams;
	smoothParams.edge1 = edge1;

	// 构建与ghost点的线段和法线
	const float convexTol = 0.01f;
	SqVec2 edge0 = SqVec2::Normalize(SqVec2::Sub(p1, segmentA->ghost1)); // ghost线段
	smoothParams.normal0 = SqVec2::RightPerp(edge0);					 // 法线

	/**
	 * 判断edge0和edge1是否形成凹角
	 *
	 * 下面的情况就是凹角，即判断polygonB如果往ghost1方向运动的时候，ghost1是否对polygonB形成阻挡
	 * 						ghost1
	 *                      /
	 *      polygonB       /
	 *                   edge0
	 *                   /
	 *  ------edge1-----/
	 *  	           p1
	 *
	 *
	 * 但下面的情况都不是凹角，不会对polygonB造成阻挡
	 *
	 * 		polygonB
	 *                p1
	 *  ------edge1-----\
	 *  	             \
	 *                    edge0
	 * 					   \
	 *
	 *  当polygonB碰撞到拐角处我们需要处理，避免polygonB陷进去卡住了，这正是ghost的目的
	 *  所以如果convex1=true，则为凹角
	 *
	 */
	smoothParams.convex1 = SqVec2::Cross(edge0, edge1) >= convexTol;

	// printf("edge0 %f %f edge1 %f %f normal1 %f %f %f\n",edge0.x,edge0.y,edge1.x,edge1.y,normal1.x,normal1.y, SqVec2::Cross(edge0, edge1));

	// printf("p1 (%f %f) ghost1:(%f %f) %f \n", p1.x, p1.y, segmentA->ghost1.x, segmentA->ghost1.y, SqVec2::Cross(edge0, edge1));
	// printf("p2 (%f %f) ghost2:(%f %f) \n", p2.x, p2.y, segmentA->ghost2.x, segmentA->ghost2.y);

	SqVec2 edge2 = SqVec2::Normalize(SqVec2::Sub(segmentA->ghost2, p2));
	smoothParams.normal2 = SqVec2::RightPerp(edge2);
	smoothParams.convex2 = SqVec2::Cross(edge1, edge2) >= convexTol;

	// printf("centroidB %f %f centroid %f %f \n",centroidB.x,centroidB.y,polygonB->centroid.x,polygonB->centroid.y);

	/**
	 * 判断多边形polygonB的中心点是否在线段的背面
	 * 所以即使xfB变换可能将polygonB的位置变换到与segmentA重叠，重要重叠的位置不超过
	 * 中心点则都不认为是穿透，还是继续执行碰撞点计算
	 */
	SqVec2 centroidBToP1 = SqVec2::Sub(centroidB, p1);

	// 通过计算投影的符号来判断centroidB点是位于normal1起点的之外还是内，
	// 小于0是钝角，即表示polygonB多边形位于线段edge1的背面
	bool behind1 = SqVec2::Dot(normal1, centroidBToP1) < 0.0f;

	// printf("behind1 %d convex1 %d convex2 %d\n",behind1,smoothParams.convex1,smoothParams.convex2);

	bool behind0 = true;
	bool behind2 = true;
	if (smoothParams.convex1)
	{
		// 如果ghost1的点对多边形形成阻挡，则判断一下多边形是否在ghost1与p1形成的线段背面
		// 也是通过投影距离的符号来判定
		behind0 = SqVec2::Dot(smoothParams.normal0, centroidBToP1) < 0.0f;
		// printf("behind0 %d \n",behind0);
	}

	if (smoothParams.convex2)
	{
		// 和上面同理
		behind2 = SqVec2::Dot(smoothParams.normal2, SqVec2::Sub(centroidB, p2)) < 0.0f;
		// printf("behind2 %d \n",behind2);
	}

	if (behind1 && behind0 && behind2)
	{
		// 表示多边形完全位于由ghost1、ghost2、p1、p2围城的区域的背面
		// 则根本不需要发生碰撞
		//  printf(">>>>>>>>all behind\n");
		//  one-sided collision
		return manifold;
	}

	// 将多边形polygonB的顶点和法线都转换到线段的坐标系中，只能在同一个坐标系计算
	int count = polygonB->count;
	SqVec2 vertices[SQ_MAX_POLYGON_VERTICES];
	SqVec2 normals[SQ_MAX_POLYGON_VERTICES];

	// for (int i = 0; i < count; ++i)
	// {
	// 	printf("before polygon %d %f %f \n", i, polygonB->vertices[i].x, polygonB->vertices[i].y);
	// }

	// printf("center %f %f \n",centroidB.x,centroidB.y);
	for (int i = 0; i < count; ++i)
	{
		vertices[i] = xf.transformPoint(polygonB->vertices[i]);
		normals[i] = xf.transformVector(polygonB->normals[i]);
		// printf("polygon normal %d %f %f \n",i,normals[i].x,normals[i].y);
		// printf("polygon vertex %d %f %f \n", i,vertices[i].x, vertices[i].y);
	}

	// for (int i = 0; i < count -1; ++i)
	// {
	// 	printf("polygon length %d %f \n", i,SqVec2::Length(SqVec2::Sub(vertices[i],vertices[i+1])));
	// }

	// printf("cache %d %d %d \n", cache->count, cache->indexA, cache->indexB);

	// Distance doesn't work correctly with partial polygons
	SqDistanceInput input;
	input.proxyA = sqMakeProxy(segmentA);
	input.proxyB = sqMakeProxy(vertices, count, 0.0f);
	input.useRadii = false;
	input.transformLocaBVertex = false;

	// 使用GJK算法计算两个多边形距离和最近点
	SqDistanceOutput output = sqShapeDistance(&input, cache, NULL, 0);

	// printf("+++ distance %f %f %f\n", output.distance,radiusB,speculativeDistance);

	if (output.distance > radiusB + speculativeDistance)
	{
		return manifold;
	}

	// Snap concave normals for partial polygon
	SqVec2 n0 = smoothParams.convex1 ? smoothParams.normal0 : normal1;
	SqVec2 n2 = smoothParams.convex2 ? smoothParams.normal2 : normal1;

	// Index of incident vertex on polygon
	int incidentIndex = -1;
	int incidentNormal = -1;

	/**
	 * cache->count = 1:
	 * 如果是多边形的两个顶点发生碰撞，则count只会是1
	 *
	 * cache->count = 2
	 * 如果多边形是边与边或顶点与边发生碰撞，则count是2
	 *
	 */

	if (behind1 == false && output.distance > 0.1f * linearSlop)
	{

		// The closest features may be two vertices or an edge and a vertex even when there should
		// be face contact

		if (cache->count == 1)
		{
			// 这里处理顶点与顶点碰撞
			//  vertex-vertex collision
			const SqVec2 &pA = output.pointA;
			const SqVec2 &pB = output.pointB;

			SqVec2 normal = SqVec2::Normalize(SqVec2::Sub(pB, pA));

			SqNormalType type = sqClassifyNormal(smoothParams, normal);
			if (type == sq_normalSkip)
			{
				return manifold;
			}

			if (type == sq_normalAdmit)
			{
				manifold.normal = xfA.transformVector(normal);
				SqManifoldPoint *cp = manifold.points + 0;
				cp->anchorA = xfA.transformVector(pA);

				// 为啥不是pB？
				cp->anchorB = SqVec2::Add(cp->anchorA, SqVec2::Sub(xfA.p, xfB.p));
				cp->point = SqVec2::Add(xfA.p, cp->anchorA);
				cp->separation = output.distance - radiusB;
				cp->id = SQ_MAKE_ID(cache->indexA[0], cache->indexB[0]);
				manifold.pointCount = 1;
				return manifold;
			}

			// fall through b2_normalSnap
			incidentIndex = cache->indexB[0];
		}
		else
		{

			// printf("++++++ cache %d \n", cache->count);

			// vertex-edge collision
			SQ_ASSERT(cache->count == 2);

			int ia1 = cache->indexA[0];
			int ia2 = cache->indexA[1];
			int ib1 = cache->indexB[0];
			int ib2 = cache->indexB[1];

			if (ia1 == ia2)
			{
				// 这里处理的是顶点与边的碰撞
				SQ_ASSERT(ib1 != ib2);

				// Find polygon normal most aligned with vector between closest points.
				// This effectively sorts ib1 and ib2
				SqVec2 normalB = SqVec2::Sub(output.pointA, output.pointB);
				float dot1 = SqVec2::Dot(normalB, normals[ib1]);
				float dot2 = SqVec2::Dot(normalB, normals[ib2]);
				int ib = dot1 > dot2 ? ib1 : ib2;

				// Use accurate normal
				normalB = normals[ib];

				SqNormalType type = sqClassifyNormal(smoothParams, SqVec2::Neg(normalB));
				if (type == sq_normalSkip)
				{
					return manifold;
				}

				if (type == sq_normalAdmit)
				{
					// Get polygon edge associated with normal
					ib1 = ib;
					ib2 = ib < count - 1 ? ib + 1 : 0;

					SqVec2 b1 = vertices[ib1];
					SqVec2 b2 = vertices[ib2];

					// Find incident segment vertex
					dot1 = SqVec2::Dot(normalB, SqVec2::Sub(p1, b1));
					dot2 = SqVec2::Dot(normalB, SqVec2::Sub(p2, b1));

					if (dot1 < dot2)
					{
						if (SqVec2::Dot(n0, normalB) < SqVec2::Dot(normal1, normalB))
						{
							// Neighbor is incident
							return manifold;
						}
					}
					else
					{
						if (SqVec2::Dot(n2, normalB) < SqVec2::Dot(normal1, normalB))
						{
							// Neighbor is incident
							return manifold;
						}
					}

					manifold = sqClipSegments(b1, b2, p1, p2, normalB, radiusB, 0.0f, SQ_MAKE_ID(ib1, 1), SQ_MAKE_ID(ib2, 0));

					SQ_ASSERT(manifold.pointCount == 0 || manifold.pointCount == 2);
					if (manifold.pointCount == 2)
					{
						manifold.normal = xfA.transformVector(SqVec2::Neg(normalB));
						manifold.points[0].anchorA = xfA.transformVector(manifold.points[0].anchorA);
						manifold.points[1].anchorA = xfA.transformVector(manifold.points[1].anchorA);
						SqVec2 pAB = SqVec2::Sub(xfA.p, xfB.p);
						manifold.points[0].anchorB = SqVec2::Add(manifold.points[0].anchorA, pAB);
						manifold.points[1].anchorB = SqVec2::Add(manifold.points[1].anchorA, pAB);
						manifold.points[0].point = SqVec2::Add(xfA.p, manifold.points[0].anchorA);
						manifold.points[1].point = SqVec2::Add(xfA.p, manifold.points[1].anchorA);
					}
					return manifold;
				}

				// fall through b2_normalSnap
				incidentNormal = ib;
			}
			else
			{
				// Get index of incident polygonB vertex
				float dot1 = SqVec2::Dot(normal1, SqVec2::Sub(vertices[ib1], p1));
				float dot2 = SqVec2::Dot(normal1, SqVec2::Sub(vertices[ib2], p2));
				incidentIndex = dot1 < dot2 ? ib1 : ib2;
			}
		}
	}
	else
	{

		// 到这里表示output.distance为0，即多边形与线段边与边发生接触了

		// SAT edge normal
		float edgeSeparation = FLT_MAX;

		// 使用SAT找到最接近的点
		for (int i = 0; i < count; ++i)
		{
			float s = SqVec2::Dot(normal1, SqVec2::Sub(vertices[i], p1));
			if (s < edgeSeparation)
			{
				edgeSeparation = s;
				incidentIndex = i;
			}
		}

		// ghost1的处理
		if (smoothParams.convex1)
		{
			float s0 = FLT_MAX;

			for (int i = 0; i < count; ++i)
			{
				float s = SqVec2::Dot(smoothParams.normal0, SqVec2::Sub(vertices[i], p1));
				if (s < s0)
				{
					s0 = s;
				}
			}

			if (s0 > edgeSeparation)
			{
				edgeSeparation = s0;

				// Indicate neighbor owns edge separation
				incidentIndex = -1;
			}
		}

		// Check convex neighbor for edge separation
		if (smoothParams.convex2)
		{
			float s2 = FLT_MAX;

			for (int i = 0; i < count; ++i)
			{
				float s = SqVec2::Dot(smoothParams.normal2, SqVec2::Sub(vertices[i], p2));
				if (s < s2)
				{
					s2 = s;
				}
			}

			if (s2 > edgeSeparation)
			{
				edgeSeparation = s2;

				// Indicate neighbor owns edge separation
				incidentIndex = -1;
			}
		}

		// SAT polygon normals
		float polygonSeparation = -FLT_MAX;
		int referenceIndex = -1;

		for (int i = 0; i < count; ++i)
		{
			SqVec2 n = normals[i];

			SqNormalType type = sqClassifyNormal(smoothParams, SqVec2::Neg(n));
			if (type != sq_normalAdmit)
			{
				continue;
			}

			// Check the infinite sides of the partial polygon
			// if ((smoothParams.convex1 && SqVec2::Cross(n0, n) > 0.0f) || (smoothParams.convex2 && SqVec2::Cross(n, n2) > 0.0f))
			//{
			//	continue;
			//}

			SqVec2 p = vertices[i];
			float s = min(SqVec2::Dot(n, SqVec2::Sub(p2, p)), SqVec2::Dot(n, SqVec2::Sub(p1, p)));

			if (s > polygonSeparation)
			{
				polygonSeparation = s;
				referenceIndex = i;
			}
		}

		if (polygonSeparation > edgeSeparation)
		{
			int ia1 = referenceIndex;
			int ia2 = ia1 < count - 1 ? ia1 + 1 : 0;
			SqVec2 a1 = vertices[ia1];
			SqVec2 a2 = vertices[ia2];

			SqVec2 n = normals[ia1];

			float dot1 = SqVec2::Dot(n, SqVec2::Sub(p1, a1));
			float dot2 = SqVec2::Dot(n, SqVec2::Sub(p2, a1));

			if (dot1 < dot2)
			{
				if (SqVec2::Dot(n0, n) < SqVec2::Dot(normal1, n))
				{
					// Neighbor is incident
					return manifold;
				}
			}
			else
			{
				if (SqVec2::Dot(n2, n) < SqVec2::Dot(normal1, n))
				{
					// Neighbor is incident
					return manifold;
				}
			}

			manifold = sqClipSegments(a1, a2, p1, p2, normals[ia1], radiusB, 0.0f, SQ_MAKE_ID(ia1, 1), SQ_MAKE_ID(ia2, 0));

			SQ_ASSERT(manifold.pointCount == 0 || manifold.pointCount == 2);
			if (manifold.pointCount == 2)
			{

				manifold.normal = xfA.transformVector(SqVec2::Neg(normals[ia1]));
				manifold.points[0].anchorA = xfA.transformVector(manifold.points[0].anchorA);
				manifold.points[1].anchorA = xfA.transformVector(manifold.points[1].anchorA);
				SqVec2 pAB = SqVec2::Sub(xfA.p, xfB.p);
				manifold.points[0].anchorB = SqVec2::Add(manifold.points[0].anchorA, pAB);
				manifold.points[1].anchorB = SqVec2::Add(manifold.points[1].anchorA, pAB);
				manifold.points[0].point = SqVec2::Add(xfA.p, manifold.points[0].anchorA);
				manifold.points[1].point = SqVec2::Add(xfA.p, manifold.points[1].anchorA);
			}

			return manifold;
		}

		if (incidentIndex == -1)
		{
			// neighboring segment is the separating axis
			return manifold;
		}

		// fall through segment normal axis
	}

	SQ_ASSERT(incidentNormal != -1 || incidentIndex != -1);

	// Segment normal

	// Find incident polygon normal: normal adjacent to deepest vertex that is most anti-parallel to segment normal
	SqVec2 b1, b2;
	int ib1, ib2;

	// printf("incidentIndex %d incidentNormal %d \n", incidentIndex, incidentNormal);

	/**
	 * 	进行线段裁剪：
	 *  p1p2为reference edge
	 *  b1b2为incident edge
	 */

	// 通过上面的SAT找到polygonB上最接近segment的顶点为incidentIndex
	// 但我们这里需要找到的是线段，即以incidentIndex为端点的线段，那么有两条，取哪条呢？

	if (incidentNormal != -1)
	{
		ib1 = incidentNormal;
		ib2 = ib1 < count - 1 ? ib1 + 1 : 0;
		b1 = vertices[ib1];
		b2 = vertices[ib2];
	}
	else
	{
		int i2 = incidentIndex;

		// 先找到incidentIndex顶点的上一个顶点（polygonB为逆时针指定顺序的）
		int i1 = i2 > 0 ? i2 - 1 : count - 1;

		// i1表示与incidentIndex相连的上一条线段
		// i2表示与incidentIndex相连的下一条线段
		// 如果不明白的看看polygonB shape是如何组织的

		float d1 = SqVec2::Dot(normal1, normals[i1]); // 上一条线段的法线与segment法线的点积
		float d2 = SqVec2::Dot(normal1, normals[i2]); // 下一条线段的法线与segment法线的点积

		// 点积越小证明这两条法线夹角越大，夹角越大则它们所在的边越接近
		// 取最接近segmet的边

		if (d1 < d2)
		{
			ib1 = i1;
			ib2 = i2;
			b1 = vertices[ib1];
			b2 = vertices[ib2];
		}
		else
		{
			ib1 = i2;
			ib2 = i2 < count - 1 ? i2 + 1 : 0;
			b1 = vertices[ib1];
			b2 = vertices[ib2];
		}
	}

	// printf("b1 %d  b2 %d \n", ib1, ib2);

	manifold = sqClipSegments(p1, p2, b1, b2, normal1, 0.0f, radiusB, SQ_MAKE_ID(0, ib2), SQ_MAKE_ID(1, ib1));

	SQ_ASSERT(manifold.pointCount == 0 || manifold.pointCount == 2);
	if (manifold.pointCount == 2)
	{
		// There may be no points c
		manifold.normal = xfA.transformVector(manifold.normal);
		manifold.points[0].anchorA = xfA.transformVector(manifold.points[0].anchorA);
		manifold.points[1].anchorA = xfA.transformVector(manifold.points[1].anchorA);
		SqVec2 pAB = SqVec2::Sub(xfA.p, xfB.p);
		manifold.points[0].anchorB = SqVec2::Add(manifold.points[0].anchorA, pAB);
		manifold.points[1].anchorB = SqVec2::Add(manifold.points[1].anchorA, pAB);

		// printf("xfB (%f %f %f %f) a %f %f b %f %f \n",xfB.p.x,xfB.p.y,xfB.q.c,xfB.q.s,
		// 	manifold.points[0].anchorA.x,manifold.points[0].anchorA.y,
		// 	manifold.points[0].anchorB.x,manifold.points[0].anchorB.y
		// );

		manifold.points[0].point = SqVec2::Add(xfA.p, manifold.points[0].anchorA);
		manifold.points[1].point = SqVec2::Add(xfA.p, manifold.points[1].anchorA);
	}

	return manifold;
}
