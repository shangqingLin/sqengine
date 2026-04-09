#include "JsToNativeObjectManager.h"

using namespace bridge;

static JsToNativeObjectManager *_inst = nullptr;

JsToNativeObjectManager::JsToNativeObjectManager()
{
}

JsToNativeObjectManager *JsToNativeObjectManager::getInstance()
{
    if (!_inst)
    {
        _inst = new JsToNativeObjectManager();
    }
    return _inst;
}

void JsToNativeObjectManager::add(JsToNativeObject *node)
{
    nodeMap[node->nativeId] = node;
}

void JsToNativeObjectManager::remove(JsToNativeObject *node)
{
    std::unordered_map<int, JsToNativeObject *>::iterator it = nodeMap.find(node->nativeId);
    if (it != nodeMap.end())
    {
        nodeMap.erase(it);
    }
}

