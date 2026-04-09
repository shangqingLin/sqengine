#pragma once
#include "./sim/SqContactSim.h"
#include "./sim/SqBodySim.h"
#include "../common/collection/SqBitSet.h"

namespace phxy
{
    class SqWorld;
    struct SqStepContext
    {
        float dt{0.0f};
        float inv_dt{0.0f};
        float h{0.f};
        float inv_h{0.f};

        int subStepCount{0};
        SqWorld *world{nullptr};

        /**
         * 当前帧上下文需要处理的所有的碰撞点
         */
        SqContactSim **contacts{nullptr};

        int contactCount{0};

        SqBodySim *bodySims{nullptr};

        int splitIslandId = SQ_NULL_INDEX;
        float splitSleepTime = 0.f;

        SqBitSet contactStateBitSet;
    };
}