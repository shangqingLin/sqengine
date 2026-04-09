#include "GraphicsAssembler.h"
#include "../render/BatcherRenderData.h"
#include "../render/vertex-format.h"
#include "../components/graphics/Graphics.h"
#include "../../core/sqstd/Array.h"
#include "../../core/sqstd/StackTempArenaAllocator.h"

#include <string.h>

static GraphicsAssembler *_ins = NULL;

GraphicsAssembler *GraphicsAssembler::getInstance()
{
   if (!_ins)
   {
      _ins = new GraphicsAssembler();
   }
   return _ins;
}

void GraphicsAssembler::updateRenderData(Graphics *graphics)
{
   int pathSize = graphics->paths.size();

   // printf("---------------------------GraphicsAssembler updateRenderData pathSize %d \n", pathSize);

   if (pathSize == 0)
   {
      return;
   }

   sqstd::Array<Vec2> points(sqstd::StackTempArenaAllocator::getInstance());
   sqstd::Array<unsigned short> triangles(sqstd::StackTempArenaAllocator::getInstance());
   int pathPointSize[pathSize];

   {
      float strokeWidth = 0;
      int prevIndex = 0;
      for (int i = 0; i < pathSize; ++i)
      {
         d2::Path *path = graphics->paths[i];
         strokeWidth = path->strokeStyle.has_value() && path->strokeStyle->width.has_value() ? path->strokeStyle->width.value() : 2.f;
         prevIndex = points.getCount();
         // printf(" index %d point size %d \n", i,points.getCount());
         path->build(points, triangles, strokeWidth);
         pathPointSize[i] = points.getCount() - prevIndex;
      }
   }

   // Model *model = graphics->getModel();
   // MeshInstance2DGraphics *instance = dynamic_cast<MeshInstance2DGraphics *>(model->meshInstances[0]);
   // if (!instance->primitive.has_value())
   // {
   //    instance->primitive = DrawPrimitiveMesh();
   // }

   if (graphics->vertData)
   {
      graphics->vertData->recycle();
      graphics->vertData = nullptr;
   }

   Mesh *mesh;
   BatcherRenderData::getInstance()->allocateMeshChunk(vfmtPosColor, points.getCount(), mesh, graphics->vertData);

   // int fuck = getAttributeStride(vfmtPosColor);
   // printf(" GraphicsAssembler::updateRenderData %p %p pointCount %d \n", mesh, graphics->vertData, points.getCount());
   // printf(" fuck one %d \n", graphics->vertData->buffer.getDataSize() );

   graphics->setMesh(mesh);
   mesh->getVertexBufer(0)->setDirty(true);

   {

      unsigned int color = 0xffffffff;
      int index = 0;
      for (int c = 0; c < pathSize; ++c)
      {
         // printf("graphics vertex %d %d %f %f \n",uiRender->node->nativeId, bufferView->vertexOffset, points[c].x,points[c].y);
         d2::Path *path = graphics->paths[c];
         if (path->action == d2::GraphicsPathAction::FILL)
         {
            color = path->fillStyle.has_value() &&
                            path->fillStyle.value().color.has_value()
                        ? path->fillStyle.value().color.value()
                        : 0xffffffff;
         }
         else if (path->action == d2::GraphicsPathAction::STROKE)
         {
            color = path->strokeStyle.has_value() && path->strokeStyle.value().color.has_value()
                        ? path->strokeStyle.value().color.value()
                        : 0xffffffff;
         }

         for (int i = 0; i < pathPointSize[c]; ++i)
         {
            Vec2 &point = points[index];

            // todo 上面需要的内存计算错误 ???
            SQ_ASSERT(graphics->vertData->buffer.getDataSize() + 12 <= graphics->vertData->buffer.getBuffSize());

            ++index;
            graphics->vertData->buffer.append(point.x);
            graphics->vertData->buffer.append(point.y);
            graphics->vertData->buffer.append(color);
         }
      }
   }
   // printf(" GraphicsAssembler %d %p %d\n",points.size(),chunk->bufferView->getArrayBuffer(),chunk->bufferView->getArrayBuffer()->getDataSize());

   unsigned short vertexOffset = graphics->vertData->offset / mesh->getVertexStride(0);
   unsigned short index = 0;
   int size = triangles.getCount();

   graphics->triangles.clear();
   for (int c = 0; c < size; ++c)
   {
      index = vertexOffset + triangles[c];
      graphics->triangles.push(index);
      // printf("index %d \n", index);
   }

   // printf(" triangles count %d \n ", graphics->triangles.getCount());
}

void GraphicsAssembler::fillBuffers(Graphics *graphics)
{
   // printf("============ GraphicsAssembler::fillBuffers %d %d \n", graphics->cacheFlagChangedVersion, graphics->node->getFlagChangeVersion());
   Mesh *mesh = graphics->getMesh();
   IndexBuffer *indexBuffer = mesh->getIndexBuffer();
   indexBuffer->append(graphics->triangles.getData(), graphics->triangles.getCount() * 2);
   // for (int c = 0; c < graphics->triangles.getCount(); ++c)
   // {
   //    printf("zha la index %d \n", graphics->triangles[c]);
   // }
}
