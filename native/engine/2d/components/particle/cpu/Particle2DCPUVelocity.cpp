#include "Particle2DCPUVelocity.h"
#include "../../../../core/math/math.h"
#include "../common/math.h"

static Vec2 get_random_direction_from_spread(Particle2DCPUUpdate *particle, Particle2DEmitterDefine *define, float spreadAngle, const Vec2 &dynamicDirection)
{

    float angle1_rad = 0.f;
    if (spreadAngle != 0.f)
    {
        float spread_rad = Math::angleToRadian(spreadAngle);
        angle1_rad = rand_from_seed(particle->randomSeed) * spread_rad;
    }

    angle1_rad += define->spreadDirection.x != 0.0 ? Math::atan2(define->spreadDirection.y, define->spreadDirection.x) : Math::sign(define->spreadDirection.y) * (Math::PI * 0.5);
    angle1_rad += dynamicDirection.lengthSqr() > 0. ? Math::atan2(dynamicDirection.y, dynamicDirection.x) : 0.;

    Vec2 velocity;
    if (angle1_rad != 0.f)
    {
        velocity.x = cos(angle1_rad);
        velocity.y = sin(angle1_rad);
        velocity.normalize();
    }

    return velocity;
}

static Vec2 process_orbit_displacement(Particle2DCPUUpdate *particle, Particle2DEmitterDefine *define, const DynamicsParameters &dynamicsParam,
                                       const Vec2 &position, const Vec2 &emission_position, float delta)
{
    if (abs(dynamicsParam.orbit_velocity) < 0.01 || delta < 0.001)
    {
        return Vec2(0.0);
    }

    Vec2 displacement;
    float orbit_amount = dynamicsParam.orbit_velocity;
    if (orbit_amount != 0.0)
    {
        Vec2 diff = position - emission_position;

        float ang = orbit_amount * Math::PI * 2.0 * delta;
        float cos = Math::cos(ang);
        float sin = Math::sin(ang);

        Vec2 rot;
        rot.x = diff.x * cos + diff.y * -sin;
        rot.y = diff.x * sin + diff.y * cos;
        Vec2 newOr = rot * diff;
        displacement = newOr - diff;
    }

    // 位移除以时间等于速度
    return displacement / delta;
}

static Vec2 process_radial_displacement(Particle2DCPUUpdate *particle, Particle2DEmitterDefine *define, const DynamicsParameters &dynamicsParam,
                                        const Vec2 &position, float delta)
{
    Vec2 radial_displacement;
    if (delta < 0.001)
    {
        return radial_displacement;
    }

    float radial_displacement_multiplier = 1.0;
    Vec2 direction = position - define->velocityPivot;

    if (dynamicsParam.radial_velocity != 0.f)
    {
        if (direction.lengthSqr() > 0.0001)
        {
            Vec2::normalize(direction, radial_displacement);
            radial_displacement *= radial_displacement_multiplier * dynamicsParam.radial_velocity;
        }
        else
        {
            radial_displacement = get_random_direction_from_spread(particle, define, 360.0, Vec2()) * dynamicsParam.radial_velocity;
        }
    }

    if (radial_displacement_multiplier * dynamicsParam.radial_velocity < 0.0)
    {
        radial_displacement.normalize();
        radial_displacement *= Math::min(abs(radial_displacement_multiplier * dynamicsParam.radial_velocity), (direction / delta).len());
    }

    // printf("radial_displacement %f %f %f \n", radial_displacement.x, radial_displacement.y, dynamicsParam.radial_velocity);
    return radial_displacement;
}

