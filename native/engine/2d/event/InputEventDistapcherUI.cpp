#include "InputEventDispatcherUI.h"
#include "../../scene/Node.h"
#include "../components/UIContentComponent.h"
#include <algorithm>
#include "../../input/InputManager.h"

/**
 * 不进行递归遍历
 * 按照渲染顺序进行排序，渲染在最前面的排在数组中的最前面.
 * 所以避免了递归遍历，也能快速找到输入事件要处理的节点。
 *
 * 返回false : process2 在 process1 的前面
 * 返回true : process1 在 process2 的前面
 */

bool sortNode(NodeEventProcess *process1, NodeEventProcess *process2)
{

    // printf("fuck1 %p %p \n",process1,process2);

    Node *node1 = process1->node;
    Node *node2 = process2->node;

    // printf("fuck2\n");
    UIContentComponent *transform = node2->getComponent<UIContentComponent>();
    if (!node2 || !node2->getVisible() || !node2->activeInHierarchy() || node2->parent == NULL || !transform)
    {
        return true;
    }

    transform = node1->getComponent<UIContentComponent>();
    if (!node1 || !node1->getVisible() || !node1->activeInHierarchy() || node1->parent == NULL || !transform)
    {
        return false;
    }

    // 先找到node1和node2共同的父节点，然后按照这个共同的父节点下一层的兄弟节点进行排序,就能得到正确的渲染顺序
    /**
     *               n1
     *              / \
     *             n2  n3
     *             /    \
     *            n4     n6
     *            /
     *           n5
     *
     *  如果当前需要比较n5和n6的节点，他们的共同父节点为n1，那么就需要比较n2和n3作为n1的子节点的位置来排序
     *  下面的代码就是找到共同父节点下的n2和n3
     *  也就是n5要找到n2的位置，n6要找到n3的位置来进行排序比较
     */

    Node *c1 = node1;
    Node *c2 = node2;

    // 表示是否交换了位置,因为c1要对应n2，c2要对应n3，这个算法中可能交换了，即：c1 = n3 和 c2 = n2
    bool ex = false;
    while (c1->parent != c2->parent)
    {

        if (c1->parent->parent == NULL)
        {
            ex = true;
            c1 = node2;
        }
        else
        {
            c1 = c1->parent;
        }

        if (c2->parent->parent == NULL)
        {
            ex = true;
            c2 = node1;
        }
        else
        {
            c2 = c2->parent;
        }
    }

    /**
     * 处理如下情况
     *          n1
     *         /
     *        n2
     *        /
     *       n3
     *       /
     *      n4
     *
     * 当比较n4和n3时，他们在同一条子树上，共同的父节点为n2。所以c1和c2都等于n3
     * 下面的代码就是处理这种情况
     */

    if (c1 == c2)
    {
        if (c1 == node2)
        {
            return true;
        }

        if (c1 == node1)
        {
            return false;
        }
    }

    int priority1 = c1 ? c1->getSiblingIndex() : 0;
    int priority2 = c2 ? c2->getSiblingIndex() : 0;

    /*
    printf("node1 : ( id: %d priority: %d) node2:( id: %d priority: %d) c1:%d c2: %d ex：%d\n",
    node1->nativeId,
    priority1,

    node2->nativeId,
    priority2,

    c1->nativeId,
    c2->nativeId,

    ex
    );*/

    int c = ex ? priority1 - priority2 : priority2 - priority1;
    return c < 0;
}

static InputEventDispatcherUI *_inst = NULL;

InputEventDispatcherUI *InputEventDispatcherUI::getInstance()
{
    if (_inst == NULL)
    {
        _inst = new InputEventDispatcherUI();
    }
    return _inst;
}

InputEventDispatcherUI::InputEventDispatcherUI()
{
    eventNodeProcesses.reserve(1300);
    waitAdd.reserve(50);
    waitRemove.reserve(50);
    InputManager::getInstance()->registerEventDispatcher(this);
}

void InputEventDispatcherUI::addNodeEventProcess(NodeEventProcess *p)
{
    if (inProcess)
    {

        waitAdd.push_back(p);
    }
    else
    {
        dirty = true;
        eventNodeProcesses.push_back(p);
    }
}

void InputEventDispatcherUI::removeNodeEventProcess(NodeEventProcess *p)
{

    if (inProcess)
    {
        waitRemove.push_back(p);
    }
    else
    {
        for (int i = 0; i < eventNodeProcesses.size(); ++i)
        {
            if (eventNodeProcesses[i] == p)
            {
                eventNodeProcesses.erase(eventNodeProcesses.begin() + i);
                break;
            }
        }
    }

    std::unordered_map<EventId, InputEventProcess>::iterator it = eventRegisterMap.begin();
    while (it != eventRegisterMap.end())
    {
        it->second.remove(p);
        ++it;
    }
}

