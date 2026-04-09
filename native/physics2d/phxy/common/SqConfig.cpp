#include "SqConfig.h"
#include "./math/SqMath.h"
#include "./SqCore.h"
using namespace phxy;

static SqConfig *config = nullptr;

SqConfig *SqConfig::getInstance()
{
    if (!config)
    {
        config = new SqConfig;
    }
    return config;
}

SqConfig::SqConfig()
{
    hugValue = 100000.0f;
    linearSlop = 0.005f;
    speculative_distance = 4.0f * linearSlop;
    aabb_margin = 0.05f;
}

void SqConfig::setLengthUnitsPerMeter(float lengthUnits)
{
    SQ_ASSERT(isValidFloat(lengthUnits) && lengthUnits > 0.0f);
    lengthUnitsPerMeter = lengthUnits;
    hugValue = 100000.0f * lengthUnits;
    linearSlop = 0.005f * lengthUnits;
    speculative_distance = 4.0f * linearSlop;
    aabb_margin = 0.05f * lengthUnits;
}

float SqConfig::getLengthUnitsPerMeter(void)
{
    return lengthUnitsPerMeter;
}
