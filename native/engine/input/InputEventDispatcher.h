#ifndef __INPUT_EVENT_DISPATCHER_H_
#define __INPUT_EVENT_DISPATCHER_H_
#include "EventType.h"

class InputEventDispatcher
{
public:  
    /**
     * 返回false表示不继续派发给下一个Eventdispatcher
     */
    virtual bool dispatchEvent(Event& event) = 0;
};

#endif