void InputEventDispatcherUI::markDirty()
{
    dirty = true;
}

/**
 * 必须对节点进行排序
 * 1、能够更快与需要接触的节点匹配，提升性能
 */
void InputEventDispatcherUI::sortEventProcessorList()
{
    if (!dirty)
        return;
    dirty = false;

    // printf("===================sort begin %d \n", eventNodeProcesses.size());

    sort(eventNodeProcesses.begin(), eventNodeProcesses.end(), sortNode);

    // for(int i = 0; i < eventNodeProcesses.size() ; ++i){
    //     printf("  %d %d\n",i,eventNodeProcesses[i]->node->nativeId);
    // }
    // printf("=========================sort end\n");
}

bool InputEventDispatcherUI::dispatchEvent(Event &event)
{
    updateEventProcessorList();
    inProcess = true;
    sortEventProcessorList();

    bool isProcess = false;

    // 对于触控事件，是必须先触发touchstart，之后才有touchmove\touchend\touchcancel
    // 对于鼠标，只要鼠标放到Canvas上，就会一直触发mousemove,所以会先move进一个Node,然后鼠标才mousedown、mouseup

    NodeEventProcess *curretForceProcess = nullptr;
    if (event.type == EventType::mousemove || event.type == EventType::touchmove || event.type == EventType::touchstart || event.type == EventType::mousedown || event.type == EventType::mouseup)
    {
        NodeEventProcess *process = nullptr;
        for (int i = 0; i < eventNodeProcesses.size(); ++i)
        {
            process = eventNodeProcesses[i];
            if (process->checkInNode(event))
            {
                isProcess = true;
                curretForceProcess = process;
                break;
            }
        }
    }

    std::unordered_map<EventId, InputEventProcess>::iterator it = eventRegisterMap.find(event.id);
    if (it == eventRegisterMap.end())
    {
        eventRegisterMap[event.id] = InputEventProcess();
        eventRegisterMap[event.id].process(event, curretForceProcess);
    }
    else
    {
        it->second.process(event, curretForceProcess);
    }

    inProcess = false;
    return isProcess;
}

void InputEventDispatcherUI::updateEventProcessorList()
{
    int size = waitRemove.size();
    if (size > 0)
    {
        for (int i = 0; i < size; ++i)
        {
            removeNodeEventProcess(waitRemove[i]);
        }
        waitRemove.clear();
    }

    size = waitAdd.size();

    if (size > 0)
    {
        for (int i = 0; i < size; ++i)
        {
            addNodeEventProcess(waitAdd[i]);
        }
        waitAdd.clear();
        dirty = true;
    }
}

