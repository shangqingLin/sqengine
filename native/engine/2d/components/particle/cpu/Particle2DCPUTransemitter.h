#pragma once
#include "./Particle2DCPUEmitterDefine.h"



class Particle2DCPUTransemitter : public Particle2DTransmitter
{
private:
    Particle2DCPUUpdate* cpuParticles;
    Paritlce2DCPURenderData* renderData;
protected:
    virtual void onUpdateParticle(Particle2D*);    
    virtual void onEmitParticle(Particle2D*);
public:

    Particle2DCPUTransemitter();
    virtual ~Particle2DCPUTransemitter();
    virtual void setRandomSeed(float seed);
    virtual void setAmount(int amount);
    inline const Particle2DCPUUpdate* getParticleCPU() { return cpuParticles; };
    inline Paritlce2DCPURenderData* getRenderData(){ return renderData; };
};
