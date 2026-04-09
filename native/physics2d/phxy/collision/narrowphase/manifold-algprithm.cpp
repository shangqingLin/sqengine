#include "manifold-algprithm.h"
#include <float.h>
#include "../../common/math/SqMath.h"
using namespace phxy;

/**
 * 求两条线段最短距离，并得出最短距离在各自线段上的哪个点
 * https://paulbourke.net/geometry/pointlineplane/
 * https://math.stackexchange.com/questions/846054/closest-points-on-two-line-segments
 */
SqSegmentDistanceResult phxy::sqSegmentDistance( SqVec2 p1, SqVec2 q1, SqVec2 p2, SqVec2 q2 )
{
	SqSegmentDistanceResult result;

	SqVec2 d1 = SqVec2::Sub( q1, p1 );
	SqVec2 d2 = SqVec2::Sub( q2, p2 );
	SqVec2 r = SqVec2::Sub( p1, p2 );

	float dd1 = SqVec2::Dot( d1, d1 );
	float dd2 = SqVec2::Dot( d2, d2 );
	float rd1 = SqVec2::Dot( r, d1 );
	float rd2 = SqVec2::Dot( r, d2 );

	const float epsSqr = FLT_EPSILON * FLT_EPSILON;

	if ( dd1 < epsSqr || dd2 < epsSqr )
	{
		// Handle all degeneracies
		if ( dd1 >= epsSqr )
		{
			// Segment 2 is degenerate
			result.fraction1 = clamp( -rd1 / dd1, 0.0f, 1.0f );
			result.fraction2 = 0.0f;
		}
		else if ( dd2 >= epsSqr )
		{
			// Segment 1 is degenerate
			result.fraction1 = 0.0f;
			result.fraction2 = clamp( rd2 / dd2, 0.0f, 1.0f );
		}
		else
		{
			result.fraction1 = 0.0f;
			result.fraction2 = 0.0f;
		}
	}
	else
	{
		// Non-degenerate segments
		float d12 = SqVec2::Dot( d1, d2 );

		float denominator = dd1 * dd2 - d12 * d12;

		// Fraction on segment 1
		float f1 = 0.0f;
		if ( denominator != 0.0f )
		{
			// not parallel
			f1 = clamp( ( d12 * rd2 - rd1 * dd2 ) / denominator, 0.0f, 1.0f );
		}

		// Compute point on segment 2 closest to p1 + f1 * d1
		float f2 = ( d12 * f1 + rd2 ) / dd2;

		// Clamping of segment 2 requires a do over on segment 1
		if ( f2 < 0.0f )
		{
			f2 = 0.0f;
			f1 = clamp( -rd1 / dd1, 0.0f, 1.0f );
		}
		else if ( f2 > 1.0f )
		{
			f2 = 1.0f;
			f1 = clamp( ( d12 - rd1 ) / dd1, 0.0f, 1.0f );
		}

		result.fraction1 = f1;
		result.fraction2 = f2;
	}

	//最短距离发生在线段a上的点
	result.closest1 = SqVec2::MulAdd( p1, result.fraction1, d1 );

	//最短距离发生线段b上的点
	result.closest2 = SqVec2::MulAdd( p2, result.fraction2, d2 );

	//得到最短距离。为平方的距离
	result.distanceSquared = SqVec2::DistanceSquared( result.closest1, result.closest2 );
	return result;
}
