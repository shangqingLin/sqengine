#include "NodeEventType.h"

bool isMouseEvent(NodeEventType eventType){
    return 
        eventType == NodeEventType::MOUSE_CLICK
        || eventType == NodeEventType::MOUSE_DOWN 
        || eventType == NodeEventType::MOUSE_UP 
        || eventType == NodeEventType::MOUSE_MOVE
        || eventType == NodeEventType::MOUSE_OUT
        || eventType == NodeEventType::MOUSE_OVER
        || eventType == NodeEventType::MOUSE_RIGHT_DOWN
        || eventType == NodeEventType::MOUSE_RIGHT_UP;
 }