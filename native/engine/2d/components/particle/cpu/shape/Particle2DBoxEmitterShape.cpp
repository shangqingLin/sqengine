#include "Particle2DBoxEmitterShape.h"
#include "../../common/math.h"

void Particle2DBoxEmitterShape::emit(Particle2DCPUUpdate* particle,Particle2DEmitterDefine* define)
{
    particle->position->x = rand_from_seed(particle->randomSeed) * 2.0 - 1.0;
    particle->position->y = rand_from_seed(particle->randomSeed) * 2.0 - 1.0;
    particle->position->x *= define->emssion_shape_radius;
}