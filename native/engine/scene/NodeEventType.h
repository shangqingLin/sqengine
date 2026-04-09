#ifndef _NODE_EVENT_TYPE_H_
#define _NODE_EVENT_TYPE_H_
#include "../core/sqstd/typedefs.h"

enum class NodeEventType : int
{
    MOUSE_DOWN = 1,   // 1
    MOUSE_MOVE = 2,   // 10
    MOUSE_UP = 4,     // 100
    MOUSE_OUT = 8,    // 1000
    MOUSE_OVER = 16,  // 10000
    MOUSE_WHEEL = 32, // 100000
    MOUSE_RIGHT_DOWN = 64,
    MOUSE_RIGHT_UP = 128,
    MOUSE_CLICK = 256,
    TRASNFORM_CHANGE = 1 << 9,
    COMPONENT_ADD,
    COMPONENT_REMOVE,
    ALL_MOUSE = MOUSE_DOWN | MOUSE_MOVE | MOUSE_UP | MOUSE_OUT | MOUSE_WHEEL | MOUSE_RIGHT_DOWN | MOUSE_RIGHT_UP | MOUSE_CLICK
};

SQ_ENUM_CONVERSION_OPERATOR(NodeEventType);

bool isMouseEvent(NodeEventType eventType);

#endif