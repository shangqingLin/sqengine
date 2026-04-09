#pragma once
#include <vector>
#include "Layers.h"
#include "NodeEventProcess.h"
#include "../../bindings/binding.h"
#include "PoolManager.h"

class Component;
class TransformBaseComponent;
class Transform2DComponent;
class Node;
class RenderComponent;
class Scene;

char upateFromParent(Node *node);
char updateLayer(Node *node);
typedef std::function<char(Node *)> walkNodePreFun;

class Node : public bridge::JsToNativeObject, PoolObject<Node>
{
private:
    static int globalFlagChangeVersion;

    /**
     * 要想在场景中隐藏节点，remove方法太重度了
     * 可以将此变量设置为false，这个比较轻度，不会执行任何的逻辑，单纯不显示而已
     */
    bool visible;
    int flagChangeVersion;

    /**
     * NodeFlag和TransformBit
     */
    int changedFlags;

    Layers layer;
    Layers realLayer;
    bool layerDirty;

    NodeEventProcess eventProcess;

    virtual void onSetParent(Node *parent);
    friend char updateLayer(Node *node);
    friend char upateFromParent(Node *node);

    void addComponent2(Component *);
    void removeRenderComponent(Component *);
    void destroy2();
    void _updateScene();

protected:
    bool _activeInHierarchy;
    Scene *scene;
    std::vector<Node *> children;
    std::vector<Component *> components;

    // 因为RenderComponent在渲染管线中频繁查找，所以为了减少查找消耗
    // 直接将RenderComponent划分为一组
    std::vector<RenderComponent *> renderComponents;

    /**
     * 作为子节点时，在父节点中位于第几个位置
     */
    int siblingIndex;

    virtual void _onHierarchyChanged();
    virtual void _onTransformChanged();
    void removeChildLightweight(Node *);

public:
    friend class Scene;
    friend class TransformBaseComponent;
    friend class Transform2DComponent;
    friend class NodeTreeManager;
    // int testNum = 0;
    // inline void setTestNum(int f) { testNum = f; };

    Node *parent;
    Transform2DComponent *transform;

    Node();
    virtual ~Node();

    template <typename T>
    T *addComponent()
    {
        T *component = getComponent<T>();
        if (component)
        {
            return component;
        }
        component = new T();
        addComponent2(component);
        return component;
    }

    template <typename T>
    void removeComponent()
    {
        bool remove = false;
        for (int i = 0; i < components.size(); ++i)
        {
            T *ptr = dynamic_cast<T *>(components[i]);
            if (ptr)
            {

                removeRenderComponent(ptr);
                ptr->onDestroy();
                delete ptr;
                components.erase(components.begin() + i);
                remove = true;
                break;
            }
        }

        if (remove)
            eventProcess.fire(NodeEventType::COMPONENT_ADD);
    }

    void removeComponent(Component *c);

    template <typename T>
    T *getComponent()
    {
        for (int i = 0; i < components.size(); ++i)
        {
            T *ptr = dynamic_cast<T *>(components[i]);
            if (ptr)
            {
                return ptr;
            }
        }
        return nullptr;
    }

    inline const std::vector<Component *> &getAllComponents()
    {
        return components;
    }

    inline const std::vector<RenderComponent *> &getRenderComponents() { return renderComponents; };
    void addChild(Node *node);
    void addChildAt(Node *node, int insertIndex);
    void removeChild(Node *node);
    void removeFromParent();

    /**
     * 查找首次具有指定的组件的子节点
     */
    template <typename T>
    Node *getChildHasComponent()
    {
        Node *g_result = nullptr;
        walkNodePreFun getNode = [&g_result](Node *node)
        {
            T *c = node->getComponent<T>();
            if (c)
            {
                g_result = node;
                return false;
            }
            return true;
        };

        walk(getNode);
        return g_result;
    }

    void walk(walkNodePreFun);
    const std::vector<Node *> &getChildren();
    Node *getChild(int index);
    Node *getParent();
    void setChangedFlags(int flag);
    bool hasChangedFags();
    inline int getChangeFlag() { return changedFlags; };
    inline int getFlagChangeVersion() { return flagChangeVersion; };
    static void resetHasChangedFlags();
    inline Scene *getScene() { return scene; };

    void destroy();
    void on(NodeEventType, EVENT_CALL);
    void off(NodeEventType);
    void off(NodeEventType, EVENT_CALL);
    void dispatchEvent(Event &);
    inline void setVisible(bool v) { visible = v; };
    inline bool getVisible() { return visible; };
    void setLayer(Layers layer);
    Layers getLayer();
    inline bool activeInHierarchy() { return _activeInHierarchy; };
    inline NodeEventProcess &getEventProcess() { return eventProcess; };
    inline int getSiblingIndex() { return siblingIndex; };
};
