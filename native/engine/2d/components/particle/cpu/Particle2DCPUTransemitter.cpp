
#include "Particle2DCPUTransemitter.h"
#include "../common/math.h"

    Particle2DCPUTransemitter::Particle2DCPUTransemitter()
    :cpuParticles(nullptr),renderData(nullptr)
    {}

    Particle2DCPUTransemitter::~Particle2DCPUTransemitter()
    {
        delete[] cpuParticles;
        cpuParticles = nullptr;

        delete renderData;
        renderData = nullptr;
    }

    void Particle2DCPUTransemitter::setRandomSeed(float seed)
    {
        if(define.randomSeed == seed) return;
        Particle2DTransmitter::setRandomSeed(seed);

        if(particles){
            for (size_t i = 0; i < define.amount; ++i)
            {
                Particle2D& particle = particles[i];
                cpuParticles[i].initSeed = hash(particle.index + 1u + define.randomSeed);
            }
        }
    }

    void Particle2DCPUTransemitter::setAmount(int amount)
    {

        if (define.amount == amount)
        return;
        
        Particle2DTransmitter::setAmount(amount);    

        if(cpuParticles){
            delete[] cpuParticles;
        }
        cpuParticles = new Particle2DCPUUpdate[amount];
        renderData = new Paritlce2DCPURenderData[amount];
        for (size_t i = 0; i < amount; i++)
        {
            Particle2D& particle = particles[i];
            cpuParticles[i].particle = &particle;
            cpuParticles[i].position = &(renderData[i].particlePosition);
            cpuParticles[i].lifeTimePercent = &(renderData[i].lifeTimePercent);
            if(define.randomSeed != -1.f){
                cpuParticles[i].initSeed = hash(particle.index + 1u + define.randomSeed);
            }
        }
}
void Particle2DCPUTransemitter::onEmitParticle(Particle2D*){}
void Particle2DCPUTransemitter::onUpdateParticle(Particle2D* particle)
{
    *(cpuParticles[particle->index].lifeTimePercent) = particle->lifetime_percent;
}