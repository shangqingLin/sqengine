#include "SqSoftConstraint.h"
#include "../common/math/SqMath.h"

using namespace phxy;

SqSoftConstraint::SqSoftConstraint() : biasRate(0.f), massScale(1.f), impulseScale(0.f)
{
}

void SqSoftConstraint::step(float hertz, float zeta, float dt)
{
    // 频率转换为角频率（角速度）
    if (hertz == 0.0f)
    {
        biasRate = 0.f;
        massScale = 1.f;
        impulseScale = 0.f;
        return;
    }
    float omega = 2.0f * SQ_PI * hertz;
    float a1 = 2.0f * zeta + dt * omega;
    float a2 = dt * omega * a1;
    float a3 = 1.0f / (1.0f + a2);

    biasRate = omega / a1;
    massScale = a2 * a3;
    impulseScale = a3;
}