static void process_physical(Particle2DCPUUpdate *particle, Particle2DEmitterDefine *define,
                             const PhysicalParameters &physicsParam, float delta, Vec2 &velocity, const Vec2 &pos, const Vec2 &emitter_pos)
{

    Vec2 temp;
    Vec2 force = define->gravity;

    if (velocity.lengthSqr() > 0.f)
    {
        Vec2::normalize(velocity, temp);
        force += temp * physicsParam.linear_accel;
    }

    Vec2 diff = pos - emitter_pos;

    if (diff.lengthSqr() > 0.f)
    {
        Vec2::normalize(diff, temp);
        force += temp * physicsParam.radial_accel;
    }

    float tangent_accel_val = physicsParam.tangent_accel;

    {
        Vec2 vertical(diff.y, diff.x);
        if (vertical.lengthSqr() > 0.0f)
        {
            Vec2::normalize(vertical * Vec2(-1.0, 1.0), temp);
            force += temp * tangent_accel_val;
        }
    }

    velocity += force * delta;

    if (physicsParam.damping > 0.0)
    {
        float v = velocity.len();
        // if(!particle_flags[PARTICLE_FLAG_DAMPING_AS_FRICTION]) {
        //     v -= physicsParam.damping * delta;
        // } else {
        if (v > 0.001)
        {
            float damp = v * v * physicsParam.damping * 0.05 * delta;
            v -= damp;
        }
        // }

        if (v < 0.0)
        {
            velocity = Vec2(0.0);
        }
        else
        {
            Vec2::normalize(velocity, temp);
            velocity = temp * v;
        }
    }
}

static void calculate_initial_dynamics_params(Particle2DCPUUpdate *particle, DynamicsParameters &dynamicsParam, Particle2DEmitterDefine *define)
{
    dynamicsParam.angular_velocity = Math::lerp(define->angular_velocity_min, define->angular_velocity_max, rand_from_seed(particle->randomSeed));
    dynamicsParam.initial_velocity_multiplier = Math::lerp(define->initEmitVelocityMinMuti, define->initEmitVelocityMaxMuti, rand_from_seed(particle->randomSeed));
    dynamicsParam.directional_velocity = Math::lerp(define->directional_velocity_min, define->directional_velocity_max, rand_from_seed(particle->randomSeed));
    dynamicsParam.radial_velocity = Math::lerp(define->radial_velocity_min, define->radial_velocity_max, rand_from_seed(particle->randomSeed));
    dynamicsParam.orbit_velocity = Math::lerp(define->orbit_velocity_min, define->orbit_velocity_max, rand_from_seed(particle->randomSeed));
    dynamicsParam.turb_influence = Math::lerp(define->turbulence_influence_min, define->turbulence_influence_max, rand_from_seed(particle->randomSeed));
}

static void calculate_initial_physical_params(Particle2DCPUUpdate *particle, PhysicalParameters &physicsParam, Particle2DEmitterDefine *define)
{
    physicsParam.linear_accel = Math::lerp(define->linear_accel_min, define->linear_accel_max, rand_from_seed(particle->randomSeed));
    physicsParam.radial_accel = Math::lerp(define->radial_accel_min, define->radial_accel_max, rand_from_seed(particle->randomSeed));
    physicsParam.tangent_accel = Math::lerp(define->tangent_accel_min, define->tangent_accel_max, rand_from_seed(particle->randomSeed));
    physicsParam.damping = Math::lerp(define->damping_min, define->damping_max, rand_from_seed(particle->randomSeed));
}

void Particle2DCPUVelocity::updateVelocity(float dt, Particle2DCPUUpdate *particle, Particle2DEmitterDefine *define)
{
    PhysicalParameters physicsParam;
    DynamicsParameters dynamicsParam;
    calculate_initial_physical_params(particle, physicsParam, define);
    calculate_initial_dynamics_params(particle, dynamicsParam, define);
    dt *= 0.001f;

    if (particle->particle->flag & toNumber(ParticleFlag::PARTICLE_FLAG_STARTED))
    {
        Vec2 velocityDirection = get_random_direction_from_spread(particle, define, define->spreadAngle, particle->shapeEmitDirection);
        Vec2 newVelocity = velocityDirection * dynamicsParam.initial_velocity_multiplier;
        newVelocity += particle->velocity * define->inheritEmitterVelocityRatio;
        particle->velocity = newVelocity;
    }

    process_physical(particle, define, physicsParam, dt, particle->velocity, *particle->position, Vec2());

    particle->velocity += process_orbit_displacement(particle, define, dynamicsParam, *particle->position, Vec2(), dt);
    particle->velocity += process_radial_displacement(particle, define, dynamicsParam, *particle->position, dt);

    particle->position->x += particle->velocity.x * dt;
    particle->position->y += particle->velocity.y * dt;
    // printf("%f %f \n", particle->position->x, particle->position->y);
}