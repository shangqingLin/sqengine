#include "NodeNativeToJsBridge.h"

static NodeNativeToJsBridge *_ins = nullptr;

void NodeNativeToJsBridge::initialize()
{
    _ins = new NodeNativeToJsBridge(bridge::NativeObjectType::Node);
}

NodeNativeToJsBridge::NodeNativeToJsBridge(bridge::NativeObjectType type) : NativeToJsObject(type) {}

NodeNativeToJsBridge *NodeNativeToJsBridge::getInstance()
{
    return _ins;
}