#pragma once
#include "../../bindings/binding.h"

class TweenObjectBridge : public bridge::JsToNativeObjectBridge
{
public:
    virtual void processDispatch(ArrayBuffer &buffer, int nativeId, unsigned int op, bridge::ObjectType type);
};
