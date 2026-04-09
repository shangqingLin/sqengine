#ifndef _2D_INPUT_EVENT_DISPATCHER_UI_H_
#define _2D_INPUT_EVENT_DISPATCHER_UI_H_

#include "../../input/InputEventDispatcher.h"
#include "../../scene/NodeEventProcess.h"
#include <vector>
#include <unordered_map>

typedef int EventId;
class InputEventDispatcherUI : public InputEventDispatcher
{
private:
    std::vector<NodeEventProcess *> eventNodeProcesses;
    std::vector<NodeEventProcess *> waitAdd;
    std::vector<NodeEventProcess *> waitRemove;

    /**
     * 避免在事件触发的过程，又往eventNodeProcesses添加NodeEventProcess，造成事件处理顺序错乱
     */
    bool inProcess = false;
    bool dirty = false;

    void sortEventProcessorList();
    void updateEventProcessorList();

    class InputEventProcess
    {
    private:
        static const int EVENT_MOUSEDOWN_LEFT = 1;
        static const int EVENT_MOUSEDOWN_RIGHT = 2;
        int eventProcessState = 0;
        int eventOutProcessState = 0;
        NodeEventProcess *moveInNodeProcess = nullptr;
        NodeEventProcess *moveOutNodeProcess = nullptr;

    public:
        void process(Event &, NodeEventProcess *);
        void remove(NodeEventProcess*);
    };

    // 实现多点触控
    std::unordered_map<EventId, InputEventProcess> eventRegisterMap;

public:
    static InputEventDispatcherUI *getInstance();
    InputEventDispatcherUI();
    virtual bool dispatchEvent(Event &event);
    void addNodeEventProcess(NodeEventProcess *);
    void removeNodeEventProcess(NodeEventProcess *);
    void markDirty();
};

#endif