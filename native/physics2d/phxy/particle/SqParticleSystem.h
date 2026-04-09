#pragma once

#include "b2ParticleSetting.h"
#include "b2BlockAllocator.h"
#include "b2GrowableBuffer.h"
#include "SqParticle.h"
#include "SqParticleContact.h"
#include "b2SlabAllocator.h"
#include "SqParticleGroup.h"
#include "b2ParticleAssembly.h"
#include "SqParticleSystemGrid.h"
#include "SqParticleSystemUtils.h"
#include "SqParticleData.h"
#include "SqParticleSystemMemory.h"
#include "SqParticleTriad.h"

#include "../../../engine/core/core.h"
#include "../common/math/SqVec2.h"
#include "../common/math/SqAABB.h"
#include "../common/math/SqTransform.h"
#include "../common/collection/SqArray.h"
#include "../common/collection/SqBitSet.h"

#include "../geometry/shape/SqShape.h"
#include "../dynamics/SqWorld.h"

#ifdef LIQUIDFUN_UNIT_TESTS
#include <gtest/gtest.h>
#endif // LIQUIDFUN_UNIT_TESTS

#if LIQUIDFUN_EXTERNAL_LANGUAGE_API
#include <cstring>
#endif // LIQUIDFUN_EXTERNAL_LANGUAGE_API

namespace phxy
{

	class b2ParticlePairSet;
	class FixtureParticleSet;
	class SqParticleSystemWorld;

	struct b2ParticleSystemDef
	{
		b2ParticleSystemDef()
		{
			strictContactCheck = false;
			density = 1.0f;
			gravityScale = 1.0f;
			radius = 1.0f;
			maxCount = 0;

			// Initialize physical coefficients to the maximum values that
			// maintain numerical stability.
			pressureStrength = 0.05f;
			dampingStrength = 1.0f;
			elasticStrength = 0.25f;
			springStrength = 0.25f;
			viscousStrength = 0.25f;
			surfaceTensionPressureStrength = 0.2f;
			surfaceTensionNormalStrength = 0.2f;
			repulsiveStrength = 1.0f;
			powderStrength = 0.5f;
			ejectionStrength = 0.5f;
			staticPressureStrength = 0.2f;
			staticPressureRelaxation = 0.2f;
			staticPressureIterations = 8;

			colorMix = false;
			useDataBuffer = false;

			/**
			 * 颜色混合因子
			 */
			colorMixingStrength = 0.5f;
			destroyByAge = false;
			lifetimeGranularity = 1.0f / 60.0f;

			userData = nullptr;
		}

		void *userData;

		bool colorMix;
		bool useDataBuffer;

		/// Enable strict Particle/Body contact check.
		/// See SetStrictContactCheck for details.
		bool strictContactCheck;

		/// 粒子的密度，影响粒子的质量（质量 = 体积 × 密度）。
		/// See SetDensity for details.
		float density;

		/// 粒子受重力的强度比例（1 表示正常重力，0 表示不受重力）。
		///  gravity vector on particles. Default value is 1.0f.
		float gravityScale;

		/// 每个粒子的半径，粒子在物理世界中是圆形,这里设置这个圆形的半径。
		float radius;

		/**
		 * 最多可以同时存在的粒子数量（默认无限，直到内存用完）。
		 * 超出这个限制粒子不会生成
		 */
		/// See SetMaxParticleCount for details.
		int maxCount;

		/**
		 * 控制粒子压强的生成大小
		 * 越大生成的压强越大
		 */
		float pressureStrength;

		/**
		 * 碰撞后速度减慢的程度（类似空气阻力），越大减速越明显。
		 */
		float dampingStrength;

		/**
		 * 弹性粒子群体恢复原始形状的能力，比如果冻弹性。
		 * 即粒子挤压之后，恢复成原来的形状的能力
		 */
		float elasticStrength;

		/**
		 * 弹簧粒子之间的长度保持力，类似布料或连接绳。
		 */
		float springStrength;

		/**
		 * 粘性粒子之间的阻力。设置粘度系数。值越大越稠
		 * 像是蜂蜜那种滑腻感。
		 */
		float viscousStrength;

		/// 粒子内部的表面张力，越大越能维持水滴的形状。
		/// 0~0.2. Larger values increase the amount of surface tension.
		float surfaceTensionPressureStrength;

		/// 让水滴边缘变光滑圆润的能力。
		/// Smoothes outline of tensile particles
		/// 0~0.2. Larger values result in rounder, smoother, water-drop-like
		/// clusters of particles.
		float surfaceTensionNormalStrength;

		/**
		 * 粒子之间互相排斥的强度，值越大越分散。值也可以是负的（变成吸引）。
		 */
		/// Produces additional pressure on repulsive particles
		/// Larger values repulse more
		/// Negative values mean attraction. The range where particles behave
		/// stably is about -0.2 to 2.0.
		float repulsiveStrength;

		/**
		 * 类似沙子/粉末那样，粒子互相弹开的强度。
		 */
		/// Produces repulsion between powder particles
		/// Larger values repulse more
		float powderStrength;

		/// 把粒子从固体粒子群里挤出来的能力（防止卡住）。
		/// Pushes particles out of solid particle group
		/// Larger values repulse more
		float ejectionStrength;

		/**
		 * 近似于真实液体的静压，会让粒子在底部有更多压力。
		 */
		/// Produces static pressure
		/// Larger values increase the pressure on neighboring partilces
		/// For a description of static pressure, see
		/// http://en.wikipedia.org/wiki/Static_pressure#Static_pressure_in_fluid_dynamics
		float staticPressureStrength;

