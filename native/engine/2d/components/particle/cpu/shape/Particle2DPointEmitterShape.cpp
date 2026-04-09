#include "Particle2DPointEmitterShape.h"

void Particle2DPointEmitterShape::emit(Particle2DCPUUpdate* particle,Particle2DEmitterDefine* define)
{
    particle->position->x = 0.f;
    particle->position->y = 0.f;
}