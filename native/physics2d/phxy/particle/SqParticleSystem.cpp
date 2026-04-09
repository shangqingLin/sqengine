
#include "SqParticleSystem.h"
#include "SqParticleContact.h"
#include "SqParticleGroup.h"
#include "SqParticleColor.h"
#include "b2VoronoiDiagram.h"
#include <algorithm>
#include <stdio.h>
#include <unordered_map>
#include "./SqParticleSystemWorld.h"
#include "FixedSetAllocator.h"
#include "ExpirationTimeComparator.h"

#include "../common/math/SqMath.h"
#include "../dynamics/SqBody.h"
#include "../geometry/shape/SqChainShape.h"
#include "../geometry/shape/SqEdgeShape.h"
#include "../geometry/shape/SqChainEdgeShape.h"
#include <engine/core/simd/simd.h>

using namespace phxy;

// 定义 LIQUIDFUN_SIMD_TEST_VS_REFERENCE 宏可同时运行 SIMD 和 普通的 版本
// 并assert两者结果完全一致。这在修改函数时特别有用，有助于验证正确性。
// #define LIQUIDFUN_SIMD_TEST_VS_REFERENCE
// 为便于调试，此处移除了'inline'关键字。这样当测试对比函数中的assert触发时，
// 您可以轻松地将指令指针跳回函数开头重新运行测试。
#define LIQUIDFUN_SIMD_INLINE inline

SqParticleSystem::SqParticleSystem(const b2ParticleSystemDef *def,
								   SqParticleSystemWorld *world) : data(this),
																   grid(this)

{
	SQ_ASSERT(def);
	m_paused = false;
	m_timestamp = 0;
	m_allParticleFlags = 0;
	m_needsUpdateAllParticleFlags = false;
	m_allGroupFlags = 0;
	m_needsUpdateAllGroupFlags = false;
	m_hasForce = false;
	m_iterationIndex = 0;
	m_userData = def->userData;

	SetStrictContactCheck(def->strictContactCheck);
	SetDensity(def->density);
	SetGravityScale(def->gravityScale);
	SetRadius(def->radius);
	SetMaxParticleCount(def->maxCount);

	m_groupCount = 0;
	m_groupList = NULL;

	SQ_ASSERT(def->lifetimeGranularity > 0.0f);
	m_def = *def;

	this->world = world;

	// m_stuckThreshold = 0;

	m_timeElapsed = 0;
	m_expirationTimeBufferRequiresSorting = false;

	SetDestructionByAge(m_def.destroyByAge);
}

SqParticleSystem::~SqParticleSystem()
{
	while (m_groupList)
	{
		DestroyParticleGroup(m_groupList);
	}
}

SqParticle *SqParticleSystem::findById(int id) const
{
	return partilceArray.get(id - 1);
}

SqParticleColor *SqParticleSystem::GetColorBuffer()
{
	return data.m_colorBuffer.data;
}

void **SqParticleSystem::GetUserDataBuffer()
{
	return data.m_userDataBuffer.data;
}

/**
 * 创建一个粒子，并将其添加到粒子系统中。
 * @param def 粒子定义，包含位置、速度、颜色等信息。
 * @return 返回新创建的粒子的索引，如果无法创建则返回 b2_invalidParticleIndex。
 */
int SqParticleSystem::CreateParticle(const SqParticleDef &def)
{
	SQ_ASSERT(world->world->lock == false);
	if (world->world->lock)
	{
		return 0;
	}

	int id = 0;
	bool hasId = poolPartilceIds.getNext<int>(id);
	if (!hasId)
	{
		// 粒子缓存池中没有粒子了，则先去销毁标没有使用的粒子，以便重用
		if (m_allParticleFlags & sq_zombieParticle)
		{
			// 表示其他地方销毁了粒子。则这里回收，好让下面新创建的从回收中重用
			SolveZombie();
		}
	}

	if (id == 0)
		poolPartilceIds.getNext<int>(id);

	SqParticle *particle = nullptr;
	if (id == 0)
	{
		particle = partilceArray.Add();
		id = partilceArray.getCount();
		particle->id = id;
		particle->system = this;
	}
	else
	{
		particle = partilceArray.get(id - 1);
		particle->flags = 0;
	}

	// printf("create Particle %d \n", id);

	particle->reset();
	particle->requestSimData();
	particle->velocity = def.velocity;
	particle->setPosition(def.position);

	// printf("create %d %f %f \n", id, particle->getPosition().x, particle->getPosition().y);

	grid.addCell(id);
	SetParticleFlags(id, def.flags);
	if (m_def.colorMix && !def.color.IsZero())
	{
		particle->setColor(def.color);
	}

	if (def.userData)
	{
		particle->setUserData(def.userData);
	}

	if (m_def.destroyByAge)
	{
		particle->setLifeTime(def.lifetime);
	}

	if (def.group)
	{
		particle->setGroup(def.group);
	}
	particles.addValue(id);

	if (def.group)
	{
		def.group->addParticle(id);
	}
	return id;
}

void SqParticleSystem::DestroyParticle(int particleId, bool callDestructionListener)
{
	unsigned int flags = sq_zombieParticle;
	if (callDestructionListener)
	{
		flags |= sq_destructionListenerParticle;
	}

	// 标记一下全局，说明可以执行SolveZombie
	m_allParticleFlags |= sq_zombieParticle;

	SetParticleFlags(particleId, findById(particleId)->flags | flags);
}

int SqParticleSystem::DestroyParticlesInShape(SqShape &shape, const SqTransform &xf, bool callDestructionListener)
{
	SQ_ASSERT(world->world->lock == false);
	if (world->world->lock)
	{
		return 0;
	}

	class DestroyParticlesInShapeCallback : public b2QueryCallback
	{
	public:
		DestroyParticlesInShapeCallback(
			SqParticleSystem *system, SqShape *shape,
			const SqTransform &xf, bool callDestructionListener, SqWorld *world)
		{
			m_system = system;
			m_shape = shape;
			m_xf = xf;
			m_callDestructionListener = callDestructionListener;
			m_destroyed = 0;
			m_world = world;
		}

		bool ReportParticle(const SqParticleSystem *particleSystem, int paricleId)
		{

			// 这不是相当于只检测当前的粒子系统吗，所以下面没必要对所有系统进行遍历
			if (particleSystem != m_system)
				return false;

			SqParticle *particle = m_system->findById(paricleId);

			// 将point转换到Shape的本地坐标中
			const SqVec2 &position = particle->getPosition();
			phxy::SqBody *body = m_world->getBody(m_shape->getBody());
			SqTransform transform = body->getTransform();
			SqVec2 point = transform.invTransformPoint(position);
			if (m_shape->PointIn(point))
			{
				m_system->DestroyParticle(paricleId, m_callDestructionListener);
				m_destroyed++;
			}
			return true;
		}

		int Destroyed() { return m_destroyed; }

	private:
		SqParticleSystem *m_system;
		SqWorld *m_world;
		SqShape *m_shape;
		SqTransform m_xf;
		bool m_callDestructionListener;
		int m_destroyed;
	} callback(this, &shape, xf, callDestructionListener, world->world);

	SqAABB aabb = shape.computeShapeAABB(xf);
	QueryAABB(&callback, aabb);

	// shape.ComputeAABB(&aabb, xf, 0);
	// m_world->QueryAABB(&callback, aabb);
	return callback.Destroyed();
}

int SqParticleSystem::CreateParticleForGroup(SqParticleGroup *group, const SqParticleGroupDef &groupDef, const SqTransform &xf, const SqVec2 &p)
{
	SqParticleDef particleDef;
	particleDef.flags = groupDef.flags;
	particleDef.position = xf.transformPoint(p);
	particleDef.group = group;
	particleDef.velocity = groupDef.linearVelocity + SqVec2::CrossSV(groupDef.angularVelocity, particleDef.position - groupDef.position);
	particleDef.color = groupDef.color;
	particleDef.lifetime = groupDef.lifetime;
	particleDef.userData = groupDef.userData;
	return CreateParticle(particleDef);
}

void SqParticleSystem::CreateParticlesStrokeShapeForGroup(SqParticleGroup *group, const SqShape *shape, const SqParticleGroupDef &groupDef, const SqTransform &xf)
{
	float stride = groupDef.stride;
	if (stride == 0)
	{
		stride = GetParticleStride();
	}
	float positionOnEdge = 0;

	if (shape->type == SqShapeType::sq_chainShape)
	{
		SqChainShape *chainShape = (SqChainShape *)shape;
		for (int i = 0; i < chainShape->count; ++i)
		{
			SqChainEdgeShape *edge = chainShape->segments.get(i);
			SqVec2 d = edge->point1 - edge->point2;
			float edgeLength = SqVec2::Length(d);
			while (positionOnEdge < edgeLength)
			{
				// 在一条直线上相隔stride个距离取一个点
				SqVec2 p = edge->point1 + positionOnEdge / edgeLength * d;
				CreateParticleForGroup(group, groupDef, xf, p);
				positionOnEdge += stride;
			}
			positionOnEdge -= edgeLength;
		}
	}
	else
	{
		SqVec2 point1, point2;

		if (shape->type == SqShapeType::sq_chainSegmentShape)
		{
			SqChainEdgeShape *edgeShape = (SqChainEdgeShape *)shape;
			point1 = edgeShape->point1;
			point2 = edgeShape->point2;
		}
		else if (shape->type == SqShapeType::sq_segmentShape)
		{
			SqEdgeShape *edgeShape = (SqEdgeShape *)shape;
			point1 = edgeShape->point1;
			point2 = edgeShape->point2;
		}

		SqVec2 d = point1 - point2;
		float edgeLength = SqVec2::Length(d);
		while (positionOnEdge < edgeLength)
		{
			// 在一条直线上相隔stride个距离取一个点
			SqVec2 p = point1 + positionOnEdge / edgeLength * d;
			CreateParticleForGroup(group, groupDef, xf, p);
			positionOnEdge += stride;
		}
		positionOnEdge -= edgeLength;
	}
}

void SqParticleSystem::CreateParticlesFillShapeForGroup(SqParticleGroup *group, const SqShape *shape, const SqParticleGroupDef &groupDef, const SqTransform &xf)
{
	float stride = groupDef.stride;
	if (stride == 0)
	{
		stride = GetParticleStride();
	}
	SqTransform identity;
	SqAABB aabb = shape->getAABB();

	SqVec2 p;
	for (float y = floorf(aabb.lowerBound.y / stride) * stride; y < aabb.upperBound.y; y += stride)
	{
		for (float x = floorf(aabb.lowerBound.x / stride) * stride; x < aabb.upperBound.x; x += stride)
		{
			p.x = x;
			p.y = y;
			if (shape->PointIn(p))
			{
				CreateParticleForGroup(group, groupDef, xf, p);
			}
		}
	}
}

void SqParticleSystem::CreateParticlesWithShapeForGroup(SqParticleGroup *group, const SqShape *shape, const SqParticleGroupDef &groupDef, const SqTransform &xf)
{
	switch (shape->type)
	{
	case SqShapeType::sq_chainSegmentShape:
	case SqShapeType::sq_segmentShape:
		CreateParticlesStrokeShapeForGroup(group, shape, groupDef, xf);
		break;
	case SqShapeType::sq_polygonShape:
	case SqShapeType::sq_circleShape:
	case SqShapeType::sq_capsuleShape:
		CreateParticlesFillShapeForGroup(group, shape, groupDef, xf);
		break;
	default:
		SQ_ASSERT(false);
		break;
	}
}

void SqParticleSystem::CreateParticlesWithShapesForGroup(SqParticleGroup *group, const SqShape *const *shapes, int shapeCount, const SqParticleGroupDef &groupDef, const SqTransform &xf)
{
	/*
	class CompositeShape : public b2Shape
	{
	public:
		CompositeShape(const b2Shape *const *shapes, int shapeCount)
		{
			m_shapes = shapes;
			m_shapeCount = shapeCount;
		}
		b2Shape *Clone(b2BlockAllocator *allocator) const
		{
			SQ_ASSERT(false);
			B2_NOT_USED(allocator);
			return NULL;
		}
		int GetChildCount() const
		{
			return 1;
		}
		bool TestPoint(const SqTransform &xf, const SqVec2 &p) const
		{
			for (int i = 0; i < m_shapeCount; i++)
			{
				if (m_shapes[i]->TestPoint(xf, p))
				{
					return true;
				}
			}
			return false;
		}
		void ComputeDistance(const SqTransform &xf, const SqVec2 &p,
							 float *distance, SqVec2 *normal, int childIndex) const
		{
			SQ_ASSERT(false);
			B2_NOT_USED(xf);
			B2_NOT_USED(p);
			B2_NOT_USED(distance);
			B2_NOT_USED(normal);
			B2_NOT_USED(childIndex);
		}
		bool RayCast(b2RayCastOutput *output, const b2RayCastInput &input,
					 const SqTransform &transform, int childIndex) const
		{
			SQ_ASSERT(false);
			B2_NOT_USED(output);
			B2_NOT_USED(input);
			B2_NOT_USED(transform);
			B2_NOT_USED(childIndex);
			return false;
		}
		void ComputeAABB(
			SqAABB *aabb, const SqTransform &xf, int childIndex) const
		{
			B2_NOT_USED(childIndex);
			aabb->lowerBound.x = +FLT_MAX;
			aabb->lowerBound.y = +FLT_MAX;
			aabb->upperBound.x = -FLT_MAX;
			aabb->upperBound.y = -FLT_MAX;
			SQ_ASSERT(childIndex == 0);
			for (int i = 0; i < m_shapeCount; i++)
			{
				int childCount = m_shapes[i]->GetChildCount();
				for (int j = 0; j < childCount; j++)
				{
					SqAABB subaabb;
					m_shapes[i]->ComputeAABB(&subaabb, xf, j);
					aabb->Combine(subaabb);
				}
			}
		}
		void ComputeMass(b2MassData *massData, float density) const
		{
			SQ_ASSERT(false);
			B2_NOT_USED(massData);
			B2_NOT_USED(density);
		}

	private:
		const b2Shape *const *m_shapes;
		int m_shapeCount;
	} compositeShape(shapes, shapeCount);
	CreateParticlesFillShapeForGroup(&compositeShape, groupDef, xf);*/
}

SqParticleGroup *SqParticleSystem::CreateParticleGroup(const SqParticleGroupDef &groupDef)
{
	SQ_ASSERT(world->world->lock == false);
	if (world->world->lock)
	{
		return 0;
	}

	// 粒子的初始位置
	SqTransform transform;
	transform.p = groupDef.position;
	transform.q = SqRot(groupDef.angle);

	SqParticleGroup *group = new SqParticleGroup();
	group->m_system = this;

	if (groupDef.shape)
	{
		CreateParticlesWithShapeForGroup(group, groupDef.shape, groupDef, transform);
	}
	if (groupDef.shapes)
	{
		CreateParticlesWithShapesForGroup(group, groupDef.shapes, groupDef.shapeCount, groupDef, transform);
	}

	if (groupDef.particleCount)
	{
		SQ_ASSERT(groupDef.positionData);
		for (int i = 0; i < groupDef.particleCount; i++)
		{
			SqVec2 p = groupDef.positionData[i];
			CreateParticleForGroup(group, groupDef, transform, p);
		}
	}

	group->m_strength = groupDef.strength;
	group->m_userData = groupDef.userData;
	group->m_transform = transform;
	group->m_prev = NULL;
	group->m_next = m_groupList;
	if (m_groupList)
	{
		m_groupList->m_prev = group;
	}
	m_groupList = group;
	++m_groupCount;

	SetGroupFlags(group, groupDef.groupFlags);

	// Create pairs and triads between particles in the group.
	ConnectionFilter filter;
	UpdateContacts(true);
	UpdatePairsAndTriads(group->particles, filter);

	return group;
}

void SqParticleSystem::JoinParticleGroups(SqParticleGroup *groupA, SqParticleGroup *groupB)
{
	SQ_ASSERT(world->world->lock == false);
	if (world->world->lock)
	{
		return;
	}

	SQ_ASSERT(groupA != groupB);
	groupB->removeAllParticle();
	groupA->addParticles(groupB->particles);

	// 看不懂这里过滤,估计也是判断是否在同一个Group中吧？
	// Create pairs and triads connecting groupA and groupB.
	// class JoinParticleGroupsFilter : public ConnectionFilter
	// {
	// 	bool ShouldCreatePair(int a, int b) const
	// 	{
	// 		return (a < m_threshold && m_threshold <= b) ||
	// 			   (b < m_threshold && m_threshold <= a);
	// 	}
	// 	bool ShouldCreateTriad(int a, int b, int c) const
	// 	{
	// 		return (a < m_threshold || b < m_threshold || c < m_threshold) &&
	// 			   (m_threshold <= a || m_threshold <= b || m_threshold <= c);
	// 	}
	// 	int m_threshold;

	// public:
	// 	JoinParticleGroupsFilter(int threshold)
	// 	{
	// 		m_threshold = threshold;
	// 	}
	// } filter(groupB->m_firstIndex);
	// UpdateContacts(true);
	// UpdatePairsAndTriads(groupA->firstParticle, groupB->lastParticle, filter);

	UpdateContacts(true);
	UpdatePairsAndTriads(groupA->particles, ConnectionFilter());

	unsigned int groupFlags = groupA->m_groupFlags | groupB->m_groupFlags;
	SetGroupFlags(groupA, groupFlags);
	DestroyParticleGroup(groupB);
}

