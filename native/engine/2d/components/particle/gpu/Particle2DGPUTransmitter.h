#pragma once
#include "../Particle2DTransmitter.h"



struct Particle2DGPUTexture
{
    float flag;
    float lifetime_percent = 0.f;
    Vec2 position;
};

class Particle2DGPUTransmitter : public Particle2DTransmitter
{
private:
    Particle2DGPUTexture *particlesGPU;

protected:
    virtual void onUpdateParticle(Particle2D*);
    virtual void onEmitParticle(Particle2D*){};
public:
   Particle2DGPUTransmitter();
   virtual ~Particle2DGPUTransmitter();
   virtual void setAmount(int ammout);
   inline const Particle2DGPUTexture* getGPUParticles() { return particlesGPU; };
};