		/**
		 * 控制静压计算的稳定性，值越大收敛越快。
		 */
		/// Reduces instability in static pressure calculation
		/// Larger values make stabilize static pressure with fewer iterations
		float staticPressureRelaxation;

		/// 迭代多少次来求出静压值，值越大越精确但越慢。
		/// Computes static pressure more precisely
		/// See SetStaticPressureIterations for details
		int staticPressureIterations;

		/// 两个发生碰撞的粒子之间进行颜色混合
		/// colorMixingStrength相当于设置混合的alpha值
		/// Determines how fast colors are mixed
		/// 1.0f ==> mixed immediately
		/// 0.5f ==> mixed half way each simulation step (see b2World::Step())
		float colorMixingStrength;

		/// Whether to destroy particles by age when no more particles can be
		/// created.  See #b2ParticleSystem::SetDestructionByAge() for
		/// more information.
		/**
		 * 一個粒子系統中的粒子个数是有限制的，
		 * 当粒子个数达到最大上限时：
		 * 1、如果这个参数为true：那么就会将最久的粒子销毁，然后添加新的粒子。
		 * 2、如果这个参数为false，那么新的粒子将添加不进去，无法再添加粒子了
		 */
		bool destroyByAge;

		/// 我们在SetParticleLifetime()中设置的粒子寿命，设置的是时长
		/// 当在实际执行的时候不是以时长来判断，而是帧数。这个就是指定帧率，即 粒子寿命 = int(时长/lifetimeGranularity)
		/// Granularity of particle lifetimes in seconds.  By default this is
		/// set to (1.0f / 60.0f) seconds.  b2ParticleSystem uses a 32-bit signed
		/// value to track particle lifetimes so the maximum lifetime of a
		/// particle is (2^32 - 1) / (1.0f / lifetimeGranularity) seconds.
		/// With the value set to 1/60 the maximum lifetime or age of a particle is
		/// 2.27 years.
		float lifetimeGranularity;
	};

	class SqParticleSystem
	{
	private:
		bool m_paused;
		int m_timestamp;
		int m_allParticleFlags;
		bool m_needsUpdateAllParticleFlags;
		int m_allGroupFlags;
		bool m_needsUpdateAllGroupFlags;
		bool m_hasForce;
		int m_iterationIndex;
		float m_inverseDensity;

		void *m_userData = nullptr;

		SqArray<SqParticle> partilceArray;

		// 记录partilceArray中哪些节点已经销毁可以重用的
		SqBitSet poolPartilceIds;

		// 记录partilceArray中哪些粒子是激活的
		SqBitSet particles;

		SqParticleSystemMemory data;
		SqParticleSystemGrid grid;

		SqShapeFilter bodyContactfilter;

		/**
		 * 粒子是按照圆形来模拟的。这里存储了位于这个粒子系统中的每个粒子的直径
		 * 通过这个直径来控制粒子的大小
		 */
		float m_particleDiameter;

		// 直径的倒数。 m_inverseDiameter = 1/m_particleDiameter
		// 后续很多算法用到不用每次计算，这里先计算缓存起来
		float m_inverseDiameter;

		// 直径的平方。m_squaredDiameter = m_particleDiameter * m_particleDiameter
		// 后续很多算法用到不用每次计算，这里先计算缓存起来
		float m_squaredDiameter;

		/// Allocator for SqParticleHandle instances.
		// b2SlabAllocator<SqParticleHandle> m_handleAllocator;

		/// Stuck particle detection parameters and record keeping
		// int m_stuckThreshold;

		int64 m_timeElapsed;
		/// Whether the expiration time buffer has been modified and needs to be
		/// resorted.
		bool m_expirationTimeBufferRequiresSorting;

		int m_groupCount;

		// 使用链表的方式存储系统中所有的粒子组
		SqParticleGroup *m_groupList;

		b2ParticleSystemDef m_def;
		SqParticleSystemWorld *world;

		struct SolveCollisionParticeInfo
		{

			static const int shapeNum = 4;
			int shapeCount{0};
			int stepStamp{0};
			int hitStepStamp{0};
			bool hasHit{false};
			float hitFraction{1.0f};
			SqVec2 hitPosition;
			SqVec2 hitNormal;
			SqShape *shape[SolveCollisionParticeInfo::shapeNum];
			SqVec2 normal[SolveCollisionParticeInfo::shapeNum];
			float distance[SolveCollisionParticeInfo::shapeNum];
			bool collision[SolveCollisionParticeInfo::shapeNum];
		};

		SolveCollisionParticeInfo *solveCollisionParticeArray;

	public:
		friend class SqParticle;
		friend class SqParticleSystemGrid;
		friend class SqParticleGroup;
		friend class SqParticleSystemMemory;
		friend class SqParticleSystemWorld;
		friend class b2ParticleBodyContactRemovePredicate;
		friend class SqParticleBodyContact;
		friend class SqParticleContact;

		friend bool b2FixtureParticleQueryCallback(SqShape *shape, void *context);
		friend bool SolveCollisionCallback(SqShape *shape, void *context);

		// 使用链表的方式存储当前创建所有的System
		SqParticleSystem *m_prev;
		SqParticleSystem *m_next;

		/**
		 * 创建一个粒子，返回粒子的ID
		 */
		int CreateParticle(const SqParticleDef &def);

		inline int findParticleIdByIndex(int index)
		{
			return data.findParticle(index);
		}