// void SqParticleSystem::SplitParticleGroup(SqParticleGroup *group)
// {
// 	UpdateContacts(true);
// 	int particleCount = group->GetParticleCount();
// 	// We create several linked lists. Each list represents a set of connected
// 	// particles.
// 	ParticleListNode *nodeBuffer = (ParticleListNode *)world->world->tempAllocator.allocateChunk(sizeof(ParticleListNode) * particleCount);
// 	InitializeParticleLists(group, nodeBuffer);
// 	MergeParticleListsInContact(group, nodeBuffer);
// 	ParticleListNode *survivingList = FindLongestParticleList(group, nodeBuffer);
// 	MergeZombieParticleListNodes(group, nodeBuffer, survivingList);
// 	CreateParticleGroupsFromParticleList(group, nodeBuffer, survivingList);
// 	UpdatePairsAndTriadsWithParticleList(group, nodeBuffer);
// 	world->world->tempAllocator.freeChunk(nodeBuffer);
// }

// void SqParticleSystem::InitializeParticleLists(const SqParticleGroup *group, ParticleListNode *nodeBuffer)
// {
// 	int bufferIndex = group->GetBufferIndex();
// 	int particleCount = group->GetParticleCount();
// 	for (int i = 0; i < particleCount; i++)
// 	{
// 		ParticleListNode *node = &nodeBuffer[i];
// 		node->list = node;
// 		node->next = NULL;
// 		node->count = 1;
// 		node->index = i + bufferIndex;
// 	}
// }

// void SqParticleSystem::MergeParticleListsInContact(const SqParticleGroup *group, ParticleListNode *nodeBuffer)
// {
// 	int bufferIndex = group->GetBufferIndex();
// 	for (int k = 0; k < data.m_contactBuffer.getCount(); k++)
// 	{
// 		const SqParticleContact &contact = *data.m_contactBuffer.get(k);
// 		int a = contact.GetIndexA();
// 		int b = contact.GetIndexB();
// 		if (!group->ContainsParticle(a) || !group->ContainsParticle(b))
// 		{
// 			continue;
// 		}
// 		ParticleListNode *listA = nodeBuffer[a - bufferIndex].list;
// 		ParticleListNode *listB = nodeBuffer[b - bufferIndex].list;
// 		if (listA == listB)
// 		{
// 			continue;
// 		}
// 		// To minimize the cost of insertion, make sure listA is longer than
// 		// listB.
// 		if (listA->count < listB->count)
// 		{
// 			sqSwap(listA, listB);
// 		}
// 		SQ_ASSERT(listA->count >= listB->count);
// 		MergeParticleLists(listA, listB);
// 	}
// }

// void SqParticleSystem::MergeParticleLists(ParticleListNode *listA, ParticleListNode *listB)
// {
// 	// Insert listB between index 0 and 1 of listA
// 	// Example:
// 	//     listA => a1 => a2 => a3 => NULL
// 	//     listB => b1 => b2 => NULL
// 	// to
// 	//     listA => listB => b1 => b2 => a1 => a2 => a3 => NULL
// 	SQ_ASSERT(listA != listB);
// 	for (ParticleListNode *b = listB;;)
// 	{
// 		b->list = listA;
// 		ParticleListNode *nextB = b->next;
// 		if (nextB)
// 		{
// 			b = nextB;
// 		}
// 		else
// 		{
// 			b->next = listA->next;
// 			break;
// 		}
// 	}
// 	listA->next = listB;
// 	listA->count += listB->count;
// 	listB->count = 0;
// }

// SqParticleSystem::ParticleListNode *SqParticleSystem::FindLongestParticleList(const SqParticleGroup *group, ParticleListNode *nodeBuffer)
// {
// 	int particleCount = group->GetParticleCount();
// 	ParticleListNode *result = nodeBuffer;
// 	for (int i = 0; i < particleCount; i++)
// 	{
// 		ParticleListNode *node = &nodeBuffer[i];
// 		if (result->count < node->count)
// 		{
// 			result = node;
// 		}
// 	}
// 	return result;
// }

// void SqParticleSystem::MergeZombieParticleListNodes(const SqParticleGroup *group, ParticleListNode *nodeBuffer, ParticleListNode *survivingList) const
// {
// 	int particleCount = group->GetParticleCount();
// 	SqParticle *particle = group->firstParticle;
// 	while (particle)
// 	{
// 		ParticleListNode *node = &nodeBuffer[particle->getId()];
// 		if (node != survivingList && (particle->flags & sq_zombieParticle))
// 		{
// 			MergeParticleListAndNode(survivingList, node);
// 		}

// 		if (particle == group->lastParticle)
// 		{
// 			break;
// 		}
// 		particle = particle->next;
// 	}
// }

// void SqParticleSystem::MergeParticleListAndNode(ParticleListNode *list, ParticleListNode *node)
// {
// 	// Insert node between index 0 and 1 of list
// 	// Example:
// 	//     list => a1 => a2 => a3 => NULL
// 	//     node => NULL
// 	// to
// 	//     list => node => a1 => a2 => a3 => NULL

// 	SQ_ASSERT(node != list);
// 	SQ_ASSERT(node->list == node);
// 	SQ_ASSERT(node->count == 1);
// 	node->list = list;
// 	node->next = list->next;
// 	list->next = node;
// 	list->count++;
// 	node->count = 0;
// }

// void SqParticleSystem::CreateParticleGroupsFromParticleList(const SqParticleGroup *group, ParticleListNode *nodeBuffer, const ParticleListNode *survivingList)
// {
// 	int particleCount = group->GetParticleCount();
// 	SqParticleGroupDef def;
// 	def.groupFlags = group->GetGroupFlags();
// 	def.userData = group->GetUserData();
// 	for (int i = 0; i < particleCount; i++)
// 	{
// 		ParticleListNode *list = &nodeBuffer[i];
// 		if (!list->count || list == survivingList)
// 		{
// 			continue;
// 		}
// 		SQ_ASSERT(list->list == list);
// 		SqParticleGroup *newGroup = CreateParticleGroup(def);
// 		for (ParticleListNode *node = list; node; node = node->next)
// 		{
// 			int oldIndex = node->index;
// 			SQ_ASSERT(!(m_flagsBuffer.data[oldIndex] & sq_zombieParticle));
// 			int newIndex = CloneParticle(oldIndex, newGroup);
// 			m_flagsBuffer.data[oldIndex] |= sq_zombieParticle;
// 			node->index = newIndex;
// 		}
// 	}
// }

// void SqParticleSystem::UpdatePairsAndTriadsWithParticleList(const SqParticleGroup *group, const ParticleListNode *nodeBuffer)
// {
// 	int bufferIndex = group->GetBufferIndex();
// 	// Update indices in pairs and triads. If an index belongs to the group,
// 	// replace it with the corresponding value in nodeBuffer.
// 	// Note that nodeBuffer is allocated only for the group and the index should
// 	// be shifted by bufferIndex.
// 	for (int k = 0; k < data.m_pairBuffer.getCount(); k++)
// 	{
// 		SqParticlePair &pair = *data.m_pairBuffer.get(k);
// 		int a = pair.indexA;
// 		int b = pair.indexB;
// 		if (group->ContainsParticle(a))
// 		{
// 			pair.indexA = nodeBuffer[a - bufferIndex].index;
// 		}
// 		if (group->ContainsParticle(b))
// 		{
// 			pair.indexB = nodeBuffer[b - bufferIndex].index;
// 		}
// 	}
// 	for (int k = 0; k < data.m_triadBuffer.getCount(); k++)
// 	{
// 		SqParticleTriad &triad = *data.m_triadBuffer.get(k);
// 		int a = triad.indexA;
// 		int b = triad.indexB;
// 		int c = triad.indexC;
// 		if (group->ContainsParticle(a))
// 		{
// 			triad.indexA = nodeBuffer[a - bufferIndex].index;
// 		}
// 		if (group->ContainsParticle(b))
// 		{
// 			triad.indexB = nodeBuffer[b - bufferIndex].index;
// 		}
// 		if (group->ContainsParticle(c))
// 		{
// 			triad.indexC = nodeBuffer[c - bufferIndex].index;
// 		}
// 	}
// }

// int SqParticleSystem::CloneParticle(int oldIndex, SqParticleGroup *group)
// {
// 	SqParticleDef def;
// 	def.flags = m_flagsBuffer.data[oldIndex];
// 	def.position = m_positionBuffer.data[oldIndex];
// 	def.velocity = m_velocityBuffer.data[oldIndex];
// 	if (m_colorBuffer.data)
// 	{
// 		def.color = m_colorBuffer.data[oldIndex];
// 	}
// 	if (m_userDataBuffer.data)
// 	{
// 		def.userData = m_userDataBuffer.data[oldIndex];
// 	}
// 	def.group = group;
// 	int newIndex = CreateParticle(def);
// 	if (m_handleIndexBuffer.data)
// 	{
// 		SqParticleHandle *handle = m_handleIndexBuffer.data[oldIndex];
// 		if (handle)
// 			handle->SetIndex(newIndex);
// 		m_handleIndexBuffer.data[newIndex] = handle;
// 		m_handleIndexBuffer.data[oldIndex] = NULL;
// 	}
// 	if (m_lastBodyContactStepBuffer.data)
// 	{
// 		m_lastBodyContactStepBuffer.data[newIndex] =
// 			m_lastBodyContactStepBuffer.data[oldIndex];
// 	}
// 	if (m_bodyContactCountBuffer.data)
// 	{
// 		m_bodyContactCountBuffer.data[newIndex] = m_bodyContactCountBuffer.data[oldIndex];
// 	}
// 	if (m_consecutiveContactStepsBuffer.data)
// 	{
// 		m_consecutiveContactStepsBuffer.data[newIndex] =
// 			m_consecutiveContactStepsBuffer.data[oldIndex];
// 	}
// 	if (m_hasForce)
// 	{
// 		m_forceBuffer[newIndex] = m_forceBuffer[oldIndex];
// 	}
// 	if (m_staticPressureBuffer)
// 	{
// 		m_staticPressureBuffer[newIndex] = m_staticPressureBuffer[oldIndex];
// 	}
// 	if (m_depthBuffer)
// 	{
// 		m_depthBuffer[newIndex] = m_depthBuffer[oldIndex];
// 	}
// 	if (m_expirationTimeBuffer.data)
// 	{
// 		m_expirationTimeBuffer.data[newIndex] = m_expirationTimeBuffer.data[oldIndex];
// 	}
// 	return newIndex;
// }

void SqParticleSystem::UpdatePairsAndTriadsWithReactiveParticles()
{
	class ReactiveFilter : public ConnectionFilter
	{
		bool IsNecessary(int particleId) const
		{
			return (m_system->findById(particleId)->flags & sq_reactiveParticle) != 0;
		}

	public:
		SqParticleSystem *m_system;
		ReactiveFilter(SqParticleSystem *m_system) : m_system(m_system) {}
	} filter(this);

	UpdatePairsAndTriads(particles, filter);

	auto callback = [this](int particleId)
	{
		SqParticle *particle = findById(particleId);
		particle->flags &= ~sq_reactiveParticle;
	};
	particles.iterate<int>(callback);
	m_allParticleFlags &= ~sq_reactiveParticle;
}

static bool ParticleCanBeConnected(unsigned int flags, SqParticleGroup *group)
{
	return (flags & (sq_wallParticle | sq_springParticle | sq_elasticParticle)) ||
		   (group && group->GetGroupFlags() & b2_rigidParticleGroup);
}

void SqParticleSystem::UpdatePairsAndTriads(SqBitSet &particles, const ConnectionFilter &filter)
{
	// Create pairs or triads.
	// All particles in each pair/triad should satisfy the following:
	// * firstIndex <= index < lastIndex
	// * don't have b2_zombieParticle
	// * ParticleCanBeConnected returns true
	// * ShouldCreatePair/ShouldCreateTriad returns true
	// Any particles in each pair/triad should satisfy the following:
	// * filter.IsNeeded returns true
	// * have one of k_pairFlags/k_triadsFlags

	unsigned int particleFlags = 0;
	int count = 0;
	std::unordered_map<int, bool> recordParticle;
	recordParticle.reserve(data.m_count);

	{
		auto callback = [this, &particleFlags, &recordParticle, &count](int particleId)
		{
			SqParticle *particle = findById(particleId);
			particleFlags |= particle->flags;
			recordParticle[particle->getId()] = true;
			++count;
		};
		particles.iterate<int>(callback);
	}

	if (particleFlags & k_pairFlags)
	{
		for (int k = 0; k < data.m_contactBuffer.getCount(); k++)
		{
			const SqParticleContact &contact = *data.m_contactBuffer.get(k);
			SqParticle *aParticle = findById(contact.GetIndexA());
			SqParticle *bParticle = findById(contact.GetIndexB());
			unsigned int af = aParticle->flags;
			unsigned int bf = bParticle->flags;
			SqParticleGroup *groupA = aParticle->getGroup();
			SqParticleGroup *groupB = bParticle->getGroup();

			if (
				!((af | bf) & sq_zombieParticle) &&
				((af | bf) & k_pairFlags) &&
				(filter.IsNecessary(aParticle->getId()) || filter.IsNecessary(bParticle->getId())) &&
				ParticleCanBeConnected(af, groupA) &&
				ParticleCanBeConnected(bf, groupB) &&
				recordParticle.find(aParticle->getId()) != recordParticle.end() &&
				recordParticle.find(bParticle->getId()) != recordParticle.end() &&
				filter.ShouldCreatePair(aParticle->getId(), bParticle->getId()))
			{
				SqParticlePair &pair = *data.m_pairBuffer.Add();
				pair.indexA = aParticle->getId();
				pair.indexB = bParticle->getId();
				pair.flags = contact.GetFlags();
				pair.strength = min(
					groupA ? groupA->m_strength : 1,
					groupB ? groupB->m_strength : 1);

				pair.distance = SqVec2::Distance(bParticle->getPosition(), bParticle->getPosition());
			}
		}

		std::stable_sort(data.m_pairBuffer.begin(), data.m_pairBuffer.end(), ComparePairIndices);
		data.m_pairBuffer.unique(MatchPairIndices);
	}

	if (particleFlags & k_triadFlags)
	{
		b2VoronoiDiagram diagram(sqstd::StackTempArenaAllocator::getInstance(), count);

		auto intereCallback = [this, &diagram, &filter](int particleId)
		{
			SqParticle *particle = findById(particleId);
			SqParticleGroup *group = particle->getGroup();
			if (!(particle->flags & sq_zombieParticle) && ParticleCanBeConnected(particle->flags, group))
			{
				diagram.AddGenerator(particle->getPosition(), particle->getId(), filter.IsNecessary(particle->getId()));
			}
		};
		particles.iterate<int>(intereCallback);

		float stride = GetParticleStride();
		diagram.Generate(stride / 2, stride * 2);

		class UpdateTriadsCallback : public b2VoronoiDiagram::NodeCallback
		{

			void operator()(int a, int b, int c)
			{
				SqParticle *aParticle = m_system->findById(a);
				SqParticle *bParticle = m_system->findById(b);
				SqParticle *cParticle = m_system->findById(c);

				unsigned int af = aParticle->flags;
				unsigned int bf = bParticle->flags;
				unsigned int cf = cParticle->flags;
				if (((af | bf | cf) & k_triadFlags) && m_filter->ShouldCreateTriad(a, b, c))
				{
					const SqVec2 &pa = aParticle->getPosition();
					const SqVec2 &pb = bParticle->getPosition();
					const SqVec2 &pc = cParticle->getPosition();
					SqVec2 dab = pa - pb;
					SqVec2 dbc = pb - pc;
					SqVec2 dca = pc - pa;
					if (SqVec2::Dot(dab, dab) > maxDistanceSquared ||
						SqVec2::Dot(dbc, dbc) > maxDistanceSquared ||
						SqVec2::Dot(dca, dca) > maxDistanceSquared)
					{
						return;
					}
					SqParticleGroup *groupA = aParticle->getGroup();
					SqParticleGroup *groupB = bParticle->getGroup();
					SqParticleGroup *groupC = cParticle->getGroup();
					SqParticleTriad &triad = *m_system->data.m_triadBuffer.Add();
					triad.indexA = a;
					triad.indexB = b;
					triad.indexC = c;
					triad.flags = af | bf | cf;
					triad.strength = min(min(
											 groupA ? groupA->m_strength : 1,
											 groupB ? groupB->m_strength : 1),
										 groupC ? groupC->m_strength : 1);

					SqVec2 midPoint = (float)1 / 3 * (pa + pb + pc);
					triad.pa = pa - midPoint;
					triad.pb = pb - midPoint;
					triad.pc = pc - midPoint;
					triad.ka = -SqVec2::Dot(dca, dab);
					triad.kb = -SqVec2::Dot(dab, dbc);
					triad.kc = -SqVec2::Dot(dbc, dca);
					triad.s = SqVec2::Cross(pa, pb) + SqVec2::Cross(pb, pc) + SqVec2::Cross(pc, pa);
				}
			}
			SqParticleSystem *m_system;
			const ConnectionFilter *m_filter;
			float maxDistanceSquared;

		public:
			UpdateTriadsCallback(SqParticleSystem *system, const ConnectionFilter *filter)
			{
				m_system = system;
				m_filter = filter;
				maxDistanceSquared = sq_maxTriadDistanceSquared * m_system->m_squaredDiameter;
			}
		} callback(this, &filter);

		diagram.GetNodes(callback);

		std::stable_sort(data.m_triadBuffer.begin(), data.m_triadBuffer.end(), CompareTriadIndices);
		data.m_triadBuffer.unique(MatchTriadIndices);
	}
}

