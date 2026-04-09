#pragma once
#include "../Particle2DTransmitter.h"

struct DynamicsParameters
{
    float angular_velocity;
    float initial_velocity_multiplier;
    float directional_velocity;
    float radial_velocity;
    float orbit_velocity;
    float turb_influence;
};

struct PhysicalParameters
{
    float linear_accel;
    float radial_accel;
    float tangent_accel;
    float damping;
};

struct Particle2DCPUUpdate
{
    Particle2D *particle{nullptr};
    Vec2 velocity;

    //引用Paritlce2DCPURenderData中的数据
    Vec2 *position{nullptr};
    float* lifeTimePercent;
    Vec2 shapeEmitDirection;
    unsigned int randomSeed{0};
    unsigned int initSeed{0};
};


struct Paritlce2DCPURenderData
{
    Vec2 particlePosition;
    float lifeTimePercent;
};
