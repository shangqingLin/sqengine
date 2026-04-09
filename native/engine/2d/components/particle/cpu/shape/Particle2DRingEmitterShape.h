#pragma once
#include "Particle2DEmitterShape.h"

class Particle2DRingEmitterShape : public Particle2DEmitterShape
{
public:
    virtual void emit(Particle2DCPUUpdate*,Particle2DEmitterDefine* define);
};

