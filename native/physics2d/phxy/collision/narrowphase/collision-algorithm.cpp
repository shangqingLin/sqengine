
#include "collision-algorithm.h"
#include <float.h>
#include <stdio.h>

using namespace phxy;


/**
 * https://dyn4j.org/2011/11/contact-points-using-clipping/
 * 使用线段裁剪算法求交点
 * 
 */
SqManifold phxy::sqClipPolygons(const SqPolygonShape *polyA, const SqPolygonShape *polyB, int edgeA, int edgeB, bool flip)
{
	SqManifold manifold;

	// reference polygon
	const SqPolygonShape *poly1;
	int i11, i12;

	// incident polygon
	const SqPolygonShape *poly2;
	int i21, i22;

	if (flip)
	{
		poly1 = polyB;
		poly2 = polyA;
		i11 = edgeB;
		i12 = edgeB + 1 < polyB->count ? edgeB + 1 : 0;
		i21 = edgeA;
		i22 = edgeA + 1 < polyA->count ? edgeA + 1 : 0;
	}
	else
	{
		poly1 = polyA;
		poly2 = polyB;
		i11 = edgeA;
		i12 = edgeA + 1 < polyA->count ? edgeA + 1 : 0;
		i21 = edgeB;
		i22 = edgeB + 1 < polyB->count ? edgeB + 1 : 0;
	}

	const SqVec2 &normal = poly1->normals[i11];

	// Reference edge vertices
	const SqVec2 &v11 = poly1->vertices[i11];
	const SqVec2 &v12 = poly1->vertices[i12];

	// Incident edge vertices
	const SqVec2 &v21 = poly2->vertices[i21];
	const SqVec2 &v22 = poly2->vertices[i22];

	/**
	 * 使用投影法：
	 * 将v21、v22 投影到（v11，v12）上，通过投影距离判断这两条线段是否相交，
	 * 即通过投影距离判断线段上的点的位置关系来判断是否相交
	 */

	// 求normal的切线，相当于poly1 上（v11，v12）线段的向量表示
	SqVec2 tangent = SqVec2::CrossSV(1.0f, normal);

	// v11 在 tangent的投影，因为tangent就是为v11起点的线段，所以为0
	float lower1 = 0.0f;

	// v12在tangent的投影。长度肯定和（v11，v12）一样，只不过这个投影是有符号，其实这里主要目的是确认upper1的符号
	float upper1 = SqVec2::Dot(SqVec2::Sub(v12, v11), tangent);

	// Incident edge points opposite of tangent due to CCW winding

	// 计算v21在tangent的投影长度
	float upper2 = SqVec2::Dot(SqVec2::Sub(v21, v11), tangent);

	// 计算v22线段在tangent的投影长度
	float lower2 = SqVec2::Dot(SqVec2::Sub(v22, v11), tangent);

	// printf("Edge(%f %f %f %f),Edge2(%f %f %f %f) lower1 %f  upper1 %f lower2 %f upper2 %f %d tangent(%f %f )\n",
	// 	   v11.x, v11.y, v12.x, v12.y,
	// 	   v21.x, v21.y, v22.x, v22.y,
	// 	   lower1, upper1,
	// 	   lower2, upper2,
	// 	   i11,
	// 	   tangent.x, tangent.y);

	/**
	 * 	upper2 < lower1 表示线段2的终点在线段1的起点的后面，所以不可能相交
	 *
	 * 即：   v12
	 * 		 /
	 *    	/
	 *     v11
	 *
	 *     v21  v21完全在v11的后面，因为upper2是小于0表示投影长度为负数，所以整条线段都在后面
	 *      \
	 *       \
	 *       v22
	 *
	 */

	/**
	 *  upper1 < lower2 表示线段1的终点在线段2的起点的后面，所以不可能相交
	 *
	 *即：   v21
	 * 		 /
	 *    	/
	 *     v22
	 *
	 *     v12
	 *      \
	 *       \
	 *       v11
	 *
	 */
	if (upper2 < lower1 || upper1 < lower2)
	{
		return manifold;
	}

	//上面已经证明两条线段，那么接下来需要对incident线段即v22_v21线段进行裁剪

	//对v22进行裁剪
	SqVec2 vLower;
	if (lower2 < lower1 && upper2 - lower2 > FLT_EPSILON)
	{
		/**
		 * 处理v22在v11后面，在reference范围外，则裁剪掉v22
		 * 
		 * 即类似于下面的情况
		 * 		    v12	
		 *      v21 /
		 *        \/
		 *        /\
		 *      v11 \
		 * 		    v22  		
		 * 
		 */
		vLower = SqVec2::Lerp(v22, v21, (lower1 - lower2) / (upper2 - lower2));
	}
	else
	{
		//v22落在v11前面，在reference范围n内则保留v22
		vLower = v22;
	}


	//对v21进行裁剪
	SqVec2 vUpper;
	if (upper2 > upper1 && upper2 - lower2 > FLT_EPSILON)
	{
		/**
		 * 处理v21在v12后面，在reference范围外，则裁掉v21
		 *    v21
		 * 	   \
		 *	 	\	 v12	
		 *  	 \  /
		 *        \/
		 *        /\
		 *      v11 \
		 * 		    v22  	
		 */
		vUpper = SqVec2::Lerp(v22, v21, (upper1 - lower2) / (upper2 - lower2));
	}
	else
	{
		//v21落在v12后面，在reference范围内，则保留v22,
		vUpper = v21;
	}

	// todo vLower can be very close to vUpper, reduce to one point?

	float separationLower = SqVec2::Dot(SqVec2::Sub(vLower, v11), normal);
	float separationUpper = SqVec2::Dot(SqVec2::Sub(vUpper, v11), normal);

	float r1 = poly1->radius;
	float r2 = poly2->radius;

	// Put contact points at midpoint, accounting for radii
	vLower = SqVec2::MulAdd(vLower, 0.5f * (r1 - r2 - separationLower), normal);
	vUpper = SqVec2::MulAdd(vUpper, 0.5f * (r1 - r2 - separationUpper), normal);

	float radius = r1 + r2;

	if (flip == false)
	{
		manifold.normal = normal;
		SqManifoldPoint *cp = manifold.points + 0;

		{
			cp->anchorA = vLower;
			cp->separation = separationLower - radius;
			cp->id = SQ_MAKE_ID(i11, i22);
			manifold.pointCount += 1;
			cp += 1;
		}

		{
			cp->anchorA = vUpper;
			cp->separation = separationUpper - radius;
			cp->id = SQ_MAKE_ID(i12, i21);
			manifold.pointCount += 1;
		}
	}
	else
	{
		manifold.normal = SqVec2::Neg(normal);
		SqManifoldPoint *cp = manifold.points + 0;

		{
			cp->anchorA = vUpper;
			cp->separation = separationUpper - radius;
			cp->id = SQ_MAKE_ID(i21, i12);
			manifold.pointCount += 1;
			cp += 1;
		}

		{
			cp->anchorA = vLower;
			cp->separation = separationLower - radius;
			cp->id = SQ_MAKE_ID(i22, i11);
			manifold.pointCount += 1;
		}
	}

	return manifold;
}

