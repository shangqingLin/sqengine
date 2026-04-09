
#include "b2ParticleSystemContactListenerImp.h"
#include "./SqParticleSystemWorld.h"
#include "../../framework/PhysicsSystem.h"
#include "../../framework/component/ParticlePhysics2DComponent.h"
using namespace phxy;

b2ParticleSystemContactListenerImp::b2ParticleSystemContactListenerImp(SqParticleSystemWorld *world) : world(world)
{
}

void b2ParticleSystemContactListenerImp::BeginBodyContact(SqParticleSystem *particleSystem, SqParticleBodyContact *particleBodyContact)
{

	ParticleSystemWorldContantListenerRegisterMap::iterator it = world->contantListenerRegister.find(particleSystem);
	if (it != world->contantListenerRegister.end())
	{
		physics2d::ParticlePhysics2DComponent *component = (physics2d::ParticlePhysics2DComponent *)(it->second);
		component->onBeginContactBody(particleBodyContact);
	}
}

void b2ParticleSystemContactListenerImp::EndBodyContact(SqParticleSystem *particleSystem, SqParticleBodyContact *particleBodyContact)
{
	ParticleSystemWorldContantListenerRegisterMap::iterator it = world->contantListenerRegister.find(particleSystem);
	if (it != world->contantListenerRegister.end())
	{
		physics2d::ParticlePhysics2DComponent *component = (physics2d::ParticlePhysics2DComponent *)(it->second);
		component->onEndContactBody(particleBodyContact);
	}
}

void b2ParticleSystemContactListenerImp::UpdateBodyContact(SqParticleSystem *particleSystem, SqParticleBodyContact *particleBodyContact)
{
	ParticleSystemWorldContantListenerRegisterMap::iterator it = world->contantListenerRegister.find(particleSystem);
	if (it != world->contantListenerRegister.end())
	{
		physics2d::ParticlePhysics2DComponent *component = (physics2d::ParticlePhysics2DComponent *)(it->second);
		// if (component->listenerState & (1 << 1))
		// {
		// physics::PhysicsWorldB2 *gameBWorld = dynamic_cast<physics::PhysicsWorldB2 *>(physics::PhysicsSystem::getInstance()->getWorld());
		// physics::PhysicsContactListenerB2 &b2ContactListener = gameBWorld->getContactListener();
		// physics::ContactManifold *mainfold = b2ContactListener.findByDataAndShapeId(particleBodyContact->shapeId, particleBodyContact->particleId);
		// SQ_ASSERT(mainfold);
		// mainfold->localNormal.x = particleBodyContact->normal.x;
		// mainfold->localNormal.y = particleBodyContact->normal.y;
		// }
	}
}

void b2ParticleSystemContactListenerImp::BeginParticleContact(SqParticleSystem *particleSystem, SqParticleContact *particleContact)
{
	ParticleSystemWorldContantListenerRegisterMap::iterator it = world->contantListenerRegister.find(particleSystem);
	if (it != world->contantListenerRegister.end())
	{
		physics2d::ParticlePhysics2DComponent *component = (physics2d::ParticlePhysics2DComponent *)(it->second);
		component->onBeginContactParticle(particleContact);
	}
}

void b2ParticleSystemContactListenerImp::EndParticleContact(SqParticleSystem *particleSystem, int indexA, int indexB)
{
	ParticleSystemWorldContantListenerRegisterMap::iterator it = world->contantListenerRegister.find(particleSystem);
	if (it != world->contantListenerRegister.end())
	{
		physics2d::ParticlePhysics2DComponent *component = (physics2d::ParticlePhysics2DComponent *)(it->second);
		component->onEndContactParticle(indexA, indexB);
	}
}
