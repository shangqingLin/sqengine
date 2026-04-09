#include "Node.h"
#include "../framework/component/TransformBaseComponent.h"
#include "../framework/component/RenderComponent.h"
#include "../2d/components/Transform2DComponent.h"

#include "Scene.h"
#include "../input/InputManager.h"
#include "../core/common/ArrayBuffer.h"
#include "../core/core.h"
#include "NodeTreeManager.h"

char upateFromParent(Node *node)
{
    node->_updateScene();
    node->layerDirty = true;
    return char(2);
}

char updateLayer(Node *node)
{
    node->layerDirty = true;
    return char(2);
}

int Node::globalFlagChangeVersion = 1;

Node::Node() : parent(NULL),
               flagChangeVersion(0),
               changedFlags(0),
               scene(nullptr),
               _activeInHierarchy(false),
               visible(true),
               siblingIndex(-1),
               eventProcess(this),
               layerDirty(true),
               transform(nullptr)
{
    layer = Layers::INHERIT;
    realLayer = layer;
}

void Node::addChild(Node *node)
{
    SQ_ASSERT(node);
    ERR_FAIL_NULL_V(node);

    if (node->parent)
    {
        node->parent->removeChildLightweight(node);
    }
    // printf("addChild: parent: %p %d child: %p %d\n",this,this->nativeId, node,node->nativeId);
    node->siblingIndex = children.size();
    children.push_back(node);
    if (node->parent != this)
        node->onSetParent(this);
    eventProcess.markDirty();
}

void Node::addChildAt(Node *node, int insertIndex)
{
    SQ_ASSERT(node);
    ERR_FAIL_NULL_V(node);

    if (node->parent == this)
    {
        for (int i = 0; i < children.size(); ++i)
        {
            if (children[i] == node && i == insertIndex)
            {
                return;
            }
        }
    }

    if (node->parent)
    {
        node->parent->removeChildLightweight(node);
    }

    if (insertIndex >= children.size())
    {
        node->siblingIndex = children.size();
        children.push_back(node);
    }
    else
    {
        auto it = children.begin() + insertIndex; // 插入位置
        children.insert(it, node);
        node->siblingIndex = insertIndex;
    }
    node->onSetParent(this);
    eventProcess.markDirty();
}

void Node::onSetParent(Node *parent)
{
    if (this->parent == parent)
        return;

    this->parent = parent;
    if (this->parent && this->parent->scene && (!this->scene || this->scene != this->parent->scene))
    {
        walk(upateFromParent);
    }

    TransformBaseComponent *t = this->getComponent<TransformBaseComponent>();
    if (t)
    {
        t->invalidateChildren(TransformBit::TRS);
    }
    _onHierarchyChanged();
}

void Node::_onHierarchyChanged()
{
    layerDirty = true;
    bool shouldActiveNow = parent && parent->_activeInHierarchy;
    // printf(" %p  currentNative:%d parentNative:%d currentActive: %d  parentActive: %d \n",this,nativeId,parent->nativeId,_activeInHierarchy,shouldActiveNow);
    if (_activeInHierarchy != shouldActiveNow)
    {
        _activeInHierarchy = shouldActiveNow;
        NodeTreeManager::getInstnace()->addInhierChangeNode(this);
        for (int j = 0; j < children.size(); ++j)
        {
            children[j]->_onHierarchyChanged();
        }
    }
}

void Node::removeChildLightweight(Node *node)
{
    for (int i = 0; i < children.size(); ++i)
    {
        if (children[i] == node)
        {
            children.erase(children.begin() + i);
            node->parent = NULL;
            node->siblingIndex = -1;
            break;
        }
    }
}

void Node::removeChild(Node *node)
{
    SQ_ASSERT(node);
    ERR_FAIL_NULL_V(node);
    for (int i = 0; i < children.size(); ++i)
    {
        if (children[i] == node)
        {
            children.erase(children.begin() + i);
            node->parent = NULL;
            node->siblingIndex = -1;
            node->_onHierarchyChanged();
            break;
        }
    }
}

void Node::removeFromParent()
{
    if (parent)
    {
        parent->removeChild(this);
    }
}
const std::vector<Node *> &Node::getChildren()
{
    return children;
}

Node *Node::getChild(int index)
{
    return index < children.size() ? children[index] : nullptr;
}

Node *Node::getParent()
{
    return parent;
}

void Node::setChangedFlags(int flag)
{
    if (changedFlags == flag)
    {
        //   if (nativeId == 127)
        //         printf(" Node::setChangedFlags same %d %d %d \n", nativeId, changedFlags,flag);
        return;
    }

    flagChangeVersion = Node::globalFlagChangeVersion;

    unsigned int t = changedFlags;
    changedFlags |= flag;

    // if (nativeId == 127)
    // {
    // printf(" Node::setChangedFlags %d %d %d oldFlag %d \n", nativeId, changedFlags, flag, t);

    //     printf(" %d %d %d \n",
    //         ((flag & TransformBit::POSITION) && !(t & TransformBit::POSITION)),
    //       ((flag & TransformBit::ROTATION) && !(t & TransformBit::ROTATION)),
    //      ((flag & TransformBit::SCALE) && !(t & TransformBit::SCALE))
    //     );
    // }

    if ((flag & TransformBit::POSITION) || (flag & TransformBit::ROTATION)  || (flag & TransformBit::SCALE))
    {
        //  if (nativeId ==31)
        // {
        // printf("fuclk _onTransformChanged trigger %d \n",nativeId);
        // }
        _onTransformChanged();
    }
}

