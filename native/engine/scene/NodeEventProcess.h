#ifndef _NODE_EVENT_PROCESS_H_
#define _NODE_EVENT_PROCESS_H_
#include "NodeEventType.h"
#include "../input/EventType.h"
#include <unordered_map>
#include <functional>

typedef std::function<void(void)> EVENT_CALL;
class Node;
class UIContentComponent;
class NodeEventProcess
{
private:
    std::unordered_map<NodeEventType, std::vector<EVENT_CALL>> eventCallMap;
    int eventRegisterState = 0;
    bool checkInNode(Event &);
    void registerEvent(NodeEventType);
public:
    friend class InputEventDispatcherUI;

    Node *node;
    NodeEventProcess(Node *);
    ~NodeEventProcess();
    void onFromJs(NodeEventType);
    void offFromJs(NodeEventType);
    void on(NodeEventType, EVENT_CALL);
    void off(NodeEventType);
    void off(NodeEventType, EVENT_CALL);
    void fire(NodeEventType);
    void destroy();
    bool hasEventListener(NodeEventType);
    bool hasMouseEvent();
    void _handlerTransformChangeEvent();
    void markDirty();
};

#endif