void InputEventDispatcherUI::InputEventProcess::process(Event &event, NodeEventProcess *process)
{
    if (event.type == EventType::mousemove || event.type == EventType::touchmove)
    {

        if (process == nullptr)
        {
            // 表示没有进入任何节点，则需要moveout当前节点
            if (moveInNodeProcess)
            {
                event.type = EventType::mouseout;
                moveInNodeProcess->node->dispatchEvent(event);
                moveInNodeProcess = nullptr;
            }
        }
        else if (moveInNodeProcess == nullptr)
        {
            moveInNodeProcess = process;
            event.type = EventType::mouseover;
            moveInNodeProcess->node->dispatchEvent(event);
        }
        else if (process == moveInNodeProcess)
        {
            // 证明一直在这个node上move。那么一直触发move事件就行
            event.type = EventType::mousemove;
            moveInNodeProcess->node->dispatchEvent(event);
        }
        else if (process != moveInNodeProcess)
        {
            // 处理当前进入节点不等于之前的节点

            event.type = EventType::mouseout;
            moveInNodeProcess->node->dispatchEvent(event);

            // 计算move out了，但首次是点击此节点的，则也需要触发mouseclick事件
            if (eventProcessState == InputEventProcess::EVENT_MOUSEDOWN_LEFT || eventProcessState == InputEventProcess::EVENT_MOUSEDOWN_RIGHT)
            {
                moveOutNodeProcess = moveInNodeProcess;
                eventOutProcessState = eventProcessState;
                eventProcessState = 0;
            }

            moveInNodeProcess = process;
            event.type = EventType::mouseover;
            moveInNodeProcess->node->dispatchEvent(event);
        }
    }
    else if (event.type == EventType::touchstart)
    {
        // 表示新进入这个节点,触发mouseover
        if (process)
        {
            moveInNodeProcess = process;
            event.type = EventType::mousedown;
            eventProcessState = InputEventProcess::EVENT_MOUSEDOWN_LEFT;
            moveInNodeProcess->node->dispatchEvent(event);
        }
    }
    else if (event.type == EventType::tocuhcancel || event.type == EventType::touchend)
    {
        // 在touchend之前，是先move进这个节点,所以不需要检查是否在节点的范围内

        // 触发前一个out的Node
        if (moveOutNodeProcess && eventOutProcessState == InputEventProcess::EVENT_MOUSEDOWN_LEFT)
        {
            eventOutProcessState = 0;
            event.type = EventType::mouseclick;
            moveOutNodeProcess->node->dispatchEvent(event);
        }

        if (moveInNodeProcess != nullptr)
        {

            event.type = EventType::mouseup;
            moveInNodeProcess->node->dispatchEvent(event);

            // 触发前一个out的Node
            if (moveInNodeProcess && eventProcessState == InputEventProcess::EVENT_MOUSEDOWN_LEFT)
            {
                eventProcessState = 0;
                event.type = EventType::mouseclick;
                moveInNodeProcess->node->dispatchEvent(event);
            }
        }
        moveOutNodeProcess = nullptr;
        moveInNodeProcess = nullptr;
    }
    else if (event.type == EventType::mousedown || event.type == EventType::mouserightdown)
    {
        // 在mousedown之前，是先move进这个节点,所以不需要检查
        if (moveInNodeProcess)
        {
            eventProcessState = event.type == EventType::mouserightdown ? InputEventProcess::EVENT_MOUSEDOWN_RIGHT : InputEventProcess::EVENT_MOUSEDOWN_LEFT;
            moveInNodeProcess->node->dispatchEvent(event);
        }
    }
    else if (event.type == EventType::mouseup || event.type == EventType::mouserightup)
    {
        // 在mouseup\mouseRightup之前是先move进这个节点,所以不需要检查

        // 触发前一个out的Node
        if (moveOutNodeProcess)
        {

            if (eventOutProcessState == InputEventProcess::EVENT_MOUSEDOWN_LEFT)
            {
                SQ_ASSERT(event.type == EventType::mouseup);
                eventOutProcessState = 0;
                event.type = EventType::mouseclick;
                moveOutNodeProcess->node->dispatchEvent(event);
            }
            else if (eventOutProcessState == InputEventProcess::EVENT_MOUSEDOWN_RIGHT)
            {
                SQ_ASSERT(event.type == EventType::mouserightup);
                eventOutProcessState = 0;
                event.type = EventType::mouserightclick;
                moveOutNodeProcess->node->dispatchEvent(event);
            }
            moveOutNodeProcess = nullptr;
        }

        if (moveInNodeProcess)
        {

            // 触发EventType::mouseup\EventType::mouserightup
            moveInNodeProcess->node->dispatchEvent(event);

            if (eventProcessState == InputEventProcess::EVENT_MOUSEDOWN_LEFT)
            {
                SQ_ASSERT(event.type == EventType::mouseup);
                eventProcessState = 0;
                event.type = EventType::mouseclick;
                moveInNodeProcess->node->dispatchEvent(event);
            }
            else if (eventProcessState == InputEventProcess::EVENT_MOUSEDOWN_RIGHT)
            {
                SQ_ASSERT(event.type == EventType::mouserightup);
                eventProcessState = 0;
                event.type = EventType::mouserightclick;
                moveInNodeProcess->node->dispatchEvent(event);
            }
            moveInNodeProcess = nullptr;
        }
    }
    else if (event.type == EventType::mouseout)
    {
        // 表示没有进入任何节点，则需要moveout当前节点
        if (moveInNodeProcess)
        {
            event.type = EventType::mouseout;
            moveInNodeProcess->node->dispatchEvent(event);
            moveInNodeProcess = nullptr;
        }
        eventProcessState = 0;
        eventOutProcessState = 0;
        moveOutNodeProcess = nullptr;
    }
}

void InputEventDispatcherUI::InputEventProcess::remove(NodeEventProcess *process)
{
    if (moveInNodeProcess == process)
    {
        moveInNodeProcess = nullptr;
    }

    if (moveOutNodeProcess == process)
    {
        moveOutNodeProcess = nullptr;
    }
}