bool Node::hasChangedFags()
{
    // 如果版本是一样的，证明在当前帧内setChangedFlags调用过了，那么changedFlags可能有更改
    // 如果版本号不一样的，表示setChangedFlags在多帧之间都没有调用过，证明changedFlags不会有更改过
    return flagChangeVersion == Node::globalFlagChangeVersion ? changedFlags : 0;
}

// 静态函数
void Node::resetHasChangedFlags()
{
    globalFlagChangeVersion += 1;
}

void Node::_onTransformChanged()
{
    eventProcess._handlerTransformChangeEvent();
    if (transform)
    {
        transform->onTransformDirty();
    }
}

void Node::setLayer(Layers layer)
{
    if (this->layer == layer)
    {
        return;
    }
    this->layer = layer;
    // printf("++++++++++++++++fyuck %d %d \n", nativeId, layer);
    if (!layerDirty)
        walk(updateLayer);
}

Layers Node::getLayer()
{
    if (layer == Layers::INHERIT)
    {
        if (layerDirty)
        {
            layerDirty = false;
            realLayer = this->parent ? this->parent->getLayer() : layer;
        }

        // if (nativeId == 14)
        // {
        //     printf("+++++++++++++++nimahao %d %d \n", nativeId, realLayer);
        // }
        return realLayer;
    }
    else
    {
        return layer;
    }
}

void Node::_updateScene()
{
    this->scene = this->parent->scene;
}

void Node::walk(walkNodePreFun preFun)
{
    sqstd::Array<Node *> nodeStack(sqstd::StackTempArenaAllocator::getInstance());
    nodeStack.resize(100);
    nodeStack.push(this);

    int i = 0;
    int size = 0;

    // 使用传统的递归方式会因为方法调用太深和栈内存占用太多造成性能问题，
    // 所以使用while循环的方式实现递归
    while (i >= 0)
    {

        Node *cur = *nodeStack.get(i);
        --i;
        char next = preFun(cur);

        // 表示跳过接下来所有的节点的遍历
        if (next == 0)
        {
            break;
        }

        // 表示只跳过当前节点的子节点
        if (next == 1)
        {
            continue;
        }

        const std::vector<Node *> &children = cur->getChildren();
        size = children.size();
        if (size > 0)
        {
            // 注意顺序，正常思路是按照子节点在children中的顺序处理
            // 所以第一个节点应该放在最后
            for (int j = size - 1; j >= 0; --j)
            {
                ++i;

                if (i >= nodeStack.getCount())
                {
                    nodeStack.Add();
                }

                nodeStack.set(i, children[j]);
            }
        }
    }
}

void Node::addComponent2(Component *component)
{
    component->node = this;
    components.push_back(component);

    RenderComponent *render = dynamic_cast<RenderComponent *>(component);
    if (render)
    {
        renderComponents.push_back(render);
        render->updateNodeRenderOrder();
    }
    else
    {
        Transform2DComponent *t = dynamic_cast<Transform2DComponent *>(component);
        if (t)
        {
            transform = t;
        }
    }

    component->onAwake();
    eventProcess.fire(NodeEventType::COMPONENT_ADD);
    NodeTreeManager::getInstnace()->addInhierChangeNode(this);
}

void Node::removeRenderComponent(Component *c)
{
    for (int n = 0; n < renderComponents.size(); ++n)
    {
        if (renderComponents[n] == c)
        {
            renderComponents.erase(renderComponents.begin() + n);
            break;
        }
    }
}

void Node::removeComponent(Component *c)
{
    bool remove = false;
    for (int i = 0; i < components.size(); ++i)
    {
        if (components[i] == c)
        {
            removeRenderComponent(c);
            c->onDestroy();
            delete c;
            components.erase(components.begin() + i);
            remove = true;
            break;
        }
    }

    if (remove)
        eventProcess.fire(NodeEventType::COMPONENT_ADD);
}

void Node::on(NodeEventType event, EVENT_CALL call)
{
    eventProcess.on(event, call);
}

void Node::off(NodeEventType event, EVENT_CALL removeCall)
{
    eventProcess.off(event, removeCall);
}

void Node::off(NodeEventType event)
{
    eventProcess.off(event);
}

void Node::dispatchEvent(Event &event)
{
    InputManager::getInstance()->onNodeDispatcher(event, this);
}

Node::~Node()
{

    // printf("destroy %d \n", nativeId);

    for (int i = 0; i < components.size(); ++i)
    {
        components[i]->onDestroy();
        delete components[i];
    }
    components.clear();
    children.clear();
    eventProcess.destroy();
}

void Node::destroy2()
{
    for (int i = 0; i < children.size(); ++i)
    {
        children[i]->destroy2();
    }

    delete this;
}

void Node::destroy()
{
    NodeTreeManager::getInstnace()->removeInhierChangeNode(this);
    if (parent)
    {
        parent->removeChild(this);
        parent = NULL;
    }
    destroy2();
}