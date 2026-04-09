#include "SpriteBarFilledAssembly.h"
#include "../SpriteComponent.h"
#include "../../../render/vertex-format.h"
#include "../../../render/BatcherRenderData.h"
#include "../../Transform2DComponent.h"

static SpriteBarFilledAssembly *_inst = nullptr;
SpriteBarFilledAssembly *SpriteBarFilledAssembly::getInstance()
{
    if (!_inst)
    {
        _inst = new SpriteBarFilledAssembly();
    }
    return _inst;
}

void SpriteBarFilledAssembly::updateRenderData(SpriteComponent *sprite)
{
    float fillStart = sprite->fillStart;
    float fillRange = sprite->fillRange;
    if (fillRange < 0)
    {
        fillStart += fillRange;
        fillRange = -fillRange;
    }

    fillRange = fillStart + fillRange;
    fillStart = fillStart > 1.0 ? 1.0 : fillStart;
    fillStart = fillStart < 0.0 ? 0.0 : fillStart;
    fillRange = fillRange > 1.0 ? 1.0 : fillRange;
    fillRange = fillRange < 0.0 ? 0.0 : fillRange;
    fillRange -= fillStart;
    fillRange = fillRange < 0 ? 0 : fillRange;
    float fillEnd = fillStart + fillRange;
    fillEnd = fillEnd > 1 ? 1 : fillEnd;

    /**
     * 实现裁剪的原理其实就是缩小uv和顶点，达到只显示某一个区域内的图片，从而达到裁剪的目的
     */
    updateVertexData(sprite, fillStart, fillEnd);
    updateUv(sprite, fillStart, fillEnd);
}

void SpriteBarFilledAssembly::updateVertexData(SpriteComponent *sprite, float fillStart, float fillEnd)
{
    float width = sprite->getWidth();
    float height = sprite->getHeight();
    float offsetX = sprite->getOffsetX();
    float offsetY = sprite->getOffsetY();

    SQ_ASSERT(width > 0);
    SQ_ASSERT(height > 0);

    // printf("SpriteBarFilledAssembly Sprite size (%f %f ) offset (%f %f)\n",width,height,offsetX,offsetY);

    float l = -offsetX;
    float r = width - offsetX;
    float b = -offsetY;
    float t = height - offsetY;

    float progressStart = 0;
    float progressEnd = 0;
    switch (sprite->fillType)
    {
    case FillType::HORIZONTAL:
        progressStart = l + (r - l) * fillStart; // 线性插值
        progressEnd = l + (r - l) * fillEnd;

        l = progressStart;
        r = progressEnd;
        break;
    case FillType::VERTICAL:
        progressStart = b + (t - b) * fillStart;
        progressEnd = b + (t - b) * fillEnd;

        b = progressStart;
        t = progressEnd;
        break;
    default:
        SQ_ASSERT(false);
    }

    float *dataList = sprite->vertexRenderData.vertexList;
    dataList[0] = l;
    dataList[1] = b;

    dataList[2] = r;
    dataList[3] = b;

    dataList[4] = l;
    dataList[5] = t;

    dataList[6] = r;
    dataList[7] = t;
}

void SpriteBarFilledAssembly::updateUv(SpriteComponent *sprite, float fillStart, float fillEnd)
{
    SpriteFrame &spriteFrame = sprite->spriteFrame.value();

    int atlasWidth = spriteFrame.texture->getWidth();
    int atlasHeight = spriteFrame.texture->getHeight();
    const Rect<float> &textureRect = spriteFrame.rect;

    float ul = 0;
    float vb = 0;
    float ur = 0;
    float vt = 0;
    float quadUV0 = 0;
    float quadUV1 = 0;
    float quadUV2 = 0;
    float quadUV3 = 0;
    float quadUV4 = 0;
    float quadUV5 = 0;
    float quadUV6 = 0;
    float quadUV7 = 0;

    if (spriteFrame.rotate)
    {
        ul = (textureRect.x) / atlasWidth;
        vb = (textureRect.y + textureRect.width) / atlasHeight;
        ur = (textureRect.x + textureRect.height) / atlasWidth;
        vt = (textureRect.y) / atlasHeight;

        quadUV0 = quadUV2 = ul;
        quadUV4 = quadUV6 = ur;
        quadUV3 = quadUV7 = vb;
        quadUV1 = quadUV5 = vt;
    }
    else
    {
        ul = (textureRect.x) / atlasWidth;
        vb = (textureRect.y + textureRect.height) / atlasHeight;
        ur = (textureRect.x + textureRect.width) / atlasWidth;
        vt = (textureRect.y) / atlasHeight;

        quadUV0 = quadUV4 = ul;
        quadUV2 = quadUV6 = ur;
        quadUV1 = quadUV3 = vb;
        quadUV5 = quadUV7 = vt;
    }

    float *uvData = sprite->spriteFrame.value().uv;
    switch (sprite->fillType)
    {
    case FillType::HORIZONTAL:
        uvData[0] = quadUV0 + (quadUV2 - quadUV0) * fillStart;
        uvData[1] = quadUV1 + (quadUV3 - quadUV1) * fillStart;
        uvData[2] = quadUV0 + (quadUV2 - quadUV0) * fillEnd;
        uvData[3] = quadUV1 + (quadUV3 - quadUV1) * fillEnd;
        uvData[4] = quadUV4 + (quadUV6 - quadUV4) * fillStart;
        uvData[5] = quadUV5 + (quadUV7 - quadUV5) * fillStart;
        uvData[6] = quadUV4 + (quadUV6 - quadUV4) * fillEnd;
        uvData[7] = quadUV5 + (quadUV7 - quadUV5) * fillEnd;
        break;
    case FillType::VERTICAL:
        uvData[0] = quadUV0 + (quadUV4 - quadUV0) * fillStart;
        uvData[1] = quadUV1 + (quadUV5 - quadUV1) * fillStart;
        uvData[2] = quadUV2 + (quadUV6 - quadUV2) * fillStart;
        uvData[3] = quadUV3 + (quadUV7 - quadUV3) * fillStart;
        uvData[4] = quadUV0 + (quadUV4 - quadUV0) * fillEnd;
        uvData[5] = quadUV1 + (quadUV5 - quadUV1) * fillEnd;
        uvData[6] = quadUV2 + (quadUV6 - quadUV2) * fillEnd;
        uvData[7] = quadUV3 + (quadUV7 - quadUV3) * fillEnd;
        break;
    default:
        SQ_ASSERT(false);
    }
}

