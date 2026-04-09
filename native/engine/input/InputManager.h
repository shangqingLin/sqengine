#ifndef _INPUT_MANAGER_H_
#define _INPUT_MANAGER_H_

#include "../core/common/ArrayBuffer.h"
#include "InputEventDispatcher.h"
#include <vector>
#include "../scene/Node.h"

class InputManager
{
private:
    std::vector<InputEventDispatcher*> eventDispatchers;
    ArrayBuffer buffer;
    void handlerEvent(Event&);
public:
    InputManager();
    static InputManager* getInstance();
    void registerEventDispatcher(InputEventDispatcher*);
    void setEventDataPointer(char* pointer,int size);
    void update();
    void onNodeDispatcher(Event&,Node*);
    void cacheNodeEventData(float x,float y);
};

#endif