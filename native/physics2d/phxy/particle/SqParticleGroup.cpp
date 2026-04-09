
#include "SqParticleGroup.h"
#include "SqParticleSystem.h"
#include "SqParticleSystemWorld.h"

using namespace phxy;

SqParticleGroup::SqParticleGroup()
{

	m_system = NULL;
	m_groupFlags = 0;
	m_strength = 1.0f;
	m_prev = NULL;
	m_next = NULL;

	m_timestamp = -1;
	m_mass = 0;
	m_inertia = 0;
	m_angularVelocity = 0;
	m_userData = NULL;
}

unsigned int SqParticleGroup::GetAllParticleFlags() const
{
	unsigned int flags = 0;
	// SqParticle *particle = firstParticle;
	// while (particle)
	// {
	// 	flags |= particle->flags;
	// 	if (particle == lastParticle)
	// 	{
	// 		break;
	// 	}
	// 	particle = particle->next;
	// }
	return flags;
}

void SqParticleGroup::SetGroupFlags(unsigned int flags)
{
	SQ_ASSERT((flags & b2_particleGroupInternalMask) == 0);
	flags |= m_groupFlags & b2_particleGroupInternalMask;
	m_system->SetGroupFlags(this, flags);
}

void SqParticleGroup::UpdateStatistics() const
{
	if (m_timestamp != m_system->m_timestamp)
	{
		float m = m_system->GetParticleMass();
		m_mass = 0;
		m_center.x = 0.f;
		m_center.y = 0.f;
		m_linearVelocity.x = 0.f;
		m_linearVelocity.y = 0.f;

		{
			auto callback = [this, m](int particleId)
			{
				SqParticle *particle = m_system->findById(particleId);
				m_mass += m;
				m_center += m * particle->getPosition();
				m_linearVelocity += m * particle->velocity;
			};
			particles.iterate<int>(callback);
		}

		if (m_mass > 0)
		{
			m_center *= 1 / m_mass;
			m_linearVelocity *= 1 / m_mass;
		}
		m_inertia = 0;
		m_angularVelocity = 0;

		{
			auto callback = [this, m](int particleId)
			{
				SqParticle *particle = m_system->findById(particleId);
				SqVec2 p = particle->getPosition() - m_center;
				SqVec2 v = particle->velocity - m_linearVelocity;
				m_inertia += m * SqVec2::Dot(p, p);
				m_angularVelocity += m * SqVec2::Cross(p, v);
			};
			particles.iterate<int>(callback);
		}

		if (m_inertia > 0)
		{
			m_angularVelocity *= 1 / m_inertia;
		}
		m_timestamp = m_system->m_timestamp;
	}
}

void SqParticleGroup::ApplyForce(const SqVec2 &force)
{
	// m_system->ApplyForce(m_firstIndex, m_lastIndex, force);
}

void SqParticleGroup::ApplyLinearImpulse(const SqVec2 &impulse)
{
	// m_system->ApplyLinearImpulse(m_firstIndex, m_lastIndex, impulse);
}

void SqParticleGroup::addParticle(int particleId)
{
	SqParticle *particle = m_system->findById(particleId);
	particle->setGroup(this);
	++particleCount;
	particles.addValue(particleId);
}

void SqParticleGroup::addParticles(SqBitSet &otherBitSet)
{
	SqBitSet::inPlaceUnion(&particles, &otherBitSet);
}

void SqParticleGroup::removeAllParticle()
{
	auto callback = [this](int particleId)
	{
		m_system->findById(particleId)->setGroup(nullptr);
		return true;
	};
	particles.iterate<int>(callback);
	particleCount = 0;
	particles.clearValue();
}

void SqParticleGroup::removeParticle(int particleId, bool removeFromSystem)
{
	SqParticle *particle = m_system->findById(particleId);

	if (particle->getGroup() == this)
	{
		particle->setGroup(nullptr);
		--particleCount;
		if (removeFromSystem)
		{
			m_system->particles.removeValue(particleId);
		}
	}
}

void SqParticleGroup::DestroyParticles(bool callDestructionListener)
{

	auto callback = [this, callDestructionListener](int particleId)
	{
		m_system->DestroyParticle(particleId, callDestructionListener);
	};
	particles.iterate<int>(callback);
	particleCount = 0;
	particles.clearValue();
}

int SqParticleGroup::GetParticleCount() const
{
	return particleCount;
}

bool SqParticleGroup::ContainsParticle(int paramParticleId) const
{

	bool b = false;
	auto callback = [&b, paramParticleId](int particleId)
	{
		if (particleId == paramParticleId)
		{
			b = true;
			return false;
		}
		return true;
	};
	particles.iterate<int>(callback);
	return b;
}

SqParticleGroup::~SqParticleGroup()
{
}

#if LIQUIDFUN_EXTERNAL_LANGUAGE_API
void SqParticleGroupDef::FreeShapesMemory()
{
	if (circleShapes)
	{
		delete[] circleShapes;
		circleShapes = NULL;
	}
	if (ownShapesArray && shapes)
	{
		delete[] shapes;
		shapes = NULL;
		ownShapesArray = false;
	}
}

void SqParticleGroupDef::SetCircleShapesFromVertexList(void *inBuf,
													   int numShapes,
													   float radius)
{
	float *points = (float *)inBuf;
	// Create circle shapes from vertex list and radius
	b2CircleShape *pCircleShapes = new b2CircleShape[numShapes];
	b2Shape **pShapes = new b2Shape *[numShapes];
	for (int i = 0; i < numShapes; ++i)
	{
		pCircleShapes[i].m_radius = radius;
		pCircleShapes[i].m_p = SqVec2(points[i * 2], points[i * 2 + 1]);
		pShapes[i] = &pCircleShapes[i];
	}

	// Clean up existing buffers
	FreeShapesMemory();

	// Assign to newly created buffers
	ownShapesArray = true;
	circleShapes = pCircleShapes;
	shapes = pShapes;
	shapeCount = numShapes;
}
#endif // LIQUIDFUN_EXTERNAL_LANGUAGE_API