bool SqParticleSystem::ComparePairIndices(const SqParticlePair &a, const SqParticlePair &b)
{
	int diffA = a.indexA - b.indexA;
	if (diffA != 0)
		return diffA < 0;
	return a.indexB < b.indexB;
}

bool SqParticleSystem::MatchPairIndices(const SqParticlePair &a, const SqParticlePair &b)
{
	return a.indexA == b.indexA && a.indexB == b.indexB;
}

bool SqParticleSystem::CompareTriadIndices(const SqParticleTriad &a, const SqParticleTriad &b)
{
	int diffA = a.indexA - b.indexA;
	if (diffA != 0)
		return diffA < 0;
	int diffB = a.indexB - b.indexB;
	if (diffB != 0)
		return diffB < 0;
	return a.indexC < b.indexC;
}

bool SqParticleSystem::MatchTriadIndices(const SqParticleTriad &a, const SqParticleTriad &b)
{
	return a.indexA == b.indexA && a.indexB == b.indexB && a.indexC == b.indexC;
}

// Only called from SolveZombie() or JoinParticleGroups().
void SqParticleSystem::DestroyParticleGroup(SqParticleGroup *group)
{
	SQ_ASSERT(m_groupCount > 0);
	SQ_ASSERT(group);

	if (world->m_destructionListener)
	{
		// world->m_destructionListener->SayGoodbye(group);
	}

	SetGroupFlags(group, 0);
	group->removeAllParticle();

	if (group->m_prev)
	{
		group->m_prev->m_next = group->m_next;
	}
	if (group->m_next)
	{
		group->m_next->m_prev = group->m_prev;
	}
	if (group == m_groupList)
	{
		m_groupList = group->m_next;
	}

	--m_groupCount;
	group->~SqParticleGroup();
	delete group;
}

// Get the world's contact filter if any particles with the
// b2_particleContactFilterParticle flag are present in the system.
inline b2ContactFilter *SqParticleSystem::GetParticleContactFilter() const
{
	// return (m_allParticleFlags & b2_particleContactFilterParticle) ? world->m_contactFilter : NULL;
	return nullptr;
}

// Get the world's contact listener if any particles with the
// b2_particleContactListenerParticle flag are present in the system.
inline b2ContactListener *SqParticleSystem::GetParticleContactListener() const
{
	return (m_allParticleFlags & sq_particleContactListenerParticle) ? world->m_contactListener : NULL;
}

void SqParticleSystem::DetectStuckParticle(int particle)
{

	// 检测卡住的粒子
	//
	// 基本算法是允许用户指定一个可选的阈值m_stuckThreshold，
	// 用于检测某个粒子在连续多于该阈值的步骤中，
	// 是否一直接触多个物体（fixture）。
	// 如果是，则认为该粒子“卡住”了。
	// 被卡住的粒子会被放入一个列表中，
	// 如果启用了该功能，用户可以在每个时间步中查询这个列表，
	// 以便处理这些粒子。

	// if (m_stuckThreshold <= 0)
	// {
	// 	return;
	// }

	// // Get the state variables for this particle.
	// int *const consecutiveCount = &m_consecutiveContactStepsBuffer.data[particle];
	// int *const lastStep = &m_lastBodyContactStepBuffer.data[particle];
	// int *const bodyCount = &m_bodyContactCountBuffer.data[particle];

	// // 记录这个粒子与多个Body发生碰撞了
	// //  This is only called when there is a body contact for this particle.
	// ++(*bodyCount);

	// // We want to only trigger detection once per step, the first time we
	// // contact more than one fixture in a step for a given particle.
	// if (*bodyCount == 2)
	// {
	// 	++(*consecutiveCount);
	// 	// 粒子与Body碰撞的个数多于这个阀值就认为是卡住了
	// 	if (*consecutiveCount > m_stuckThreshold)
	// 	{
	// 		int &newStuckParticle = m_stuckParticleBuffer.Append();
	// 		newStuckParticle = particle;
	// 	}
	// }
	// *lastStep = m_timestamp;
}

/// Compute the axis-aligned bounding box for all particles contained
/// within this particle system.
/// @param aabb Returns the axis-aligned bounding box of the system.
void SqParticleSystem::ComputeAABB(SqAABB *const aabb, bool newPosition) const
{
	SQ_ASSERT(aabb);
	aabb->lowerBound.x = +sq_maxFloat;
	aabb->lowerBound.y = +sq_maxFloat;
	aabb->upperBound.x = -sq_maxFloat;
	aabb->upperBound.y = -sq_maxFloat;

	auto callback = [this, newPosition, aabb](int particleId)
	{
		SqParticle *particle = findById(particleId);
		const SqVec2 &p = particle->getPosition();
		if (newPosition)
		{
			// 由（SolveVelocity）计算得到当前最新速度所处的位置与粒子上一帧位置得到的newPosition
			const SqVec2 &pnew = particle->getNewPosition();
			aabb->lowerBound = SqVec2::Min(aabb->lowerBound, SqVec2::Min(p, pnew));
			aabb->upperBound = SqVec2::Max(aabb->upperBound, SqVec2::Max(p, pnew));
		}
		else
		{
			aabb->lowerBound = SqVec2::Min(aabb->lowerBound, p);
			aabb->upperBound = SqVec2::Max(aabb->upperBound, p);
		}
	};
	particles.iterate<int>(callback);
	aabb->lowerBound.x -= m_particleDiameter;
	aabb->lowerBound.y -= m_particleDiameter;
	aabb->upperBound.x += m_particleDiameter;
	aabb->upperBound.y += m_particleDiameter;
}

void SqParticleSystem::RemoveSpuriousBodyContacts()
{
	// At this point we have a list of contact candidates based on AABB
	// overlap.The AABB query that  generated this returns all collidable
	// fixtures overlapping particle bounding boxes.  This breaks down around
	// vertices where two shapes intersect, such as a "ground" surface made
	// of multiple b2PolygonShapes; it potentially applies a lot of spurious
	// impulses from normals that should not actually contribute.  See the
	// Ramp example in Testbed.
	//
	// To correct for this, we apply this algorithm:
	//   * sort contacts by particle and subsort by weight (nearest to farthest)
	//   * for each contact per particle:
	//      - project a point at the contact distance along the inverse of the
	//        contact normal
	//      - if this intersects the fixture that generated the contact, apply
	//         it, otherwise discard as impossible
	//      - repeat for up to n nearest contacts, currently we get good results
	//        from n=3.

	// std::sort(m_bodyContactBuffer.Begin(), m_bodyContactBuffer.End(),
	// 		  SqParticleSystem::BodyContactCompare);

	// int discarded = 0;
	// std::remove_if(m_bodyContactBuffer.Begin(),
	// 			   m_bodyContactBuffer.End(),
	// 			   b2ParticleBodyContactRemovePredicate(this, &discarded));

	// m_bodyContactBuffer.SetCount(m_bodyContactBuffer.GetCount() - discarded);
}

/**
 * 粒子系统帧循环入口函数
 */
void SqParticleSystem::Solve(const SqStepContext &step)
{

	// return;

	if (data.m_count == 0)
	{
		return;
	}

	// printf("particle num %d \n",data.m_count);

	// If particle lifetimes are enabled, destroy particles that are too old.
	if (data.m_expirationTimeBuffer.data)
	{
		SolveLifetimes(step);
	}

	// 通过m_allParticleFlags来记录粒子系统中是否有粒子被销毁才执行此方法
	// 不用每帧都执行销毁操作
	if (m_allParticleFlags & sq_zombieParticle)
	{
		SolveZombie();
		if (data.m_count == 0)
		{
			return;
		}
	}

	if (m_needsUpdateAllParticleFlags)
	{
		UpdateAllParticleFlags();
	}

	if (m_needsUpdateAllGroupFlags)
	{
		UpdateAllGroupFlags();
	}

	if (m_paused)
	{
		return;
	}

	// printf("chekc arena size %d \n ", b2GetArenaAllocation(&world->m_world->arena));

	// printf("sssssssssssssssss\n");

	int size = data.m_count * sizeof(SolveCollisionParticeInfo);
	solveCollisionParticeArray = (SolveCollisionParticeInfo *)sqstd::StackTempArenaAllocator::getInstance()->allocateChunk(size);
	memset((char *)solveCollisionParticeArray, 0, size);

	for (m_iterationIndex = 0; m_iterationIndex < world->particleIterations; m_iterationIndex++)
	{
		++m_timestamp;
		SqStepContext subStep = step;
		subStep.dt /= world->particleIterations;
		subStep.inv_dt *= world->particleIterations;

		// 生成粒子与粒子之间的碰撞
		UpdateContacts(false);

		// 生成粒子与Box2D的刚体碰撞
		UpdateBodyContacts();

		ComputeWeight();

		if (m_allGroupFlags & b2_particleGroupNeedsUpdateDepth)
		{
			ComputeDepth();
		}

		if (m_allParticleFlags & sq_reactiveParticle)
		{
			UpdatePairsAndTriadsWithReactiveParticles();
		}
		if (m_hasForce)
		{
			SolveForce(subStep);
		}
		if (m_allParticleFlags & sq_viscousParticle)
		{
			SolveViscous();
		}
		if (m_allParticleFlags & sq_repulsiveParticle)
		{
			SolveRepulsive(subStep);
		}
		if (m_allParticleFlags & sq_powderParticle)
		{
			SolvePowder(subStep);
		}
		if (m_allParticleFlags & sq_tensileParticle)
		{
			SolveTensile(subStep);
		}
		if (m_allGroupFlags & b2_solidParticleGroup)
		{
			SolveSolid(subStep);
		}
		if (m_allParticleFlags & sq_colorMixingParticle)
		{
			SolveColorMixing();
		}

		SqParticle *p = findById(1);
		// printf("begin %f %f \n", p->velocity.x, p->velocity.y);

		SolveGravity(subStep);

		// printf("Gravity %f %f \n", p->velocity.x, p->velocity.y);

		if (m_allParticleFlags & sq_staticPressureParticle)
		{
			SolveStaticPressure(subStep);
		}

		SolvePressure(subStep);

		// printf("Pressure %f %f \n", p->velocity.x, p->velocity.y);

		SolveDamping(subStep);

		// printf("SolveDamping %f %f \n", p->velocity.x, p->velocity.y);
		if (m_allParticleFlags & k_extraDampingFlags)
		{
			SolveExtraDamping();
		}
		if (m_allParticleFlags & sq_elasticParticle)
		{
			SolveElastic(subStep);
		}
		if (m_allParticleFlags & sq_springParticle)
		{
			SolveSpring(subStep);
		}

		if (m_allGroupFlags & b2_rigidParticleGroup)
		{
			SolveRigidDamping();
		}
		if (m_allParticleFlags & sq_barrierParticle)
		{
			SolveBarrier(subStep);
		}

		SolveCollision(subStep);
		if (m_allGroupFlags & b2_rigidParticleGroup)
		{
			SolveRigid(subStep);
		}
		if (m_allParticleFlags & sq_wallParticle)
		{
			SolveWall();
		}

		// printf("m_allParticleFlags %d \n",m_allParticleFlags);
		SolveVelocity(subStep, false);
	}

	sqstd::StackTempArenaAllocator::getInstance()->freeChunk(solveCollisionParticeArray);
	solveCollisionParticeArray = nullptr;
}

void SqParticleSystem::SolveVelocity(const SqStepContext &subStep, bool forTempPos)
{
	auto callback = [this, forTempPos, &subStep](int id)
	{
		SqParticle *particle = findById(id);
		if (forTempPos)
		{
			particle->getNewPosition() = particle->getPosition() + subStep.dt * particle->velocity;
		}
		else
		{
			particle->getPosition() += subStep.dt * particle->velocity;
		}
	};
	particles.iterate<int>(callback);
}

//==============粗略阶段计算粒子与Body发生碰撞
SqParticleBodyContact *SqParticleSystem::CreateContact(int particleId, SqShape *shape)
{
	SqParticleBodyContact *contact = nullptr;
	if (sq_fixtureContactListenerParticle & m_allParticleFlags)
	{
		// 创建一个b2ParticleBodyContact实例，记录哪个粒子与哪个刚体发生碰撞
		SqParticleBodyContact find;
		find.shape = shape;
		find.particleId = particleId;
		contact = data.m_bodyContactBuffer.findWithBinarySearch(&find, &SqParticleBodyContact::bodyContactListCompare, &SqParticleBodyContact::bodyContactListEquals);
		if (contact)
		{
			// 找到，表示还是旧的碰撞点
			contact->state = 2;
			SQ_ASSERT(contact->particleId == particleId);
		}
	}

	if (!contact)
	{
		contact = data.m_bodyContactBuffer.Add();
		contact->system = this;
		contact->state = 1;
		contact->particleId = particleId;
		contact->shape = shape;
		contact->bodyId = shape->getBody();
	}
	return contact;
}

void SqParticleSystem::fixtureParticleQueryCallback(SqShape *shape)
{
	const SqShapeFilter &filterB = shape->getFilter();
	bool collide = (bodyContactfilter.maskBits & filterB.categoryBits) != 0 && (bodyContactfilter.categoryBits & filterB.maskBits) != 0;
	if (!collide)
	{
		return;
	}

	const SqAABB &aabb = shape->getAABB();

	// printf(" chek aabb %f %f %f %f \n", aabb.lowerBound.x, aabb.lowerBound.y, aabb.upperBound.x, aabb.upperBound.y);

	InsideBoundsEnumerator enumerator = grid.GetInsideBoundsEnumerator(aabb);
	int particleId;
	float d;
	SqVec2 n;
	SqBody *body = world->world->getBody(shape->getBody());

	// printf("collison shape %p\n",shape);

	while ((particleId = enumerator.GetNext()) >= 0)
	{

		SqParticle *particle = findById(particleId);

		// 计算粒子到刚体的距离
		const SqVec2 &ap = particle->getPosition();

		shape->computeDistance(ap, body->getTransform(), d, n);

		SolveCollisionParticeInfo *collisionInfo = &solveCollisionParticeArray[particle->getBufferIndex()];
		if (collisionInfo->stepStamp != m_timestamp)
		{
			collisionInfo->shapeCount = 0;
			collisionInfo->stepStamp = m_timestamp;
		}

		bool collision = d < m_particleDiameter;

		// 一个粒子可能同时处于多个Shape的AABB中，所以这里需要记录多个
		// SolveCollisionParticeInfo::shapeNum目前支持4个，估计足够了
		if (!shape->isSensor())
		{
			int infoIndex = -1;
			for (int i = 0; i < collisionInfo->shapeCount; ++i)
			{
				if (collisionInfo->shape[i] == shape)
				{
					infoIndex = i;
					break;
				}
			}

			if (infoIndex == -1 && collisionInfo->shapeCount < SolveCollisionParticeInfo::shapeNum)
			{
				infoIndex = collisionInfo->shapeCount;
				collisionInfo->shape[collisionInfo->shapeCount++] = shape;
			}

			if (infoIndex != -1)
			{
				// Always keep latest coarse info for this sub-step.
				collisionInfo->normal[infoIndex] = n;
				collisionInfo->distance[infoIndex] = d * d;
				collisionInfo->collision[infoIndex] = collision;
			}
		}

		// printf("check distance ap %f %f  n %f %f d %f \n", ap.x, ap.y, n.x, n.y, d);

		// SqTransform tt = body->getTransform();
		// printf("body Transform %f %f %f %f\n", tt.p.x, tt.p.y, tt.q.c, tt.q.s);
		// SqAABB aabb = shape->computeShapeAABB(tt);
		// fixture->m_shape->ComputeAABB(&aabb, tt, 0);
		// printf("aabb %f %f %f %f \n", aabb.lowerBound.x, aabb.lowerBound.y, aabb.upperBound.x, aabb.upperBound.y);

		// printf("collision %p pos(%f %f) d: %f normal(%f %f) m_particleDiameter %f %d \n",shape,ap.x,ap.y,d,n.x,n.y,m_particleDiameter,d < m_particleDiameter);

		// 如果这个距离小于粒子直径，则表示这个粒子与刚体发生碰撞
		if (collision)
		{

			if (shape->isSensor())
			{
				// 传感器不需要计算碰撞点，是穿过去的
				if (sq_fixtureContactListenerParticle & m_allParticleFlags)
				{
					CreateContact(particleId, shape);
				}
				continue;
			}

			// printf("particle collision shape %d \n", particle->getId());

			SqBody *body = world->world->getBody(shape->getBody());
			SqMassData massData = body->getMassData();
			SqVec2 bp = body->getWorldCenterOfMass();
			float bm = massData.mass;
			float bI = massData.rotationalInertia - bm * SqVec2::LengthSquared(body->getLocalCenterOfMass());
			float invBm = bm > 0 ? 1 / bm : 0;
			float invBI = bI > 0 ? 1 / bI : 0;
			float invAm = particle->flags & sq_wallParticle ? 0 : GetParticleInvMass();

			// 计算对粒子应用的冲量大小
			SqVec2 rp = ap - bp; // 刚体上的点到质心的向量
			float rpn = SqVec2::Cross(rp, n);
			float invM = invAm + invBm + invBI * rpn * rpn;

			SqParticleBodyContact *contact = CreateContact(particleId, shape);
			DetectStuckParticle(particle->getId());
			contact->weight = 1.f - d * m_inverseDiameter;
			contact->normal = -n; // 使得法线由粒子指向Shape
			contact->mass = invM > 0 ? 1 / invM : 0;
			// printf("+++ contact begin normal %f %f n %f %f \n", contact->normal.x, contact->normal.y, n.x, n.y);
			// printf("---new contact %d %d %d state %d \n", contact->particleId, contact->index, contact->shapeId.index1,contact->state);
		}
	}
}

