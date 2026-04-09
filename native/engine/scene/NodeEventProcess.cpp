#include "NodeEventProcess.h"
#include "Node.h"
#include "../2d/event/InputEventDispatcherUI.h"
#include "../2d/components/UIContentComponent.h"
#include "./NodeNativeToJsBridge.h"

NodeEventProcess::NodeEventProcess(Node *node) : node(node)
{
}

NodeEventProcess::~NodeEventProcess()
{
}

void NodeEventProcess::registerEvent(NodeEventType eventType)
{
    if (isMouseEvent(eventType) && !hasMouseEvent())
    {
        InputEventDispatcherUI::getInstance()->addNodeEventProcess(this);
    }
    eventRegisterState |= toNumber(eventType);
}

/**
 * 这种表示是JS端注册的，JS端只是告诉一下C++注册了事件。
 * 因为回调在JS端，这里on没有回调函数
 */
void NodeEventProcess::onFromJs(NodeEventType eventType)
{
    // printf(">>>>> %d %d \n", node->nativeId, eventType);
    registerEvent(eventType);
}

/**
 * 专门提供在C++端注册的。
 */
void NodeEventProcess::on(NodeEventType eventType, EVENT_CALL callback)
{
    // printf("on event %d %d\n",node->nativeId,eventType);

    registerEvent(eventType);
    std::unordered_map<NodeEventType, std::vector<EVENT_CALL>>::iterator it = eventCallMap.find(eventType);
    if (it == eventCallMap.end())
    {
        eventCallMap[eventType] = std::vector<EVENT_CALL>();
        it = eventCallMap.find(eventType);
    }
    it->second.push_back(callback);
}

/**
 * 移除指定事件上所有的函数
 */
void NodeEventProcess::off(NodeEventType eventType)
{
    std::unordered_map<NodeEventType, std::vector<EVENT_CALL>>::iterator it = eventCallMap.find(eventType);
    if (it != eventCallMap.end())
    {
        eventCallMap.erase(it);
    }

    bool has = hasMouseEvent();
    eventRegisterState &= ~toNumber(eventType);

    if (has && isMouseEvent(eventType) && !hasMouseEvent())
    {
        InputEventDispatcherUI::getInstance()->removeNodeEventProcess(this);
    }
}

/**
 * 移除指定事件上指定的函数
 */
void NodeEventProcess::off(NodeEventType eventType, EVENT_CALL call)
{
    std::unordered_map<NodeEventType, std::vector<EVENT_CALL>>::iterator it = eventCallMap.find(eventType);
    if (it != eventCallMap.end())
    {
        std::vector<EVENT_CALL> &calls = it->second;
        for (int i = 0; i < calls.size(); ++i)
        {
            if (call.target<EVENT_CALL>() == calls[i].target<EVENT_CALL>())
            {
                calls.erase(calls.begin() + i);
                break;
            }
        }

        if (calls.size() == 0)
        {
            off(eventType);
        }
    }
}

/**
 * JS完全移除该节点上的这个事件，但C++端可能会在C++这边自己注册了JS端不知道的
 * 所以需要判断一下C++端是否还有
 */
void NodeEventProcess::offFromJs(NodeEventType eventType)
{

    // 判断C++是否还有此事件
    std::unordered_map<NodeEventType, std::vector<EVENT_CALL>>::iterator it = eventCallMap.find(eventType);
    if (it == eventCallMap.end())
    {
        off(eventType);
    }
}

void NodeEventProcess::fire(NodeEventType eventType)
{
    std::unordered_map<NodeEventType, std::vector<EVENT_CALL>>::iterator it = eventCallMap.find(eventType);
    if (it != eventCallMap.end())
    {
        std::vector<EVENT_CALL> &calls = it->second;
        for (int i = 0; i < calls.size(); ++i)
        {
            calls[i]();
        }
    }
    // else{
    //     if(node->nativeId == 31)
    //     printf("+++++++++++++++++++++++++没找到 %d \n",eventType);
    // }
}

bool NodeEventProcess::hasEventListener(NodeEventType evenType)
{
    return toNumber(evenType) & eventRegisterState;
}

bool NodeEventProcess::hasMouseEvent()
{
    return eventRegisterState & toNumber(NodeEventType::ALL_MOUSE);
}

bool NodeEventProcess::checkInNode(Event &event)
{
    if (!node->getVisible())
        return false;

    UIContentComponent *content = node->getComponent<UIContentComponent>();
    if (!content)
        return false;
    return content->hitTest(event);
}

void NodeEventProcess::_handlerTransformChangeEvent()
{

    if (!(eventRegisterState & toNumber(NodeEventType::TRASNFORM_CHANGE)))
        return;

    fire(NodeEventType::TRASNFORM_CHANGE);

    // printf("fire event %d \n", node->nativeId);
    
    // C++端创建的节点
    if (node->nativeId == -1)
        return;
    // if(node->nativeId == 127)
    // printf("trigger %d\n",node->nativeId);
    NodeNativeToJsBridge::getInstance()->beginOp(1, true);
    NodeNativeToJsBridge::getInstance()->writeOpArg(node->nativeId);
    NodeNativeToJsBridge::getInstance()->endOp();
}

void NodeEventProcess::destroy()
{
    if (hasMouseEvent())
    {
        InputEventDispatcherUI::getInstance()->removeNodeEventProcess(this);
    }
}

void NodeEventProcess::markDirty()
{
    InputEventDispatcherUI::getInstance()->markDirty();
}