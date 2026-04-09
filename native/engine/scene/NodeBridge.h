#pragma once
#include "../../bindings/binding.h"
#include "../../engine/scene/Node.h"
#include <unordered_map>
namespace bridge
{

    typedef void (*NODE_PROCESS_COMPONENT_FUN)(ComponentType, char, char, ArrayBuffer &, Node *);
    class NodeBridge : public JsToNativeObjectBridge
    {
    protected:
        std::unordered_map<ComponentType, NODE_PROCESS_COMPONENT_FUN> processComponentMap;
        void dispatchProcessComponent(unsigned int &nodeOp, ArrayBuffer &buffer, Node *node);
    public:
        static NodeBridge *getInstance();
        NodeBridge();
        void registerNodeProcessComponent(ComponentType, NODE_PROCESS_COMPONENT_FUN);
        void processComponent(ComponentType, char, char, ArrayBuffer &, Node *);
        virtual void processDispatch(ArrayBuffer &buffer, int nativeId, unsigned int op, ObjectType type);
    };
}