		/**
		 * 用于销毁指定区域内的粒子
		 * @param shape 这个shape就是指定一个区域，位于这个区域内的粒子被销毁
		 * @param SqTransform shape的世界变换矩阵
		 * @return 返回被销毁的粒子个数
		 */
		inline int DestroyParticlesInShape(SqShape &shape, const SqTransform &xf)
		{
			return DestroyParticlesInShape(shape, xf, false);
		}
		int DestroyParticlesInShape(SqShape &shape, const SqTransform &xf, bool callDestructionListener);
		SqParticleGroup *CreateParticleGroup(const SqParticleGroupDef &def);
		void JoinParticleGroups(SqParticleGroup *groupA, SqParticleGroup *groupB);
		// void SplitParticleGroup(SqParticleGroup *group);
		SqParticleGroup *GetParticleGroupList();
		const SqParticleGroup *GetParticleGroupList() const;

		int GetParticleGroupCount() const;

		/// Get the number of particles.
		int GetParticleCount() const;

		/// Get the maximum number of particles.
		int GetMaxParticleCount() const;

		/// Set the maximum number of particles.
		/// A value of 0 means there is no maximum. The particle buffers can
		/// continue to grow while b2World's block allocator still has memory.
		/// Note: If you try to CreateParticle() with more than this count,
		/// b2_invalidParticleIndex is returned unless
		/// SetDestructionByAge() is used to enable the destruction of the
		/// oldest particles in the system.
		void SetMaxParticleCount(int count);

		/// Get all existing particle flags.
		unsigned int GetAllParticleFlags() const;

		/// Get all existing particle group flags.
		unsigned int GetAllGroupFlags() const;

		inline void *GetUserData() const
		{
			return m_userData;
		}

		/// Pause or unpause the particle system. When paused, b2World::Step()
		/// skips over this particle system. All SqParticleSystem function calls
		/// still work.
		/// @param paused is true to pause, false to un-pause.
		void SetPaused(bool paused);

		/// @return true if the particle system is being updated in
		/// b2World::Step().
		/// Initially, true, then, the last value passed into SetPaused().
		bool GetPaused() const;

		/// Change the particle density.
		/// Particle density affects the mass of the particles, which in turn
		/// affects how the particles interact with b2Bodies. Note that the density
		/// does not affect how the particles interact with each other.
		void SetDensity(float density);

		/// Get the particle density.
		float GetDensity() const;

		/// Change the particle gravity scale. Adjusts the effect of the global
		/// gravity vector on particles.
		void SetGravityScale(float gravityScale);

		/// Get the particle gravity scale.
		float GetGravityScale() const;

		/// Damping is used to reduce the velocity of particles. The damping
		/// parameter can be larger than 1.0f but the damping effect becomes
		/// sensitive to the time step when the damping parameter is large.
		void SetDamping(float damping);

		/// Get damping for particles
		float GetDamping() const;

		/// Change the number of iterations when calculating the static pressure of
		/// particles. By default, 8 iterations. You can reduce the number of
		/// iterations down to 1 in some situations, but this may cause
		/// instabilities when many particles come together. If you see particles
		/// popping away from each other like popcorn, you may have to increase the
		/// number of iterations.
		/// For a description of static pressure, see
		/// http://en.wikipedia.org/wiki/Static_pressure#Static_pressure_in_fluid_dynamics
		void SetStaticPressureIterations(int iterations);

		/// Get the number of iterations for static pressure of particles.
		int GetStaticPressureIterations() const;

		/// Change the particle radius.
		/// You should set this only once, on world start.
		/// If you change the radius during execution, existing particles may
		/// explode, shrink, or behave unexpectedly.
		void SetRadius(float radius);

		/// Get the particle radius.
		float GetRadius() const;

		float GetInverseDiameter();

		/// Get the position of each particle
		/// Array is length GetParticleCount()
		/// @return the pointer to the head of the particle positions array.
		SqVec2 *GetPositionBuffer();
		const SqVec2 *GetPositionBuffer() const;

		/// Get the color of each particle
		/// Array is length GetParticleCount()
		/// @return the pointer to the head of the particle colors array.
		SqParticleColor *GetColorBuffer();
		const SqParticleColor *GetColorBuffer() const;

		/// Get the particle-group of each particle.
		/// Array is length GetParticleCount()
		/// @return the pointer to the head of the particle group array.
		SqParticleGroup *const *GetGroupBuffer();
		const SqParticleGroup *const *GetGroupBuffer() const;

		/// Get the weight of each particle
		/// Array is length GetParticleCount()
		/// @return the pointer to the head of the particle positions array.
		float *GetWeightBuffer();
		const float *GetWeightBuffer() const;

		/// Get the user-specified data of each particle.
		/// Array is length GetParticleCount()
		/// @return the pointer to the head of the particle user-data array.
		void **GetUserDataBuffer();
		void *const *GetUserDataBuffer() const;

		/// Get flags for a particle. See the b2ParticleFlag enum.
		void SetParticleFlags(int id, unsigned int mask);
		unsigned int GetParticleFlags(int id);

		/// Get contacts between particles
		/// Contact data can be used for many reasons, for example to trigger
		/// rendering or audio effects.
		const SqArray<phxy::SqParticleContact> &GetContacts() const;
		int GetContactCount() const;

		const phxy::SqArray<SqParticleBodyContact> &GetBodyContacts() const;
		int GetBodyContactCount() const;

