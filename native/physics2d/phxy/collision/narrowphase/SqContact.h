#pragma once
#include "../../common/SqCore.h"
#include "../../geometry/shape/SqShape.h"
#include <stdint.h>

namespace phxy
{

    enum SqContactFlags
    {
        // Set when the solid shapes are touching.
        sq_contactTouchingFlag = 0x00000001,

        // Contact has a hit event
        sq_contactHitEventFlag = 0x00000002
    };

    struct SqContactEdge
    {
        int bodyIndex{SQ_NULL_INDEX};
        int prevKey{SQ_NULL_INDEX};
        int nextKey{SQ_NULL_INDEX};

        SqContactEdge();
        void reset();
    };

    struct SqContact
    {
        /**
         * SqContactManager中contacts数组的下标
         */
        int contactId{SQ_NULL_INDEX};

        /**
         * SqContactSim在SqWorld中的solverSets哪个set中，为solverSets数组的index
         */
        int setIndex{SQ_NULL_INDEX};

        // index into the constraint graph color array
        // B2_NULL_INDEX for non-touching or sleeping contacts
        // B2_NULL_INDEX when slot is free
        int colorIndex{SQ_NULL_INDEX};

        // 在SqSolverSet中contactSims的index。记录与之关联的SqContactSim
        int simIndex{SQ_NULL_INDEX};

        // 记录当前的Contact与哪两个Body关联
        SqContactEdge edges[2];

        SqShape *shapeA{nullptr};
        SqShape *shapeB{nullptr};

        // 在island中使用链表方式存储碰撞点
        int islandPrev{SQ_NULL_INDEX};
        int islandNext{SQ_NULL_INDEX};
        int islandId{SQ_NULL_INDEX}; // 这个碰撞点在哪个island中
        bool isMarkedForIslandSplit = false;

        // SqContactFlags
        uint32_t flags{0};

        bool isMarked{false};
        SqContact();
        void reset();
    };

}