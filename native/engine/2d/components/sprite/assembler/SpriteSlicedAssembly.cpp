#include "SpriteSlicedAssembly.h"
#include "../SpriteComponent.h"
#include "../../../render/vertex-format.h"
#include "../../../render/BatcherRenderData.h"
#include "../../Transform2DComponent.h"

/**
 *
 * 9宫格绘制其实就是将一张图划分为9块区域，这9块区域单独绘制（相对于绘制9个Sprite），
 * 只不过这9个区域的图片内容都是从一张图中获取，通过设置偏移中心多少的left、right、top、bottom从而模拟拉伸的效果，而不变形。
 *
 * 如下图所示9块区域
 *         left   right
    ┌───────┬───────┬───────┐
    │       │       │       │
    │   1   │   2   │   3   │
    │       │       │       │
    ├───────┼───────┼───────┤ top
    │       │   5   │       │
    │   4   │ center│   6   │
    │       │       │       │
    ├───────┼───────┼───────┤ bottom
    │       │       │       │
    │   7   │   8   │   9   │
    │       │       │       │
    └───────┴───────┴───────┘

   渲染原理：
    总共需要16个顶点，绘制9个区域。
    周边的1、2、3、4、6、7、8、9这8个区域渲染的大小保持你设置的大小，中间center拉伸
 */
static SpriteSlicedAssembly *instance = new SpriteSlicedAssembly;

SpriteSlicedAssembly *SpriteSlicedAssembly::getInstance()
{
    return instance;
}

void SpriteSlicedAssembly::requestRenderData(SpriteComponent *sprite)
{

    BatcherRenderData::getInstance()->allocateMeshChunk(vfmtPosUvColor, 16, sprite->mesh, sprite->vertexData);
    SQ_ASSERT(sprite->mesh && sprite->vertexData);
}

void SpriteSlicedAssembly::destroyRenderData(SpriteComponent *sprite)
{
    if (sprite->vertexData)
    {
        sprite->vertexData->recycle();
        sprite->vertexData = nullptr;
        sprite->mesh = nullptr;
    }
}

void SpriteSlicedAssembly::updateVertexData(SpriteComponent *sprite)
{
    float width = sprite->getWidth();
    float height = sprite->getHeight();
    float offsetX = sprite->getOffsetX();
    float offsetY = sprite->getOffsetY();

    SQ_ASSERT(width > 0);
    SQ_ASSERT(height > 0);

    Vec4 &grid9Setting = sprite->spriteFrame.value().grid9.value();
    float leftWidth = grid9Setting.x;
    float rightWidth = grid9Setting.y;
    float topHeight = grid9Setting.z;
    float bottomHeight = grid9Setting.w;

    float column[4];
    float row[4];

    float lwrw = leftWidth + rightWidth;
    float thbh = topHeight + bottomHeight;
    float centerWidth = width - lwrw;
    float centerHeight = height - thbh;

    // Grid的设置可以超出资源本身的大小，因此会有lwrw大于width的情况
    // 如果lwrw < width，则xScale > 1.0，表示使用grid9Setting.x 和 grid9Setting.y设置的大小,不进行缩放
    // 如果lwrw > width, 则xScale < 1.0，表示设置的Grid大于渲染的大小了，则 grid9Setting.x 和 grid9Setting.y 也需要相应缩小
    // 同理，对于高度也是一样

    float xScale = 1.0f, yScale = 1.0f;
    if (lwrw > 0)
    {
        xScale = width / lwrw;
        xScale = xScale > 1.f ? 1.f : xScale;
    }

    if (thbh > 0.)
    {
        yScale = height / thbh;
        yScale = yScale > 1.f ? 1.f : yScale;
    }

    // 也是因为Grid设置的大小超出了资源本身的大小
    centerWidth = centerWidth < 0 ? 0 : centerWidth;
    centerHeight = centerHeight < 0 ? 0 : centerHeight;

    column[0] = -offsetX;                     // 第一列
    column[1] = leftWidth * xScale - offsetX; // 第二列
    column[2] = column[1] + centerWidth;      // 第三列
    column[3] = width - offsetX;              // 第四列

    row[0] = -offsetY;                        // 第一行
    row[1] = bottomHeight * yScale - offsetY; // 第二行
    row[2] = row[1] + centerHeight;           // 第三行
    row[3] = height - offsetY;                // 第四行

    // 顶点按照9宫网格从左到右，从下到上填充

    float *vertexList = sprite->vertexRenderData.vertexList;
    int index = 0;

    // 第一行第一列
    vertexList[index++] = column[0];
    vertexList[index++] = row[0];

    if (leftWidth != 0.f)
    {
        // 第一行第二列
        vertexList[index++] = column[1];
        vertexList[index++] = row[0];
    }

    if (rightWidth != 0.f)
    {
        // 第一行第三列
        vertexList[index++] = column[2];
        vertexList[index++] = row[0];
    }

    // 第一行第四列
    vertexList[index++] = column[3];
    vertexList[index++] = row[0];

    if (bottomHeight != 0.f)
    {

        // 第二行第一列
        vertexList[index++] = column[0];
        vertexList[index++] = row[1];
        if (leftWidth != 0.f)
        {
            // 第二行第二列
            vertexList[index++] = column[0];
            vertexList[index++] = row[1];
        }

        if (rightWidth != 0.f)
        {
            // 第二行第二列
            vertexList[index++] = column[2];
            vertexList[index++] = row[1];
        }

        // 第二行第三列
        vertexList[index++] = column[3];
        vertexList[index++] = row[1];
    }

    if (topHeight != 0.f)
    {
        // 第三行第一列
        vertexList[index++] = column[0];
        vertexList[index++] = row[2];

        if (leftWidth != 0.f)
        {
            // 第三行第二列
            vertexList[index++] = column[1];
            vertexList[index++] = row[2];
        }

        if (rightWidth != 0.f)
        {
            // 第三行第二列
            vertexList[index++] = column[2];
            vertexList[index++] = row[2];
        }

        // 第三行第三列
        vertexList[index++] = column[3];
        vertexList[index++] = row[2];
    }

    // 第四行第一列
    vertexList[index++] = column[0];
    vertexList[index++] = row[3];

    if (leftWidth != 0.f)
    {
        // 第四行第二列
        vertexList[index++] = column[1];
        vertexList[index++] = row[3];
    }

    if (rightWidth != 0.f)
    {
        // 第四行第三列
        vertexList[index++] = column[2];
        vertexList[index++] = row[3];
    }

    // 第四行第四列
    vertexList[index++] = column[3];
    vertexList[index++] = row[3];

    sprite->vertexRenderData.vertexNum = index;
}

