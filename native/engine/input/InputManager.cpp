#include "InputManager.h"
#include <string.h>
#include <stdio.h>
#include "../core/base/config.h"

static InputManager *_inst = NULL;

InputManager::InputManager()
{
    _inst = this;
}

InputManager *InputManager::getInstance()
{
    return _inst;
}

void InputManager::setEventDataPointer(char *pointer, int size)
{
    buffer.setExternalBuffer(pointer, size);
}

void InputManager::registerEventDispatcher(InputEventDispatcher *dispatcher)
{
    eventDispatchers.push_back(dispatcher);
}

void InputManager::onNodeDispatcher(Event &event, Node *node)
{
    SQ_ASSERT(node->nativeId > 0);
    
    // 记录C++端触发的，然后在Js端读取
    unsigned char type = toNumber(event.type);
    buffer.append(event.id);       // 4
    buffer.append(type);           // 1
    buffer.append(node->nativeId); // 4
    buffer.append(event.worldX);   // 4
    buffer.append(event.worldY);   // 4
    buffer.append(event.localX);   // 4
    buffer.append(event.localY);   // 4
}

// 处理JS 告诉C++端需要检查的事件
void InputManager::handlerEvent(Event &event)
{
    for (int i = 0; i < eventDispatchers.size(); ++i)
    {
        if (eventDispatchers[i]->dispatchEvent(event))
        {
            break;
        }
    }
}

void InputManager::update()
{
    int size = *buffer.popp<int>();
    if (size > 0)
    {

        char data[size];
        memcpy(data, buffer.readBuffer(size), size);

        ArrayBuffer eventBuffer;
        eventBuffer.setExternalBuffer(data, size);

        buffer.clearData();
        buffer.setWritePos(4); // 跳过size

        // 处理js端传递过来的需要检查的事件
        while (eventBuffer.getReadPos() < eventBuffer.getDataSize())
        {
            int id = *eventBuffer.popp<int>();
            EventType type = EventType(*eventBuffer.popp<unsigned char>());
            if (isMouseEventType(type))
            {
                EventMouse event;
                event.id = id;
                event.type = type;
                event.screenX = *eventBuffer.popp<float>();
                event.screenY = *eventBuffer.popp<float>();

                //  printf("process mouse type:%d id:%d (%f,%f)\n",type,id,event.x,event.y);

                handlerEvent(event);
            }
            else if (isTouchEventType(type))
            {
                EventTouch event;
                event.id = id;
                event.type = type;
                event.screenX = *eventBuffer.popp<float>();
                event.screenY = *eventBuffer.popp<float>();
                handlerEvent(event);
            }
        }

        size = buffer.getDataSize() - 4;
        buffer.setWritePos(0);
        buffer.append(size);
        buffer.setReadPos(0);
    }
}