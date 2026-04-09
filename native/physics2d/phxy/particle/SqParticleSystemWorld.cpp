#include "SqParticleSystemWorld.h"
#include "../common/SqCore.h"
#include "../../../engine/core/core.h"
#include "./b2ParticleSystemContactListenerImp.h"

using namespace phxy;

static phxy::SqParticleSystemWorld *inst = nullptr;

SqParticleSystemWorld::SqParticleSystemWorld(SqWorld *world) : world(world),
															   m_particleSystemList(nullptr),
															   m_destructionListener(nullptr),
															   //   m_contactFilter(nullptr),
															   m_contactListener(nullptr)
{
	inst = this;
	world->setParticleSystem(this);
}

SqParticleSystem *SqParticleSystemWorld::GetParticleSystemList()
{
	return m_particleSystemList;
}

SqParticleSystem *SqParticleSystemWorld::CreateParticleSystem(const b2ParticleSystemDef *def)
{
	SQ_ASSERT(world->lock == false);
	if (world->lock)
	{
		return NULL;
	}

	SqParticleSystem *p = new SqParticleSystem(def, this);

	// 使用双向链表的方式存储所有的System
	p->m_prev = NULL;
	p->m_next = m_particleSystemList;
	if (m_particleSystemList)
	{
		m_particleSystemList->m_prev = p;
	}
	m_particleSystemList = p;

	return p;
}

void SqParticleSystemWorld::QueryAABB(b2QueryCallback *callback, const SqAABB &aabb) const
{

	for (SqParticleSystem *p = m_particleSystemList; p; p = p->GetNext())
	{
		if (callback->ShouldQueryParticleSystem(p))
		{
			p->QueryAABB(callback, aabb);
		}
	}
}

void SqParticleSystemWorld::step(SqStepContext &context)
{
	for (SqParticleSystem *p = m_particleSystemList; p; p = p->GetNext())
	{
		p->Solve(context);
	}
}

void SqParticleSystemWorld::DestroyParticleSystem(SqParticleSystem *p)
{
	SQ_ASSERT(m_particleSystemList != NULL);
	SQ_ASSERT(world->lock == false);
	if (world->lock)
	{
		return;
	}

	// Remove world particleSystem list.
	if (p->m_prev)
	{
		p->m_prev->m_next = p->m_next;
	}

	if (p->m_next)
	{
		p->m_next->m_prev = p->m_prev;
	}

	if (p == m_particleSystemList)
	{
		m_particleSystemList = p->m_next;
	}

	delete p;

	// p->~SqParticleSystem();
	// m_blockAllocator.Free(p, sizeof(SqParticleSystem));
}

void SqParticleSystemWorld::registerContactListener(SqParticleSystem *p, void *userData)
{
	if (!m_contactListener)
	{
		m_contactListener = new phxy::b2ParticleSystemContactListenerImp(this);
	}
	ParticleSystemWorldContantListenerRegisterMap::iterator it = contantListenerRegister.find(p);
	if (it == contantListenerRegister.end())
	{
		contantListenerRegister[p] = userData;
	}
}

void SqParticleSystemWorld::removeContantListener(SqParticleSystem *p)
{
	if (!m_contactListener)
		return;

	ParticleSystemWorldContantListenerRegisterMap::iterator it = contantListenerRegister.find(p);
	if (it == contantListenerRegister.end())
		return;
	contantListenerRegister.erase(it);
}