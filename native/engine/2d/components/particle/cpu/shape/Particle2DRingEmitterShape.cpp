#include "Particle2DRingEmitterShape.h"
#include "../../../../../core/math/math.h"
#include "../../common/math.h"

void Particle2DRingEmitterShape::emit(Particle2DCPUUpdate* particle ,Particle2DEmitterDefine* define)
{
    float emission_ring_radius = define->emssion_shape_radius;
    float radiusClamped = Math::max(0.001f, emission_ring_radius);
    float angle = 90.f - define->emission_ring_cone_angle;
    float topRadius = angle != 90. && angle != 180. ? Math::max(radiusClamped - tan(Math::angleToRadian(angle)) * define->emission_ring_height, 0.f) : 0.0f;

    float y_pos = rand_from_seed(particle->randomSeed);
    float skew = Math::max( Math::min(radiusClamped, topRadius) / Math::max(radiusClamped, topRadius), 0.5f);
    y_pos = radiusClamped < topRadius ? pow(y_pos, skew) : 1.0 - pow(y_pos, skew);

    float ringRandomRadius = sqrt(rand_from_seed(particle->randomSeed) * (radiusClamped * radiusClamped 
            - define->emission_ring_inner_radius * define->emission_ring_inner_radius) + 
            define->emission_ring_inner_radius * define->emission_ring_inner_radius);

    ringRandomRadius = Math::lerp(ringRandomRadius, ringRandomRadius * (topRadius / radiusClamped), y_pos);

    Vec2 axis;
    if(define->emission_ring_axis == Vec2(0.0f)) {
        axis = Vec2(0.f, 1.0f);
    } else{
        Vec2::normalize(define->emission_ring_axis,axis);
    }

    Vec2 vertical_axis(-axis.y, axis.x);
    vertical_axis.normalize();

    *particle->position = vertical_axis * ringRandomRadius +
         axis *  (y_pos * define->emission_ring_height - define->emission_ring_height / 2.0);
}

