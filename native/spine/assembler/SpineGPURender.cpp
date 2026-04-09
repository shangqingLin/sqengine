#include "SpineGPURender.h"
#include <spine/RegionAttachment.h>
#include <spine/MeshAttachment.h>
#include <spine/extension.h>
#include <spine/Slot.h>
#include <engine/core/utils/texture-hepler.h>
#include "../framework/MeshInstanceSpine.h"

/**
 * 将BoneIndex转成存储在纹理中的Index
 */
static int mapSkBoneIndexToIndex(int skBoneIndex, int boneToIndex[], int saveBoneMap[], int &saveBoneMapIndex)
{
    if (boneToIndex[skBoneIndex] == -1)
    {
        saveBoneMap[saveBoneMapIndex] = skBoneIndex;
        boneToIndex[skBoneIndex] = saveBoneMapIndex;
        ++saveBoneMapIndex;
    }
    return boneToIndex[skBoneIndex];
}

static void fillRenderData(VertexBuffer &renderData, float vertexIndex, float deformIndex, float r, float g, float b, float a)
{
    renderData.append(vertexIndex);
    renderData.append(deformIndex);

    int color0 = r * 255;
    int color1 = g * 255;
    int color2 = b * 255;
    int color3 = a * 255;

    unsigned int colorNum = color0 | 0 + (color1 << 8) + (color2 << 16) + (color3 << 24);
    renderData.append(colorNum);
}

SpineGPURender::SpineGPURender()
    : cacheSkin(NULL),
      tempSkinBuffer(NULL),
      boneDeformBuffer(NULL),
      currentAnimation(-1),
      boneAndDeformTextureSize(0),
      hasDeform(false),
      boneAndDeformTexture(NULL)
{

    AttributeCustomSetting aIndex = {
        0,
        2,
        12,
        Format::RG32F};

    AttributeCustomSetting aColor = {
        8,
        4,
        12,
        Format::R8};

    vfmtPosColor = {
        {"aIndex", Format::RG32F, aIndex},  // 8
        {"aColor", Format::RGBA8UI, aColor} // 使用一个数字来存储RGBA四分量颜色值 color 4
    };

    vertexBuffer = new VertexBuffer();
    indexBuffer = new IndexBuffer();
    // clipper = spSkeletonClipping_create();
}

