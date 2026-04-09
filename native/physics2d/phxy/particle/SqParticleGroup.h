#pragma once

#include "../common/math/SqVec2.h"
#include "../common/math/SqTransform.h"
#include "../geometry/shape/SqShape.h"
#include "../common/collection/SqBitSet.h"
#include "SqParticleColor.h"

namespace phxy
{
	class SqParticleSystem;
	class SqParticleGroup;

	/// The particle group type.  Can be combined with the | operator.
	enum SqParticleGroupFlag
	{
		/// 阻止组与组之间穿透或粒子逃逸出group Prevents overlapping or leaking.
		b2_solidParticleGroup = 1 << 0,

		/// Keeps its shape.
		b2_rigidParticleGroup = 1 << 1,
		/// Won't be destroyed if it gets empty.
		b2_particleGroupCanBeEmpty = 1 << 2,
		/// Will be destroyed on next simulation step.
		b2_particleGroupWillBeDestroyed = 1 << 3,
		/// Updates depth data on next simulation step.
		b2_particleGroupNeedsUpdateDepth = 1 << 4,
		b2_particleGroupInternalMask = b2_particleGroupWillBeDestroyed | b2_particleGroupNeedsUpdateDepth,
	};

	/// A particle group definition holds all the data needed to construct a
	/// particle group.  You can safely re-use these definitions.
	struct SqParticleGroupDef
	{

		SqParticleGroupDef()
		{
			flags = 0;
			groupFlags = 0;
			angle = 0;
			angularVelocity = 0;
			strength = 1;
			shape = nullptr;
			shapes = nullptr;
			shapeCount = 0;
			stride = 0;
			particleCount = 0;
			positionData = nullptr;
			lifetime = 0.0f;
			userData = nullptr;
		}

		~SqParticleGroupDef()
		{

			if (shape)
			{
				delete shape;
			}

			if (shapes)
			{
				delete[] shapes;
			}
		}

		/// The particle-behavior flags (See #b2ParticleFlag).
		unsigned int flags;

		/// The group-construction flags (See #b2ParticleGroupFlag).
		unsigned int groupFlags;

		/// The world position of the group.
		/// Moves the group's shape a distance equal to the value of position.
		SqVec2 position;

		/// The world angle of the group in radians.
		/// Rotates the shape by an angle equal to the value of angle.
		float angle;

		/// The linear velocity of the group's origin in world co-ordinates.
		SqVec2 linearVelocity;

		/// The angular velocity of the group.
		float angularVelocity;

		/// The color of all particles in the group.
		SqParticleColor color;

		/// The strength of cohesion among the particles in a group with flag
		/// b2_elasticParticle or b2_springParticle.
		float strength;

		/// The shape where particles will be added.
		const SqShape *shape;

		/// A array of shapes where particles will be added.
		const SqShape *const *shapes;

		/// The number of shapes.
		int shapeCount;

		/// 定义每个粒子之间的间隔。通过定义Shape和这个，
		/// 那么LiqFun就可以计算这个形状空间中有多少个粒子了
		/// If it is 0, b2_particleStride * particleDiameter is used instead.
		float stride;

		/**
		 * 除了可以通过形状创建，还可你自己指定每个粒子的初始位置来创建
		 * positionData就是用来指定粒子位置，particleCount是数组大小
		 */
		int particleCount;
		const SqVec2 *positionData;

		/// Lifetime of the particle group in seconds.  A value <= 0.0f indicates a
		/// particle group with infinite lifetime.
		float lifetime;

		/// Use this to store application-specific group data.
		void *userData;
	};

	/// A group of particles. SqParticleGroup::CreateParticleGroup creates these.
	class SqParticleGroup
	{
	private:
		friend class SqParticleSystem;
		SqParticleSystem *m_system;
		unsigned int m_groupFlags;
		float m_strength;
		SqParticleGroup *m_prev;
		SqParticleGroup *m_next;
		
		int particleCount = 0;
		SqBitSet particles;

		mutable int m_timestamp;
		mutable float m_mass;
		mutable float m_inertia;
		mutable SqVec2 m_center;
		mutable SqVec2 m_linearVelocity;
		mutable float m_angularVelocity;
		mutable SqTransform m_transform;

		void *m_userData;

		SqParticleGroup();
		~SqParticleGroup();
		void UpdateStatistics() const;

	public:
		/// Get the next particle group from the list in b2_World.
		SqParticleGroup *GetNext();
		const SqParticleGroup *GetNext() const;

		/// Get the particle system that holds this particle group.
		SqParticleSystem *GetParticleSystem();
		const SqParticleSystem *GetParticleSystem() const;

