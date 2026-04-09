#pragma once
#include "../../collision/narrowphase/SqManifold.h"
#include "../../collision/narrowphase/gjk-distance.h"
#include "../../common/SqCore.h"
#include "../../common/constants-define.h"
#include "../../geometry/shape/SqShape.h"

#include "SqBodySim.h"

namespace phxy
{

    enum SqContactSimFlags
    {
        // 表示当前的Contanct正接触状态
        sq_simTouchingFlag = 1 << 1,

        // 表示AABB有之前的重叠进入到不重叠状态。即AABB不再重叠啦
        sq_simDisjoint = 1 << 2, 

        // This contact started touching
        sq_simStartedTouching = 1 << 3,

        // This contact stopped touching
        sq_simStoppedTouching = 1 << 4,

        // This contact has a hit event
        sq_simEnableHitEvent = 1 << 5,

        // This contact wants pre-solve events
        sq_simEnablePreSolveEvents = 1 << 6
    };

    struct SqContactSim
    {

        /**
         * SqContact、SqBodySim等这些都是保存在一个可变
         * 大小的数组中，在数组内存重新分配的时候，地址全都改变了
         * 所以，这里不能保存地址
         */
        int contactId{SQ_NULL_INDEX};
        int bodyAIndex{SQ_NULL_INDEX};
        int bodyBIndex{SQ_NULL_INDEX};

        SqShape *shapeA{nullptr};
        SqShape *shapeB{nullptr};

        SqManifold manifold;

        float friction{0.f};
        float restitution{0.f};
        float rollingResistance{0.f};
        float tangentSpeed{0.f};

        // SqContactSimFlags
        uint32_t simFlags{0};

        SqSimplexCache cache;

        SqContactSim();

        // alloc分配的内存无法初始化，所以必须调用这个初始化
        void reset();
    };

}