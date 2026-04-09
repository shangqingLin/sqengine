#include "collide-capsule-circle.h"
#include "../../common/SqConfig.h"

using namespace phxy;

/// Compute the collision manifold between a capsule and circle
SqManifold phxy::sqCollideCapsuleAndCircle( const SqCapsuleShape* capsuleA, SqTransform& xfA, const SqCircleShape* circleB, SqTransform& xfB )
{
	SqManifold manifold;
    const float speculativeDistance = SqConfig::getInstance()->getSpeculativeDistance();
	SqTransform xf = SqTransform::InvMulTransforms( xfA, xfB );

	// Compute circle position in the frame of the capsule.
	SqVec2 pB = xf.transformPoint(circleB->center );

	// Compute closest point
	SqVec2 p1 = capsuleA->center1;
	SqVec2 p2 = capsuleA->center2;

	SqVec2 e = SqVec2::Sub( p2, p1 );

	// dot(p - pA, e) = 0
	// dot(p - (p1 + s1 * e), e) = 0
	// s1 = dot(p - p1, e)
	SqVec2 pA;
	float s1 = SqVec2::Dot( SqVec2::Sub( pB, p1 ), e );
	float s2 = SqVec2::Dot( SqVec2::Sub( p2, pB ), e );
	if ( s1 < 0.0f )
	{
		// p1 region
		pA = p1;
	}
	else if ( s2 < 0.0f )
	{
		// p2 region
		pA = p2;
	}
	else
	{
		// circle colliding with segment interior
		float s = s1 / SqVec2::Dot( e, e );
		pA = SqVec2::MulAdd( p1, s, e );
	}

	float distance;
	SqVec2 normal = SqVec2::GetLengthAndNormalize( &distance, SqVec2::Sub( pB, pA ) );

	float radiusA = capsuleA->radius;
	float radiusB = circleB->radius;
	float separation = distance - radiusA - radiusB;
	if ( separation > speculativeDistance )
	{
		return manifold;
	}

	SqVec2 cA = SqVec2::MulAdd( pA, radiusA, normal );
	SqVec2 cB = SqVec2::MulAdd( pB, -radiusB, normal );
	SqVec2 contactPointA = SqVec2::Lerp( cA, cB, 0.5f );

	manifold.normal = xfA.transformVector(normal );
	SqManifoldPoint* mp = manifold.points + 0;
	mp->anchorA = xfA.transformVector(contactPointA );
	mp->anchorB = SqVec2::Add( mp->anchorA, SqVec2::Sub( xfA.p, xfB.p ) );
	mp->point = SqVec2::Add( xfA.p, mp->anchorA );
	mp->separation = separation;
	mp->id = 0;
	manifold.pointCount = 1;
	return manifold;
}
