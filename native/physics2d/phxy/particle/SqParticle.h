#pragma once
#include "../common/math/SqVec2.h"
#include "SqParticleColor.h"
#include "SqIntrusiveList.h"
#include "b2ParticleSetting.h"
#include "SqParticleSystemGrid.h"

namespace phxy
{
	/// @file

	class SqParticleGroup;
	class SqParticleSystem;
	class SqParticleSystemMemory;
	/// The particle type. Can be combined with the | operator.
	enum SqParticleFlag
	{
		/// Water particle.
		sq_waterParticle = 0,
		/// 将粒子标记为destroy
		sq_zombieParticle = 1 << 1,

		/// 标记粒子为不能移动的粒子，速度一直都会为0。一般做碰撞体使用
		sq_wallParticle = 1 << 2,
		/// With restitution from stretching.
		sq_springParticle = 1 << 3,
		/// With restitution from deformation.
		sq_elasticParticle = 1 << 4,
		/// With viscosity.
		sq_viscousParticle = 1 << 5,
		/// Without isotropic pressure.
		sq_powderParticle = 1 << 6,
		/// With surface tension.
		sq_tensileParticle = 1 << 7,
		/// Mix color between contacting particles.
		sq_colorMixingParticle = 1 << 8,
		/// Call b2DestructionListener on destruction.
		sq_destructionListenerParticle = 1 << 9,
		/// Prevents other particles from leaking.
		sq_barrierParticle = 1 << 10,
		/// Less compressibility.
		sq_staticPressureParticle = 1 << 11,
		/// Makes pairs or triads with other particles.
		sq_reactiveParticle = 1 << 12,
		/// With high repulsive force.
		sq_repulsiveParticle = 1 << 13,
		/// Call b2ContactListener when this particle is about to interact with
		/// a rigid body or stops interacting with a rigid body.
		/// This results in an expensive operation compared to using
		/// sq_fixtureContactFilterParticle to detect collisions between
		/// particles.
		sq_fixtureContactListenerParticle = 1 << 14,
		/// Call b2ContactListener when this particle is about to interact with
		/// another particle or stops interacting with another particle.
		/// This results in an expensive operation compared to using
		/// sq_particleContactFilterParticle to detect collisions between
		/// particles.
		sq_particleContactListenerParticle = 1 << 15,

		// 是否应用粒子对其他粒子的压力
		sq_noPressure = 1 << 16
	};

	/// A particle definition holds all the data needed to construct a particle.
	/// You can safely re-use these definitions.
	struct SqParticleDef
	{

#if LIQUIDFUN_EXTERNAL_LANGUAGE_API
		/// Set position with direct floats
		void SetPosition(float x, float y);

		/// Set color with direct ints.
		void SetColor(int r, int g, int b, int a);
#endif // LIQUIDFUN_EXTERNAL_LANGUAGE_API

		/// b2ParticleFlag枚举定义的值。
		/// flags记录粒子的状态。一个粒子可能多个状态，所以这里是一个Mask操作
		/// A particle may be more than one type.
		/// Multiple types are chained by logical sums, for example:
		/// pd.flags = sq_elasticParticle | sq_viscousParticle
		unsigned int flags{0};

		/// The world position of the particle.
		SqVec2 position;

		/// The linear velocity of the particle in world co-ordinates.
		SqVec2 velocity;

		/// The color of the particle.
		SqParticleColor color;

		/// Lifetime of the particle in seconds.  A value <= 0.0f indicates a
		/// particle with infinite lifetime.
		float lifetime{0.f};

		/// Use this to store application-specific body data.
		void *userData{nullptr};

		/// An existing particle group to which the particle will be added.
		SqParticleGroup *group{nullptr};
	};

	class SqParticle
	{

	private:
		int particleBufferIndex = -1;
		int id = -1;
		SqParticleSystem *system = nullptr;
		float ExpirationTimeToLifetime(const int expirationTime) const;

		//使用当前粒子所处的速度计算一个位置，是粒子更新流程中所产生的一个临时位置来的
		//一般用来做碰撞检测
		SqVec2 newPosition;
		inline SqVec2& getNewPosition() { return newPosition; };
	public:
		friend class SqParticleSystem;
		friend class SqParticleSystemMemory;
		friend class SqParticleLink;
		friend class SqParticleGroup;
		friend class SqParticleSystemGrid;
		
		unsigned int flags = 0;

		/**
		 * 当前粒子的速度
		 */
		SqVec2 velocity;

		SqParticle();
		inline int getId() { return id; };
		inline int getBufferIndex() { return particleBufferIndex; };
		void reset();
		void requestSimData();
		SqVec2 &getPosition() const;
		void setPosition(const SqVec2 &pos);
		void setColor(const SqParticleColor &);
		void setUserData(void *userData);
		void *getUserData();
		void setGroup(SqParticleGroup *);
		SqParticleGroup *getGroup();
		void setLifeTime(float lifeTime);
		float getLifetime();
		float getDepth();
		float getWeight();
		SqParticleColor &getColor();
		SqVec2 &getForce();
	};

	/// A helper function to calculate the optimal number of iterations.
	int sqCalculateParticleIterations(
		float gravity, float radius, float timeStep);

#if LIQUIDFUN_EXTERNAL_LANGUAGE_API
	inline void SqParticleDef::SetPosition(float x, float y)
	{
		position.Set(x, y);
	}

	inline void SqParticleDef::SetColor(int r, int g, int b, int a)
	{
		color.Set((unsigned char)r, (unsigned char)g, (unsigned char)b, (unsigned char)a);
	}
#endif // LIQUIDFUN_EXTERNAL_LANGUAGE_API
}