void SpineGPURender::switchSkin(SpineComponent *component)
{

    SpineGPURenderCache *cache = component->spine->spineCache->gpuCache;
    if (!cache)
    {

        cache = new SpineGPURenderCache();
        component->spine->spineCache->gpuCache = cache;
    }

    spSkeleton *skeleton = component->skeleton;
    spSkin *currentSkin = component->getSkin();
    for (int i = 0; i < cache->skins.size(); ++i)
    {
        if (cache->skins[i]->skin == currentSkin)
        {
            this->cacheSkin = cache->skins[i];
            return;
        }
    }

    SpineGPURenderCacheSkin *cacheSkin = new SpineGPURenderCacheSkin();
    cacheSkin->skin = currentSkin;
    cacheSkin->saveBoneMap = new int[skeleton->bonesCount];
    cacheSkin->saveBoneNum = 0;
    this->cacheSkin = cacheSkin;
    cache->skins.push_back(cacheSkin);

    int boneToIndex[skeleton->bonesCount];
    std::fill(boneToIndex, boneToIndex + skeleton->bonesCount, -1);
    int boneIndex = 0;

    spSkinEntry *entry = spSkin_getAttachments(cacheSkin->skin);

    // 先计算需要多少内存
    int itemCount = 0;
    {
        int attachmentCount = 0;
        while (entry)
        {

            spAttachment *baseAttachement = entry->attachment;
            if (baseAttachement->type == SP_ATTACHMENT_REGION)
            {
                itemCount += 32;
                ++attachmentCount;
            }
            else if (baseAttachement->type == SP_ATTACHMENT_MESH)
            {
                ++attachmentCount;
                spMeshAttachment *mesh = SUB_CAST(spMeshAttachment, baseAttachement);
                spVertexAttachment *vertextAttachement = &mesh->super;
                int vertexCount = vertextAttachement->worldVerticesLength >> 1;
                if (vertextAttachement->bones)
                {
                    int bindBoneIndex = 0;
                    for (int i = 0; i < vertexCount; ++i)
                    {
                        int bindBoneCount = vertextAttachement->bones[bindBoneIndex++];
                        bindBoneIndex += bindBoneCount;
                        itemCount += bindBoneCount * 8;
                    }
                }
                else
                {
                    itemCount += vertexCount * 8;
                }
            }
            entry = entry->next;
        }
        cacheSkin->saveAttachmentMap.reserve(attachmentCount);
    }

    entry = spSkin_getAttachments(cacheSkin->skin);

    // 填充蒙皮数据
    float *buffer = new float[itemCount];
    int vertexIndex = 0, bufferIndex = 0, saveVertexIndex = 0;
    while (entry)
    {
        spAttachment *baseAttachement = entry->attachment;
        if (baseAttachement->type == SP_ATTACHMENT_REGION)
        {
            spRegionAttachment *attachment = SUB_CAST(spRegionAttachment, baseAttachement);

            cacheSkin->saveAttachmentMap[baseAttachement->id] = saveVertexIndex;
            const float *offsets = attachment->offset;
            const float *uvs = attachment->uvs;
            spSlot *slot = skeleton->slots[entry->slotIndex];
            spBone *bone = slot->bone;
            for (int i = 0; i < skeleton->bonesCount; ++i)
            {
                if (skeleton->bones[i] == bone)
                {
                    boneIndex = (float)i;
                    break;
                }
            }

            boneIndex = mapSkBoneIndexToIndex(boneIndex, boneToIndex, cacheSkin->saveBoneMap, cacheSkin->saveBoneNum);

            buffer[bufferIndex++] = offsets[6]; // x
            buffer[bufferIndex++] = offsets[7]; // y
            buffer[bufferIndex++] = uvs[0];
            buffer[bufferIndex++] = uvs[1];
            buffer[bufferIndex++] = -1.0f;     // 这个顶点的蒙皮绑了多少个Bone。使用-1表示它没有绑骨，并且不是一个Mesh，是一个Region
            buffer[bufferIndex++] = boneIndex; // 附件所在的骨骼
            buffer[bufferIndex++] = 1.0f;      // 权重
            buffer[bufferIndex++] = 0.f;       // 纹理Index

            buffer[bufferIndex++] = offsets[0];
            buffer[bufferIndex++] = offsets[1];
            buffer[bufferIndex++] = uvs[2];
            buffer[bufferIndex++] = uvs[3];
            buffer[bufferIndex++] = -1.0f;
            buffer[bufferIndex++] = boneIndex;
            buffer[bufferIndex++] = 1.0f;
            buffer[bufferIndex++] = 0.f;

            buffer[bufferIndex++] = offsets[2];
            buffer[bufferIndex++] = offsets[3];
            buffer[bufferIndex++] = uvs[4];
            buffer[bufferIndex++] = uvs[5];
            buffer[bufferIndex++] = -1.0f;
            buffer[bufferIndex++] = boneIndex;
            buffer[bufferIndex++] = 1.0f;
            buffer[bufferIndex++] = 0.f;

            buffer[bufferIndex++] = offsets[4];
            buffer[bufferIndex++] = offsets[5];
            buffer[bufferIndex++] = uvs[6];
            buffer[bufferIndex++] = uvs[7];
            buffer[bufferIndex++] = -1.0f;
            buffer[bufferIndex++] = boneIndex;
            buffer[bufferIndex++] = 1.0f;
            buffer[bufferIndex++] = 0.f;

            // printf("offset (%f %f),(%f %f),(%f %f),(%f %f) \n",
            //        offsets[6], offsets[7],
            //        offsets[0], offsets[1],
            //        offsets[2], offsets[3],
            //        offsets[4], offsets[5]);

            saveVertexIndex += 4;
        }
        else if (baseAttachement->type == SP_ATTACHMENT_MESH)
        {
            cacheSkin->saveAttachmentMap[baseAttachement->id] = saveVertexIndex;

            vertexIndex = 0;
            spMeshAttachment *mesh = SUB_CAST(spMeshAttachment, baseAttachement);
            spVertexAttachment *vertextAttachement = &mesh->super;
            int uvIndex = 0;
            int vertexCount = vertextAttachement->worldVerticesLength >> 1; // 除以2
            float u, v;

            if (vertextAttachement->bones)
            {
                int bindBoneIndex = 0;

                // printf("==================\n");

                for (int i = 0; i < vertexCount; ++i)
                {
                    u = mesh->uvs[uvIndex++];
                    v = mesh->uvs[uvIndex++];

                    int bindBoneCount = vertextAttachement->bones[bindBoneIndex++];
                    for (int b = 0; b < bindBoneCount; ++b)
                    {
                        // 也当作一个完整的顶点来对待，方便在着色器中查找到对应的顶点数据
                        boneIndex = vertextAttachement->bones[bindBoneIndex++];

                        // float x = vertextAttachement->vertices[vertexIndex++];
                        // float y = vertextAttachement->vertices[vertexIndex++];
                        // buffer[bufferIndex++] = x;
                        // buffer[bufferIndex++] = y;
                        // int testBoneIndex = boneIndex;

                        boneIndex = mapSkBoneIndexToIndex(boneIndex, boneToIndex, cacheSkin->saveBoneMap, cacheSkin->saveBoneNum);
                        buffer[bufferIndex++] = vertextAttachement->vertices[vertexIndex++];
                        buffer[bufferIndex++] = vertextAttachement->vertices[vertexIndex++];

                        buffer[bufferIndex++] = u;
                        buffer[bufferIndex++] = v;
                        buffer[bufferIndex++] = bindBoneCount;
                        buffer[bufferIndex++] = boneIndex;
                        buffer[bufferIndex++] = vertextAttachement->vertices[vertexIndex++];
                        buffer[bufferIndex++] = 0.f;

                        // printf("boneIndex %d bindBoneCount %d v (%f %f) \n",testBoneIndex,bindBoneCount, x, y);
                    }
                    saveVertexIndex += bindBoneCount;
                }

                // for (int i = 0; i < mesh->trianglesCount; ++i)
                // {
                //     printf("i %d \n",mesh->triangles[i]);
                // }
            }
            else
            {
                // 没有绑骨的情况

                spSlot *slot = skeleton->slots[entry->slotIndex];
                spBone *bone = slot->bone;
                for (int i = 0; i < skeleton->bonesCount; ++i)
                {
                    if (skeleton->bones[i] == bone)
                    {
                        boneIndex = i;
                        break;
                    }
                }

                boneIndex = mapSkBoneIndexToIndex(boneIndex, boneToIndex, cacheSkin->saveBoneMap, cacheSkin->saveBoneNum);
                for (int i = 0; i < vertexCount; ++i)
                {
                    buffer[bufferIndex++] = vertextAttachement->vertices[vertexIndex++];
                    buffer[bufferIndex++] = vertextAttachement->vertices[vertexIndex++];
                    buffer[bufferIndex++] = mesh->uvs[uvIndex++];
                    buffer[bufferIndex++] = mesh->uvs[uvIndex++];
                    buffer[bufferIndex++] = 0.f; // 使用0表示没有绑到骨骼，但它还是一个Mesh
                    buffer[bufferIndex++] = boneIndex;
                    buffer[bufferIndex++] = 1.f;
                    buffer[bufferIndex++] = 0.f;
                }

                saveVertexIndex += vertexCount;
            }
        }
        entry = entry->next;
    }

    // 将上面的蒙皮数据存储在一个纹理中，顶点着色器读取这张纹理得到所有的数据
    // 需要的像素个数
    int numPixel = ceil(itemCount / 4.0f);
    Texture2d *texture = new Texture2d();
    ITexture2DCreateInfo textureInfo;
    textureInfo.format = toNumber(Format::RGBA32F);

    // 保证顶点着色器中能够精确读取纹理中的数据
    SamplerInfo sampler = {TextureFilter::NEAREST_MIPMAP_NEAREST, TextureFilter::NEAREST};
    textureInfo.sampler = sampler;

    utils::calculateTextureSize(numPixel, textureInfo.width, textureInfo.height, false);
    texture->create(&textureInfo);
    cacheSkin->texture = texture;
    tempSkinBuffer = buffer;
    BufferTextureCopyRegion region;
    region.texOffsetX = 0;
    region.texOffsetY = 0;
    region.texWidth = textureInfo.width;
    region.texHeight = textureInfo.height;
    texture->updateData((unsigned char *)buffer, region);
}

