#pragma once
#include "./Camera.h"
#include "./graphics/Mesh.h"
#include "../assets/Material.h"
#include "../../bindings/binding.h"


class CommandBuffer : public bridge::JsToNativeObject
{
private:
    
public:
        void drawMesh(Mesh*,Material*);
        void stepUpCamera(Camera*);
        void render();
};

class CommandBufferBridge : public bridge::JsToNativeObjectBridge
{
public:
    virtual void processDispatch(ArrayBuffer& buffer,int nativeId,unsigned int op, bridge::ObjectType type);
};    
