#include "SpriteCustomMeshAssembly.h"
#include "../../../render/vertex-format.h"
#include "../../../render/BatcherRenderData.h"
#include "../../Transform2DComponent.h"

static SpriteCustomMeshAssembly *instance = nullptr;

SpriteCustomMeshAssembly *SpriteCustomMeshAssembly::getInstance()
{
    if (instance == nullptr)
    {
        instance = new SpriteCustomMeshAssembly();
    }
    return instance;
}

void SpriteCustomMeshAssembly::requestRenderData(SpriteComponent *) {}
void SpriteCustomMeshAssembly::destroyRenderData(SpriteComponent *sprite)
{
    if (sprite->vertexData)
    {
        sprite->vertexData->recycle();
        sprite->vertexData = nullptr;
        sprite->mesh = nullptr;
    }
}
void SpriteCustomMeshAssembly::updateTextureIndex(SpriteComponent *sprite, int textureIndex)
{
    SQ_ASSERT(textureIndex != -1);
    sqstd::ByteBlockChunk *chunk = sprite->vertexData;
    Mesh *mesh = sprite->getMesh();
    VertexBuffer *vertexBuffer = mesh->getVertexBufer(0);
    vertexBuffer->setDirty(true);
    int offset = 20;
    float fTextureIndex = (float)textureIndex;
    for (int i = 0; i < sprite->vertexRenderData.vertexNum; ++i)
    {
        chunk->buffer.setValue<float>(offset, fTextureIndex);
        offset += mesh->getVertexStride(0);
    }
}
void SpriteCustomMeshAssembly::updateColor(SpriteComponent *sprite)
{
    Mesh *mesh = sprite->getMesh();
    VertexBuffer *vertexBuffer = mesh->getVertexBufer(0);
    vertexBuffer->setDirty(true);

    sqstd::ByteBlockChunk *chunk = sprite->vertexData;

    int offset = 16;
    unsigned int color = sprite->getColorNum();
    for (int i = 0; i < sprite->vertexRenderData.vertexNum; ++i)
    {
        chunk->buffer.setValue<unsigned int>(offset, color);
        offset += mesh->getVertexStride(0);
    }
}
void SpriteCustomMeshAssembly::updateWorldVerts(SpriteComponent *sprite)
{

    sqstd::ByteBlockChunk *chunk = sprite->vertexData;
    Mesh *mesh = sprite->getMesh();
    VertexBuffer *vertexBuffer = mesh->getVertexBufer(0);
    vertexBuffer->setDirty(true);

    Transform2DComponent *transform = sprite->node->getComponent<Transform2DComponent>();
    Mat3 &worldMatrix = transform->getWorldTransform();
    
    float *dataList = sprite->vertexRenderData.customVertexList != nullptr ? sprite->vertexRenderData.customVertexList : sprite->vertexRenderData.vertexList;
    Vec2 point;
    int offset = 0;
    for (int i = 0; i < sprite->vertexRenderData.vertexNum * 2; i += 2)
    {
        point.set(dataList[i], dataList[i + 1]);
        worldMatrix.transformPoint(point, point);
        chunk->buffer.setValue<float>(offset, point.x);
        chunk->buffer.setValue<float>(offset + 4, point.y);

        //   if(sprite->texture->getKeyUrl() == "map/level/level_2/ziyan_bg_jj")
        // printf("==================Sprite point %d (%f %f )\n",i,point.x,point.y);

        offset += mesh->getVertexStride(0);
    }
}

void SpriteCustomMeshAssembly::fillChunkIndices(SpriteComponent *sprite)
{
    Mesh *mesh = sprite->getMesh();
    VertexBuffer *vertexBuffer = mesh->getVertexBufer(0);
    sqstd::ByteBlockChunk *chunk = sprite->vertexData;
    unsigned short vertexOffset = chunk->offset / mesh->getVertexStride(0);
    unsigned short index = vertexOffset;

    // if (sprite->texture->getKeyUrl() == "map/level/level_2/ziyan_bg_jj")
    //    printf("=commitSprite %s vertexOffset %d \n", sprite->texture->getKeyUrl().c_str(), vertexOffset);

    IndexBuffer &ib = (*mesh->getIndexBuffer());

    for (int i = 0; i < sprite->vertexRenderData.customIndexNum; ++i)
    {
        ib.append<unsigned short>(sprite->vertexRenderData.customIndexList[i] + vertexOffset);
    }
}

void SpriteCustomMeshAssembly::fillBuffers(SpriteComponent *sprite, int textureIndex)
{
    // printf("fillBuffers sprite %p, textureIndex %d \n", sprite, textureIndex);
    if (sprite->vertexRenderData.cacheFlagChangedVersion != sprite->node->getFlagChangeVersion())
    {
        sprite->vertexRenderData.cacheFlagChangedVersion = sprite->node->getFlagChangeVersion();
        updateWorldVerts(sprite);
    }

    if (sprite->vertexRenderData.dirtyColor())
    {
        sprite->vertexRenderData.clearDirtyColor();
        updateColor(sprite);
    }

    if (textureIndex != sprite->vertexRenderData.cacheTextureIndex)
    {
        sprite->vertexRenderData.cacheTextureIndex = textureIndex;
        updateTextureIndex(sprite, textureIndex);
    }
    fillChunkIndices(sprite);
}

void SpriteCustomMeshAssembly::updateMesh(SpriteComponent *sprite,const float *uv)
{

    if (sprite->vertexData != nullptr)
    {
        int vertexByteLength = getAttributeStride(vfmtPosUvColor);
        int needBytes = vertexByteLength * sprite->vertexRenderData.vertexNum;

        if (sprite->vertexData->length < needBytes)
        {
            sprite->vertexData->recycle();
            BatcherRenderData::getInstance()->allocateMeshChunk(vfmtPosUvColor, sprite->vertexRenderData.vertexNum, sprite->mesh, sprite->vertexData);
        }
    }
    else
    {
        BatcherRenderData::getInstance()->allocateMeshChunk(vfmtPosUvColor, sprite->vertexRenderData.vertexNum, sprite->mesh, sprite->vertexData);
    }

    SQ_ASSERT(sprite->mesh && sprite->vertexData);

    {
        Mesh *mesh = sprite->getMesh();
        VertexBuffer *vertexBuffer = mesh->getVertexBufer(0);
        vertexBuffer->setDirty(true);
        sqstd::ByteBlockChunk *chunk = sprite->vertexData;
        int offset = 8;
        for (int i = 0; i < sprite->vertexRenderData.vertexNum * 2; i += 2)
        {
            chunk->buffer.setValue<float>(offset, uv[i]);
            chunk->buffer.setValue<float>(offset + 4, uv[i + 1]);
            offset += mesh->getVertexStride(0);
        }
    }
}