bool phxy::b2FixtureParticleQueryCallback(SqShape *shape, void *context)
{
	phxy::SqParticleSystem *system = (phxy::SqParticleSystem *)context;
	system->fixtureParticleQueryCallback(shape);
	return true;
}

/**
 * 检查粒子是否与Box2D世界中的刚体接触。
 *
 * 1、这里只是基于上一帧的粒子的位置来判断
 * 2、这里通过查询粒子与Body的距离来计算是否发生碰撞，这里不是精确判断是否发生碰撞，是一个粗略计算的阶段
 *    精确计算发生碰撞在SolveCollision中
 */
void SqParticleSystem::UpdateBodyContacts()
{

	// 外部需要监听End事件，所以上一帧需要被移除的Contact到下一帧再移除
	data.m_bodyContactBuffer.removeIf(&SqParticleBodyContact::bodyContactListRemoveInvalid);

	// 先清空所有的碰撞点信息。每帧都清空
	if (sq_fixtureContactListenerParticle & m_allParticleFlags)
	{
		// 必须先排序，因为接下来的逻辑需要使用二分法查找碰撞点
		data.m_bodyContactBuffer.sort(&SqParticleBodyContact::bodyContactListCompare);

		for (int k = 0; k < data.m_bodyContactBuffer.getCount(); k++)
		{
			SqParticleBodyContact *contact = data.m_bodyContactBuffer.get(k);
			// printf("cache contact particleId %d shape %p state %d \n",contact->particleId,contact->shape,contact->state);
			contact->state = 0;
		}
	}
	else
	{
		data.m_bodyContactBuffer.clear();
	}

	// particles.sort(SqParticle::CompareIndex);
	// m_stuckParticleBuffer.SetCount(0);

	// 查询所有的粒子组成的AABB盒子内的Body，这样就只检查位于这个AABB盒子内的Body就行了
	SqAABB aabb;
	ComputeAABB(&aabb);

	// printf("query aabb %f %f %f %f \n", aabb.lowerBound.x, aabb.lowerBound.y, aabb.upperBound.x, aabb.upperBound.y);

	// 查找指定区域内的Body
	world->world->overlapAABB(aabb, bodyContactfilter, &b2FixtureParticleQueryCallback, this);

	// if (m_def.strictContactCheck)
	// {
	// 	RemoveSpuriousBodyContacts();
	// }

	// for (int k = 0; k < m_bodyContactBuffer.GetCount(); k++)
	// {
	// 	SqParticleBodyContact &contact = m_bodyContactBuffer[k];
	// 	printf("after %d %d\n", k, contact.index);
	// }
	// for (int k = 0; k < m_bodyContactBuffer.GetCount(); k++)
	// {
	// 	SqParticleBodyContact &contact = m_bodyContactBuffer[k];
	// 	printf("after cache %p %d %d %d\n",&contact, contact.state,contact.index,contact.shapeId.index1);
	// }
}

// int count = 0;

//=================== 精确阶段检测与Box2D发生碰撞的粒子 =======================
void SqParticleSystem::SystemSolveCollisionCallback(SqShape *shape, const SqStepContext *context)
{

	const SqShapeFilter &filterB = shape->getFilter();
	bool collide = (bodyContactfilter.maskBits & filterB.categoryBits) != 0 && (bodyContactfilter.categoryBits & filterB.maskBits) != 0;
	if (!collide)
	{
		return;
	}

	const SqAABB &aabb = shape->getFatAABB();

	// LiquidFun本来有一个问题：gird中还是上一帧的位置计算得到的Tag，不是拿当前最新newPos，速度过快好像有粒子穿墙。
	// 所以我添加了SqParticleSystemGrid::updateAndSortNewPos() 来处理这种情况
	InsideBoundsEnumerator enumerator = grid.GetInsideBoundsEnumerator(aabb);
	int particleId;
	while ((particleId = enumerator.GetNext()) >= 0)
	{
		SqParticle *particle = findById(particleId);
		const SqVec2 &ap = particle->getPosition();
		const SqVec2 &newPos = particle->getNewPosition();
		SolveCollisionParticeInfo *collisionInfo = &solveCollisionParticeArray[particle->getBufferIndex()];
		if (collisionInfo->hitStepStamp != m_timestamp)
		{
			collisionInfo->hitStepStamp = m_timestamp;
			collisionInfo->hasHit = false;
			collisionInfo->hitFraction = 1.0f;
		}

		/**
		 * 1、如果collisionInfo->shapeCount == 0则表示上一帧的位置不与任何的shape发生碰撞，但这里新的位置发生碰撞了，所以需要检测
		 * 2、如果上一帧的位置与当前的Shape发生碰撞了，那么检测新的位置是否真正能够与这个Shape发生碰撞，不能的话就没必要往下计算，节省很多性能（因为下面需要射线检测）
		 * 3、还有一种情况就是：这里是查询Shape的AABB范围内的粒子去计算RayCast，由于水体的粒子总是挤在一起的特性可能当前系统所有的粒子都在一个Shape的AABB内但没有与这个Shape发生碰撞
		 *    如果没有solveCollisionParticeArray这个东西的话也会进行射线检测，造成不必要的浪费
		 *
		 * 基于上面的情况，我们构建solveCollisionParticeArray这个数据的目的就是来减少这个阶段的粒子检测
		 */

		if (collisionInfo->stepStamp == m_timestamp && collisionInfo->shapeCount > 0)
		// if (false)
		{
			int index = -1;
			for (int i = 0; i < collisionInfo->shapeCount; ++i)
			{
				SqShape *collisionShape = collisionInfo->shape[i];
				if (collisionShape == shape)
				{
					index = i;
					break;
				}
			}

			if (index != -1)
			{

				/**
				 * 	首先判断新的位置是否还是往Shape方向靠近：
				 * 1、如果是则证明可能与这个Shape发生碰撞，那么继续判断从上一帧位置到newPos位置的长度是否超过了collisionInfo存储的长度，超出证明可能发生碰撞
				 * 2、如果向另外的方向去了，则肯定不会发生碰撞，直接不用往下计算了
				 *
				 */

				SqVec2 moveDirection = newPos - ap;
				float moveLenSq = SqVec2::LengthSquared(moveDirection);
				float d = -1.0f;
				if (moveLenSq > 1e-8f)
				{
					d = SqVec2::Dot(SqVec2::Normalize(moveDirection), collisionInfo->normal[index]);
				}

				// 点积大于0形成锐角\等于0是直角，证明移动方向向别的地方去了，不可能与当前Shape发生碰撞
				// 点积小于0是钝角，粒子正往Shape的方向移动
				if (moveLenSq > 1e-8f && d >= 0)
				{
					// printf("????\n");
					continue;
				}

				// 如果上一帧位置发生了碰撞，那么新的位置肯定需要检测了
				if (!collisionInfo->collision[index])
				{
					// 上一帧位置没有发生碰撞，那么新的位置就有可能发生碰撞，继续判断移动的距离是否超出了上面碰撞的距离

					// printf("ll %f %f \n", SqVec2::LengthSquared(moveDirection), collisionInfo->distance[index]);

					if (moveLenSq > 1e-8f && moveLenSq <= collisionInfo->distance[index])
					{
						// 不可能发生碰撞
						// printf("????2\n");
						continue;
					}
				}
			}
		}

		// ++count;

		SqRayCastInput input;
		SqBody *body = world->world->getBody(shape->getBody());
		SqBodySim *sim = body->getBodySim();

		if (m_iterationIndex == 0)
		{

			// 计算从上一帧的位置作为起点，当前帧位置作为终点的射线
			SqTransform prevTransform;
			prevTransform.q = sim->prevSim.rotation;
			prevTransform.p = SqVec2::Sub(sim->prevSim.center, SqRot::transformVector(sim->prevSim.rotation, sim->localCenter));

			// printf("?????????? %f %f %f %f \n",prevTransform.p.x,prevTransform.p.y,prevTransform.q.c,prevTransform.q.s);

			SqVec2 p1 = prevTransform.invTransformPoint(ap);
			if (shape->type == SqShapeType::sq_circleShape)
			{
				// Make relative to the center of the circle
				p1 -= body->getLocalCenterOfMass();
				// Re-apply rotation about the center of the
				// circle
				p1 = SqRot::invRotateVector(prevTransform.q, p1);
				// Subtract rotation of the current frame
				p1 = SqRot::transformVector(sim->transform.q, p1);
				// Return to local space
				p1 += body->getLocalCenterOfMass();
			}

			// Return to global space and apply rotation of current frame
			input.origin = sim->transform.transformPoint(p1);
		}
		else
		{
			input.origin = ap;
		}

		// 上面得到的是Shape的AABB区域内的粒子，而不是真正与Shape形状发生碰撞的粒子，
		// 所以还需要做射线检测得到精确与Shape发生碰撞的粒子

		// 从当前位置到下一个位置作一条射线，检测是否还与Shape发生碰撞

		input.translation = newPos - input.origin; // 要求传进去的是方向和大小
		input.maxFraction = 1.f;
		SqCastOutput output;
		shape->RayCast(output, input, sim->transform);

		// printf("particle v %f %f \n", particle->velocity.x, particle->velocity.y);
		// printf("body Transform p %f %f c %f s %f \n", sim->transform.p.x, sim->transform.p.y, sim->transform.q.c, sim->transform.q.s);
		// printf("particle hit %d origin %f %f translation %f %f rayLength %f fraction %f\n",
		// 	   output.hit,
		// 	   input.origin.x, input.origin.y,
		// 	   input.translation.x, input.translation.y,
		// 	   SqVec2::Length(input.translation),
		// 	   output.fraction);

		// printf("collisoin  %f %f hit %d \n", particle->velocity.x, particle->velocity.y,output.hit);

		if (output.hit)
		{

			// 这里修改粒子的速度，这里计算出来的速度只能让粒子停留在墙上，防止粒子穿墙
			// 撞到墙面弹开是由SolvePressure等方式弹开的

			// 从当前位置线性插值到新的位置，这个位置就是碰撞处的位置，然后再在法线的方向上加一点位置
			SqVec2 p = (1 - output.fraction) * input.origin + output.fraction * newPos + sq_linearSlop * output.normal;
			if (!collisionInfo->hasHit || output.fraction < collisionInfo->hitFraction)
			{
				collisionInfo->hasHit = true;
				collisionInfo->hitFraction = output.fraction;
				collisionInfo->hitPosition = p;
				collisionInfo->hitNormal = output.normal;
			}

			// printf("before v %f %f %f\n",particle->velocity.x,particle->velocity.y,output.fraction);
			// 延后到 SolveCollision 末尾统一处理，避免同一粒子被多个 shape 覆写。
		}
	}
}

struct SolveCollisionCallbackContext
{
	SqParticleSystem *system;
	const SqStepContext *context;
};

bool phxy::SolveCollisionCallback(SqShape *shape, void *context)
{
	if (shape->isSensor())
	{
		return true;
	}
	SolveCollisionCallbackContext *context1 = (SolveCollisionCallbackContext *)context;
	context1->system->SystemSolveCollisionCallback(shape, context1->context);
	return true;
}

/**
 * 精确阶段检测与Box2D发生碰撞的粒子
 */
void SqParticleSystem::SolveCollision(const SqStepContext &step)
{

	// count = 0;
	SqAABB aabb;
	SolveCollisionCallbackContext callbackContext;
	callbackContext.system = this;
	callbackContext.context = &step;

	// 使用最新的速度计算新的位置，这样上一帧位置到新的位置可以围成更大的盒子，从而碰撞检测就不会穿墙
	SolveVelocity(step, true);

	// 跟上一帧位置和最新位置计算得到一个AABB盒子
	ComputeAABB(&aabb, true);

	grid.updateAndSortNewPos();

	// 查找指定区域内的Body
	world->world->overlapAABB(aabb, bodyContactfilter, &SolveCollisionCallback, &callbackContext);

	auto applyCollisionHitCallback = [this, &step](int particleId)
	{
		SqParticle *particle = findById(particleId);
		SolveCollisionParticeInfo *collisionInfo = &solveCollisionParticeArray[particle->getBufferIndex()];
		if (collisionInfo->hitStepStamp != m_timestamp || !collisionInfo->hasHit)
		{
			return;
		}

		const SqVec2 &ap = particle->getPosition();
		const SqVec2 &av = particle->velocity;
		SqVec2 v = step.inv_dt * (collisionInfo->hitPosition - ap);
		particle->velocity = v;
		SqVec2 f = step.inv_dt * GetParticleMass() * (av - v);
		ParticleApplyForce(particleId, f);
	};
	particles.iterate<int>(applyCollisionHitCallback);

	// printf("===========check %d %d \n", data.m_count, count);
}

//=================计算粒子与粒子之间的碰撞
void SqParticleSystem::AddContact(int a, int b)
{

	SqParticle *aParticle = findById(a);
	SqParticle *bParticle = findById(b);

	// 计算两个粒子的距离向量
	SqVec2 d = bParticle->getPosition() - aParticle->getPosition();

	// 得到粒子距离的平方
	float distBtParticlesSq = SqVec2::LengthSquared(d);

	// 如果距离小于粒子直径的平方，则表示这两个粒子发生碰撞
	if (distBtParticlesSq < m_squaredDiameter)
	{

		// 为这对粒子创建一个接触实例

		phxy::SqParticleContact *contact = nullptr;
		b2ContactListener *const contactListener = GetParticleContactListener();
		if (contactListener)
		{
			phxy::SqParticleContact find;
			find.indexA = a;
			find.indexB = b;
			contact = data.m_contactBuffer.findWithBinarySearch(&find, &SqParticleContact::contactListSortCompare, SqParticleContact::contactEquals);
			if (contact)
			{
				contact->state = 1;
			}
		}

		if (!contact)
		{
			contact = data.m_contactBuffer.Add();
			contact->system = this;
			contact->state = 2;
			// 记录哪是哪两个粒子
			contact->SetIndices(a, b);
			contact->SetFlags(aParticle->flags | bParticle->flags);
		}

		// 可能两个粒子重合了，距离为0
		if (distBtParticlesSq == 0.)
		{
			contact->SetWeight(1.0f);
			contact->SetNormal(SqVec2(1.f, 0.f));
			return;
		}

		float invD = sqInvSqrt(distBtParticlesSq);

		// 计算权重
		// 跟粒子与Body碰撞一样，也是根据粒子穿透距离来计算权重值
		// distBtParticlesSq * invD 迷惑？ 因为invD是开方的倒数啊，即invD = 1/sqrt{distBtParticlesSq}
		// 所以 distBtParticlesSq * invD 其实就是开平方后两个粒子的距离
		contact->SetWeight(1 - distBtParticlesSq * invD * m_inverseDiameter);

		// printf("collision %d %d invD %f weight %d d %f %f pb %f %f pa %f %f \n", a, b, invD, contact->GetWeight(), d.x, d.y,
		// 	   bParticle->getPosition().x, bParticle->getPosition().y,
		// 	   aParticle->getPosition().x, aParticle->getPosition().y);

		// 碰撞法线
		contact->SetNormal(invD * d);
	}
}

