#include "JsToNativeObject.h"
#include "JsToNativeObjectManager.h"

using namespace bridge;

JsToNativeObject::JsToNativeObject() : nativeId(-1) {}

void JsToNativeObject::initialize(int id)
{
    nativeId = id;
    JsToNativeObjectManager::getInstance()->add(this);
}

JsToNativeObject::~JsToNativeObject()
{
    if (nativeId != -1)
    {
        JsToNativeObjectManager::getInstance()->remove(this);
        nativeId = -1;
    }
};