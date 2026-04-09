#include "GraphicsBridge.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "Mesh.h"
#include "MeshInstance.h"
#include "../../assets/AssetManager.h"

void MeshGraphicsBridge::processDispatch(ArrayBuffer &buffer, int nativeId, unsigned int op, bridge::ObjectType type)
{
    switch (op)
    {
    case 1:
    {
        Mesh *mesh = new Mesh();
        mesh->initialize(nativeId);
        break;
    }
    case 2:
    {
        Mesh *mesh = bridge::JsToNativeObjectManager::getInstance()->getById<Mesh>(nativeId);
        if (mesh)
        {
            delete mesh;
        }
        break;
    }
    case 3:
    {
        Mesh *mesh = bridge::JsToNativeObjectManager::getInstance()->getById<Mesh>(nativeId);
        VertexBuffer *vertexBuffer = bridge::JsToNativeObjectManager::getInstance()->getById<VertexBuffer>(*buffer.popp<int>());
        SQ_ASSERT(mesh && vertexBuffer);

        int count = *buffer.popp<int>();
        std::vector<Attribute> attributes;
        attributes.assign(count, Attribute());
        for (int i = 0; i < count; ++i)
        {
            Attribute &a = attributes[i];
            buffer.poppUTFStringg(a.name);
            a.format = Format(*buffer.popp<int>());
            if (*buffer.popp<char>())
            {
                AttributeCustomSetting custom;
                custom.offset = *buffer.popp<int>();
                custom.count = *buffer.popp<int>();
                custom.stride = *buffer.popp<int>();
                custom.type = Format(*buffer.popp<int>());
                custom.instanceStride = *buffer.popp<int>();
                a.custom = custom;
            }
        }
        mesh->addVertextBuffer(vertexBuffer, attributes);
        break;
    }
    case 4:
    {
        Mesh *mesh = bridge::JsToNativeObjectManager::getInstance()->getById<Mesh>(nativeId);
        IndexBuffer *indexBuffer = bridge::JsToNativeObjectManager::getInstance()->getById<IndexBuffer>(*buffer.popp<int>());
        SQ_ASSERT(mesh && indexBuffer);
        mesh->setIndexBuffer(indexBuffer);
        break;
    }
    case 5:
    {

        break;
    }
    }
}

void IndexBufferGraphicsBridge::processDispatch(ArrayBuffer &buffer, int nativeId, unsigned int op, bridge::ObjectType type)
{
    switch (op)
    {
    case 1:
    {
        IndexBuffer *indexBuffer = new IndexBuffer();
        indexBuffer->initialize(nativeId);
        break;
    }
    case 2:
    {
        IndexBuffer *indexBuffer = bridge::JsToNativeObjectManager::getInstance()->getById<IndexBuffer>(nativeId);
        if (indexBuffer)
        {
            delete indexBuffer;
        }
        break;
    }
    case 3:
    {
        IndexBuffer *indexBuffer = bridge::JsToNativeObjectManager::getInstance()->getById<IndexBuffer>(nativeId);
        SQ_ASSERT(indexBuffer);
        indexBuffer->setDirty(true);
        break;
    }
    case 4:
    {
        IndexBuffer *indexBuffer = bridge::JsToNativeObjectManager::getInstance()->getById<IndexBuffer>(nativeId);
        SQ_ASSERT(indexBuffer);
        indexBuffer->resizeBufferFromJs(*buffer.popp<int>());
        break;
    }
    }
}

void VertexBufferGraphicsBridge::processDispatch(ArrayBuffer &buffer, int nativeId, unsigned int op, bridge::ObjectType type)
{
    switch (op)
    {
    case 1:
    {
        VertexBuffer *vertexBuffer = new VertexBuffer();
        vertexBuffer->initialize(nativeId);
        break;
    }
    case 2:
    {
        VertexBuffer *vertexBuffer = bridge::JsToNativeObjectManager::getInstance()->getById<VertexBuffer>(nativeId);
        SQ_ASSERT(vertexBuffer);
        delete vertexBuffer;
        break;
    }
    case 3:
    {
        VertexBuffer *vertexBuffer = bridge::JsToNativeObjectManager::getInstance()->getById<VertexBuffer>(nativeId);
        SQ_ASSERT(vertexBuffer);
        vertexBuffer->setDirty(true);
        break;
    }
    case 4:
    {
        VertexBuffer *vertexBuffer = bridge::JsToNativeObjectManager::getInstance()->getById<VertexBuffer>(nativeId);
        SQ_ASSERT(vertexBuffer);
        vertexBuffer->resizeBufferFromJs(*buffer.popp<int>());
        break;
    }
    }
}