void SqParticleSystem::FindContacts_Reference()
{
	const GridCell *beginProxy = grid.cells.begin();
	const GridCell *endProxy = grid.cells.end();

	/**
	 * 对粒子的8个相邻格子进行遍历,检查他们的距离，如果发生碰撞则生成碰撞点
	 */

	// 在这里contacts已经是按照网格的位置即tag从小到大排序的了,亦即按照二维网格从左到右从上大小排列粒子
	for (const GridCell *a = beginProxy, *c = beginProxy; a < endProxy; a++)
	{

		// 获取当前格子右边的格子的tag
		unsigned int rightTag = computeRelativeTag(a->tag, 1, 0);
		for (const GridCell *b = a + 1; b < endProxy; b++)
		{
			// 如果右边有粒子应该是rightTag==b->tag,如果大于了，表示右边有空隙格子
			if (rightTag < b->tag)
				break;
			AddContact(a->particleId, b->particleId);
		}

		// 获取当前格子左下角的格子
		unsigned int bottomLeftTag = computeRelativeTag(a->tag, -1, 1);
		for (; c < endProxy; c++)
		{
			// 这时，c就是左下角格子的Proxy实例
			if (bottomLeftTag <= c->tag)
				break;
		}

		// 获取当前格子右下角的格子
		unsigned int bottomRightTag = computeRelativeTag(a->tag, 1, 1);

		// 从左下角到右下角之间的格子
		// 所以这里相当于检查：左下角、正对下面的格子和右下角 的三个格子
		for (const GridCell *b = c; b < endProxy; b++)
		{
			if (bottomRightTag < b->tag)
				break;
			AddContact(a->particleId, b->particleId);
		}
	}

	/**
	 * 你可能会问，为啥不检查a左侧的格子呢？
	 * 因为contacts是排序过的，a左侧的格子已经被检查过了。
	 * 举例子：
	 * 如果格子是1，则就不用检查格子0，因为格子0已经被检查过了。
	 * 因为在检查格子0的时候，格子0的右侧就是1，那么0和1这一对已经检查过了，所以到1的时候，1和0就不需要检查啦
	 *
	 * 同理：我们不用检查粒子的左上角、右上角和上方，因为位于a上面的格子已经检查过了。
	 */
}

LIQUIDFUN_SIMD_INLINE
void SqParticleSystem::FindContacts()
{
#if defined(LIQUIDFUN_SIMD_NEON)
	FindContacts_Simd(contacts);
#else
	FindContacts_Reference();
#endif

#if defined(LIQUIDFUN_SIMD_TEST_VS_REFERENCE)
	b2GrowableBuffer<SqParticleContact>
		reference(m_blockAllocator);
	FindContacts_Reference(reference);

	SQ_ASSERT(contacts.GetCount() == reference.GetCount());
	for (int i = 0; i < contacts.GetCount(); ++i)
	{
		SQ_ASSERT(contacts[i].ApproximatelyEqual(reference[i]));
	}
#endif // defined(LIQUIDFUN_SIMD_TEST_VS_REFERENCE)
}

void SqParticleSystem::UpdateContacts(bool exceptZombie)
{

	// 如果外部需要监听粒子碰撞事件，那么我们这里就需要处理了
	b2ContactListener *const contactListener = GetParticleContactListener();
	if (contactListener)
	{
		// 必须先排序，因为接下来使用二分法来查找
		data.m_contactBuffer.sort(&SqParticleContact::contactListSortCompare);

		for (int i = 0; i < data.m_contactBuffer.getCount(); ++i)
		{
			data.m_contactBuffer.get(i)->state = 0;
		}
	}
	else
	{
		// 如果没有事件监听，则直接简单处理即可,一键清空所有的碰撞点所有都重新生成即可
		data.m_contactBuffer.clear();
	}

	grid.update();

	// 生成碰撞点
	FindContacts();

	if (contactListener)
	{
		for (int i = 0; i < data.m_contactBuffer.getCount(); ++i)
		{
			SqParticleContact *contact = data.m_contactBuffer.get(i);
			if (contact->state == 2)
			{
				contactListener->BeginParticleContact(this, contact);
			}
			else if (contact->state == 0)
			{
				contactListener->EndParticleContact(this, contact->GetIndexA(), contact->GetIndexB());
			}
		}
		if (exceptZombie)
		{
			// 如果粒子标记为僵尸粒子（即表示可以被销毁的粒子），则从碰撞点列表中移除
			data.m_contactBuffer.removeIf(SqParticleContact::ContactIsZombie);
		}
	}
}

void SqParticleSystem::ComputeWeight()
{
	// calculates the sum of contact-weights for each particle
	// that means dimensionless density

	// 先将粒子所有权重值重置为0
	memset(data.m_weightBuffer, 0, sizeof(*data.m_weightBuffer) * data.m_count);

	/**
	 * 计算的算法就是将粒子与其发生碰撞粒子或与其发生碰撞的刚体所产生的权重加起来作为这个粒子的权重值
	 */
	for (int k = 0; k < data.m_bodyContactBuffer.getCount(); k++)
	{
		const SqParticleBodyContact *contact = data.m_bodyContactBuffer.get(k);

		if (contact->state == 0 || contact->shape->isSensor())
			continue;

		SqParticle *particle = findById(contact->particleId);
		data.m_weightBuffer[particle->particleBufferIndex] += contact->weight;
	}

	for (int k = 0; k < data.m_contactBuffer.getCount(); k++)
	{
		const phxy::SqParticleContact *contact = data.m_contactBuffer.get(k);
		SqParticle *aParticle = findById(contact->GetIndexA());
		SqParticle *bParticle = findById(contact->GetIndexB());
		float w = contact->GetWeight();
		data.m_weightBuffer[aParticle->particleBufferIndex] += w;
		data.m_weightBuffer[bParticle->particleBufferIndex] += w;
	}
}

void SqParticleSystem::ComputeDepth()
{
	SqParticleContact *contactGroups = (SqParticleContact *)sqstd::StackTempArenaAllocator::getInstance()->allocateChunk(sizeof(SqParticleContact) * data.m_contactBuffer.getCount());

	int contactGroupsCount = 0;
	for (int k = 0; k < data.m_contactBuffer.getCount(); k++)
	{
		const SqParticleContact &contact = *data.m_contactBuffer.get(k);
		SqParticle *aParticle = findById(contact.GetIndexA());
		SqParticle *bParticle = findById(contact.GetIndexB());
		const SqParticleGroup *groupA = aParticle->getGroup();
		const SqParticleGroup *groupB = bParticle->getGroup();
		if (groupA && groupA == groupB && (groupA->m_groupFlags & b2_particleGroupNeedsUpdateDepth))
		{
			contactGroups[contactGroupsCount++] = contact;
		}
	}

	SqParticleGroup **groupsToUpdate = (SqParticleGroup **)sqstd::StackTempArenaAllocator::getInstance()->allocateChunk(sizeof(SqParticleGroup *) * m_groupCount);
	int groupsToUpdateCount = 0;

	{
		auto callback = [this](int particleId)
		{
			SqParticle *particle = findById(particleId);
			data.m_accumulationBuffer[particle->particleBufferIndex] = 0;
		};
		for (SqParticleGroup *group = m_groupList; group; group = group->GetNext())
		{
			if (group->m_groupFlags & b2_particleGroupNeedsUpdateDepth)
			{
				groupsToUpdate[groupsToUpdateCount++] = group;
				SetGroupFlags(group, group->m_groupFlags & ~b2_particleGroupNeedsUpdateDepth);
				group->particles.iterate<int>(callback);
			}
		}
	}

	// Compute sum of weight of contacts except between different groups.
	for (int k = 0; k < contactGroupsCount; k++)
	{
		const SqParticleContact &contact = contactGroups[k];
		SqParticle *aParticle = findById(contact.GetIndexA());
		SqParticle *bParticle = findById(contact.GetIndexB());
		float w = contact.GetWeight();
		data.m_accumulationBuffer[aParticle->particleBufferIndex] += w;
		data.m_accumulationBuffer[bParticle->particleBufferIndex] += w;
	}

	{
		auto callback = [this](int particleId)
		{
			SqParticle *particle = findById(particleId);
			float w = data.m_accumulationBuffer[particle->particleBufferIndex];
			data.m_depthBuffer.data[particle->particleBufferIndex] = w < 0.8f ? 0 : sq_maxFloat;
		};

		for (int i = 0; i < groupsToUpdateCount; i++)
		{
			const SqParticleGroup *group = groupsToUpdate[i];
			group->particles.iterate<int>(callback);
		}
	}

	// The number of iterations is equal to particle number from the deepest
	// particle to the nearest surface particle, and in general it is smaller
	// than sqrt of total particle number.
	int iterationCount = (int)sqrt((float)data.m_count);
	for (int t = 0; t < iterationCount; t++)
	{
		bool updated = false;
		for (int k = 0; k < contactGroupsCount; k++)
		{
			const SqParticleContact &contact = contactGroups[k];
			SqParticle *aParticle = findById(contact.GetIndexA());
			SqParticle *bParticle = findById(contact.GetIndexB());
			float r = 1 - contact.GetWeight();
			float &ap0 = data.m_depthBuffer.data[aParticle->particleBufferIndex];
			float &bp0 = data.m_depthBuffer.data[bParticle->particleBufferIndex];
			float ap1 = bp0 + r;
			float bp1 = ap0 + r;
			if (ap0 > ap1)
			{
				ap0 = ap1;
				updated = true;
			}
			if (bp0 > bp1)
			{
				bp0 = bp1;
				updated = true;
			}
		}
		if (!updated)
		{
			break;
		}
	}

	{
		auto callback = [this](int particleId)
		{
			SqParticle *particle = findById(particleId);
			float &p = data.m_depthBuffer.data[particle->particleBufferIndex];
			if (p < sq_maxFloat)
			{
				p *= m_particleDiameter;
			}
			else
			{
				p = 0;
			}
		};

		for (int i = 0; i < groupsToUpdateCount; i++)
		{
			const SqParticleGroup *group = groupsToUpdate[i];
			group->particles.iterate<int>(callback);
		}
	}

	sqstd::StackTempArenaAllocator::getInstance()->freeChunk(groupsToUpdate);
	sqstd::StackTempArenaAllocator::getInstance()->freeChunk(contactGroups);
}

/**
 *
 * 我已经有Box2D刚体，也可以将粒子设置为b2_wallParticle作粒子系统的碰撞体，
 * 使得粒子不能穿过这些碰撞体。但这里还还可以提供另外一种碰撞体，称为Barrier，虚拟碰撞体。
 *
 * SolveBarrier 就是通过一对对标记了k_barrierWallFlags的粒子，动态构建出一道“虚拟的碰撞体”，让普通粒子无法穿过它。
 * 就k_barrierWallFlags粒子两两连成线，其他的粒子不能穿过这条线，但这对k_barrierWallFlags粒子是可以运动的，而b2_wallParticle粒子是不能运动的。
 *
 * 就像你在水池边放两根杆子（barrier粒子），看起来没连在一起，但如果水滴试图穿过去，就会被空气墙“弹回”。
 * 当这两根杠子是可以随着流动的，而不像2_wallParticle粒子固定不动。
 */
void SqParticleSystem::SolveBarrier(const SqStepContext &step)
{
	// If a particle is passing between paired barrier particles,
	// its velocity will be decelerated to avoid passing.
	static unsigned int k_barrierWallFlags = sq_barrierParticle | sq_wallParticle;

	auto callback = [this](int particleId)
	{
		SqParticle *particle = findById(particleId);
		if ((particle->flags & k_barrierWallFlags) == k_barrierWallFlags)
		{
			particle->velocity.zero();
		}
	};
	particles.iterate<int>(callback);

	float tmax = sq_barrierCollisionTime * step.dt;
	for (int k = 0; k < data.m_pairBuffer.getCount(); k++)
	{
		const SqParticlePair &pair = *data.m_pairBuffer.get(k);
		if (pair.flags & sq_barrierParticle)
		{
			SqParticle *aParitcle = findById(pair.indexA);
			SqParticle *bParticle = findById(pair.indexB);
			const SqVec2 &pa = aParitcle->getPosition();
			const SqVec2 &pb = bParticle->getPosition();
			SqAABB aabb;
			aabb.lowerBound = SqVec2::Min(pa, pb);
			aabb.upperBound = SqVec2::Max(pa, pb);
			SqParticleGroup *aGroup = aParitcle->getGroup();
			SqParticleGroup *bGroup = bParticle->getGroup();
			SqVec2 va = GetLinearVelocity(aGroup, aParitcle->getId(), pa);
			SqVec2 vb = GetLinearVelocity(bGroup, bParticle->getId(), pb);
			SqVec2 pba = pb - pa;
			SqVec2 vba = vb - va;
			InsideBoundsEnumerator enumerator = grid.GetInsideBoundsEnumerator(aabb);

			int c;
			while ((c = enumerator.GetNext()) >= 0)
			{
				SqParticle *particle = findById(c);
				const SqVec2 &pc = particle->getPosition();
				SqParticleGroup *cGroup = particle->getGroup();
				if (aGroup != cGroup && bGroup != cGroup)
				{
					SqVec2 vc = GetLinearVelocity(cGroup, c, pc);
					// Solve the equation below:
					//   (1-s)*(pa+t*va)+s*(pb+t*vb) = pc+t*vc
					// which expresses that the particle c will pass a line
					// connecting the particles a and b at the time of t.
					// if s is between 0 and 1, c will pass between a and b.
					SqVec2 pca = pc - pa;
					SqVec2 vca = vc - va;
					float e2 = SqVec2::Cross(vba, vca);
					float e1 = SqVec2::Cross(pba, vca) - SqVec2::Cross(pca, vba);
					float e0 = SqVec2::Cross(pba, pca);
					float s, t;
					SqVec2 qba, qca;
					if (e2 == 0)
					{
						if (e1 == 0)
							continue;
						t = -e0 / e1;
						if (!(t >= 0 && t < tmax))
							continue;
						qba = pba + t * vba;
						qca = pca + t * vca;
						s = SqVec2::Dot(qba, qca) / SqVec2::Dot(qba, qba);
						if (!(s >= 0 && s <= 1))
							continue;
					}
					else
					{
						float det = e1 * e1 - 4 * e0 * e2;
						if (det < 0)
							continue;
						float sqrtDet = sqrt(det);
						float t1 = (-e1 - sqrtDet) / (2 * e2);
						float t2 = (-e1 + sqrtDet) / (2 * e2);
						if (t1 > t2)
							sqSwap(t1, t2);
						t = t1;
						qba = pba + t * vba;
						qca = pca + t * vca;
						s = SqVec2::Dot(qba, qca) / SqVec2::Dot(qba, qba);
						if (!(t >= 0 && t < tmax && s >= 0 && s <= 1))
						{
							t = t2;
							if (!(t >= 0 && t < tmax))
								continue;
							qba = pba + t * vba;
							qca = pca + t * vca;
							s = SqVec2::Dot(qba, qca) / SqVec2::Dot(qba, qba);
							if (!(s >= 0 && s <= 1))
								continue;
						}
					}
					// Apply a force to particle c so that it will have the
					// interpolated velocity at the collision point on line ab.
					SqVec2 dv = va + s * vba - vc;
					SqVec2 f = GetParticleMass() * dv;
					if (IsRigidGroup(cGroup))
					{
						// If c belongs to a rigid group, the force will be
						// distributed in the group.
						float mass = cGroup->GetMass();
						float inertia = cGroup->GetInertia();
						if (mass > 0)
						{
							cGroup->m_linearVelocity += 1 / mass * f;
						}
						if (inertia > 0)
						{
							cGroup->m_angularVelocity += SqVec2::Cross(pc - cGroup->GetCenter(), f) / inertia;
						}
					}
					else
					{
						particle->velocity += dv;
					}
					// Apply a reversed force to particle c after particle
					// movement so that momentum will be preserved.
					ParticleApplyForce(c, -step.inv_dt * f);
				}
			}
		}
	}
}

void SqParticleSystem::UpdateAllParticleFlags()
{
	m_allParticleFlags = 0;

	auto callback = [this](int particleId)
	{
		SqParticle *particle = findById(particleId);
		m_allParticleFlags |= particle->flags;
	};
	particles.iterate<int>(callback);
	m_needsUpdateAllParticleFlags = false;
}

void SqParticleSystem::UpdateAllGroupFlags()
{
	m_allGroupFlags = 0;
	for (const SqParticleGroup *group = m_groupList; group; group = group->GetNext())
	{
		m_allGroupFlags |= group->m_groupFlags;
	}
	m_needsUpdateAllGroupFlags = false;
}

void SqParticleSystem::LimitVelocity(const SqStepContext &step)
{
	float criticalVelocitySquared = GetCriticalVelocitySquared(step);

	auto callback = [this, criticalVelocitySquared](int particleId)
	{
		SqParticle *particle = findById(particleId);
		SqVec2 &v = particle->velocity;
		float v2 = SqVec2::Dot(v, v);
		if (v2 > criticalVelocitySquared)
		{
			v *= sqrt(criticalVelocitySquared / v2);
		}
	};
	particles.iterate<int>(callback);
}

