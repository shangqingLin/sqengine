#pragma once

#include "../../common/constants-define.h"
namespace phxy
{

    enum SqIslandSimFlag
    {
        sq_notNeedToSleep = 1 << 1
    };

    struct SqIslandSim
    {
        int islandId = SQ_NULL_INDEX;

        //SqIslandSimFlag
        int flag = 0;
    };
}