#include "collide-capsule-capsule.h"
#include "../../common/SqCore.h"
#include "../../common/SqConfig.h"
#include "../../common/math/SqMath.h"

#include <float.h>

using namespace phxy;

// Follows Ericson 5.1.9 Closest Points of Two Line Segments
// Adds some logic to support clipping to get two contact points
SqManifold phxy::sqCollideCapsules( const SqCapsuleShape* capsuleA, SqTransform& xfA, const SqCapsuleShape* capsuleB, SqTransform& xfB )
{
	SqVec2 origin = capsuleA->center1;
    const float speculativeDistance = SqConfig::getInstance()->getSpeculativeDistance();
    const float linearSlop = SqConfig::getInstance()->getLinearSlop();


	// Shift polyA to origin
	// pw = q * pb + p
	// pw = q * (pbs + origin) + p
	// pw = q * pbs + (p + q * origin)
	SqTransform sfA = { SqVec2::Add( xfA.p, xfA.transformVector(origin ) ), xfA.q };
	SqTransform xf = SqTransform::InvMulTransforms( sfA, xfB );

	SqVec2 p1;
	SqVec2 q1 = SqVec2::Sub( capsuleA->center2, origin );

	SqVec2 p2 = xf.transformPoint(capsuleB->center1 );
	SqVec2 q2 = xf.transformPoint(capsuleB->center2 );

	SqVec2 d1 = SqVec2::Sub( q1, p1 );
	SqVec2 d2 = SqVec2::Sub( q2, p2 );

	float dd1 = SqVec2::Dot( d1, d1 );
	float dd2 = SqVec2::Dot( d2, d2 );

	const float epsSqr = FLT_EPSILON * FLT_EPSILON;
	SQ_ASSERT( dd1 > epsSqr && dd2 > epsSqr );

	SqVec2 r = SqVec2::Sub( p1, p2 );
	float rd1 = SqVec2::Dot( r, d1 );
	float rd2 = SqVec2::Dot( r, d2 );

	float d12 = SqVec2::Dot( d1, d2 );

	float denom = dd1 * dd2 - d12 * d12;

	// Fraction on segment 1
	float f1 = 0.0f;
	if ( denom != 0.0f )
	{
		// not parallel
		f1 = clamp( ( d12 * rd2 - rd1 * dd2 ) / denom, 0.0f, 1.0f );
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

	SqVec2 closest1 = SqVec2::MulAdd( p1, f1, d1 );
	SqVec2 closest2 = SqVec2::MulAdd( p2, f2, d2 );
	float distanceSquared = SqVec2::DistanceSquared( closest1, closest2 );

	SqManifold manifold;
	float radiusA = capsuleA->radius;
	float radiusB = capsuleB->radius;
	float radius = radiusA + radiusB;
	float maxDistance = radius + speculativeDistance;

	if ( distanceSquared > maxDistance * maxDistance )
	{
		return manifold;
	}

	float distance = sqrtf( distanceSquared );

	float length1, length2;
	SqVec2 u1 = SqVec2::GetLengthAndNormalize( &length1, d1 );
	SqVec2 u2 = SqVec2::GetLengthAndNormalize( &length2, d2 );

	// Does segment B project outside segment A?
	float fp2 = SqVec2::Dot( SqVec2::Sub( p2, p1 ), u1 );
	float fq2 = SqVec2::Dot( SqVec2::Sub( q2, p1 ), u1 );
	bool outsideA = ( fp2 <= 0.0f && fq2 <= 0.0f ) || ( fp2 >= length1 && fq2 >= length1 );

	// Does segment A project outside segment B?
	float fp1 = SqVec2::Dot( SqVec2::Sub( p1, p2 ), u2 );
	float fq1 = SqVec2::Dot( SqVec2::Sub( q1, p2 ), u2 );
	bool outsideB = ( fp1 <= 0.0f && fq1 <= 0.0f ) || ( fp1 >= length2 && fq1 >= length2 );

	if ( outsideA == false && outsideB == false )
	{
		// attempt to clip
		// this may yield contact points with excessive separation
		// in that case the algorithm falls back to single point collision

		// find reference edge using SAT
		SqVec2 normalA;
		float separationA;

		{
			normalA = SqVec2::LeftPerp( u1 );
			float ss1 = SqVec2::Dot( SqVec2::Sub( p2, p1 ), normalA );
			float ss2 = SqVec2::Dot( SqVec2::Sub( q2, p1 ), normalA );
			float s1p = ss1 < ss2 ? ss1 : ss2;
			float s1n = -ss1 < -ss2 ? -ss1 : -ss2;

			if ( s1p > s1n )
			{
				separationA = s1p;
			}
			else
			{
				separationA = s1n;
				normalA = SqVec2::Neg( normalA );
			}
		}

		SqVec2 normalB;
		float separationB;
		{
			normalB = SqVec2::LeftPerp( u2 );
			float ss1 = SqVec2::Dot( SqVec2::Sub( p1, p2 ), normalB );
			float ss2 = SqVec2::Dot( SqVec2::Sub( q1, p2 ), normalB );
			float s1p = ss1 < ss2 ? ss1 : ss2;
			float s1n = -ss1 < -ss2 ? -ss1 : -ss2;

			if ( s1p > s1n )
			{
				separationB = s1p;
			}
			else
			{
				separationB = s1n;
				normalB = SqVec2::Neg( normalB );
			}
		}

		// biased to avoid feature flip-flop
		// todo more testing?
		if ( separationA + 0.1f * linearSlop >= separationB )
		{
			manifold.normal = normalA;

			SqVec2 cp = p2;
			SqVec2 cq = q2;

			// clip to p1
			if ( fp2 < 0.0f && fq2 > 0.0f )
			{
				cp = SqVec2::Lerp( p2, q2, ( 0.0f - fp2 ) / ( fq2 - fp2 ) );
			}
			else if ( fq2 < 0.0f && fp2 > 0.0f )
			{
				cq = SqVec2::Lerp( q2, p2, ( 0.0f - fq2 ) / ( fp2 - fq2 ) );
			}

			// clip to q1
			if ( fp2 > length1 && fq2 < length1 )
			{
				cp = SqVec2::Lerp( p2, q2, ( fp2 - length1 ) / ( fp2 - fq2 ) );
			}
			else if ( fq2 > length1 && fp2 < length1 )
			{
				cq = SqVec2::Lerp( q2, p2, ( fq2 - length1 ) / ( fq2 - fp2 ) );
			}

			float sp = SqVec2::Dot( SqVec2::Sub( cp, p1 ), normalA );
			float sq = SqVec2::Dot( SqVec2::Sub( cq, p1 ), normalA );

			if ( sp <= distance + linearSlop || sq <= distance + linearSlop )
			{
				SqManifoldPoint* mp;
				mp = manifold.points + 0;
				mp->anchorA = SqVec2::MulAdd( cp, 0.5f * ( radiusA - radiusB - sp ), normalA );
				mp->separation = sp - radius;
				mp->id = SQ_MAKE_ID( 0, 0 );

				mp = manifold.points + 1;
				mp->anchorA = SqVec2::MulAdd( cq, 0.5f * ( radiusA - radiusB - sq ), normalA );
				mp->separation = sq - radius;
				mp->id = SQ_MAKE_ID( 0, 1 );
				manifold.pointCount = 2;
			}
		}
		else
		{
			// normal always points from A to B
			manifold.normal = SqVec2::Neg( normalB );

			SqVec2 cp = p1;
			SqVec2 cq = q1;

			// clip to p2
			if ( fp1 < 0.0f && fq1 > 0.0f )
			{
				cp = SqVec2::Lerp( p1, q1, ( 0.0f - fp1 ) / ( fq1 - fp1 ) );
			}
			else if ( fq1 < 0.0f && fp1 > 0.0f )
			{
				cq = SqVec2::Lerp( q1, p1, ( 0.0f - fq1 ) / ( fp1 - fq1 ) );
			}

			// clip to q2
			if ( fp1 > length2 && fq1 < length2 )
			{
				cp = SqVec2::Lerp( p1, q1, ( fp1 - length2 ) / ( fp1 - fq1 ) );
			}
			else if ( fq1 > length2 && fp1 < length2 )
			{
				cq = SqVec2::Lerp( q1, p1, ( fq1 - length2 ) / ( fq1 - fp1 ) );
			}

			float sp = SqVec2::Dot( SqVec2::Sub( cp, p2 ), normalB );
			float sq = SqVec2::Dot( SqVec2::Sub( cq, p2 ), normalB );

			if ( sp <= distance + linearSlop || sq <= distance + linearSlop )
			{
				SqManifoldPoint* mp;
				mp = manifold.points + 0;
				mp->anchorA = SqVec2::MulAdd( cp, 0.5f * ( radiusB - radiusA - sp ), normalB );
				mp->separation = sp - radius;
				mp->id = SQ_MAKE_ID( 0, 0 );
				mp = manifold.points + 1;
				mp->anchorA = SqVec2::MulAdd( cq, 0.5f * ( radiusB - radiusA - sq ), normalB );
				mp->separation = sq - radius;
				mp->id = SQ_MAKE_ID( 1, 0 );
				manifold.pointCount = 2;
			}
		}
	}

	if ( manifold.pointCount == 0 )
	{
		// single point collision
		SqVec2 normal = SqVec2::Sub( closest2, closest1 );
		if ( SqVec2::Dot( normal, normal ) > epsSqr )
		{
			normal = SqVec2::Normalize( normal );
		}
		else
		{
			normal = SqVec2::LeftPerp( u1 );
		}

		SqVec2 c1 = SqVec2::MulAdd( closest1, radiusA, normal );
		SqVec2 c2 = SqVec2::MulAdd( closest2, -radiusB, normal );

		int i1 = f1 == 0.0f ? 0 : 1;
		int i2 = f2 == 0.0f ? 0 : 1;

		manifold.normal = normal;
		manifold.points[0].anchorA = SqVec2::Lerp( c1, c2, 0.5f );
		manifold.points[0].separation = sqrtf( distanceSquared ) - radius;
		manifold.points[0].id = SQ_MAKE_ID( i1, i2 );
		manifold.pointCount = 1;
	}

	// Convert manifold to world space
	manifold.normal = xfA.transformVector(manifold.normal );
	for ( int i = 0; i < manifold.pointCount; ++i )
	{
		SqManifoldPoint* mp = manifold.points + i;

		// anchor points relative to shape origin in world space
		mp->anchorA = xfA.transformVector(SqVec2::Add( mp->anchorA, origin ) );
		mp->anchorB = SqVec2::Add( mp->anchorA, SqVec2::Sub( xfA.p, xfB.p ) );
		mp->point = SqVec2::Add( xfA.p, mp->anchorA );
	}

	return manifold;
}
