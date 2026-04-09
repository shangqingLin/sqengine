#include "BuildInMeshDataManager.h"
#include "../scene/graphics/IndexBuffer.h"
#include "../scene/graphics/VertexBuffer.h"

using namespace pipeline;

static BuildInMeshDataManager *_ins = new BuildInMeshDataManager();

std::vector<Attribute> BuildInMeshDataManager::posUvVertexAttributes;
Attribute BuildInMeshDataManager::posVertexAttributes;

BuildInMeshDataManager *BuildInMeshDataManager::getIntance()
{
    return _ins;
}

BuildInMeshDataManager::BuildInMeshDataManager()
    : posRectMesh{nullptr}, posUvRectMesh{nullptr}, screenMesh{nullptr}, drawQuatIndex{nullptr}
{
}

IndexBuffer *BuildInMeshDataManager::getDrawQuatIndex()
{
    if (!drawQuatIndex)
    {
        drawQuatIndex = new IndexBuffer(BufferUsage::STATIC_DRAW, 6);
        drawQuatIndex->append<unsigned short>(0);
        drawQuatIndex->append<unsigned short>(1);
        drawQuatIndex->append<unsigned short>(2);
        drawQuatIndex->append<unsigned short>(0);
        drawQuatIndex->append<unsigned short>(2);
        drawQuatIndex->append<unsigned short>(3);
        drawQuatIndex->setDirty(true);
    }
    return drawQuatIndex;
}

Mesh *BuildInMeshDataManager::getPosRectMesh()
{
    if (!posRectMesh)
    {
        Mesh *mesh = new Mesh();
        posRectMesh = mesh;
        Attribute &pos = (Attribute &)BuildInMeshDataManager::posVertexAttributes;
        pos.name = "aPos";
        pos.format = Format::RG32F;
        pos.custom = {
            0,
            2,
            8,
            Format::RG32F};
        VertexBuffer *vertexBuffer = new VertexBuffer(BufferUsage::STATIC_DRAW);
        vertexBuffer->resizeBuffer(4 * getAttributeStride(pos), true);

        // 大小为100的矩形
        float w = 50.f, h = 50.f;
        vertexBuffer->append(-w);
        vertexBuffer->append(-h);

        vertexBuffer->append(w);
        vertexBuffer->append(-h);

        vertexBuffer->append(w);
        vertexBuffer->append(h);

        vertexBuffer->append(-w);
        vertexBuffer->append(h);

        vertexBuffer->setDirty(true);

        IndexBuffer *indexBuffer = getDrawQuatIndex();
        mesh->addVertextBuffer(vertexBuffer, pos);
        mesh->setIndexBuffer(indexBuffer);
    }

    return posRectMesh;
}

VertexBuffer *BuildInMeshDataManager::getPosVertexBuffer()
{
    getPosRectMesh();
    return posRectMesh->getVertexBufer(0);
}

IndexBuffer *BuildInMeshDataManager::getPosIndexBuffer()
{
    getPosRectMesh();
    return posRectMesh->getIndexBuffer();
}

Mesh *BuildInMeshDataManager::getPosUvRectMesh()
{

    if (!posUvRectMesh)
    {
        Mesh *mesh = new Mesh();
        posUvRectMesh = mesh;

        AttributeCustomSetting aPos = {
            0,
            2,
            16,
            Format::RG32F};
        AttributeCustomSetting aUV = {
            8,
            2,
            16,
            Format::RG32F};
        Attribute pos = {"aPos", Format::RG32F, aPos};
        Attribute uv = {"aUv", Format::RG32F, aUV};
        std::vector<Attribute> &attributes = (std::vector<Attribute> &)BuildInMeshDataManager::posUvVertexAttributes;
        attributes.reserve(2);
        attributes.push_back(pos);
        attributes.push_back(uv);
        VertexBuffer *vertexBuffer = new VertexBuffer(BufferUsage::STATIC_DRAW);
        vertexBuffer->resizeBuffer(4 * getAttributeStride(attributes), true);

        // 大小为100的矩形
        float w = 50.f, h = 50.f;

        vertexBuffer->append(-w);
        vertexBuffer->append(-h);
        vertexBuffer->append(0.f);
        vertexBuffer->append(0.f);

        vertexBuffer->append(w);
        vertexBuffer->append(-h);
        vertexBuffer->append(1.f);
        vertexBuffer->append(0.f);

        vertexBuffer->append(w);
        vertexBuffer->append(h);
        vertexBuffer->append(1.f);
        vertexBuffer->append(1.f);

        vertexBuffer->append(-w);
        vertexBuffer->append(h);
        vertexBuffer->append(0.f);
        vertexBuffer->append(1.f);
        vertexBuffer->setDirty(true);

        IndexBuffer *indexBuffer = getDrawQuatIndex();
        mesh->addVertextBuffer(vertexBuffer, attributes);
        mesh->setIndexBuffer(indexBuffer);
        
    }
    return posUvRectMesh;
}

VertexBuffer *BuildInMeshDataManager::getPosUvVertexBuffer()
{
    getPosUvRectMesh();
    return posUvRectMesh->getVertexBufer(0);
}

IndexBuffer *BuildInMeshDataManager::getPosUvIndexBuffer()
{
    getPosUvRectMesh();
    return posUvRectMesh->getIndexBuffer();
}

Mesh *BuildInMeshDataManager::getScreenMesh()
{
    if (!screenMesh)
    {
        Mesh *mesh = new Mesh();
        screenMesh = mesh;
        Attribute &pos = (Attribute &)BuildInMeshDataManager::posVertexAttributes;
        pos.name = "aPos";
        pos.format = Format::RG32F;
        pos.custom = {
            0,
            2,
            8,
            Format::RG32F};
        VertexBuffer *vertexBuffer = new VertexBuffer(BufferUsage::STATIC_DRAW);
        vertexBuffer->resizeBuffer(4 * getAttributeStride(pos), true);

        vertexBuffer->append(-1.f);
        vertexBuffer->append(-1.f);

        vertexBuffer->append(1.f);
        vertexBuffer->append(-1.f);

        vertexBuffer->append(1.f);
        vertexBuffer->append(1.f);

        vertexBuffer->append(-1.f);
        vertexBuffer->append(1.f);

        vertexBuffer->setDirty(true);

        IndexBuffer *indexBuffer = getDrawQuatIndex();
        mesh->addVertextBuffer(vertexBuffer, pos);
        mesh->setIndexBuffer(indexBuffer);
    }
    return screenMesh;
}