		/// Get array of particle pairs. The particles in a pair:
		///   (1) are contacting,
		///   (2) are in the same particle group,
		///   (3) are part of a rigid particle group, or are spring, elastic,
		///       or wall particles.
		///   (4) have at least one particle that is a spring or barrier
		///       particle (i.e. one of the types in k_pairFlags),
		///   (5) have at least one particle that returns true for
		///       ConnectionFilter::IsNecessary,
		///   (6) are not zombie particles.
		/// Essentially, this is an array of spring or barrier particles that
		/// are interacting. The array is sorted by SqParticlePair's indexA,
		/// and then indexB. There are no duplicate entries.
		const SqParticlePair *GetPairs() const;
		int GetPairCount() const;

		/// Get array of particle triads. The particles in a triad:
		///   (1) are in the same particle group,
		///   (2) are in a Voronoi triangle together,
		///   (3) are within b2_maxTriadDistance particle diameters of each
		///       other,
		///   (4) return true for ConnectionFilter::ShouldCreateTriad
		///   (5) have at least one particle of type elastic (i.e. one of the
		///       types in k_triadFlags),
		///   (6) are part of a rigid particle group, or are spring, elastic,
		///       or wall particles.
		///   (7) are not zombie particles.
		/// Essentially, this is an array of elastic particles that are
		/// interacting. The array is sorted by SqParticleTriad's indexA,
		/// then indexB, then indexC. There are no duplicate entries.
		const SqParticleTriad *GetTriads() const;
		int GetTriadCount() const;

		/// Set an optional threshold for the maximum number of
		/// consecutive particle iterations that a particle may contact
		/// multiple bodies before it is considered a candidate for being
		/// "stuck". Setting to zero or less disables.
		void SetStuckThreshold(int iterations);

		/// Get potentially stuck particles from the last step; the user must
		/// decide if they are stuck or not, and if so, delete or move them
		const int *GetStuckCandidates() const;

		/// Get the number of stuck particle candidates from the last step.
		int GetStuckCandidateCount() const;

		/// Compute the kinetic energy that can be lost by damping force
		float ComputeCollisionEnergy();

		/// Set strict Particle/Body contact check.
		/// This is an option that will help ensure correct behavior if there are
		/// corners in the world model where Particle/Body contact is ambiguous.
		/// This option scales at n*log(n) of the number of Particle/Body contacts,
		/// so it is best to only enable if it is necessary for your geometry.
		/// Enable if you see strange particle behavior around b2Body
		/// intersections.
		void SetStrictContactCheck(bool enabled);
		/// Get the status of the strict contact check.
		bool GetStrictContactCheck() const;

		/// Enable / disable destruction of particles in CreateParticle() when
		/// no more particles can be created due to a prior call to
		/// SetMaxParticleCount().  When this is enabled, the oldest particle is
		/// destroyed in CreateParticle() favoring the destruction of particles
		/// with a finite lifetime over particles with infinite lifetimes.
		/// This feature is enabled by default when particle lifetimes are
		/// tracked.  Explicitly enabling this feature using this function enables
		/// particle lifetime tracking.
		void SetDestructionByAge(const bool enable);
		/// Get whether the oldest particle will be destroyed in CreateParticle()
		/// when the maximum number of particles are present in the system.
		bool GetDestructionByAge() const;

		void SetViscousStrength(float);
		void SetSurfaceTensionPressureStrength(float);
		void SetSurfaceTensionNormalStrength(float);
		void setPressureStrength(float);
		/// Get the array of particle expiration times indexed by particle index.
		/// GetParticleCount() items are in the returned array.
		const int *GetExpirationTimeBuffer();

		/// Apply an impulse to one particle. This immediately modifies the
		/// velocity. Similar to b2Body::ApplyLinearImpulse.
		/// @param index the particle that will be modified.
		/// @param impulse the world impulse vector, usually in N-seconds or
		///        kg-m/s.
		void ParticleApplyLinearImpulse(int index, const SqVec2 &impulse);

		/// Apply an impulse to all particles between 'firstIndex' and 'lastIndex'.
		/// This immediately modifies the velocity. Note that the impulse is
		/// applied to the total mass of all particles. So, calling
		/// ParticleApplyLinearImpulse(0, impulse) and
		/// ParticleApplyLinearImpulse(1, impulse) will impart twice as much
		/// velocity as calling just ApplyLinearImpulse(0, 1, impulse).
		/// @param firstIndex the first particle to be modified.
		/// @param lastIndex the last particle to be modified.
		/// @param impulse the world impulse vector, usually in N-seconds or
		///        kg-m/s.
		// void ApplyLinearImpulse(int firstIndex, int lastIndex, const SqVec2 &impulse);

		/// Apply a force to the center of a particle.
		/// @param index the particle that will be modified.
		/// @param force the world force vector, usually in Newtons (N).
		void ParticleApplyForce(int particleId, const SqVec2 &force);

		/// Distribute a force across several particles. The particles must not be
		/// wall particles. Note that the force is distributed across all the
		/// particles, so calling this function for indices 0..N is not the same as
		/// calling ParticleApplyForce(i, force) for i in 0..N.
		/// @param firstIndex the first particle to be modified.
		/// @param lastIndex the last particle to be modified.
		/// @param force the world force vector, usually in Newtons (N).
		// void ApplyForce(int firstIndex, int lastIndex, const SqVec2 &force);

		/// Get the next particle-system in the world's particle-system list.
		SqParticleSystem *GetNext();
		const SqParticleSystem *GetNext() const;

		/// Query the particle system for all particles that potentially overlap
		/// the provided AABB. b2QueryCallback::ShouldQueryParticleSystem is
		/// ignored.
		/// @param callback a user implemented callback class.
		/// @param aabb the query box.
		void QueryAABB(b2QueryCallback *callback, const SqAABB &aabb);