/**
 * 这里使用SAT算法思路，计算多边形poly2到poly1最短距离，返回值就是最短距离。edgeIndex返回在poly1哪条法线上产生了最短距离
 *
 */
float phxy::sqFindMaxSeparation(int *edgeIndex, const SqPolygonShape *poly1, const SqPolygonShape *poly2)
{
	int count1 = poly1->count;
	int count2 = poly2->count;
	const SqVec2 *n1s = poly1->normals;
	const SqVec2 *v1s = poly1->vertices;
	const SqVec2 *v2s = poly2->vertices;

	int bestIndex = 0;
	float maxSeparation = -FLT_MAX;

	// printf("=====check begine ===== \n");
	for (int i = 0; i < count1; ++i)
	{

		const SqVec2 &n = n1s[i];
		const SqVec2 &v1 = v1s[i];

		// printf("poly1 check v1(%f %f ) n(%f %f) \n", v1.x, v1.y, n.x, n.y);

		/**
		 * 获取poly1中的v1顶点，然后获取以v1为端点的那一条线段的法线n
		 *
		 * 然后计算以v1为起点，以poly2上的每个顶点为终点的线段在n上的垂直投影，就可以得出poly2
		 * 中每个顶点投影在n上的点假设为p点，则可以得到p点到v1点的距离。在这么多p点到v1点中，最短的距离的那个顶点就可以认为
		 * 是poly2上离poly1的v1顶点最近的点。
		 *
		 * 1、SqVec2::Sub( v2s[j], v1 ) 这句代码计算以v1为起点，poly2上的每个顶点为终点的线段
		 * 2、然后和n做点积，根据点积的几何意义，这个点积的大小就是为到v1点的投影距离，并且这个距离是有方向（即有正负的）
		 *
		 */

		float si = FLT_MAX;
		for (int j = 0; j < count2; ++j)
		{
			float sij = SqVec2::Dot(n, SqVec2::Sub(v2s[j], v1));

			// printf("   poly2 check index %d v(%f %f ) dot:%f \n", j, v2s[j].x, v2s[j].y, sij);

			if (sij < si)
			{
				si = sij;
			}
		}
		// printf("   bestDot %f \n", si);

		if (si > maxSeparation)
		{
			maxSeparation = si;
			bestIndex = i;
		}
	}

	// printf("------check end bestIndex %d maxSeparation: %f \n", bestIndex, maxSeparation);

	*edgeIndex = bestIndex;
	return maxSeparation;
}

