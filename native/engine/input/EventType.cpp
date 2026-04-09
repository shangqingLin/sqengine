#include "EventType.h"

 bool isMouseEventType(EventType event){
    return 
        event ==  EventType::mouseclick
        || event == EventType::mousedown
        || event == EventType::mouseup
        || event == EventType::mousemove
        || event == EventType::mouserightdown
        || event == EventType::mouserightup
        || event == EventType::mouseout
        || event == EventType::mousewheel;
 }

 bool isTouchEventType(EventType event){
    return event ==  EventType::touchstart
    || event == EventType::touchend
    || event == EventType::touchmove
    || event == EventType::tocuhcancel;
 }