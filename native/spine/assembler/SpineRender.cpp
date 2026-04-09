#include "SpineRender.h"
#include <spine/RegionAttachment.h>
#include <spine/extension.h>
#include <engine/core/core.h>

bool SpineRender::checkSlotHidden(spSlot *slot)
{
    if (!slot->attachment || !slot->bone->active || slot->color.a == 0)
    {
        return false;
    }
    spAttachment *attachement = slot->attachment;
    if (attachement->type == spAttachmentType::SP_ATTACHMENT_REGION || attachement->type == spAttachmentType::SP_ATTACHMENT_MESH || attachement->type == spAttachmentType::SP_ATTACHMENT_CLIPPING)
    {
        return true;
    }
    return false;
}

void SpineRender::prepareRender(SpineComponent *component, IndexBuffer *indexBuffer,bool& needNewIndices, int &verticesCount)
{
    verticesCount = 0;
    needNewIndices = false;
    spSkeleton *skeleton = component->skeleton;
    spColor *attachmentColor = NULL;
    Spine *spineAsset = component->spine;
    int indicesCount = 0;
    sqstd::hash_t indicesKey = 555;
    for (int i = 0; i < skeleton->slotsCount; ++i)
    {
        spSlot *slot = skeleton->drawOrder[i];
        if (!checkSlotHidden(slot))
        {
            continue;
        }

        spAttachment *baseAttachement = slot->attachment;
        if (baseAttachement->type == spAttachmentType::SP_ATTACHMENT_REGION)
        {
            spRegionAttachment *region = SUB_CAST(spRegionAttachment, baseAttachement);
            attachmentColor = &region->color;
            if (attachmentColor->a == 0)
            {
                continue;
            }
            verticesCount += 4;
            indicesCount += 6;
            sqstd::hash_combine(indicesKey, baseAttachement->id);
        }
        else if (baseAttachement->type == spAttachmentType::SP_ATTACHMENT_MESH)
        {
            spMeshAttachment *mesh = SUB_CAST(spMeshAttachment, baseAttachement);
            attachmentColor = &mesh->color;
            if (attachmentColor->a == 0)
            {
                continue;
            }
            verticesCount += mesh->super.worldVerticesLength >> 1;
            indicesCount += mesh->trianglesCount;
            sqstd::hash_combine(indicesKey, baseAttachement->id);
        }
        else if (baseAttachement->type == spAttachmentType::SP_ATTACHMENT_CLIPPING)
        {
            sqstd::hash_combine(indicesKey, baseAttachement->id);
        }
    }

    sqstd::Byte *indicesBuBuffer = spineAsset->spineCache->getCacheIndexBuffer(indicesKey);
    if (indicesBuBuffer)
    {
        needNewIndices = false;
    }
    else
    {
        int needBytes = indicesCount << 1;
        indicesBuBuffer = new sqstd::Byte();
        indicesBuBuffer->resize(needBytes);
        spineAsset->spineCache->setCacheIndexBuffer(indicesKey, indicesBuBuffer);
        needNewIndices = true;
    }
    indexBuffer->setExternalBuffer(indicesBuBuffer);
}