int SpineGPURender::deformNeedBufferSize(SpineComponent *component, int bonePixelOffset)
{
    const spAnimation *animation = component->spine->getAnimation(component->currentAnimation);
    int timelineCount = animation->timelines->size;
    spTimeline **timelines = animation->timelines->items;
    deformOffsetMap.clear();
    int pixelSize = 0;

    // printf("========== %d \n",bonePixelOffset);

    for (int i = 0; i < timelineCount; ++i)
    {
        spTimeline *timeline = timelines[i];
        if (timeline->type == spTimelineType::SP_TIMELINE_DEFORM)
        {
            spDeformTimeline *self = (spDeformTimeline *)timeline;
            deformOffsetMap[self->attachment->id] = bonePixelOffset + pixelSize;
            pixelSize += std::ceil(self->frameVerticesCount / 4.0f);

            // printf("deform %d %d \n",pixelSize,self->frameVerticesCount);
        }
    }
    return pixelSize;
}

void SpineGPURender::updateBoneAndDeformData(SpineComponent *component)
{
    spSkeleton *skeleton = component->skeleton;

    if (currentAnimation != component->currentAnimation)
    {
        currentAnimation = component->currentAnimation;
        bool needUpdateTexture = false;
        int size = cacheSkin->saveBoneNum * 8;
        int numPixel = std::ceil(size / 4.0f);
        int deformPixel = deformNeedBufferSize(component, numPixel);
        hasDeform = deformPixel > 0;
        numPixel += deformPixel;

        if (numPixel > boneAndDeformTextureSize)
        {
            boneAndDeformTextureSize = numPixel;

            if (boneDeformBuffer)
            {
                delete boneDeformBuffer;
                boneDeformBuffer = NULL;
            }

            if (!boneAndDeformTexture)
            {
                boneAndDeformTexture = new Texture2d();
                ITexture2DCreateInfo textureInfo;
                textureInfo.format = toNumber(Format::RGBA32F);
                SamplerInfo sampler = {TextureFilter::NEAREST, TextureFilter::NEAREST};
                textureInfo.sampler = sampler;
                boneAndDeformTexture->create(&textureInfo);
            }

            int texWidth, texHeight;
            utils::calculateTextureSize(numPixel, texWidth, texHeight, false);
            boneAndDeformTexture->resize(texWidth, texHeight);

            if (!boneDeformBuffer)
            {
                boneDeformBuffer = new float[texWidth * texHeight * 4];
            }
            // printf("bone update %d %d %d %d %d \n", cacheSkin->saveBoneNum, skeleton->bonesCount,numPixel, boneAndDeformTexture->getWidth(), boneAndDeformTexture->getHeight());
        }
    }

    // printf("=========================\n");

    int dataIndex = 0;
    int boneIndex = 0;
    for (int i = 0; i < cacheSkin->saveBoneNum; ++i)
    {
        boneIndex = cacheSkin->saveBoneMap[i];
        spBone *bone = skeleton->bones[boneIndex];

        // printf("===set bones %s \n",bone->data->name);

        boneDeformBuffer[dataIndex++] = bone->a;
        boneDeformBuffer[dataIndex++] = bone->b;
        boneDeformBuffer[dataIndex++] = bone->c;
        boneDeformBuffer[dataIndex++] = bone->d;

        boneDeformBuffer[dataIndex++] = bone->worldX;
        boneDeformBuffer[dataIndex++] = bone->worldY;
        boneDeformBuffer[dataIndex++] = 0.f;
        boneDeformBuffer[dataIndex++] = 0.f;

        // if(boneIndex == 1)
        //     printf("%d %s (%f %f %f %f %f %f)\n",i,bone->data->name,bone->a,bone->b,bone->c,bone->d,bone->worldX,bone->worldY);
    }

    if (hasDeform)
    {
        for (int i = 0; i < skeleton->slotsCount; ++i)
        {
            spSlot *slot = skeleton->drawOrder[i];
            spAttachment *baseAttachement = slot->attachment;
            if (baseAttachement && baseAttachement->type == spAttachmentType::SP_ATTACHMENT_MESH)
            {
                if (slot->deformCount > 0)
                {
                    int deformOffsetPixel = deformOffsetMap[baseAttachement->id];

                    // printf("?? %d %d \n",deformOffsetPixel,slot->deformCount);

                    memcpy(boneDeformBuffer + deformOffsetPixel * 4, slot->deform, slot->deformCount * 4);

                    // for(int c = 0; c < slot->deformCount ; c+=2){
                    //      printf("  (%f %f)\n",slot->deform[c],slot->deform[c+1]);
                    // }
                }
            }
        }
    }

    BufferTextureCopyRegion region;
    region.texOffsetX = 0;
    region.texOffsetY = 0;
    region.texWidth = boneAndDeformTexture->getWidth();
    region.texHeight = boneAndDeformTexture->getHeight();
    boneAndDeformTexture->updateData((unsigned char *)boneDeformBuffer, region);
}