/**
 * 为所有粒子应用重力
 */
void SqParticleSystem::SolveGravity(const SqStepContext &step)
{
	SqVec2 gravity = step.dt * m_def.gravityScale * world->world->getGravity();
	auto callback = [this, &gravity](int particleId)
	{
		SqParticle *particle = findById(particleId);
		particle->velocity += gravity;
		// printf("SolveGravity %d %f %f %f\n", particle->getId(), particle->getPosition().x, particle->getPosition().y, SqVec2::Length(particle->velocity));
	};
	particles.iterate<int>(callback);
}

/**
 * SolveStaticPressure 让粒子模拟的液体具有体积感，不容易被挤穿，表现出抗压和膨胀反弹的真实液体行为。
 * 可以通俗想象为给被压缩的粒子“打气”，恢复正常体积。即根据压力对每个接触粒子施加排斥力。
 *  模拟效果表现为：
	 1、当粒子挤压过度时，会自动在其局部区域建立一个“压力场”，将其向外推开；
	 2、表现上类似液体或凝胶状物体在受到压缩时自动反弹；
	 3、避免粒子密堆时体积不守恒或出现高频数值不稳定。

	这其实模拟的是一种准不可压缩性（quasi-incompressibility）的流体模拟，是替代真实流体 Navier-Stokes 求解压力的一种便捷方式。
	该方法通过迭代求解近似的泊松方程（Poisson Equation），在粒子之间生成一个局部的静态压力场，从而让粒子不会因为压缩太多而出现异常密度或穿插。
 */
void SqParticleSystem::SolveStaticPressure(const SqStepContext &step)
{
	data.requestStaticPressureBuffer();
	float criticalPressure = GetCriticalPressure(step);
	float pressurePerWeight = m_def.staticPressureStrength * criticalPressure;
	float maxPressure = sq_maxParticlePressure * criticalPressure;
	float relaxation = m_def.staticPressureRelaxation;
	/// Compute pressure satisfying the modified Poisson equation:
	///     Sum_for_j((p_i - p_j) * w_ij) + relaxation * p_i =
	///     pressurePerWeight * (w_i - b2_minParticleWeight)
	/// by iterating the calculation:
	///     p_i = (Sum_for_j(p_j * w_ij) + pressurePerWeight *
	///           (w_i - b2_minParticleWeight)) / (w_i + relaxation)
	/// where
	///     p_i and p_j are static pressure of particle i and j
	///     w_ij is contact weight between particle i and j
	///     w_i is sum of contact weight of particle i

	auto callback = [this, pressurePerWeight, relaxation, maxPressure](int particleId)
	{
		SqParticle *particle = findById(particleId);
		float w = particle->getWeight();
		if (particle->flags & sq_staticPressureParticle)
		{
			float wh = data.m_accumulationBuffer[particle->particleBufferIndex];
			float h = (wh + pressurePerWeight * (w - sq_minParticleWeight)) / (w + relaxation);
			data.m_staticPressureBuffer.data[particle->particleBufferIndex] = clamp(h, 0.0f, maxPressure);
		}
		else
		{
			data.m_staticPressureBuffer.data[particle->particleBufferIndex] = 0.f;
		}
	};

	for (int t = 0; t < m_def.staticPressureIterations; t++)
	{
		data.resetAccumulationBuffer();

		for (int k = 0; k < data.m_contactBuffer.getCount(); k++)
		{
			const SqParticleContact &contact = *data.m_contactBuffer.get(k);
			if (contact.GetFlags() & sq_staticPressureParticle)
			{
				SqParticle *aParticle = findById(contact.GetIndexA());
				SqParticle *bParticle = findById(contact.GetIndexB());
				float w = contact.GetWeight();
				data.m_accumulationBuffer[aParticle->particleBufferIndex] += w * data.m_staticPressureBuffer.data[bParticle->particleBufferIndex]; // a <- b
				data.m_accumulationBuffer[bParticle->particleBufferIndex] += w * data.m_staticPressureBuffer.data[aParticle->particleBufferIndex]; // b <- a
			}
		}
		particles.iterate<int>(callback);
	}
}

/**
 * 这个方法和上面的SolveStaticPressure的目的是一样的，根据压力对每个接触粒子施加排斥力。
 * SolvePressure模拟的是可压缩的液体，即粒子之间可以由些少的重叠。
 */
void SqParticleSystem::SolvePressure(const SqStepContext &step)
{
	// calculates pressure as a linear function of density
	float criticalPressure = GetCriticalPressure(step);
	float pressurePerWeight = m_def.pressureStrength * criticalPressure;
	float maxPressure = sq_maxParticlePressure * criticalPressure;

	// 根据权重计算每个粒子产生的压力，最大压力上限为maxPressure
	// 所以如果权重为0，则这个粒子不会产生压力，权重为0说明没有产生碰撞

	auto callback = [this, pressurePerWeight, maxPressure](int particleId)
	{
		SqParticle *particle = findById(particleId);
		if (particle->flags & k_noPressureFlags)
		{
			data.m_accumulationBuffer[particle->particleBufferIndex] = 0.0f;
		}
		else
		{
			float h = pressurePerWeight * max(0.0f, data.m_weightBuffer[particle->particleBufferIndex] - sq_minParticleWeight);
			data.m_accumulationBuffer[particle->particleBufferIndex] = min(h, maxPressure);
			if (particle->flags & sq_staticPressureParticle)
			{
				data.m_accumulationBuffer[particle->particleBufferIndex] += data.m_staticPressureBuffer.data[particle->particleBufferIndex];
			}
		}
	};
	particles.iterate<int>(callback);

	// m_def.density * m_particleDiameter 相当于计算一个粒子的质量
	// velocityPerPressure = dt * (F/m)
	float velocityPerPressure = step.dt / (m_def.density * m_particleDiameter);

	{
		float invMass = GetParticleInvMass();
		for (int k = 0; k < data.m_bodyContactBuffer.getCount(); k++)
		{
			const SqParticleBodyContact &contact = *data.m_bodyContactBuffer.get(k);
			if (contact.state == 0 || contact.shape->isSensor())
				continue;

			SqBody *body = world->world->getBody(contact.bodyId);
			SqParticle *particle = findById(contact.particleId);
			float w = contact.weight;
			float m = contact.mass;
			const SqVec2 &n = contact.normal;
			const SqVec2 &p = particle->getPosition();
			float h = data.m_accumulationBuffer[particle->particleBufferIndex] + pressurePerWeight * w;
			SqVec2 f = velocityPerPressure * w * m * h * n; // 在法线的方向应用冲量，也就是力
			particle->velocity -= invMass * f;				//  a = f/m

			// printf(" Pressure velocityPerPressure %f w %f m %f h %f n(%f %f) f(%f %f) \n", velocityPerPressure, w, m, h, n.x, n.y, f.x, f.y);

			if (body->isEnableParticlePressure())
				body->applyLinearImpulse(f, p, true); // 对刚体也应用压力，给刚体一个推力
		}
	}

	// return;

	for (int k = 0; k < data.m_contactBuffer.getCount(); k++)
	{
		const SqParticleContact &contact = *data.m_contactBuffer.get(k);
		SqParticle *aParticle = findById(contact.GetIndexA());
		SqParticle *bParticle = findById(contact.GetIndexB());

		// h为压力的大小
		float h = data.m_accumulationBuffer[aParticle->particleBufferIndex] + data.m_accumulationBuffer[bParticle->particleBufferIndex];
		if (h == 0.f)
		{
			// printf("+++++++++++++++node pressure %d %d w %f \n", data.m_accumulationBuffer[aParticle->particleBufferIndex],
			// 	data.m_accumulationBuffer[bParticle->particleBufferIndex],contact.GetWeight());
			continue;
		}
				
		// 根据权重，即穿透深度来调整压力大小
		float w = contact.GetWeight();

		// 在碰撞法线方向上应用这个压力
		const SqVec2 &n = contact.GetNormal();

		// 因为F=ma，所以a = F/ma，即 velocityPerPressure = 1/m * dt, F = w * h * n ，所以 f = velocityPerPressure * w * h * n
		SqVec2 f = velocityPerPressure * w * h * n;

		// 因为法线是从a指向b的，所以-f表示在反方向为a粒子添加排斥力
		aParticle->velocity -= f;
		bParticle->velocity += f;

		// printf("collision particle  %d %d f %f %f v %f w %f h %f n %f %f ac %f bc %f vA(%f %f) vB(%f %f)\n",
		// 	   contact.GetIndexA(), contact.GetIndexB(), f.x, f.y,
		// 	   velocityPerPressure, w, h, n.x, n.y,
		// 	   data.m_accumulationBuffer[aParticle->particleBufferIndex],
		// 	   data.m_accumulationBuffer[bParticle->particleBufferIndex],
		// 	   aParticle->velocity.x, aParticle->velocity.y,
		// 	   bParticle->velocity.x, bParticle->velocity.y);
	}
}

/**
 * 实现碰撞响应中的摩擦力。就在碰撞的两个物体的切线方向上应用摩擦力，从而阻止物体在切线方向上的运动
 * 因为是阻止运动，所有这里命名称为阻尼了
 *
 */
void SqParticleSystem::SolveDamping(const SqStepContext &step)
{
	// reduces normal velocity of each contact
	float linearDamping = m_def.dampingStrength;
	float quadraticDamping = 1 / GetCriticalVelocity(step);
	for (int k = 0; k < data.m_bodyContactBuffer.getCount(); k++)
	{
		const SqParticleBodyContact &contact = *data.m_bodyContactBuffer.get(k);

		if (contact.state == 0 || contact.shape->isSensor())
			continue;

		SqParticle *particle = findById(contact.particleId);
		float w = contact.weight;
		float m = contact.mass;
		SqVec2 n = contact.normal;
		const SqVec2 &p = particle->getPosition();
		SqBody *body = world->world->getBody(contact.bodyId);
		SqVec2 v = body->getLinearVelocityFromWorldPoint(p) - particle->velocity;
		float vn = SqVec2::Dot(v, n);

		SqVec2 vvv = body->getLinearVelocityFromWorldPoint(p);
		// printf("w %f %f vn %f vvv %f %f veloctiy %f %f \n", w, quadraticDamping, vn, vvv.x, vvv.y, particle->velocity.x, particle->velocity.y);
		// printf("normal %f %f \n", n.x, n.y);

		if (vn < 0)
		{
			float damping = max(linearDamping * w, min(-quadraticDamping * vn, 0.5f));
			SqVec2 f = damping * m * vn * n; // 切线方向应用冲量。即摩擦力
			particle->velocity += GetParticleInvMass() * f;
			if (body->isEnableParticleDamping())
				body->applyLinearImpulse(SqVec2::Neg(f), p, true);
		}
	}

	for (int k = 0; k < data.m_contactBuffer.getCount(); k++)
	{
		const SqParticleContact &contact = *data.m_contactBuffer.get(k);
		int a = contact.GetIndexA();
		int b = contact.GetIndexB();
		SqParticle *aParticle = findById(contact.GetIndexA());
		SqParticle *bParticle = findById(contact.GetIndexB());
		float w = contact.GetWeight();
		SqVec2 n = contact.GetNormal();
		SqVec2 v = bParticle->velocity - aParticle->velocity;
		float vn = SqVec2::Dot(v, n);
		if (vn < 0)
		{
			float damping = max(linearDamping * w, min(-quadraticDamping * vn, 0.5f));
			SqVec2 f = damping * vn * n;
			aParticle->velocity += f;
			bParticle->velocity -= f;
		}
	}
}

void SqParticleSystem::SolveRigidDamping()
{
	// Apply impulse to rigid particle groups colliding with other objects
	// to reduce relative velocity at the colliding point.
	float damping = m_def.dampingStrength;
	for (int k = 0; k < data.m_bodyContactBuffer.getCount(); k++)
	{
		const SqParticleBodyContact &contact = *data.m_bodyContactBuffer.get(k);
		if (contact.state == 0 || contact.shape->isSensor())
			continue;
		SqParticle *particle = findById(contact.particleId);
		SqParticleGroup *aGroup = particle->getGroup();
		if (IsRigidGroup(aGroup))
		{
			SqBody *body = world->world->getBody(contact.bodyId);
			SqVec2 n = contact.normal;
			float w = contact.weight;
			const SqVec2 &p = particle->getPosition();
			SqVec2 v = body->getLinearVelocityFromWorldPoint(p) - aGroup->GetLinearVelocityFromWorldPoint(p);
			float vn = SqVec2::Dot(v, n);
			if (vn < 0)
			// The group's average velocity at particle position 'p' is pushing
			// the particle into the body.
			{
				float invMassA, invInertiaA, tangentDistanceA;
				float invMassB, invInertiaB, tangentDistanceB;
				InitDampingParameterWithRigidGroupOrParticle(&invMassA, &invInertiaA, &tangentDistanceA, true, aGroup, particle->getId(), p, n);

				SqMassData massData = body->getMassData();

				InitDampingParameter(
					&invMassB, &invInertiaB, &tangentDistanceB,
					massData.mass,

					// Calculate b->m_I from public functions of b2Body.
					massData.rotationalInertia - massData.mass * SqVec2::Length(body->getLocalCenterOfMass()),
					massData.center,
					p, n);

				float f = damping * min(w, 1.0f) * ComputeDampingImpulse(invMassA, invInertiaA, tangentDistanceA, invMassB, invInertiaB, tangentDistanceB, vn);
				ApplyDamping(invMassA, invInertiaA, tangentDistanceA, true, aGroup, particle->getId(), f, n);
				body->applyLinearImpulse(-f * n, p, true);
			}
		}
	}
	for (int k = 0; k < data.m_contactBuffer.getCount(); k++)
	{
		const SqParticleContact &contact = *data.m_contactBuffer.get(k);
		SqParticle *aParticle = findById(contact.GetIndexA());
		SqParticle *bParticle = findById(contact.GetIndexB());
		SqVec2 n = contact.GetNormal();
		float w = contact.GetWeight();
		SqParticleGroup *aGroup = aParticle->getGroup();
		SqParticleGroup *bGroup = bParticle->getGroup();
		bool aRigid = IsRigidGroup(aGroup);
		bool bRigid = IsRigidGroup(bGroup);
		if (aGroup != bGroup && (aRigid || bRigid))
		{
			SqVec2 p = 0.5f * (aParticle->getPosition() + bParticle->getPosition());
			SqVec2 v = GetLinearVelocity(bGroup, bParticle->getId(), p) - GetLinearVelocity(aGroup, aParticle->getId(), p);
			float vn = SqVec2::Dot(v, n);
			if (vn < 0)
			{
				float invMassA, invInertiaA, tangentDistanceA;
				float invMassB, invInertiaB, tangentDistanceB;
				InitDampingParameterWithRigidGroupOrParticle(&invMassA, &invInertiaA, &tangentDistanceA, aRigid, aGroup, aParticle->getId(), p, n);
				InitDampingParameterWithRigidGroupOrParticle(&invMassB, &invInertiaB, &tangentDistanceB, bRigid, bGroup, bParticle->getId(), p, n);
				float f = damping * w * ComputeDampingImpulse(invMassA, invInertiaA, tangentDistanceA, invMassB, invInertiaB, tangentDistanceB, vn);
				ApplyDamping(invMassA, invInertiaA, tangentDistanceA, aRigid, aGroup, aParticle->getId(), f, n);
				ApplyDamping(invMassB, invInertiaB, tangentDistanceB, bRigid, bGroup, bParticle->getId(), -f, n);
			}
		}
	}
}

void SqParticleSystem::SolveExtraDamping()
{
	// Applies additional damping force between bodies and particles which can
	// produce strong repulsive force. Applying damping force multiple times
	// is effective in suppressing vibration.
	for (int k = 0; k < data.m_bodyContactBuffer.getCount(); k++)
	{
		const SqParticleBodyContact &contact = *data.m_bodyContactBuffer.get(k);
		if (contact.state == 0 || contact.shape->isSensor())
			continue;
		SqParticle *particle = findById(contact.particleId);
		if (particle->flags & k_extraDampingFlags)
		{
			float m = contact.mass;
			SqVec2 n = contact.normal;
			const SqVec2 &p = particle->getPosition();
			SqBody *body = world->world->getBody(contact.bodyId);
			SqVec2 v = body->getLinearVelocityFromWorldPoint(p) - particle->velocity;
			float vn = SqVec2::Dot(v, n);
			if (vn < 0)
			{
				SqVec2 f = 0.5f * m * vn * n;
				particle->velocity += GetParticleInvMass() * f;
				if (body->isEnableParticleDamping())
					body->applyLinearImpulse(-f, p, true);
			}
		}
	}
}

void SqParticleSystem::SolveWall()
{
	auto callback = [this](int particleId)
	{
		SqParticle *particle = findById(particleId);
		if (particle->flags & sq_wallParticle)
		{
			particle->velocity.zero();
		}
	};
	particles.iterate<int>(callback);
}

