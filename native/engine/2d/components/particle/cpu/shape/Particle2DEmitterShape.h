#pragma once
#include "../../../../../core/math/Vec2.h"
#include "../Particle2DCPUEmitterDefine.h"
#include "../../Particle2DEmitterDefine.h"

class Particle2DEmitterShape
{
public:
    virtual void emit(Particle2DCPUUpdate*,Particle2DEmitterDefine*) = 0;
    virtual ~Particle2DEmitterShape() = default;
};



