#pragma once

#include "b2ParticleSetting.h"
#include "SqParticleContact.h"

namespace phxy
{

	class SqParticleGroup;
	struct SqParticleBodyContact;
	class SqParticleSystem;

	/// Joints and fixtures are destroyed when their associated
	/// body is destroyed. Implement this listener so that you
	/// may nullify references to these joints and shapes.
	class b2DestructionListener
	{
	public:
		virtual ~b2DestructionListener() {}
		/// Called when any particle group is about to be destroyed.
		virtual void SayGoodbye(SqParticleGroup *group)
		{
			B2_NOT_USED(group);
		}
		virtual void SayGoodbye(SqParticleSystem *particleSystem, int index)
		{
			B2_NOT_USED(particleSystem);
			B2_NOT_USED(index);
		}
	};

	/// Implement this class to provide collision filtering. In other words, you can implement
	/// this class if you want finer control over contact creation.
	class b2ContactFilter
	{
	public:
		virtual ~b2ContactFilter() {}

		virtual bool ShouldCollide(
			SqParticleSystem *particleSystem,
			int particleIndex);

		/// Return true if contact calculations should be performed between two
		/// particles.  This is only called if the
		/// b2_particleContactListenerParticle flag is set on the particle.
		virtual bool ShouldCollide(SqParticleSystem *particleSystem,
								   int particleIndexA, int particleIndexB);
	};

	/// Contact impulses for reporting. Impulses are used instead of forces because
	/// sub-step forces may approach infinity for rigid body collisions. These
	/// match up one-to-one with the contact points in b2Manifold.
	struct b2ContactImpulse
	{
		float normalImpulses[sq_maxManifoldPoints];
		float tangentImpulses[sq_maxManifoldPoints];
		int count;
	};

	/// Implement this class to get contact information. You can use these results for
	/// things like sounds and game logic. You can also get contact results by
	/// traversing the contact lists after the time step. However, you might miss
	/// some contacts because continuous physics leads to sub-stepping.
	/// Additionally you may receive multiple callbacks for the same contact in a
	/// single time step.
	/// You should strive to make your callbacks efficient because there may be
	/// many callbacks per time step.
	/// @warning You cannot create/destroy Box2D entities inside these callbacks.
	class b2ContactListener
	{
	public:
		virtual ~b2ContactListener() {}

		/// Called when a fixture and particle start touching if the
		/// b2_fixtureContactFilterParticle flag is set on the particle.
		virtual void BeginBodyContact(SqParticleSystem *particleSystem,
									  SqParticleBodyContact *particleBodyContact)
		{
			// B2_NOT_USED(particleSystem);
			// B2_NOT_USED(particleBodyContact);
		}

		/// Called when a fixture and particle stop touching if the
		/// b2_fixtureContactFilterParticle flag is set on the particle.
		virtual void EndBodyContact(SqParticleSystem *particleSystem, SqParticleBodyContact *particleBodyContact)
		{
			// B2_NOT_USED(fixture);
			// B2_NOT_USED(particleSystem);
			// B2_NOT_USED(index);
		}

		virtual void UpdateBodyContact(SqParticleSystem *particleSystem, SqParticleBodyContact *particleBodyContact)
		{
		}
		
		/// Called when two particles start touching if
		/// b2_particleContactFilterParticle flag is set on either particle.
		virtual void BeginParticleContact(SqParticleSystem *particleSystem, SqParticleContact *particleContact)
		{
			// B2_NOT_USED(particleSystem);
			// B2_NOT_USED(particleContact);
		}

		/// Called when two particles start touching if
		/// b2_particleContactFilterParticle flag is set on either particle.
		virtual void EndParticleContact(SqParticleSystem *particleSystem, int indexA, int indexB)
		{
			// B2_NOT_USED(particleSystem);
			// B2_NOT_USED(indexA);
			// B2_NOT_USED(indexB);
		}
	};

	/// Callback class for AABB queries.
	/// See b2World::Query
	class b2QueryCallback
	{
	public:
		virtual ~b2QueryCallback() {}

		/// Cull an entire particle system from b2World::QueryAABB. Ignored for
		/// SqParticleSystem::QueryAABB.
		/// @return true if you want to include particleSystem in the AABB query,
		/// or false to cull particleSystem from the AABB query.
		virtual bool ShouldQueryParticleSystem(const SqParticleSystem *particleSystem)
		{
			B2_NOT_USED(particleSystem);
			return true;
		}

		virtual bool ReportParticle(const SqParticleSystem *particleSystem, int index)
		{
			return false;
		}
	};

	/// Callback class for ray casts.
	/// See b2World::RayCast
	class b2RayCastCallback
	{
	public:
		virtual ~b2RayCastCallback() {}

		/// Called for each particle found in the query. You control how the ray
		/// cast proceeds by returning a float:
		/// return <=0: ignore the remaining particles in this particle system
		/// return fraction: ignore particles that are 'fraction' percent farther
		///   along the line from 'point1' to 'point2'. Note that 'point1' and
		///   'point2' are parameters to b2World::RayCast.
		/// @param particleSystem the particle system containing the particle
		/// @param index the index of the particle in particleSystem
		/// @param point the point of intersection bt the ray and the particle
		/// @param normal the normal vector at the point of intersection
		/// @param fraction percent (0.0~1.0) from 'point0' to 'point1' along the
		///   ray. Note that 'point1' and 'point2' are parameters to
		///   b2World::RayCast.
		/// @return <=0 to ignore rest of particle system, fraction to ignore
		/// particles that are farther away.
		virtual float ReportParticle(const SqParticleSystem *particleSystem,
									 int index, const SqVec2 &point,
									 const SqVec2 &normal, float fraction)
		{
			// B2_NOT_USED(particleSystem);
			// B2_NOT_USED(index);
			// B2_NOT_USED(&point);
			// B2_NOT_USED(&normal);
			// B2_NOT_USED(fraction);
			return 0;
		}

		/// Cull an entire particle system from b2World::RayCast. Ignored in
		/// SqParticleSystem::RayCast.
		/// @return true if you want to include particleSystem in the RayCast, or
		/// false to cull particleSystem from the RayCast.
		virtual bool ShouldQueryParticleSystem(
			const SqParticleSystem *particleSystem)
		{
			// B2_NOT_USED(particleSystem);
			return true;
		}
	};
}