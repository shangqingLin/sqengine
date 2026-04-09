
#include "SqParticle.h"
#include "../common/math/SqMath.h"
#include "../../../engine/core/core.h"
#include "SqParticleSystem.h"

using namespace phxy;

int phxy::sqCalculateParticleIterations(float gravity, float radius, float timeStep)
{
	// In some situations you may want more particle iterations than this,
	// but to avoid excessive cycle cost, don't recommend more than this.
	const int B2_MAX_RECOMMENDED_PARTICLE_ITERATIONS = 8;
	const float B2_RADIUS_THRESHOLD = 0.01f;
	int iterations = (int)ceilf(sqrt(gravity / (B2_RADIUS_THRESHOLD * radius)) * timeStep);
	return clamp(iterations, 1, B2_MAX_RECOMMENDED_PARTICLE_ITERATIONS);
}

SqParticle::SqParticle() : id(-1)
{
	reset();
	// printf("create particle \n");
}

void SqParticle::reset()
{
	flags = 0;
	particleBufferIndex = -1;
	velocity.zero();
}


void SqParticle::requestSimData()
{
	int index = system->data.addParticle(id);
	particleBufferIndex = index;
}

void SqParticle::setColor(const SqParticleColor &color)
{
	system->data.m_colorBuffer.data[particleBufferIndex] = color;
}

SqVec2 &SqParticle::getPosition() const
{
	return system->data.m_positionBuffer[particleBufferIndex];
}

void SqParticle::setPosition(const SqVec2 &pos)
{
	system->data.m_positionBuffer[particleBufferIndex] = pos;
}

void SqParticle::setUserData(void *userData)
{
	system->data.requestUserDataBuffer();
	system->data.m_userDataBuffer.data[particleBufferIndex] = userData;
}

void *SqParticle::getUserData()
{
	if (system->data.m_userDataBuffer.data)
	{
		return system->data.m_userDataBuffer.data[particleBufferIndex];
	}
	return nullptr;
}

void SqParticle::setGroup(SqParticleGroup *group)
{
	system->data.requestGroupBuffer();
	system->data.m_groupBuffer.data[particleBufferIndex] = group;
}

SqParticleGroup *SqParticle::getGroup()
{
	return system->data.m_groupBuffer.data ? system->data.m_groupBuffer.data[particleBufferIndex] : nullptr;
}

float SqParticle::getDepth()
{
	return system->data.m_depthBuffer.data ? system->data.m_depthBuffer.data[particleBufferIndex] : 0.f;
}

float SqParticle::getWeight()
{
	return system->data.m_weightBuffer[particleBufferIndex];
}

SqParticleColor &SqParticle::getColor()
{
	return system->data.m_colorBuffer.data[particleBufferIndex];
}

SqVec2 &SqParticle::getForce()
{
	return system->data.m_forceBuffer.data[particleBufferIndex];
}

/**
 * 如果lifetime <= 0 表示粒子不受生命周期控制，是永驻的。
 */
void SqParticle::setLifeTime(float lifetime)
{
	system->data.requestLifeTimeBuffer();

	// 将时长转换为帧数
	const int quantizedLifetime = (int)(lifetime / system->m_def.lifetimeGranularity);

	// printf("setLifeTime %d qtime %d et %d \n",id,quantizedLifetime,system->GetQuantizedTimeElapsed());
	/**
	 * 如果newExpirationTime <= 0 表示粒子不受生命周期控制，是永驻的。
	 */
	const int newExpirationTime = quantizedLifetime > 0 ? system->GetQuantizedTimeElapsed() + quantizedLifetime : quantizedLifetime;

	if (system->data.m_expirationTimeBuffer.data[particleBufferIndex] != newExpirationTime)
	{
		system->data.m_expirationTimeBuffer.data[particleBufferIndex] = newExpirationTime;
		system->m_expirationTimeBufferRequiresSorting = true;
	}
}

float SqParticle::getLifetime()
{
	return ExpirationTimeToLifetime(system->data.m_expirationTimeBuffer.data[particleBufferIndex]);
}

/**
 * 将生命周期的帧数表示转换为浮点数表示
 */
float SqParticle::ExpirationTimeToLifetime(const int expirationTime) const
{
	return (float)(expirationTime > 0 ? expirationTime - system->GetQuantizedTimeElapsed() : expirationTime) * system->m_def.lifetimeGranularity;
}