		void addParticle(int);
		void addParticles(SqBitSet&);
		void removeAllParticle();
		void removeParticle(int,bool removeFromSystem);

		/// Get the number of particles.
		int GetParticleCount() const;

		/// Does this group contain the particle.
		bool ContainsParticle(int index) const;

		/// Get the logical sum of particle flags.
		unsigned int GetAllParticleFlags() const;

		/// Get the construction flags for the group.
		unsigned int GetGroupFlags() const;

		/// Set the construction flags for the group.
		void SetGroupFlags(unsigned int flags);

		/// Get the total mass of the group: the sum of all particles in it.
		float GetMass() const;

		/// Get the moment of inertia for the group.
		float GetInertia() const;

		/// Get the center of gravity for the group.
		SqVec2 GetCenter() const;

		/// Get the linear velocity of the group.
		SqVec2 GetLinearVelocity() const;

		/// Get the angular velocity of the group.
		float GetAngularVelocity() const;

		/// Get the position of the group's origin and rotation.
		/// Used only with groups of rigid particles.
		const SqTransform &GetTransform() const;

		/// Get position of the particle group as a whole.
		/// Used only with groups of rigid particles.
		const SqVec2 &GetPosition() const;

		/// Get the rotational angle of the particle group as a whole.
		/// Used only with groups of rigid particles.
		float GetAngle() const;

		/// Get the world linear velocity of a world point, from the average linear
		/// and angular velocities of the particle group.
		/// @param a point in world coordinates.
		/// @return the world velocity of a point.
		SqVec2 GetLinearVelocityFromWorldPoint(const SqVec2 &worldPoint) const;

		/// Get the user data pointer that was provided in the group definition.
		void *GetUserData() const;

		/// Set the user data. Use this to store your application specific data.
		void SetUserData(void *data);

		/// Call SqParticleSystem::ApplyForce for every particle in the group.
		void ApplyForce(const SqVec2 &force);

		/// Call SqParticleSystem::ApplyLinearImpulse for every particle in the
		/// group.
		void ApplyLinearImpulse(const SqVec2 &impulse);

		/// Destroy all the particles in this group.
		/// This function is locked during callbacks.
		/// @param Whether to call the world b2DestructionListener for each
		/// particle is destroyed.
		/// @warning This function is locked during callbacks.
		void DestroyParticles(bool callDestructionListener);

		/// Destroy all particles in this group without enabling the destruction
		/// callback for destroyed particles.
		/// This function is locked during callbacks.
		/// @warning This function is locked during callbacks.
		void DestroyParticles();
	};

	inline SqParticleGroup *SqParticleGroup::GetNext()
	{
		return m_next;
	}

	inline const SqParticleGroup *SqParticleGroup::GetNext() const
	{
		return m_next;
	}

	inline SqParticleSystem *SqParticleGroup::GetParticleSystem()
	{
		return m_system;
	}

	inline const SqParticleSystem *SqParticleGroup::GetParticleSystem() const
	{
		return m_system;
	}

	inline unsigned int SqParticleGroup::GetGroupFlags() const
	{
		return m_groupFlags & ~b2_particleGroupInternalMask;
	}

	inline float SqParticleGroup::GetMass() const
	{
		UpdateStatistics();
		return m_mass;
	}

	inline float SqParticleGroup::GetInertia() const
	{
		UpdateStatistics();
		return m_inertia;
	}

	inline SqVec2 SqParticleGroup::GetCenter() const
	{
		UpdateStatistics();
		return m_center;
	}

	inline SqVec2 SqParticleGroup::GetLinearVelocity() const
	{
		UpdateStatistics();
		return m_linearVelocity;
	}

	inline float SqParticleGroup::GetAngularVelocity() const
	{
		UpdateStatistics();
		return m_angularVelocity;
	}

	inline const SqTransform &SqParticleGroup::GetTransform() const
	{
		return m_transform;
	}

	inline const SqVec2 &SqParticleGroup::GetPosition() const
	{
		return m_transform.p;
	}

	inline float SqParticleGroup::GetAngle() const
	{
		return SqRot::GetAngle(m_transform.q);
	}

	inline SqVec2 SqParticleGroup::GetLinearVelocityFromWorldPoint(
		const SqVec2 &worldPoint) const
	{
		UpdateStatistics();
		return m_linearVelocity + SqVec2::CrossSV(m_angularVelocity, worldPoint - m_center);
	}

	inline void *SqParticleGroup::GetUserData() const
	{
		return m_userData;
	}

	inline void SqParticleGroup::SetUserData(void *data)
	{
		m_userData = data;
	}

	inline void SqParticleGroup::DestroyParticles()
	{
		DestroyParticles(false);
	}
}