		/// Query the particle system for all particles that potentially overlap
		/// the provided shape's AABB. Calls QueryAABB internally.
		/// b2QueryCallback::ShouldQueryParticleSystem is ignored.
		/// @param callback a user implemented callback class.
		/// @param shape the query shape
		/// @param xf the transform of the AABB
		void QueryShapeAABB(b2QueryCallback *callback, const SqShape &shape, const SqTransform &xf);

		/// Ray-cast the particle system for all particles in the path of the ray.
		/// Your callback controls whether you get the closest point, any point, or
		/// n-points. The ray-cast ignores particles that contain the starting
		/// point. b2RayCastCallback::ShouldQueryParticleSystem is ignored.
		/// @param callback a user implemented callback class.
		/// @param point1 the ray starting point
		/// @param point2 the ray ending point
		void RayCast(b2RayCastCallback *callback, const SqVec2 &point1, const SqVec2 &point2);

		/// Compute the axis-aligned bounding box for all particles contained
		/// within this particle system.
		/// @param aabb Returns the axis-aligned bounding box of the system.
		void ComputeAABB(SqAABB *const aabb, bool newPosition = false) const;

		void enableAllParticleFlags(bool, unsigned int);
		SqParticleSystemWorld *getWorld();

		void setBodyContactFilter(const SqShapeFilter &bodyContactfilter);
		void SetParticleLifetime(int particleId, float lifetime);
		float GetParticleLifetime(int particleId);
		void DestroyParticle(int particleId, bool callDestructionListener);

		/**
		 * 预先分配指定数量的粒子
		 */
		void allocateParticle(int);

		SqParticle *findById(int id) const;

		inline const SqParticleSystemGrid &GetGrid() { return grid; };
		inline const SqParticleSystemMemory &GetData() { return data; };

	private:
		/// Class for filtering pairs or triads.
		class ConnectionFilter
		{
		public:
			virtual ~ConnectionFilter() {}
			/// Is the particle necessary for connection?
			/// A pair or a triad should contain at least one 'necessary' particle.
			virtual bool IsNecessary(int index) const
			{
				// B2_NOT_USED(index);
				return true;
			}
			/// An additional condition for creating a pair.
			virtual bool ShouldCreatePair(int a, int b) const
			{
				// B2_NOT_USED(a);
				// B2_NOT_USED(b);
				return true;
			}
			/// An additional condition for creating a triad.
			virtual bool ShouldCreateTriad(int a, int b, int c) const
			{
				// B2_NOT_USED(a);
				// B2_NOT_USED(b);
				// B2_NOT_USED(c);
				return true;
			}
		};

		/// Node of linked lists of connected particles
		struct ParticleListNode
		{
			/// The head of the list.
			ParticleListNode *list;
			/// The next node in the list.
			ParticleListNode *next;
			/// Number of entries in the list. Valid only for the node at the head
			/// of the list.
			int count;
			/// Particle index.
			int index;
		};

		/// All particle types that require creating pairs
		static const int k_pairFlags = sq_springParticle | sq_barrierParticle;

		/// All particle types that require creating triads
		static const int k_triadFlags = sq_elasticParticle;

		/// All particle types that do not produce dynamic pressure
		static const int k_noPressureFlags = sq_powderParticle | sq_tensileParticle | sq_noPressure;

		/// All particle types that apply extra damping force with bodies
		static const int k_extraDampingFlags = sq_staticPressureParticle;

		SqParticleSystem(const b2ParticleSystemDef *def, SqParticleSystemWorld *world);
		~SqParticleSystem();

		int CreateParticleForGroup(SqParticleGroup *group, const SqParticleGroupDef &groupDef, const SqTransform &xf, const SqVec2 &position);

		/**
		 * 在由线段指定的形状内创建粒子（Edge或b2ShapChain）
		 */
		void CreateParticlesStrokeShapeForGroup(SqParticleGroup *group, const SqShape *shape, const SqParticleGroupDef &groupDef, const SqTransform &xf);
		void CreateParticlesFillShapeForGroup(SqParticleGroup *group, const SqShape *shape, const SqParticleGroupDef &groupDef, const SqTransform &xf);
		void CreateParticlesWithShapeForGroup(SqParticleGroup *group, const SqShape *shape, const SqParticleGroupDef &groupDef, const SqTransform &xf);
		void CreateParticlesWithShapesForGroup(SqParticleGroup *group, const SqShape *const *shapes, int shapeCount, const SqParticleGroupDef &groupDef, const SqTransform &xf);
		// int CloneParticle(int index, SqParticleGroup *group);
		void DestroyParticleGroup(SqParticleGroup *group);

		void UpdatePairsAndTriads(SqBitSet &particles, const ConnectionFilter &filter);
		void UpdatePairsAndTriadsWithReactiveParticles();
		static bool ComparePairIndices(const SqParticlePair &a, const SqParticlePair &b);
		static bool MatchPairIndices(const SqParticlePair &a, const SqParticlePair &b);
		static bool CompareTriadIndices(const SqParticleTriad &a, const SqParticleTriad &b);
		static bool MatchTriadIndices(const SqParticleTriad &a, const SqParticleTriad &b);

