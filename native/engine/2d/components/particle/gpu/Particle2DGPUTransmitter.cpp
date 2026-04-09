#include "Particle2DGPUTransmitter.h"


Particle2DGPUTransmitter::Particle2DGPUTransmitter()
: particlesGPU(nullptr)
{}

Particle2DGPUTransmitter::~Particle2DGPUTransmitter()
{
    delete[] particlesGPU;
    particlesGPU = nullptr;
}

void Particle2DGPUTransmitter::setAmount(int amout)
{
    if (define.amount == amout)
        return;
    Particle2DTransmitter::setAmount(amout);    

    if(particlesGPU){
        delete[] particlesGPU;
        particlesGPU = nullptr;
    }
    particlesGPU = new Particle2DGPUTexture[amout];
}

void Particle2DGPUTransmitter::onUpdateParticle(Particle2D* particle)
{
    Particle2DGPUTexture& gpu = particlesGPU[particle->index];
    gpu.flag = (float)particle->flag;
    gpu.lifetime_percent = particle->lifetime_percent;
}