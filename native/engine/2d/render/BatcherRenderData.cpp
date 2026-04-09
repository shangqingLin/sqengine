#include "BatcherRenderData.h"
#include "../../core/base/config.h"

static BatcherRenderData *_inst = NULL;
BatcherRenderData *BatcherRenderData::getInstance()
{
    if (!_inst)
    {
        _inst = new BatcherRenderData();
    }
    return _inst;
}

void BatcherRenderData::allocateMeshChunk(const std::vector<Attribute> &vertexFormat, int needVertexNum, Mesh *&mesh, sqstd::ByteBlockChunk *&chunk)
{
    int vertexByteLength = getAttributeStride(vertexFormat);
    int needBytes = vertexByteLength * needVertexNum;
    mesh = getMesh(vertexFormat, needVertexNum);
    chunk = mesh->getVertexBufer(0)->getByteBlock()->allocateChunk(needBytes);
}

Mesh *BatcherRenderData::getMesh(const std::vector<Attribute> &vertexFormat, int numVertices)
{
    int vertexNum = DeviceManager::getInstance()->device->deviceCaps->getMaxVBOVertices();
    SQ_ASSERT(numVertices <= vertexNum);

    vertexNum = std::max(vertexNum, numVertices);

    // printf(">>>>>> getMesh %d  %d \n",numVertices,vertexNum);
    int vertexFromatBytes = getAttributeStride(vertexFormat);
    int needBytes = vertexFromatBytes * numVertices;
    std::map<int, Mesh *>::iterator it = _bufferAccessors.find(vertexFromatBytes);
    Mesh *bigMesh = nullptr;
    bool needAllocate = false;
    if (it == _bufferAccessors.end())
    {
        needAllocate = true;
    }
    else
    {
        bigMesh = it->second;
        VertexBuffer *vertxBuffer = bigMesh->getVertexBufer(0);
        sqstd::ByteBlock *bufferView = vertxBuffer->getByteBlock();
        if (!bufferView->hasEnoughSpace(needBytes))
        {
            needAllocate = true;
        }
    }
    if (needAllocate)
    {
        int indexNum = vertexNum << 4; // vertexNum*4
        bigMesh = allocateMesh(vertexFormat, vertexNum, indexNum);
    }
    return bigMesh;
}

Mesh *BatcherRenderData::allocateMesh(const std::vector<Attribute> &vertexFormat, int numVertices, int numIndices)
{
    int vertexFromatBytes = getAttributeStride(vertexFormat);
    Mesh *mesh = new Mesh();
    VertexBuffer *vertex = new VertexBuffer();
    mesh->addVertextBuffer(vertex);
    mesh->setVertexAttributeDscriptor(vertexFormat, 0);
    vertex->resizeBuffer(mesh->getVertexStride(0) * numVertices,true);

    IndexBuffer *indexBuffer = new IndexBuffer(numIndices);
    mesh->setIndexBuffer(indexBuffer);
    _bufferAccessors[vertexFromatBytes] = mesh;
    return mesh;
}

void BatcherRenderData::reset()
{
    std::map<int, Mesh *>::iterator it = _bufferAccessors.begin();
    while (it != _bufferAccessors.end())
    {
        it->second->getIndexBuffer()->clearData();
        ++it;
    }
}