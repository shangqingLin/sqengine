#include "SpineCPURender.h"
#include <spine/RegionAttachment.h>
#include <spine/extension.h>
#include <engine/2d/render/vertex-format.h>
#include "../framework/MeshInstanceSpine.h"
#include <engine/core/core.h>

static void fillRenderData(VertexBuffer &renderData, float x, float y, float u, float v, float r, float g, float b, float a, int textureIndex)
{
    renderData.append(x);
    renderData.append(y);
    renderData.append(u);
    renderData.append(v);
    int color0 = r * 255;
    int color1 = g * 255;
    int color2 = b * 255;
    int color3 = a * 255;

    unsigned int colorNum = color0 | 0 + (color1 << 8) + (color2 << 16) + (color3 << 24);
    renderData.append(colorNum);
    renderData.append((float)textureIndex);
}

SpineCPURender::SpineCPURender()
{
    vertexBuffer = new VertexBuffer();
    indexBuffer = new IndexBuffer();
    mesh = std::make_shared<Mesh>();
    mesh.get()->setIndexBuffer(indexBuffer);
    mesh.get()->addVertextBuffer(vertexBuffer,vfmtPosUvColor);
    clipper = spSkeletonClipping_create();
}

void SpineCPURender::render(SpineComponent *component, float dt)
{
    /*
    vertexBuffer->clearData();
    
    // 没有动画播放，则显示Steup状态
    if (component->currentAnimation == -1)
    {
        spSkeleton_setToSetupPose(component->skeleton);
    }
    else
    {
        spAnimationState_update(component->animationState, dt);
        spAnimationState_apply(component->animationState, component->skeleton);
    }

    spSkeleton_updateWorldTransform(component->skeleton, spPhysics::SP_PHYSICS_UPDATE);
    std::vector<MeshInstance *> pool;
    if (component->model->meshInstances.size() > 0)
    {
        for (int i = 0; i < component->model->meshInstances.size(); ++i)
        {
            pool.push_back(component->model->meshInstances[i]);
        }
        component->model->meshInstances.clear();
    }

    // printf("============================render %p \n",vertexBuffer->getBuffer());

    spSkeleton *skeleton = component->skeleton;
    spColor *attachmentColor = NULL;
    int verticesCount = 0;

    bool needNewIndices = false;
    Spine *spineAsset = component->spine;
    if (!spineAsset->spineCache.has_value())
    {
        spineAsset->spineCache = SpineCache();
    }

    prepareRender(component, indexBuffer, needNewIndices, verticesCount);

    {
        // 预先分配好内存，避免下面不断执行分配内存操作，造成卡顿
        int needBytes = verticesCount * 24;
        if (vertexBuffer->getBufferSize() < needBytes)
        {
            vertexBuffer->resizeBuffer(needBytes);
        }
    }

    // printf("============== slot %d verticesCount : %d size %d\n", skeleton->slotsCount, verticesCount,vertexBuffer->getBuffer()->getBuffSize());

    float tempVertices[verticesCount << 1];
    float *resultVertices = NULL;
    int currentDrawCall = -1;
    float *uvs = NULL;
    int indicesCount = 0;
    unsigned short *indices;
    int totalIndices = 0;
    unsigned short REGION_QUAD_TRIANGLES[6] = {0, 1, 2, 2, 3, 0};
    spAtlasPage *page = NULL;
    bool needNewDrawCall = false;
    bool needAddTexture = false;
    int textureIndex = 0;
    unsigned short indicesBegin = 0;

    for (int i = 0; i < skeleton->slotsCount; ++i)
    {
        spSlot *slot = skeleton->drawOrder[i];
        // printf(">>>>>>> %d \n",i);
        if (!checkSlotHidden(slot))
        {
            spSkeletonClipping_clipEnd(clipper, slot);
            continue;
        }

        spAttachment *baseAttachement = slot->attachment;
        if (baseAttachement->type == spAttachmentType::SP_ATTACHMENT_REGION)
        {
            spRegionAttachment *region = SUB_CAST(spRegionAttachment, baseAttachement);
            attachmentColor = &region->color;
            if (attachmentColor->a == 0)
            {
                spSkeletonClipping_clipEnd(clipper, slot);
                continue;
            }

            verticesCount = 4;
            uvs = region->uvs;
            indices = REGION_QUAD_TRIANGLES;
            indicesCount = 6;
            page = ((spAtlasRegion *)region->region)->page;
            spRegionAttachment_computeWorldVertices(region, slot, tempVertices, 0, 2);
            resultVertices = tempVertices;
            // printf("spRegionAttachment_computeWorldVertices (%f %f),(%f %f),(%f %f),(%f %f) \n",
            //     resultVertices[0],resultVertices[1],
            //     resultVertices[2],resultVertices[3],
            //     resultVertices[4],resultVertices[5],
            //     resultVertices[6],resultVertices[7]
            //     );
        }
        else if (baseAttachement->type == spAttachmentType::SP_ATTACHMENT_MESH)
        {
            spMeshAttachment *mesh = SUB_CAST(spMeshAttachment, baseAttachement);
            attachmentColor = &mesh->color;
            if (attachmentColor->a == 0)
            {
                spSkeletonClipping_clipEnd(clipper, slot);
                continue;
            }
            verticesCount = mesh->super.worldVerticesLength >> 1;
            uvs = mesh->uvs;
            indices = mesh->triangles;
            indicesCount = mesh->trianglesCount;
            page = ((spAtlasRegion *)mesh->region)->page;
            spVertexAttachment_computeWorldVertices(&mesh->super, slot, 0, mesh->super.worldVerticesLength, tempVertices, 0, 2);
            resultVertices = tempVertices;

            //   if(strcmp(slot->data->name,"shouzi5") == 0){
            //     printf("========mesh %s\n", ((spAtlasRegion *)mesh->region)->name);
            // }
        }
        else if (baseAttachement->type == spAttachmentType::SP_ATTACHMENT_CLIPPING)
        {
            spClippingAttachment *clip = SUB_CAST(spClippingAttachment, baseAttachement);
            spSkeletonClipping_clipStart(clipper, slot, clip);
            continue;
        }
        else
        {
            continue;
        }

        if (spSkeletonClipping_isClipping(clipper))
        {
            spSkeletonClipping_clipTriangles(clipper, tempVertices, verticesCount << 1, indices, indicesCount, uvs, 2);
            resultVertices = clipper->clippedVertices->items;
            verticesCount = clipper->clippedVertices->size >> 1;
            uvs = clipper->clippedUVs->items;
            indices = clipper->clippedTriangles->items;
            indicesCount = clipper->clippedTriangles->size;
        }

        float r = skeleton->color.r * slot->color.r * attachmentColor->r;
        float g = skeleton->color.g * slot->color.g * attachmentColor->g;
        float b = skeleton->color.b * slot->color.b * attachmentColor->b;
        float a = skeleton->color.a * slot->color.a * attachmentColor->a;
        needNewDrawCall = currentDrawCall == -1;
        if (currentDrawCall != -1)
        {
            MeshInstanceSpine *mesInstance = dynamic_cast<MeshInstanceSpine *>(component->model->meshInstances[currentDrawCall]);

            if (mesInstance->textures.size() < 10)
            {
                Texture2d *texture = static_cast<Texture2d *>(page->rendererObject);
                needAddTexture = true;
                for (int c = 0; c < mesInstance->textures.size(); ++c)
                {
                    if (mesInstance->textures[c] == texture)
                    {
                        textureIndex = c;
                        needAddTexture = false;
                        break;
                    }
                }
            }
            else
            {
                needNewDrawCall = true;
            }
        }

        if (needNewDrawCall)
        {

            ++currentDrawCall;
            MeshInstanceSpine *mesInstance = NULL;
            if (pool.size() > 0)
            {
                mesInstance = dynamic_cast<MeshInstanceSpine *>(pool[pool.size() - 1]);
                mesInstance->clear();
                pool.pop_back();
            }

            if (!mesInstance)
            {
                mesInstance = new MeshInstanceSpine();
            }
            component->model->meshInstances.push_back(mesInstance);
            if (!mesInstance->primitive.has_value())
            {
                mesInstance->primitive = DrawPrimitiveMesh();
            }

            mesInstance->setMesh(mesh);
            mesInstance->primitive.value().count = 0;
            mesInstance->primitive.value().offset = totalIndices;
            needAddTexture = true;
        }
        MeshInstanceSpine *mesInstance = dynamic_cast<MeshInstanceSpine *>(component->model->meshInstances[currentDrawCall]);
        totalIndices += indicesCount;
        mesInstance->primitive.value().count = totalIndices;

        if (needAddTexture)
        {
            textureIndex = mesInstance->textures.size();
            mesInstance->textures.push_back(static_cast<Texture2d *>(page->rendererObject));
        }

        // printf("======================\n");
        for (int c = 0, n = verticesCount << 1; c < n; c += 2)
        {
            // if(baseAttachement->type == spAttachmentType::SP_ATTACHMENT_MESH)
            // {
            //     printf("(%f %f)\n", resultVertices[c], resultVertices[c + 1]);
            // }

            fillRenderData(*vertexBuffer, resultVertices[c], resultVertices[c + 1], uvs[c], uvs[c + 1], r, g, b, a, textureIndex);
        }

        if (needNewIndices)
        {
            // Spine是按照顺时针给的index
            for (int c = indicesCount - 1; c >= 0; --c)
            {
                indexBuffer->append<unsigned short>(indicesBegin + indices[c]);
            }
            indicesBegin += verticesCount;
        }

        spSkeletonClipping_clipEnd(clipper, slot);
    }
    spSkeletonClipping_clipEnd2(clipper);

    for (int i = 0; i < pool.size(); ++i)
    {
        delete pool[i];
    }*/
}

SpineCPURender::~SpineCPURender()
{
    spSkeletonClipping_dispose(clipper);
    clipper = NULL;
}