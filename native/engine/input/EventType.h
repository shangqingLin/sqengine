#ifndef _EVENT_TYPE_H_
#define _EVENT_TYPE_H_
#include "../core/sqstd/typedefs.h"

enum class EventType : unsigned char
{
    keydown,
    keyup,
    mousedown,
    mouseup,
    mouserightdown,
    mouserightup,
    mouseout,
    mousemove,
    mouseover,
    mouseclick,
    mouserightclick,
    mousewheel,
    touchstart,
    touchend,
    touchmove,
    tocuhcancel
};
SQ_ENUM_CONVERSION_OPERATOR(EventType);

bool isMouseEventType(EventType);
bool isTouchEventType(EventType);

class Event
{
public:
    EventType type;
    int id;
    /**
     * Canvas空间下的位置
     */
    float screenX;
    float screenY;

    //鼠标在世界坐标系下的坐标
    float worldX;
    float worldY;

    //鼠标在本地节点上的坐标
    float localX;
    float localY;
};

class EventMouse : public Event
{
};

class EventTouch : public Event
{

};

class TransformEvent : public Event
{
    
};

#endif