		// static void InitializeParticleLists(const SqParticleGroup *group, ParticleListNode *nodeBuffer);
		// void MergeParticleListsInContact(const SqParticleGroup *group, ParticleListNode *nodeBuffer);
		// static void MergeParticleLists(ParticleListNode *listA, ParticleListNode *listB);
		// static ParticleListNode *FindLongestParticleList(const SqParticleGroup *group, ParticleListNode *nodeBuffer);
		// void MergeZombieParticleListNodes(const SqParticleGroup *group, ParticleListNode *nodeBuffer, ParticleListNode *survivingList) const;
		// static void MergeParticleListAndNode(ParticleListNode *list, ParticleListNode *node);
		// void CreateParticleGroupsFromParticleList(const SqParticleGroup *group, ParticleListNode *nodeBuffer, const ParticleListNode *survivingList);
		// void UpdatePairsAndTriadsWithParticleList(const SqParticleGroup *group, const ParticleListNode *nodeBuffer);

		void ComputeDepth();
		void UpdateAllParticleFlags();
		void UpdateAllGroupFlags();
		void AddContact(int a, int b);
		void FindContacts_Reference();
		// void ReorderForFindContact(FindContactInput *reordered, int alignedCount) const;
		// void GatherChecksOneParticle(const unsigned int bound, const int startIndex, const int particleIndex, int *nextUncheckedIndex, b2GrowableBuffer<FindContactCheck> &checks) const;
		// void GatherChecks(b2GrowableBuffer<FindContactCheck> &checks) const;
		void FindContacts_Simd(b2GrowableBuffer<SqParticleContact> &contacts) const;
		void FindContacts();
		SqParticleBodyContact *CreateContact(int particleId, SqShape *);
		void UpdateContacts(bool exceptZombie);
		void UpdateBodyContacts();
		void fixtureParticleQueryCallback(SqShape *);
		void SystemSolveCollisionCallback(SqShape *, const SqStepContext *context);

		void Solve(const SqStepContext &dt);
		void SolveVelocity(const SqStepContext &dt, bool);
		void SolveCollision(const SqStepContext &dt);
		void LimitVelocity(const SqStepContext &dt);
		void SolveGravity(const SqStepContext &step);
		void SolveBarrier(const SqStepContext &step);
		void SolveStaticPressure(const SqStepContext &step);
		void ComputeWeight();
		void SolvePressure(const SqStepContext &step);
		void SolveDamping(const SqStepContext &step);
		void SolveRigidDamping();
		void SolveExtraDamping();
		void SolveWall();
		void SolveRigid(const SqStepContext &step);
		void SolveElastic(const SqStepContext &step);
		void SolveSpring(const SqStepContext &step);
		void SolveTensile(const SqStepContext &step);
		void SolveViscous();
		void SolveRepulsive(const SqStepContext &step);
		void SolvePowder(const SqStepContext &step);
		void SolveSolid(const SqStepContext &step);
		void SolveForce(const SqStepContext &step);
		void SolveColorMixing();
		void SolveZombie();
		/// Destroy all particles which have outlived their lifetimes set by
		/// SetParticleLifetime().
		void SolveLifetimes(const SqStepContext &step);

		float GetCriticalVelocity(const SqStepContext &step) const;
		float GetCriticalVelocitySquared(const SqStepContext &dt) const;
		float GetCriticalPressure(const SqStepContext &step) const;
		float GetParticleStride() const;
		float GetParticleMass() const;
		float GetParticleInvMass() const;

		// Get the world's contact filter if any particles with the
		// b2_contactFilterParticle flag are present in the system.
		// b2ContactFilter *GetFixtureContactFilter() const;

		// Get the world's contact filter if any particles with the
		// b2_particleContactFilterParticle flag are present in the system.
		b2ContactFilter *GetParticleContactFilter() const;

		// Get the world's contact listener if any particles with the
		// b2_particleContactListenerParticle flag are present in the system.
		b2ContactListener *GetParticleContactListener() const;

		void SetGroupFlags(SqParticleGroup *group, unsigned int flags);

		void RemoveSpuriousBodyContacts();

		void DetectStuckParticle(int particle);

		/// Determine whether a particle index is valid.
		bool ValidateParticleIndex(const int index) const;

		/// Get the time elapsed in b2ParticleSystemDef::lifetimeGranularity.
		int GetQuantizedTimeElapsed() const;
		/// Convert a lifetime in seconds to an expiration time.
		int64 LifetimeToExpirationTime(const float32 lifetime) const;

		bool ForceCanBeApplied(unsigned int flags) const;
		void PrepareForceBuffer();

		bool IsRigidGroup(SqParticleGroup *group) const;
		SqVec2 GetLinearVelocity(SqParticleGroup *group, int particleIndex, const SqVec2 &point);
		void InitDampingParameter(float *invMass, float *invInertia, float *tangentDistance, float mass, float inertia, const SqVec2 &center,
								  const SqVec2 &point, const SqVec2 &normal) const;
		void InitDampingParameterWithRigidGroupOrParticle(
			float *invMass, float *invInertia, float *tangentDistance,
			bool isRigidGroup, SqParticleGroup *group, int particleIndex,
			const SqVec2 &point, const SqVec2 &normal);
		float ComputeDampingImpulse(
			float invMassA, float invInertiaA, float tangentDistanceA,
			float invMassB, float invInertiaB, float tangentDistanceB,
			float normalVelocity) const;
		void ApplyDamping(
			float invMass, float invInertia, float tangentDistance,
			bool isRigidGroup, SqParticleGroup *group, int particleIndex,
			float impulse, const SqVec2 &normal);
	};

	// ====================inline=========================================================
	inline SqParticleGroup *SqParticleSystem::GetParticleGroupList()
	{
		return m_groupList;
	}

	inline const SqParticleGroup *SqParticleSystem::GetParticleGroupList() const
	{
		return m_groupList;
	}

	inline int SqParticleSystem::GetParticleGroupCount() const
	{
		return m_groupCount;
	}

