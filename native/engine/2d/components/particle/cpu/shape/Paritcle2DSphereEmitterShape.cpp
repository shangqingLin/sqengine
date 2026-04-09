#include "Paritcle2DSphereEmitterShape.h"
#include "../../common/math.h"
#include "../../../../../core/math/math.h"

void Paritcle2DSphereEmitterShape::emit(Particle2DCPUUpdate* particle ,Particle2DEmitterDefine* define)
{
    float radius = define->emssion_shape_radius * sqrt(rand_from_seed(particle->randomSeed));
    float theta = rand_from_seed(particle->randomSeed) * 2.0 * Math::PI;
    float x1 = radius * cos(theta);
    float y1 = radius * sin(theta);
    float p = rand_from_seed(particle->randomSeed);
    particle->position->x = Math::lerp(0., x1, p);
    particle->position->y = Math::lerp(0., y1, p);
}