void SqParticleSystem::SolveRigid(const SqStepContext &step)
{
	/**
	 * 模拟粒子组成的软体。组作为粒子的边界
	 */

	SqTransform velocityTransform;
	SqTransform transform;
	auto callback = [this, &velocityTransform](int particleId)
	{
		SqParticle *particle = findById(particleId);
		particle->velocity = velocityTransform.transformPoint(particle->getPosition());
	};

	for (SqParticleGroup *group = m_groupList; group; group = group->GetNext())
	{
		if (group->m_groupFlags & b2_rigidParticleGroup)
		{
			group->UpdateStatistics();
			SqRot rotation(step.dt * group->m_angularVelocity);
			transform.p = group->m_center + step.dt * group->m_linearVelocity - SqRot::transformVector(rotation, group->m_center);
			transform.q = rotation;

			group->m_transform = SqTransform::Mul(transform, group->m_transform);
			velocityTransform.p.x = step.inv_dt * transform.p.x;
			velocityTransform.p.y = step.inv_dt * transform.p.y;
			velocityTransform.q.s = step.inv_dt * transform.q.s;
			velocityTransform.q.c = step.inv_dt * (transform.q.c - 1);
			group->particles.iterate<int>(callback);
		}
	}
}

void SqParticleSystem::SolveElastic(const SqStepContext &step)
{
	float elasticStrength = step.inv_dt * m_def.elasticStrength;
	for (int k = 0; k < data.m_triadBuffer.getCount(); k++)
	{
		const SqParticleTriad &triad = *data.m_triadBuffer.get(k);
		if (triad.flags & sq_elasticParticle)
		{
			SqParticle *aParticle = findById(triad.indexA);
			SqParticle *bParticle = findById(triad.indexB);
			SqParticle *cParticle = findById(triad.indexC);
			const SqVec2 &oa = triad.pa;
			const SqVec2 &ob = triad.pb;
			const SqVec2 &oc = triad.pc;
			SqVec2 pa = aParticle->getPosition();
			SqVec2 pb = bParticle->getPosition();
			SqVec2 pc = cParticle->getPosition();
			SqVec2 &va = aParticle->velocity;
			SqVec2 &vb = bParticle->velocity;
			SqVec2 &vc = cParticle->velocity;
			pa += step.dt * va;
			pb += step.dt * vb;
			pc += step.dt * vc;
			SqVec2 midPoint = (float)1 / 3 * (pa + pb + pc);
			pa -= midPoint;
			pb -= midPoint;
			pc -= midPoint;
			SqRot r;
			r.s = SqVec2::Cross(oa, pa) + SqVec2::Cross(ob, pb) + SqVec2::Cross(oc, pc);
			r.c = SqVec2::Dot(oa, pa) + SqVec2::Dot(ob, pb) + SqVec2::Dot(oc, pc);
			float r2 = r.s * r.s + r.c * r.c;
			float invR = sqInvSqrt(r2);
			r.s *= invR;
			r.c *= invR;
			float strength = elasticStrength * triad.strength;
			va += strength * (SqRot::transformVector(r, oa) - pa);
			vb += strength * (SqRot::transformVector(r, ob) - pb);
			vc += strength * (SqRot::transformVector(r, oc) - pc);
		}
	}
}

/**
 * 粒子的弹性效果
 */
void SqParticleSystem::SolveSpring(const SqStepContext &step)
{
	float springStrength = step.inv_dt * m_def.springStrength;
	for (int k = 0; k < data.m_pairBuffer.getCount(); k++)
	{
		const SqParticlePair &pair = *data.m_pairBuffer.get(k);
		if (pair.flags & sq_springParticle)
		{
			SqParticle *aParticle = findById(pair.indexA);
			SqParticle *bParticle = findById(pair.indexB);
			SqVec2 pa = aParticle->getPosition();
			SqVec2 pb = bParticle->getPosition();
			SqVec2 &va = aParticle->velocity;
			SqVec2 &vb = bParticle->velocity;
			pa += step.dt * va;
			pb += step.dt * vb;
			SqVec2 d = pb - pa;
			float r0 = pair.distance;
			float r1 = SqVec2::Length(d);
			float strength = springStrength * pair.strength;
			SqVec2 f = strength * (r0 - r1) / r1 * d;
			va -= f;
			vb += f;
		}
	}
}

/**
 * 通俗地说：所谓的拉伸效果，就是彼此靠近的粒子具有“吸附”效果，这就像我们生活中看到的水滴或油滴：
 *  1、水滴倾斜时会保持圆润；
	2、两个水珠靠近时会相互吸引
	3、液体表面会趋向于最小面积（球形）；
	4、拖动水面会看到像“橡皮筋”一样的拉扯回弹。
	如果没有这个 tensile 力，液体边缘会破碎、拉开；粒子像沙子一样散开，粘不起来；模拟出来更像气体、粉尘或颗粒集合，而不像液体。
 *  从更专业的物理角度来说，这个是模拟表面张力（Surface Tension）。
 *  // 表面张力模拟中的拉伸力模型（Tensile force model）：
 *
 *   F = [k_p * (w_a + w_b - 2) + k_n * dot(s_b - s_a, n)] * w * n
 *	 其中：
 *	 k_p：surfaceTensionPressureStrength * criticalVelocity（压力项系数）
 *	 k_n：surfaceTensionNormalStrength * criticalVelocity（法线项系数）
 *	 w  ：粒子接触权重（重叠程度）
 *	 n  ：粒子间接触法线（从b指向a）
 *	 s_a/s_b：由(1 - w) * w * n 累积得到的法线方向拉伸权值
 *	 h = w_a + w_b，粒子a和b的密度权重之和（表示该区域密度）
 *	 限制项：F 被截断为最大速度变化 maxVelocityVariation
 *
 *  此公式是 Box2D 原创的经验性表面张力模型，源自论文：
 * "Particle-based Viscoelastic Fluid Simulation" – Clavet et al. 2005 的思想，但做了进一步简化。
 *  https://www.ljll.fr/~frey/papers/levelsets/Clavet%20S.,%20Particle-based%20viscoelastic%20fluid%20simulation.pdf
 */
void SqParticleSystem::SolveTensile(const SqStepContext &step)
{
	SQ_ASSERT(data.m_accumulation2Buffer.data);
	data.resetAccumulation2Buffer();

	for (int k = 0; k < data.m_contactBuffer.getCount(); k++)
	{
		const SqParticleContact &contact = *data.m_contactBuffer.get(k);
		if (contact.GetFlags() & sq_tensileParticle)
		{
			SqParticle *aParticle = findById(contact.GetIndexA());
			SqParticle *bParticle = findById(contact.GetIndexB());
			float w = contact.GetWeight();
			SqVec2 n = contact.GetNormal();
			SqVec2 weightedNormal = (1 - w) * w * n;
			data.m_accumulation2Buffer.data[aParticle->particleBufferIndex] -= weightedNormal;
			data.m_accumulation2Buffer.data[bParticle->particleBufferIndex] += weightedNormal;
		}
	}

	float criticalVelocity = GetCriticalVelocity(step);
	float pressureStrength = m_def.surfaceTensionPressureStrength * criticalVelocity;
	float normalStrength = m_def.surfaceTensionNormalStrength * criticalVelocity;
	float maxVelocityVariation = sq_maxParticleForce * criticalVelocity;
	for (int k = 0; k < data.m_contactBuffer.getCount(); k++)
	{
		const SqParticleContact &contact = *data.m_contactBuffer.get(k);
		if (contact.GetFlags() & sq_tensileParticle)
		{
			SqParticle *aParticle = findById(contact.GetIndexA());
			SqParticle *bParticle = findById(contact.GetIndexB());
			float w = contact.GetWeight();
			SqVec2 n = contact.GetNormal();
			float h = aParticle->getDepth() + bParticle->getDepth();
			SqVec2 s = data.m_accumulation2Buffer.data[bParticle->particleBufferIndex] - data.m_accumulation2Buffer.data[aParticle->particleBufferIndex];
			float fn = min(pressureStrength * (h - 2) + normalStrength * SqVec2::Dot(s, n), maxVelocityVariation) * w;
			SqVec2 f = fn * n;
			aParticle->velocity -= f;
			bParticle->velocity += f;
		}
	}
}

/**
 * 计算水体的粘稠效果
 *
 * 在连续介质力学中，粘性力源于流体内部不同速度层之间的剪切应力，例如牛顿流体中的公式：
 * 	F = μ∇^2v
 * 1、μ 是流体的动力粘性系数（dynamic viscosity）
 * 2、∇^2 v 是速度场的拉普拉斯算子
 *
 * 当求解上面的公式太复杂了，这种公式属于偏微分方程，只能在网格模拟或有限元中使用，不适合粒子系统。
 *
 * 在像 Box2D 或 SPH（光滑粒子流体动力学）中，人们常使用下面这种经验形式的粘性力：
 * 加入权重的粘性力公式：
 * 	F_viscous = -k * m * w * v_rel
 * 其中：
		F_viscous：粘性阻力
		k         ：粘性强度系数（viscousStrength）
		m         ：接触质量（contact.mass）
		w         ：接触权重（contact.weight），控制距离越近粘性越大。可以去掉
		v_rel     : 是两个物体的当前的相对速度
  如果你在做流体模拟或者柔性体与刚体耦合，这种粘性阻尼是非常常见的处理方式。

  这些是工程经验模型，核心思想是：
	如果两个粒子或粒子-刚体之间速度差很大，就说明流体“剪切”剧烈，应该有粘性力阻止。
	越靠近（权重越大），力越大。
	使用简单的线性比例关系代替真实的微分方程计算。
 */
void SqParticleSystem::SolveViscous()
{
	float viscousStrength = m_def.viscousStrength;

	// 计算粒子与Box2D的粘性力
	for (int k = 0; k < data.m_bodyContactBuffer.getCount(); k++)
	{
		const SqParticleBodyContact &contact = *data.m_bodyContactBuffer.get(k);
		if (contact.state == 0 || contact.shape->isSensor())
			continue;
		SqParticle *particle = findById(contact.particleId);
		if (particle->flags & sq_viscousParticle)
		{
			float w = contact.weight;
			float m = contact.mass;
			SqVec2 &p = particle->getPosition();
			SqBody *body = world->world->getBody(contact.bodyId);
			SqVec2 v = body->getLinearVelocityFromWorldPoint(p) - particle->velocity;
			SqVec2 f = viscousStrength * m * w * v;
			particle->velocity += GetParticleInvMass() * f; // 力变成速度

			// 刚体也给点粘力
			body->applyLinearImpulse(-f, p, true);
		}
	}

	// 计算粒子与粒子的粘性力
	for (int k = 0; k < data.m_contactBuffer.getCount(); k++)
	{
		const SqParticleContact &contact = *data.m_contactBuffer.get(k);
		if (contact.GetFlags() & sq_viscousParticle)
		{
			SqParticle *aParticle = findById(contact.GetIndexA());
			SqParticle *bParticle = findById(contact.GetIndexB());
			float w = contact.GetWeight();
			SqVec2 v = bParticle->velocity - aParticle->velocity; // 计算由a指向b的速度，相当于谁的速度大就向谁方向运动，造成粘性效果
			SqVec2 f = viscousStrength * w * v;					  // 粒子之间不用考虑粒子的质量
			aParticle->velocity += f;							  // 速度由a指向b，a往b方向运动
			bParticle->velocity -= f;							  // b往a方向运动，这样他们两个相当于挤在一起了
		}
	}
}

/**
 * 这段代码是用于模拟粒子组之间的排斥力（repulsion force），主要在粒子属于不同 group 时才发生作用，即组与组编译粒子发生碰撞时分开的力度。
 *
 * 排斥力模型：F_repulsion = k * w * n
 * 其中：
	 k = repulsiveStrength，排斥强度，基于临界速度调整
	 w = contact weight，表示粒子间重叠程度
	 n = 接触法线方向，指向排斥方向（从b指向a）
 */
void SqParticleSystem::SolveRepulsive(const SqStepContext &step)
{
	float repulsiveStrength = m_def.repulsiveStrength * GetCriticalVelocity(step);
	for (int k = 0; k < data.m_contactBuffer.getCount(); k++)
	{
		const SqParticleContact &contact = *data.m_contactBuffer.get(k);
		if (contact.GetFlags() & sq_repulsiveParticle)
		{
			SqParticle *aParticle = findById(contact.GetIndexA());
			SqParticle *bParticle = findById(contact.GetIndexB());

			if (aParticle->getGroup() != bParticle->getGroup())
			{
				float w = contact.GetWeight();
				SqVec2 n = contact.GetNormal();
				SqVec2 f = repulsiveStrength * w * n;
				aParticle->velocity -= f;
				bParticle->velocity += f;
			}
		}
	}
}

/**
 * 计算粒子粉末的运动方式
 */
void SqParticleSystem::SolvePowder(const SqStepContext &step)
{
	float powderStrength = m_def.powderStrength * GetCriticalVelocity(step);
	float minWeight = 1.0f - sq_particleStride;
	for (int k = 0; k < data.m_contactBuffer.getCount(); k++)
	{
		const SqParticleContact &contact = *data.m_contactBuffer.get(k);
		if (contact.GetFlags() & sq_powderParticle)
		{
			float w = contact.GetWeight();
			if (w > minWeight)
			{
				SqParticle *aParticle = findById(contact.GetIndexA());
				SqParticle *bParticle = findById(contact.GetIndexB());
				SqVec2 n = contact.GetNormal();
				SqVec2 f = powderStrength * (w - minWeight) * n;
				aParticle->velocity -= f;
				bParticle->velocity += f;
			}
		}
	}
}

/**
 * 这个也是在多个粒子组中才由于的，用于控制两个粒子组直接的重叠程度
 * 当两个不同的固体粒子组靠得太近时，施加一股强烈的反向力将它们分开、弹开，避免交叉或穿透。
 *
 * SqVec2 f = ejectionStrength * h * w * n;
 * 可以理解为：
	h: 两个粒子的“重叠深度”之和，粒子压得越深，弹开的力越大；
	w: 权重（表示粒子接触紧密程度）；
	n: 接触法线（弹开方向）；
	ejectionStrength: 拉开强度，与 inv_dt 成正比，确保每帧的速度不受帧率影响。
 */
void SqParticleSystem::SolveSolid(const SqStepContext &step)
{
	// applies extra repulsive force from solid particle groups
	SQ_ASSERT(data.m_depthBuffer.data);
	float ejectionStrength = step.inv_dt * m_def.ejectionStrength;
	for (int k = 0; k < data.m_contactBuffer.getCount(); k++)
	{
		const SqParticleContact &contact = *data.m_contactBuffer.get(k);
		SqParticle *aParticle = findById(contact.GetIndexA());
		SqParticle *bParticle = findById(contact.GetIndexB());
		if (aParticle->getGroup() != bParticle->getGroup())
		{
			float w = contact.GetWeight();
			SqVec2 n = contact.GetNormal();
			float h = aParticle->getDepth() + bParticle->getDepth();
			SqVec2 f = ejectionStrength * h * w * n;
			aParticle->velocity -= f;
			bParticle->velocity += f;
		}
	}
}

/**
 * 应用外部设置的力
 * 即ApplyForce、ParticleApplyForce等函数设置的力
 */
void SqParticleSystem::SolveForce(const SqStepContext &step)
{

	// a = F/m * dt
	float velocityPerForce = step.dt * GetParticleInvMass();
	auto callback = [this, velocityPerForce](int particleId)
	{
		SqParticle *particle = findById(particleId);
		particle->velocity += velocityPerForce * particle->getForce();
	};
	particles.iterate<int>(callback);

	m_hasForce = false;
}

void SqParticleSystem::SolveColorMixing()
{
	// mixes color between contacting particles
	SQ_ASSERT(data.m_colorBuffer.data);
	const int colorMixing128 = (int)(128 * m_def.colorMixingStrength);
	if (colorMixing128)
	{
		for (int k = 0; k < data.m_contactBuffer.getCount(); k++)
		{
			const SqParticleContact &contact = *data.m_contactBuffer.get(k);
			SqParticle *aParticle = findById(contact.GetIndexA());
			SqParticle *bParticle = findById(contact.GetIndexB());
			if (aParticle->flags & bParticle->flags & sq_colorMixingParticle)
			{
				SqParticleColor &colorA = aParticle->getColor();
				SqParticleColor &colorB = bParticle->getColor();

				// Use the static method to ensure certain compilers inline
				// this correctly.
				SqParticleColor::MixColors(&colorA, &colorB, colorMixing128);
			}
		}
	}
}

/**
 * 销毁被标记为destroy的粒子
 */