	inline int SqParticleSystem::GetParticleCount() const
	{
		return data.m_count;
	}

	inline void SqParticleSystem::SetPaused(bool paused)
	{
		m_paused = paused;
	}

	inline bool SqParticleSystem::GetPaused() const
	{
		return m_paused;
	}

	inline const SqArray<phxy::SqParticleContact> &SqParticleSystem::GetContacts() const
	{
		return data.m_contactBuffer;
	}

	inline int SqParticleSystem::GetContactCount() const
	{
		return data.m_contactBuffer.getCount();
	}

	inline const phxy::SqArray<SqParticleBodyContact> &SqParticleSystem::GetBodyContacts() const
	{
		return data.m_bodyContactBuffer;
	}

	inline int SqParticleSystem::GetBodyContactCount() const
	{
		return data.m_bodyContactBuffer.getCount();
	}

	inline const SqParticlePair *SqParticleSystem::GetPairs() const
	{
		return data.m_pairBuffer.getData();
	}

	inline int SqParticleSystem::GetPairCount() const
	{
		return data.m_pairBuffer.getCount();
	}

	inline const SqParticleTriad *SqParticleSystem::GetTriads() const
	{
		return data.m_triadBuffer.getData();
	}

	inline int SqParticleSystem::GetTriadCount() const
	{
		return data.m_triadBuffer.getCount();
	}

	inline SqParticleSystem *SqParticleSystem::GetNext()
	{
		return m_next;
	}

	inline const SqParticleSystem *SqParticleSystem::GetNext() const
	{
		return m_next;
	}

	inline const int *SqParticleSystem::GetStuckCandidates() const
	{
		return nullptr; // data.m_stuckParticleBuffer.getData();
	}

	inline int SqParticleSystem::GetStuckCandidateCount() const
	{
		return 0; // data.m_stuckParticleBuffer.getCount();
	}

	inline void SqParticleSystem::SetStrictContactCheck(bool enabled)
	{
		m_def.strictContactCheck = enabled;
	}

	inline bool SqParticleSystem::GetStrictContactCheck() const
	{
		return m_def.strictContactCheck;
	}

	inline void SqParticleSystem::SetRadius(float radius)
	{
		m_particleDiameter = 2 * radius;
		m_squaredDiameter = m_particleDiameter * m_particleDiameter;
		m_inverseDiameter = 1 / m_particleDiameter;
	}

	inline float SqParticleSystem::GetInverseDiameter() { return m_inverseDiameter; };

	inline void SqParticleSystem::SetDensity(float density)
	{
		m_def.density = density;
		m_inverseDensity = 1 / m_def.density;
	}

	inline float SqParticleSystem::GetDensity() const
	{
		return m_def.density;
	}

	inline void SqParticleSystem::SetGravityScale(float gravityScale)
	{
		m_def.gravityScale = gravityScale;
	}

	inline float SqParticleSystem::GetGravityScale() const
	{
		return m_def.gravityScale;
	}

	inline void SqParticleSystem::SetDamping(float damping)
	{
		m_def.dampingStrength = damping;
	}

	inline float SqParticleSystem::GetDamping() const
	{
		return m_def.dampingStrength;
	}

	inline void SqParticleSystem::SetStaticPressureIterations(int iterations)
	{
		m_def.staticPressureIterations = iterations;
	}

	inline int SqParticleSystem::GetStaticPressureIterations() const
	{
		return m_def.staticPressureIterations;
	}

	inline float SqParticleSystem::GetRadius() const
	{
		return m_particleDiameter * 0.5f;
	}

	/**
	 * 这个相当于计算一个粒子要在当前的时间内移动m_particleDiameter距离需要多大的速度
	 * 即移动一个粒子直径的距离所需要的速度，按照当前的帧时间来算
	 */
	inline float SqParticleSystem::GetCriticalVelocity(const SqStepContext &step) const
	{
		return m_particleDiameter * step.inv_dt;
	}

	inline float SqParticleSystem::GetCriticalVelocitySquared(const SqStepContext &step) const
	{
		float velocity = GetCriticalVelocity(step);
		return velocity * velocity;
	}

	/**
	 * 得到粒子的动压（dynamic pressure，通常用符号 q 表示）是流体动力学中的一个重要概念，表示流体因运动而产生的压力，即一个可以对另外一个粒子产生压力，
	 * 那么产生的压力是多少，就是这里计算的。
	 * 其物理公式为：
	 * q = (1/2) x ρ x v^2
	 * 其中 ρ 是流体的密度，v是粒子当前速度的平方
	 * 虽然这里少了1/2，但形式一致，说明它想表达的其实是：
	 * 粒子移动速度对应的一个参考压力，用于确定当粒子密度或速度变化时，它们应产生多大压力效果
	 */
	inline float SqParticleSystem::GetCriticalPressure(const SqStepContext &step) const
	{
		return m_def.density * GetCriticalVelocitySquared(step);
	}

	inline float SqParticleSystem::GetParticleStride() const
	{
		return sq_particleStride * m_particleDiameter;
	}

	/**
	 * 计算每个粒子的质量
	 */
	inline float SqParticleSystem::GetParticleMass() const
	{
		float stride = GetParticleStride();
		return m_def.density * stride * stride;
	}

	inline float SqParticleSystem::GetParticleInvMass() const
	{
		// mass = density * stride^2, so we take the inverse of this.
		float inverseStride = m_inverseDiameter * (1.0f / sq_particleStride);
		return m_inverseDensity * inverseStride * inverseStride;
	}

	inline SqVec2 *SqParticleSystem::GetPositionBuffer()
	{
		return data.m_positionBuffer;
	}