void SpineGPURender::render(SpineComponent *component, float dt)
{
    /*
    vertexBuffer->clearData();

    spAnimationState_update(component->animationState, dt);
    spAnimationState_apply(component->animationState, component->skeleton);
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

    spSkeleton *skeleton = component->skeleton;
    if (!component->spine->spineCache.has_value())
    {
        component->spine->spineCache = SpineCache();
    }

    if (tempSkinBuffer)
    {
        delete[] tempSkinBuffer;
        tempSkinBuffer = NULL;
    }
    if (!cacheSkin || cacheSkin->skin != component->getSkin())
    {
        switchSkin(component);
    }

    updateBoneAndDeformData(component);

    bool needNewIndices = false;
    int verticesCount = 0;
    prepareRender(component, indexBuffer, needNewIndices, verticesCount);

    {
        // 预先分配好内存，避免下面不断执行分配内存操作，造成卡顿
        int needBytes = verticesCount * getAttributeStride(vfmtPosColor);
        if (vertexBuffer->getBufferSize() < needBytes)
        {
            vertexBuffer->resizeBuffer(needBytes);
        }
    }

    spColor *attachmentColor = NULL;
    spAtlasPage *page = NULL;
    unsigned short *indices;
    int indicesCount = 0;
    unsigned short REGION_QUAD_TRIANGLES[6] = {0, 1, 2, 2, 3, 0};
    int indicesBegin = 0;
    bool needNewDrawCall = false;
    int currentDrawCall = -1;
    bool needAddTexture = false;
    int totalIndices = 0;
    // printf("====================\n");

    for (int i = 0; i < skeleton->slotsCount; ++i)
    {
        spSlot *slot = skeleton->drawOrder[i];
        if (!checkSlotHidden(slot))
        {
            // spSkeletonClipping_clipEnd(clipper, slot);
            continue;
        }

        spAttachment *baseAttachement = slot->attachment;
        if (baseAttachement->type == spAttachmentType::SP_ATTACHMENT_REGION)
        {
            spRegionAttachment *region = SUB_CAST(spRegionAttachment, baseAttachement);
            attachmentColor = &region->color;
            if (attachmentColor->a == 0)
            {
                // spSkeletonClipping_clipEnd(clipper, slot);
                continue;
            }
            indices = REGION_QUAD_TRIANGLES;
            indicesCount = 6;
            verticesCount = 4;
            page = ((spAtlasRegion *)region->region)->page;
            // printf("regions id %d \n",baseAttachement->id);
        }
        else if (baseAttachement->type == spAttachmentType::SP_ATTACHMENT_MESH)
        {
            spMeshAttachment *mesh = SUB_CAST(spMeshAttachment, baseAttachement);
            attachmentColor = &mesh->color;
            if (attachmentColor->a == 0)
            {
                // spSkeletonClipping_clipEnd(clipper, slot);
                continue;
            }
            indices = mesh->triangles;
            indicesCount = mesh->trianglesCount;
            verticesCount = mesh->super.worldVerticesLength >> 1;
            //  printf("mesh %d\n",baseAttachement->id);
        }
        else if (baseAttachement->type == spAttachmentType::SP_ATTACHMENT_CLIPPING)
        {
            // spClippingAttachment *clip = SUB_CAST(spClippingAttachment, baseAttachement);
            // spSkeletonClipping_clipStart(clipper, slot, clip);
            continue;
        }
        else
        {
            continue;
        }

        // if (spSkeletonClipping_isClipping(clipper))
        // {
        // spSkeletonClipping_clipTriangles(&clipper, vertices->items, verticesCount << 1, indices, indicesCount, uvs, 2);
        // indices = clipper.clippedTriangles->items;
        // indicesCount = clipper.clippedTriangles->size;
        // verticesCount = clipper.clippedVertices->size >> 1;
        // }

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
            std::shared_ptr<Mesh> mesh = mesInstance->getMesh();
            if (!mesh)
            {
                mesh = std::make_shared<Mesh>();
                mesInstance->setMesh(mesh);
                mesh->addVertextBuffer(vertexBuffer, vfmtPosColor);
            }
            mesh->setIndexBuffer(indexBuffer);
            if (!mesInstance->primitive.has_value())
            {
                mesInstance->primitive = DrawPrimitiveMesh();
            }
            mesInstance->primitive.value().count = 0;
            mesInstance->primitive.value().offset = totalIndices;
            needAddTexture = true;

            Material *material = component->getMaterial();

            // todo 这里设置Material会卡，why?
            float texSize[2];
            texSize[0] = cacheSkin->texture->getWidth();
            texSize[1] = cacheSkin->texture->getHeight();

            std::string name = "skinTexSize";
            material->setProperty(name, texSize);

            name = "boneTexSize";
            texSize[0] = boneAndDeformTexture->getWidth();
            texSize[1] = boneAndDeformTexture->getHeight();
            material->setProperty(name, texSize);

            name = "skinTexture";
            material->setTexture(name, cacheSkin->texture);

            name = "boneAndDeformTexture";
            material->setTexture(name, boneAndDeformTexture);
        }

        MeshInstanceSpine *mesInstance = dynamic_cast<MeshInstanceSpine *>(component->model->meshInstances[currentDrawCall]);
        std::shared_ptr<Mesh> mesh = mesInstance->getMesh();
        totalIndices += indicesCount;
        mesInstance->primitive.value().count = totalIndices;

        if (needAddTexture)
        {
            mesInstance->textures.push_back(static_cast<Texture2d *>(page->rendererObject));
        }

        float beginIndex = (float)cacheSkin->saveAttachmentMap[baseAttachement->id];

        // printf("=======beginIndex %d %d\n",beginIndex,baseAttachement->id);

        if (baseAttachement->type == spAttachmentType::SP_ATTACHMENT_MESH)
        {
            spVertexAttachment *vertexAttachment = &SUB_CAST(spMeshAttachment, baseAttachement)->super;
            bool activeDeform = slot->deformCount > 0;
            float beginOffsetDeform = -1.f;
            if (activeDeform)
            {
                beginOffsetDeform = (float)deformOffsetMap[baseAttachement->id];
            }

            if (vertexAttachment->bones)
            {
                int bindBoneIndex = 0;
                for (int c = 0; c < verticesCount; ++c)
                {

                    int bindBoneCount = vertexAttachment->bones[bindBoneIndex++];
                    bindBoneIndex += bindBoneCount;

                    fillRenderData(
                        *vertexBuffer,
                        beginIndex,
                        beginOffsetDeform,
                        r, g, b, a);

                    // 一个像素存储2个顶点数据
                    // 在GLSL中通过判断beginOffsetDeform是否有小数部分来取xy还是zw，从而得到对应的数据
                    if (activeDeform)
                        beginOffsetDeform += 0.5f;

                    beginIndex += bindBoneCount;
                }
            }
            else
            {
                for (int c = 0; c < verticesCount; ++c)
                {
                    fillRenderData(
                        *vertexBuffer,
                        beginIndex++,
                        beginOffsetDeform,
                        r, g, b, a);

                    if (activeDeform)
                        beginOffsetDeform += 0.5f;
                }
            }
        }
        else
        {
            for (int c = 0; c < verticesCount; ++c)
            {
                fillRenderData(
                    *vertexBuffer,
                    beginIndex++,
                    -1.f,
                    r, g, b, a);
            }
        }

        if (needNewIndices)
        {
            for (int c = indicesCount - 1; c >= 0; --c)
            {
                indexBuffer->append<unsigned short>(indicesBegin + indices[c]);
            }
            indicesBegin += verticesCount;
        }

        // spSkeletonClipping_clipEnd(clipper, slot);
    }

    // spSkeletonClipping_clipEnd2(clipper);*/
    
}

SpineGPURender::~SpineGPURender()
{
    if (tempSkinBuffer)
    {
        delete[] tempSkinBuffer;
        tempSkinBuffer = NULL;
    }

    delete[] boneDeformBuffer;
    delete boneAndDeformTexture;
    delete[] indexBuffer;
    delete[] vertexBuffer;
    // spSkeletonClipping_dispose(clipper);
}