void SqParticleSystem::SolveZombie()
{

	int *particleState = (int *)sqstd::StackTempArenaAllocator::getInstance()->allocateChunk(sizeof(int) * data.m_count);
	bool hasRemove = false;
	m_allParticleFlags = 0;

	auto callback = [this, particleState, &hasRemove](int particleId)
	{
		SqParticle *particle = findById(particleId);
		if (particle->flags & sq_zombieParticle)
		{
			hasRemove = true;
			b2DestructionListener *const destructionListener = world->m_destructionListener;
			if ((particle->flags & sq_destructionListenerParticle) && destructionListener)
			{
				// 有粒子销毁则执行告诉一下外部销毁了哪个粒子
				destructionListener->SayGoodbye(this, particleId);
			}

			SqParticleGroup *group = particle->getGroup();
			if (group)
			{
				group->removeParticle(particleId, false);
				if (group->GetParticleCount() > 0)
				{
					if (group->m_groupFlags & b2_solidParticleGroup && !(group->m_groupFlags & b2_particleGroupNeedsUpdateDepth))
					{
						SetGroupFlags(group, group->m_groupFlags | b2_particleGroupNeedsUpdateDepth);
					}
				}
				else
				{
					if (!(group->m_groupFlags & b2_particleGroupCanBeEmpty) && !(group->m_groupFlags & b2_particleGroupWillBeDestroyed))
					{
						SetGroupFlags(group, group->m_groupFlags | b2_particleGroupWillBeDestroyed);
					}
				}
			}

			particleState[particle->particleBufferIndex] = 0;
			poolPartilceIds.addValue(particleId);
			particles.removeValue(particleId);
		}
		else
		{
			particleState[particle->particleBufferIndex] = 1;
			m_allParticleFlags |= particle->flags;
		}
	};

	particles.iterate<int>(callback);

	if (hasRemove)
	{
		data.removeParticle(particleState);

		// 删除哪些没有粒子的组
		for (SqParticleGroup *group = m_groupList; group;)
		{
			SqParticleGroup *next = group->GetNext();
			if (group->m_groupFlags & b2_particleGroupWillBeDestroyed)
			{
				DestroyParticleGroup(group);
			}
			group = next;
		}

		grid.removeInvaildParticle();

		if (data.m_contactBuffer.getCount() > 0)
		{
			data.m_contactBuffer.removeIf(SqParticleContact::ContactIsZombie);
		}

		if (data.m_bodyContactBuffer.getCount() > 0)
		{
			data.m_bodyContactBuffer.removeIf(SqParticleBodyContact::bodyContactListRemoveInvalid);
		}

		if (data.m_triadBuffer.getCount() > 0)
		{
			data.m_triadBuffer.removeIf(SqParticleTriad::IsTriadInvalid);
		}

		if (data.m_pairBuffer.getCount() > 0)
		{
			data.m_pairBuffer.removeIf(SqParticlePair::IsPairInvalid);
		}
	}

	sqstd::StackTempArenaAllocator::getInstance()->freeChunk(particleState);
	m_needsUpdateAllParticleFlags = false;
}

/**
 * 如果开启了粒子超过了生命周期就可以销毁的功能，则这个函数就是处理销毁的逻辑
 * SetParticleLifetime()
 */
void SqParticleSystem::SolveLifetimes(const SqStepContext &step)
{

	SQ_ASSERT(data.m_expirationTimeBuffer.data);
	SQ_ASSERT(data.m_sortExpirationTimeBuffer.data);

	// Update the time elapsed.
	m_timeElapsed = LifetimeToExpirationTime(step.dt);

	// Get the floor (non-fractional component) of the elapsed time.
	// 相当于得到当前累计的运行的帧数
	const int quantizedTimeElapsed = GetQuantizedTimeElapsed();
	const int *const expirationTimes = data.m_expirationTimeBuffer.data;
	int *const expirationTimeIndices = data.m_sortExpirationTimeBuffer.data;
	const int particleCount = GetParticleCount();

	// 按照lifeTime对粒子进行排序，这样下面生命周期判断就不用遍历所有的粒子
	// 按照从大到小的顺序排列，永驻的粒子排在最前面,所以lifeTime越小越在后面
	if (m_expirationTimeBufferRequiresSorting)
	{
		const ExpirationTimeComparator expirationTimeComparator(expirationTimes);

		std::sort(expirationTimeIndices,
				  expirationTimeIndices + particleCount,
				  expirationTimeComparator);

		m_expirationTimeBufferRequiresSorting = false;

		// for (int i = particleCount - 1; i >= 0; --i)
		// {
		// 	const int particleIndex = expirationTimeIndices[i];
		// 	const int expirationTime = expirationTimes[particleIndex];
		// 	printf("SolveLifetimes id %d index %d time %d \n",data.findParticle(particleIndex), particleIndex, expirationTime);
		// }
	}

	// Destroy particles which have expired.
	for (int i = particleCount - 1; i >= 0; --i)
	{
		const int particleIndex = expirationTimeIndices[i];
		const int expirationTime = expirationTimes[particleIndex];

		// expirationTime <= 0 表示这个粒子是不受生命周期控制，永驻的。
		// 由于上面expirationTimeIndices是从小到大排序的，所以quantizedTimeElapsed < expirationTime 表示接下来的粒子都还没有到时间
		// 避免了遍历所有的粒子
		if (quantizedTimeElapsed < expirationTime || expirationTime <= 0)
		{
			break;
		}

		// printf("SolveLifetimes %d %d quantizedTimeElapsed %d \n", particleIndex, expirationTime, quantizedTimeElapsed);

		// Destroy this particle.
		DestroyParticle(data.findParticle(particleIndex), false);
	}
}

/// Set the lifetime (in seconds) of a particle relative to the current
/// time.
void SqParticleSystem::SetParticleLifetime(int particleId, float lifetime)
{
	SqParticle *particle = findById(particleId);
	SQ_ASSERT(particle);
	particle->setLifeTime(lifetime);
}

/// Get the lifetime (in seconds) of a particle relative to the current
/// time.
float SqParticleSystem::GetParticleLifetime(int particleId)
{
	SqParticle *particle = findById(particleId);
	SQ_ASSERT(particle);
	return particle->getLifetime();
}

/// Get the array of particle lifetimes indexed by particle index.
/// GetParticleCount() items are in the returned array.
const int *SqParticleSystem::GetExpirationTimeBuffer()
{
	return data.m_expirationTimeBuffer.data;
}

void SqParticleSystem::SetDestructionByAge(const bool enable)
{
	if (enable)
	{
		data.requestLifeTimeBuffer();
	}
	else
	{
		data.destroyLifeTimeBuffer();
	}
	m_def.destroyByAge = enable;
}

/// Get the time elapsed in b2ParticleSystemDef::lifetimeGranularity.
int SqParticleSystem::GetQuantizedTimeElapsed() const
{
	return (int)(m_timeElapsed >> 32);
}

/// Convert a lifetime in seconds to an expiration time.
int64 SqParticleSystem::LifetimeToExpirationTime(const float32 lifetime) const
{
	// lifetime / m_def.lifetimeGranularity 结果可能非常小，浮点数可能会丢失精度，造成0的结果
	// 这里是将浮点数乘以一个很大的整数，保留浮点数后面很多位，即将浮点后面的32位保存下来
	// 然后整个数据使用int64存储，在取的时候再往后面挪32,保证精度不会丢失
	return m_timeElapsed + (int64)((lifetime / m_def.lifetimeGranularity) * (float32)(1LL << 32));
}

void SqParticleSystem::SetGroupFlags(SqParticleGroup *group, unsigned int newFlags)
{
	unsigned int *oldFlags = &group->m_groupFlags;

	// 新的与旧的比较，没有b2_solidParticleGroup才是true
	if ((*oldFlags ^ newFlags) & b2_solidParticleGroup)
	{
		// If the b2_solidParticleGroup flag changed schedule depth update.
		newFlags |= b2_particleGroupNeedsUpdateDepth;
	}

	// 判断是否有新的flag加进来
	if (*oldFlags & ~newFlags)
	{
		// If any flags might be removed
		m_needsUpdateAllGroupFlags = true;
	}

	if (~m_allGroupFlags & newFlags)
	{
		// If any flags were added
		if (newFlags & b2_solidParticleGroup)
		{
			data.requestDepthBuffer();
		}
		m_allGroupFlags |= newFlags;
	}
	*oldFlags = newFlags;
}

inline bool SqParticleSystem::ForceCanBeApplied(unsigned int flags) const
{
	return !(flags & sq_wallParticle);
}

void SqParticleSystem::PrepareForceBuffer()
{
	if (!m_hasForce)
	{
		data.resetForceBuffer();
		m_hasForce = true;
	}
}

inline bool SqParticleSystem::IsRigidGroup(SqParticleGroup *group) const
{
	return group && (group->m_groupFlags & b2_rigidParticleGroup);
}

inline SqVec2 SqParticleSystem::GetLinearVelocity(SqParticleGroup *group, int particleId, const SqVec2 &point)
{
	if (IsRigidGroup(group))
	{
		return group->GetLinearVelocityFromWorldPoint(point);
	}
	else
	{
		return findById(particleId)->velocity;
	}
}

inline void SqParticleSystem::InitDampingParameter(
	float *invMass, float *invInertia, float *tangentDistance,
	float mass, float inertia, const SqVec2 &center,
	const SqVec2 &point, const SqVec2 &normal) const
{
	*invMass = mass > 0 ? 1 / mass : 0;
	*invInertia = inertia > 0 ? 1 / inertia : 0;
	*tangentDistance = SqVec2::Cross(point - center, normal);
}

inline void SqParticleSystem::InitDampingParameterWithRigidGroupOrParticle(
	float *invMass, float *invInertia, float *tangentDistance,
	bool isRigidGroup, SqParticleGroup *group, int particleIndex,
	const SqVec2 &point, const SqVec2 &normal)
{
	if (isRigidGroup)
	{
		InitDampingParameter(
			invMass, invInertia, tangentDistance,
			group->GetMass(), group->GetInertia(), group->GetCenter(),
			point, normal);
	}
	else
	{
		unsigned int flags = findById(particleIndex)->flags;
		InitDampingParameter(
			invMass, invInertia, tangentDistance,
			flags & sq_wallParticle ? 0 : GetParticleMass(), 0, point,
			point, normal);
	}
}

inline float SqParticleSystem::ComputeDampingImpulse(
	float invMassA, float invInertiaA, float tangentDistanceA,
	float invMassB, float invInertiaB, float tangentDistanceB,
	float normalVelocity) const
{
	float invMass =
		invMassA + invInertiaA * tangentDistanceA * tangentDistanceA +
		invMassB + invInertiaB * tangentDistanceB * tangentDistanceB;
	return invMass > 0 ? normalVelocity / invMass : 0;
}

inline void SqParticleSystem::ApplyDamping(
	float invMass, float invInertia, float tangentDistance,
	bool isRigidGroup, SqParticleGroup *group, int particleIndex,
	float impulse, const SqVec2 &normal)
{
	if (isRigidGroup)
	{
		group->m_linearVelocity += impulse * invMass * normal;
		group->m_angularVelocity += impulse * tangentDistance * invInertia;
	}
	else
	{
		findById(particleIndex)->velocity += impulse * invMass * normal;
	}
}

static inline bool IsSignificantForce(const SqVec2 &force)
{
	return force.x != 0 || force.y != 0;
}

/**
 * 可以对一个范围内的粒子应用一个力
 */
// void SqParticleSystem::ApplyForce(int firstIndex, int lastIndex, const SqVec2 &force)
// {
// 	// Ensure we're not trying to apply force to particles that can't move,
// 	// such as wall particles.
// #if B2_ASSERT_ENABLED
// 	unsigned int flags = 0;
// 	for (int i = firstIndex; i < lastIndex; i++)
// 	{
// 		flags |= m_flagsBuffer.data[i];
// 	}
// 	SQ_ASSERT(ForceCanBeApplied(flags));
// #endif

// 	// Early out if force does nothing (optimization).
// 	const SqVec2 distributedForce = force / (float)(lastIndex - firstIndex);
// 	if (IsSignificantForce(distributedForce))
// 	{
// 		PrepareForceBuffer();

// 		// Distribute the force over all the particles.
// 		for (int i = firstIndex; i < lastIndex; i++)
// 		{
// 			m_forceBuffer[i] += distributedForce;
// 		}
// 	}
// }

void SqParticleSystem::ParticleApplyForce(int particleId, const SqVec2 &force)
{
	data.requestForceBuffer();
	SqParticle *particle = findById(particleId);
	if (IsSignificantForce(force) && ForceCanBeApplied(particle->flags))
	{
		PrepareForceBuffer();
		particle->getForce() += force;
	}
}

// void SqParticleSystem::ApplyLinearImpulse(int firstIndex, int lastIndex, const SqVec2 &impulse)
// {
// 	const float numParticles = (float)(lastIndex - firstIndex);
// 	const float totalMass = numParticles * GetParticleMass();
// 	const SqVec2 velocityDelta = impulse / totalMass;
// 	for (int i = firstIndex; i < lastIndex; i++)
// 	{
// 		m_velocityBuffer.data[i] += velocityDelta;
// 	}
// }

void SqParticleSystem::QueryAABB(b2QueryCallback *callback, const SqAABB &aabb)
{
	grid.queryAABB(callback, aabb);
}

void SqParticleSystem::QueryShapeAABB(b2QueryCallback *callback,
									  const SqShape &shape,
									  const SqTransform &xf)
{
	SqAABB aabb = shape.computeShapeAABB(xf);
	QueryAABB(callback, aabb);
}

void SqParticleSystem::RayCast(b2RayCastCallback *callback, const SqVec2 &point1, const SqVec2 &point2)
{
	grid.rayCast(callback, point1, point2);
}

float SqParticleSystem::ComputeCollisionEnergy()
{
	float sum_v2 = 0;
	for (int k = 0; k < data.m_contactBuffer.getCount(); k++)
	{
		const phxy::SqParticleContact &contact = *data.m_contactBuffer.get(k);
		SqParticle *aParticle = findById(contact.GetIndexA());
		SqParticle *bParticle = findById(contact.GetIndexB());
		SqVec2 n = contact.GetNormal();
		SqVec2 v = bParticle->velocity - aParticle->velocity;
		float vn = SqVec2::Dot(v, n);
		if (vn < 0)
		{
			sum_v2 += vn * vn;
		}
	}
	return 0.5f * GetParticleMass() * sum_v2;
}

void SqParticleSystem::SetStuckThreshold(int steps)
{
	// m_stuckThreshold = steps;

	// if (steps > 0)
	// {
	// 	m_lastBodyContactStepBuffer.data = RequestBuffer(m_lastBodyContactStepBuffer.data);
	// 	m_bodyContactCountBuffer.data = RequestBuffer(m_bodyContactCountBuffer.data);
	// 	m_consecutiveContactStepsBuffer.data = RequestBuffer(m_consecutiveContactStepsBuffer.data);
	// }
}

#if LIQUIDFUN_EXTERNAL_LANGUAGE_API

SqParticleSystem::b2ExceptionType SqParticleSystem::IsBufCopyValid(
	int startIndex, int numParticles, int copySize, int bufSize) const
{
	const int maxNumParticles = GetParticleCount();

	// are we actually copying?
	if (copySize == 0)
	{
		return b2_noExceptions;
	}

	// is the index out of bounds?
	if (startIndex < 0 ||
		startIndex >= maxNumParticles ||
		numParticles < 0 ||
		numParticles + startIndex > maxNumParticles)
	{
		return b2_particleIndexOutOfBounds;
	}

	// are we copying within the boundaries?
	if (copySize > bufSize)
	{
		return b2_bufferTooSmall;
	}

	return b2_noExceptions;
}

#endif // LIQUIDFUN_EXTERNAL_LANGUAGE_API

unsigned int SqParticleSystem::GetParticleFlags(int id)
{
	SqParticle *particle = findById(id);
	SQ_ASSERT(particle);
	return particle->flags;
}

void SqParticleSystem::SetParticleFlags(int id, unsigned int newFlags)
{
	SqParticle *particle = findById(id);
	SQ_ASSERT(particle);

	unsigned int oldFlags = particle->flags;

	// 判断flag是否有改变
	if (oldFlags & ~newFlags)
	{
		// If any flags might be removed
		m_needsUpdateAllParticleFlags = true;
	}

	// 判断与全局的flag是否有改变
	if (~m_allParticleFlags & newFlags)
	{
		// If any flags were added
		if (newFlags & sq_tensileParticle)
		{
			data.requestAccumulation2Buffer();
		}

		if (newFlags & sq_colorMixingParticle)
		{
			data.requestColorBuffer();
		}

		m_allParticleFlags |= newFlags;
	}

	particle->flags = newFlags;
}

void SqParticleSystem::allocateParticle(int particleCount)
{
	if (data.getCapacity() < particleCount)
	{
		data.resize(particleCount);
	}

	if (partilceArray.getCapacity() < particleCount)
	{
		uint32_t blockIndex = particleCount / 64;
		poolPartilceIds.growBitSet(blockIndex);
		partilceArray.resize(particleCount);
		grid.cells.resize(particleCount);
	}
}