void SpriteBarFilledAssembly::requestRenderData(SpriteComponent *sprite)
{
    BatcherRenderData::getInstance()->allocateMeshChunk(vfmtPosUvColor, 4, sprite->mesh, sprite->vertexData);
    SQ_ASSERT(sprite->mesh && sprite->vertexData);
}

void SpriteBarFilledAssembly::destroyRenderData(SpriteComponent *sprite)
{

    if (sprite->vertexData)
    {
        sprite->vertexData->recycle();
        sprite->vertexData = nullptr;
        sprite->mesh = nullptr;
    }
}

void SpriteBarFilledAssembly::updateWorldVerts(SpriteComponent *sprite)
{

    sqstd::ByteBlockChunk *chunk = sprite->vertexData;
    Mesh *mesh = sprite->getMesh();
    VertexBuffer *vertexBuffer = mesh->getVertexBufer(0);
    vertexBuffer->setDirty(true);

    Transform2DComponent *transform = sprite->node->getComponent<Transform2DComponent>();
    Mat3 &worldMatrix = transform->getWorldTransform();
    float *dataList = sprite->vertexRenderData.vertexList;
    Vec2 point;
    int offset = 0;
    for (int i = 0; i < 8; i += 2)
    {
        point.set(dataList[i], dataList[i + 1]);
        worldMatrix.transformPoint(point, point);
        chunk->buffer.setValue<float>(offset, point.x);
        chunk->buffer.setValue<float>(offset + 4, point.y);
        offset += mesh->getVertexStride(0);
    }
}

void SpriteBarFilledAssembly::updateFillUVs(SpriteComponent *sprite)
{

    Mesh *mesh = sprite->getMesh();
    VertexBuffer *vertexBuffer = mesh->getVertexBufer(0);
    vertexBuffer->setDirty(true);
    sqstd::ByteBlockChunk *chunk = sprite->vertexData;
    const float *uv = sprite->spriteFrame.value().uv;
    SQ_ASSERT(uv);
    int offset = 8;
    // printf("++updateUVs\n");
    for (int i = 0; i < 8; i += 2)
    {
        chunk->buffer.setValue<float>(offset, uv[i]);
        chunk->buffer.setValue<float>(offset + 4, uv[i + 1]);
        offset += mesh->getVertexStride(0);
    }
}

void SpriteBarFilledAssembly::updateColor(SpriteComponent *sprite)
{
    Mesh *mesh = sprite->getMesh();
    VertexBuffer *vertexBuffer = mesh->getVertexBufer(0);
    vertexBuffer->setDirty(true);

    sqstd::ByteBlockChunk *chunk = sprite->vertexData;

    int offset = 16;
    unsigned int color = sprite->getColorNum();
    for (int i = 0; i < 4; ++i)
    {
        chunk->buffer.setValue<unsigned int>(offset, color);
        offset += mesh->getVertexStride(0);
    }
}

void SpriteBarFilledAssembly::updateTextureIndex(SpriteComponent *sprite, int textureIndex)
{
    SQ_ASSERT(textureIndex != -1);
    sqstd::ByteBlockChunk *chunk = sprite->vertexData;
    Mesh *mesh = sprite->getMesh();
    VertexBuffer *vertexBuffer = mesh->getVertexBufer(0);
    vertexBuffer->setDirty(true);

    int offset = 20;
    // 顶点着色器对int支持不好，使用float
    float fTextureIndex = (float)textureIndex;
    for (int i = 0; i < 4; ++i)
    {
        chunk->buffer.setValue<float>(offset, fTextureIndex);
        offset += mesh->getVertexStride(0);
    }
}

void SpriteBarFilledAssembly::fillChunkIndices(SpriteComponent *sprite)
{
    Mesh *mesh = sprite->getMesh();
    VertexBuffer *vertexBuffer = mesh->getVertexBufer(0);
    sqstd::ByteBlockChunk *chunk = sprite->vertexData;
    unsigned short vertexOffset = chunk->offset / mesh->getVertexStride(0);
    unsigned short index = vertexOffset;

    IndexBuffer &ib = (*mesh->getIndexBuffer());

    // 0
    ib.append<unsigned short>(index);

    // 1
    index = vertexOffset + 1;
    ib.append<unsigned short>(index);

    // 2
    index = vertexOffset + 2;
    ib.append<unsigned short>(index);

    // 1
    index = vertexOffset + 1;
    ib.append<unsigned short>(index);

    // 3
    index = vertexOffset + 3;
    ib.append<unsigned short>(index);

    // 2
    index = vertexOffset + 2;
    ib.append<unsigned short>(index);
}

void SpriteBarFilledAssembly::fillBuffers(SpriteComponent *sprite, int textureIndex)
{
    if (sprite->vertexRenderData.dirtyVertex())
    {
        sprite->vertexRenderData.clearDirtyVertex();
        updateRenderData(sprite);
        updateFillUVs(sprite);
    }

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