SqManifold phxy::sqClipSegments(SqVec2 a1, SqVec2 a2, SqVec2 b1, SqVec2 b2, SqVec2 normal, float ra, float rb, uint16_t id1, uint16_t id2)
{
	SqManifold manifold;

	SqVec2 tangent = SqVec2::LeftPerp(normal);

	// Barycentric coordinates of each point relative to a1 along tangent
	float lower1 = 0.0f;
	float upper1 = SqVec2::Dot(SqVec2::Sub(a2, a1), tangent);

	// Incident edge points opposite of tangent due to CCW winding
	float upper2 = SqVec2::Dot(SqVec2::Sub(b1, a1), tangent);
	float lower2 = SqVec2::Dot(SqVec2::Sub(b2, a1), tangent);

	// Do segments overlap?
	if (upper2 < lower1 || upper1 < lower2)
	{
		return manifold;
	}

	SqVec2 vLower;
	if (lower2 < lower1 && upper2 - lower2 > FLT_EPSILON)
	{
		vLower = SqVec2::Lerp(b2, b1, (lower1 - lower2) / (upper2 - lower2));
	}
	else
	{
		vLower = b2;
	}

	SqVec2 vUpper;
	if (upper2 > upper1 && upper2 - lower2 > FLT_EPSILON)
	{
		vUpper = SqVec2::Lerp(b2, b1, (upper1 - lower2) / (upper2 - lower2));
	}
	else
	{
		vUpper = b1;
	}

	
	//计算碰撞点在碰撞法线上的距离
	float separationLower = SqVec2::Dot(SqVec2::Sub(vLower, a1), normal);
	float separationUpper = SqVec2::Dot(SqVec2::Sub(vUpper, a1), normal);

	/**
	 * 裁剪出来的vLower和vUpper这两个顶点可能非常接近，这不利于后续的模拟阶段
	 * 所以，需要将vLower和vUpper沿着碰撞法线的方向移动一半的距离
	 */
	vLower = SqVec2::MulAdd(vLower, 0.5f * (ra - rb - separationLower), normal);
	vUpper = SqVec2::MulAdd(vUpper, 0.5f * (ra - rb - separationUpper), normal);
	
	float radius = ra + rb;

	manifold.normal = normal;
	{
		SqManifoldPoint *cp = manifold.points + 0;
		cp->anchorA = vLower;
		cp->separation = separationLower - radius; //法线上的投影距离
		cp->id = id1;
	}

	{
		SqManifoldPoint *cp = manifold.points + 1;
		cp->anchorA = vUpper;
		cp->separation = separationUpper - radius; //法线上的投影距离
		cp->id = id2;
	}

	manifold.pointCount = 2;

	return manifold;
}
