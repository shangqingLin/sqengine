#pragma once
#include "../../../bindings/binding.h"

class MeshGraphicsBridge : public bridge::JsToNativeObjectBridge
{
public:
    virtual void processDispatch(ArrayBuffer &buffer, int nativeId, unsigned int op, bridge::ObjectType type);
};

class IndexBufferGraphicsBridge : public bridge::JsToNativeObjectBridge
{
public:
    virtual void processDispatch(ArrayBuffer &buffer, int nativeId, unsigned int op, bridge::ObjectType type);
};

class VertexBufferGraphicsBridge : public bridge::JsToNativeObjectBridge
{
public:
    virtual void processDispatch(ArrayBuffer &buffer, int nativeId, unsigned int op, bridge::ObjectType type);
};