void SpriteSlicedAssembly::updateUVs(SpriteComponent *sprite)
{

    Mesh *mesh = sprite->getMesh();
    VertexBuffer *vertexBuffer = mesh->getVertexBufer(0);
    vertexBuffer->setDirty(true);

    sqstd::ByteBlockChunk *chunk = sprite->vertexData;
    const float *uv = sprite->spriteFrame.value().slicedUv.value().uv;
    SQ_ASSERT(uv);
    int offset = 8;
    // printf("++updateUVs\n");
    for (int i = 0; i < sprite->vertexRenderData.vertexNum; i += 2)
    {
        chunk->buffer.setValue<float>(offset, uv[i]);
        chunk->buffer.setValue<float>(offset + 4, uv[i + 1]);

        // printf("uv %d %f %f \n",i,uv[i],uv[i + 1]);

        offset += mesh->getVertexStride(0);
    }
}

void SpriteSlicedAssembly::updateTextureIndex(SpriteComponent *sprite, int textureIndex)
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

void SpriteSlicedAssembly::updateColor(SpriteComponent *sprite)
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

void SpriteSlicedAssembly::updateWorldVerts(SpriteComponent *sprite)
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
    for (int i = 0; i < sprite->vertexRenderData.vertexNum; i += 2)
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

void SpriteSlicedAssembly::fillChunkIndices(SpriteComponent *sprite)
{
    Mesh *mesh = sprite->getMesh();
    VertexBuffer *vertexBuffer = mesh->getVertexBufer(0);
    sqstd::ByteBlockChunk *chunk = sprite->vertexData;
    unsigned short vertexOffset = chunk->offset / mesh->getVertexStride(0);
    unsigned short index = vertexOffset;
    IndexBuffer &ib = (*mesh->getIndexBuffer());

    int numVertex = sprite->vertexRenderData.vertexNum / 2;

    for (int row = 0; row < 4; ++row)
    {
        for (int column = 0; column < 4; ++column)
        {
            int blIndex = row * 4 + column;
            if (blIndex > numVertex)
                break;

            int brIndex = row * 4 + column + 1;
            if (brIndex > numVertex)
                break;

            int tlIndex = (row + 1) * 4 + column;
            if (tlIndex > numVertex)
                break;

            int trIndex = (row + 1) * 4 + column + 1;
            if (trIndex > numVertex)
                break;

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

            vertexOffset += 4;
            index = vertexOffset;
        }
    }
}

void SpriteSlicedAssembly::fillBuffers(SpriteComponent *sprite, int textureIndex)
{

    if (sprite->vertexRenderData.dirtyVertex())
    {
        sprite->vertexRenderData.clearDirtyVertex();
        updateVertexData(sprite);
    }

    if (sprite->vertexRenderData.cacheFlagChangedVersion != sprite->node->getFlagChangeVersion())
    {
        sprite->vertexRenderData.cacheFlagChangedVersion = sprite->node->getFlagChangeVersion();
        updateWorldVerts(sprite);
    }

    if (sprite->vertexRenderData.dirtyUv())
    {
        sprite->vertexRenderData.clearDirtyUv();
        updateUVs(sprite);
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
}