	inline float *SqParticleSystem::GetWeightBuffer()
	{
		return data.m_weightBuffer;
	}

	inline int SqParticleSystem::GetMaxParticleCount() const
	{
		return m_def.maxCount;
	}

	inline void SqParticleSystem::SetMaxParticleCount(int count)
	{
		SQ_ASSERT(data.m_count <= count);
		m_def.maxCount = count;
	}

	inline unsigned int SqParticleSystem::GetAllParticleFlags() const
	{
		return m_allParticleFlags;
	}

	inline unsigned int SqParticleSystem::GetAllGroupFlags() const
	{
		return m_allGroupFlags;
	}

	inline const SqVec2 *SqParticleSystem::GetPositionBuffer() const
	{
		return data.m_positionBuffer;
	}

	inline const SqParticleColor *SqParticleSystem::GetColorBuffer() const
	{
		return ((SqParticleSystem *)this)->GetColorBuffer();
	}

	inline const SqParticleGroup *const *SqParticleSystem::GetGroupBuffer() const
	{
		return data.m_groupBuffer.data;
	}

	inline const float *SqParticleSystem::GetWeightBuffer() const
	{
		return data.m_weightBuffer;
	}

	inline void *const *SqParticleSystem::GetUserDataBuffer() const
	{
		return ((SqParticleSystem *)this)->GetUserDataBuffer();
	}

	inline SqParticleGroup *const *SqParticleSystem::GetGroupBuffer()
	{
		return data.m_groupBuffer.data;
	}

	inline bool SqParticleSystem::ValidateParticleIndex(const int index) const
	{
		return index >= 0 && index < GetParticleCount() &&
			   index != sq_invalidParticleIndex;
	}

	inline bool SqParticleSystem::GetDestructionByAge() const
	{
		return m_def.destroyByAge;
	}

	inline void SqParticleSystem::ParticleApplyLinearImpulse(int index,
															 const SqVec2 &impulse)
	{
		// ApplyLinearImpulse(index, index + 1, impulse);
	}

	inline void SqParticleSystem::SetViscousStrength(float v)
	{
		m_def.viscousStrength = v;
	}

	inline void SqParticleSystem::SetSurfaceTensionPressureStrength(float v)
	{
		m_def.surfaceTensionPressureStrength = v;
	}

	inline void SqParticleSystem::SetSurfaceTensionNormalStrength(float v)
	{
		m_def.surfaceTensionNormalStrength = v;
	}

	inline void SqParticleSystem::setPressureStrength(float v)
	{
		m_def.pressureStrength = v;
	}

	inline void SqParticleSystem::enableAllParticleFlags(bool b, unsigned int mask)
	{
		if (b)
		{
			m_allParticleFlags |= mask;
		}
		else
		{
			m_allParticleFlags &= ~mask;
		}
	}

	inline SqParticleSystemWorld *SqParticleSystem::getWorld()
	{
		return world;
	}

	inline void SqParticleSystem::setBodyContactFilter(const SqShapeFilter &bodyContactfilter)
	{
		this->bodyContactfilter = bodyContactfilter;
	}

// Note: These functions must go in the header so the unit tests will compile
// them. SqParticleSystem.cpp does not compile with this #define.
#if LIQUIDFUN_EXTERNAL_LANGUAGE_API

	inline void SqParticleSystem::SetParticleVelocity(int index,
													  float vx,
													  float vy)
	{
		SqVec2 &v = GetVelocityBuffer()[index];
		v.x = vx;
		v.y = vy;
	}

	inline float SqParticleSystem::GetParticlePositionX(int index) const
	{
		return GetPositionBuffer()[index].x;
	}

	inline float SqParticleSystem::GetParticlePositionY(int index) const
	{
		return GetPositionBuffer()[index].y;
	}

	inline int SqParticleSystem::CopyPositionBuffer(int startIndex,
													int numParticles,
													void *outBuf,
													int size) const
	{
		int copySize = numParticles * sizeof(SqVec2);
		void *inBufWithOffset = (void *)(GetPositionBuffer() + startIndex);
		return CopyBuffer(startIndex, numParticles, inBufWithOffset, outBuf, size,
						  copySize);
	}

	inline int SqParticleSystem::CopyColorBuffer(int startIndex,
												 int numParticles,
												 void *outBuf,
												 int size) const
	{
		int copySize = numParticles * sizeof(SqParticleColor);
		void *inBufWithOffset = (void *)(GetColorBuffer() + startIndex);
		return CopyBuffer(startIndex, numParticles, inBufWithOffset, outBuf, size,
						  copySize);
	}

	inline int SqParticleSystem::CopyWeightBuffer(int startIndex,
												  int numParticles,
												  void *outBuf,
												  int size) const
	{
		int copySize = numParticles * sizeof(float);
		void *inBufWithOffset = (void *)(GetWeightBuffer() + startIndex);
		return CopyBuffer(startIndex, numParticles, inBufWithOffset, outBuf, size,
						  copySize);
	}

	inline int SqParticleSystem::CopyBuffer(int startIndex, int numParticles,
											void *inBufWithOffset, void *outBuf,
											int outBufSize, int copySize) const
	{
		b2ExceptionType exception = IsBufCopyValid(startIndex, numParticles,
												   copySize, outBufSize);
		if (exception != b2_noExceptions)
		{
			return exception;
		}

		memcpy(outBuf, inBufWithOffset, copySize);
		return b2_noExceptions;
	}

#endif // LIQUIDFUN_EXTERNAL_LANGUAGE_API

}
