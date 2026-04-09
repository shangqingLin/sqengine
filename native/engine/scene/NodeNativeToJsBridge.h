
#include "../../bindings/binding.h"

class NodeNativeToJsBridge : public bridge::NativeToJsObject
{
private:
    
public:
    static void initialize();
    static NodeNativeToJsBridge* getInstance();
    NodeNativeToJsBridge(bridge::NativeObjectType type);
};
