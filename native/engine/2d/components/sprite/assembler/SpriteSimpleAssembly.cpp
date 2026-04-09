#include "SpriteSimpleAssembly.h"
#include "../../Transform2DComponent.h"
#include "../../../../core/math/Mat3.h"
#include "../../../render/vertex-format.h"
#include "../../../render/BatcherRenderData.h"
#include "../SpriteComponent.h"

static SpriteSimpleAssembly *instance = new SpriteSimpleAssembly;

void SpriteSimpleAssembly::requestRenderData(SpriteComponent *sprite)
{
   BatcherRenderData::getInstance()->allocateMeshChunk(vfmtPosUvColor, 4, sprite->mesh, sprite->vertexData);
   SQ_ASSERT(sprite->mesh && sprite->vertexData);
}

void SpriteSimpleAssembly::destroyRenderData(SpriteComponent *sprite)
{
   if (sprite->vertexData)
   {
      sprite->vertexData->recycle();
      sprite->vertexData = nullptr;
      sprite->mesh = nullptr;
   }
}

void SpriteSimpleAssembly::updateVertexData(SpriteComponent *sprite)
{
   float width = sprite->getWidth();
   float height = sprite->getHeight();
   float offsetX = sprite->getOffsetX();
   float offsetY = sprite->getOffsetY();

   SQ_ASSERT(width > 0);
   SQ_ASSERT(height > 0);

   // printf(" node %d SpriteSimpleAssembly::updateVertexData Sprite size (%f %f ) offset (%f %f)\n", sprite->node->nativeId, width, height, offsetX, offsetY);

   float l = -offsetX;
   float r = width - offsetX;
   float b = -offsetY;
   float t = height - offsetY;

   /**
    *  left_bottom: [0,0]
    *  right_bottom: [width,0]
    *  left_top:[0,height]
    *  right_top:[width,height]
    */

   // if(sprite->node->nativeId == 87)
   //  if(sprite->node->testNum != 0)
   // printf(" SpriteSimpleAssembly::updateVertexData sprite testNum %d nativeId %d vertex %f %f %f %f \n",sprite->node->testNum, sprite->node->nativeId, l, r, b, t);

   sprite->vertexRenderData.vertexNum = 4;
   float *vertices = sprite->vertexRenderData.vertexList;
   vertices[0] = l;
   vertices[1] = b;

   vertices[2] = r;
   vertices[3] = b;

   vertices[4] = l;
   vertices[5] = t;

   vertices[6] = r;
   vertices[7] = t;
}

void SpriteSimpleAssembly::updateWorldVerts(SpriteComponent *sprite)
{

   sqstd::ByteBlockChunk *chunk = sprite->vertexData;
   Mesh *mesh = sprite->getMesh();
   VertexBuffer *vertexBuffer = mesh->getVertexBufer(0);
   vertexBuffer->setDirty(true);

   Transform2DComponent *transform = sprite->node->getComponent<Transform2DComponent>();
   Mat3 &worldMatrix = transform->getWorldTransform();

   // if (sprite->node->testNum != 0)
   // {
   //    printf("mat node %d %d \n",sprite->node->testNum,sprite->node->nativeId);
   //    worldMatrix.print();
   // }

   float *dataList = sprite->vertexRenderData.vertexList;
   Vec2 point;
   int offset = 0;
   for (int i = 0; i < 8; i += 2)
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

void SpriteSimpleAssembly::fillBuffers(SpriteComponent *sprite, int textureIndex)
{

   if (sprite->vertexRenderData.dirtyVertex())
   {
      sprite->vertexRenderData.clearDirtyVertex();
      updateVertexData(sprite);
   }

   // printf("fuck %d %d %d\n",sprite->node->nativeId,sprite->cacheFlagChangedVersion,sprite->node->getFlagChangeVersion());

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

   // 每帧都需要重新填充，虽然顶点数据在Mesh中的位置不变（即index是一样）
   // 这里填充是因为需要保证IndexBuffer内存数据是连续可用的，比如A\B\C三个Sprite，
   // 如果B被删除了（或visible=false）就不会被使用渲染，如果重新填充中间IndexBuffer还有B的数据，底层是无法渲染的
   fillChunkIndices(sprite);
}

void SpriteSimpleAssembly::fillChunkIndices(SpriteComponent *sprite)
{
   Mesh *mesh = sprite->getMesh();

   // 填充索引。索引每帧都填充，索引保证在缓存区中紧凑排在一起，因为绘制无法在有间隔的内存进行绘制，必须是连续的内存的
   VertexBuffer *vertexBuffer = mesh->getVertexBufer(0);
   sqstd::ByteBlockChunk *chunk = sprite->vertexData;
   unsigned short vertexOffset = chunk->offset / mesh->getVertexStride(0);
   unsigned short index = vertexOffset;

   // if (sprite->texture->getKeyUrl() == "map/level/level_2/ziyan_bg_jj")
   //    printf("=commitSprite %s vertexOffset %d \n", sprite->texture->getKeyUrl().c_str(), vertexOffset);

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

void SpriteSimpleAssembly::updateUVs(SpriteComponent *sprite)
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

      // printf("uv %d %f %f \n",i,uv[i],uv[i + 1]);

      offset += mesh->getVertexStride(0);
   }
}

void SpriteSimpleAssembly::updateColor(SpriteComponent *sprite)
{
   Mesh *mesh = sprite->getMesh();
   VertexBuffer *vertexBuffer = mesh->getVertexBufer(0);
   vertexBuffer->setDirty(true);

   sqstd::ByteBlockChunk *chunk = sprite->vertexData;

   int offset = 16;
   unsigned int color = sprite->getColorNum();
   // printf("===update color %u \n",color);
   for (int i = 0; i < 4; ++i)
   {
      chunk->buffer.setValue<unsigned int>(offset, color);
      offset += mesh->getVertexStride(0);
   }
}

void SpriteSimpleAssembly::updateTextureIndex(SpriteComponent *sprite, int textureIndex)
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

SpriteSimpleAssembly *SpriteSimpleAssembly::getInstance()
{
   return instance;
}
