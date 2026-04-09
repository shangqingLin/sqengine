#include "NodeTreeManager.h"
#include "NodeEnum.h"
#include "../framework/component/Component.h"
#include "../framework/Application.h"
#include "Node.h"

static NodeTreeManager *_ins = new NodeTreeManager();
NodeTreeManager *NodeTreeManager::getInstnace()
{
    return _ins;
}

void NodeTreeManager::addInhierChangeNode(Node *node)
{

    if (lock)
    {

        // 表示NodeTree遍历已经开始了，在NodeTreeManager执行生命周期函数的过程又有节点加入进来了
        // 因此这些应该放到下一帧执行

        bool find = false;
        for (int i = 0; i < waitArray.getCount(); ++i)
        {
            if (waitArray[i] == node)
            {
                find = true;
                break;
            }
        }
        if (!find)
        {
            waitArray.push(node);
            // printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>add %d %p %d lock %d fuck %d\n", node->nativeId, node, node->activeInHierarchy(), lock,node->getAllComponents().size());
        }
        return;
    }

    if (inhierChangeNodeArray.getCapacity() == 0)
    {
        inhierChangeNodeArray.setTempAllocator(sqstd::StackTempArenaAllocator::getInstance());
        inhierChangeNodeArray.resize(2000);
    }

    bool find = false;
    for (int i = 0; i < inhierChangeNodeArray.getCount(); ++i)
    {
        if (inhierChangeNodeArray[i] == node)
        {
            find = true;
            break;
        }
    }

    if (!find)
    {

        inhierChangeNodeArray.push(node);
        // printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>add %d %p %d lock %d \n", node->nativeId, node, node->activeInHierarchy(), lock);
    }
}

void NodeTreeManager::removeInhierChangeNode(Node *removeNode)
{
    for (int i = 0; i < inhierChangeNodeArray.getCount(); ++i)
    {
        Node *node = inhierChangeNodeArray[i];
        if (node == removeNode)
        {
            inhierChangeNodeArray.removeAt(i);
            return;
        }
    }

    for (int i = 0; i < waitArray.getCount(); ++i)
    {
        Node *node = waitArray[i];
        if (node == removeNode)
        {
            waitArray.removeAt(i);
            return;
        }
    }
}

void NodeTreeManager::updateEnable()
{

    lock = true;
    if (waitArray.getCount() > 0)
    {
        for (int i = 0; i < waitArray.getCount(); ++i)
        {
            inhierChangeNodeArray.push(waitArray[i]);
        }
        waitArray.clear();
    }

    for (int i = 0; i < inhierChangeNodeArray.getCount(); ++i)
    {
        Node *node = inhierChangeNodeArray[i];
        if (node->activeInHierarchy())
        {
            const std::vector<Component *> &components = node->getAllComponents();
            for (int n = 0; n < components.size(); ++n)
            {
                Component *c = components[n];
                if (c->state == 0 || c->state & Component::DISABLE)
                {
                    c->onEnable();
                    c->state &= ~Component::DISABLE;
                    c->state |= Component::ENABLE;
                }
            }
        }
    }
}

void NodeTreeManager::start()
{
    // printf(">>>start run %d \n", inhierChangeNodeArray.getCount());
    for (int i = 0; i < inhierChangeNodeArray.getCount(); ++i)
    {
        Node *node = inhierChangeNodeArray[i];

        // printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>star check %d %p %d \n", node->nativeId, node, node->activeInHierarchy());

        if (node->activeInHierarchy())
        {
            const std::vector<Component *> &components = node->getAllComponents();
            for (int n = 0; n < components.size(); ++n)
            {
                Component *c = components[n];
                if (!(c->state & Component::START) && (c->state & Component::ENABLE))
                {
                    // printf("..................start \n");
                    c->state |= Component::START;
                    c->onStart();
                }
            }
        }
    }
}

void NodeTreeManager::updateDisable()
{
    // printf("?????-============== %d \n",inhierChangeNodeArray.getCount());
    for (int i = 0; i < inhierChangeNodeArray.getCount(); ++i)
    {
        Node *node = inhierChangeNodeArray[i];
        if (!node->activeInHierarchy())
        {
            const std::vector<Component *> &components = node->getAllComponents();
            for (int n = 0; n < components.size(); ++n)
            {
                Component *c = components[n];
                if (c->state & Component::ENABLE)
                {

                    c->onDisable();
                    c->state &= ~Component::ENABLE;
                    c->state |= Component::DISABLE;
                }
            }
        }
    }

    // printf("+++++++++++++++++++++++++freeData\n");
    inhierChangeNodeArray.freeData();
    lock = false;
}

void NodeTreeManager::update()
{

    auto callback = [](Node *node)
    {
        if (!node->getVisible() || !node->activeInHierarchy())
        {
            return char(1);
        }
        const std::vector<Component *> &components = node->getAllComponents();
        for (int n = 0; n < components.size(); ++n)
        {
            if (components[n]->state & Component::ENABLE)
            {
                components[n]->onUpdate();
            }
        }
        return char(2);
    };
    Application::getInstance()->getRunScene()->walk(callback);
}