#include "LabelAssembler.h"
#include "../components/Transform2DComponent.h"
#include "../../core/math/Mat3.h"
#include "../render/vertex-format.h"
#include "../render/BatcherRenderData.h"

static LabelAssembler *instance = new LabelAssembler;

LabelAssembler *LabelAssembler::getInstance()
{
    return instance;
}

void LabelAssembler::fillBuffers(LabelComponent *component,int textureIndex)
{    
    std::vector<unsigned int> &text = component->getTextCode();
    int textLength = component->getRender().getValidCharNum();
    if (textLength == 0)
        return;

    Mesh* mesh = component->getMesh();
    VertexBuffer *vertexBuffer = mesh->getVertexBufer(0);
    vertexBuffer->setDirty(true);
    sqstd::ByteBlockChunk *chunk = component->vertData;

    int needVertices = textLength << 4; // textLength * 4

    int offset = 20;
    float fTextureIndex = (float)textureIndex;
    for (int i = 0; i < needVertices; ++i)
    {
        chunk->buffer.setValue<float>(offset, fTextureIndex);
        offset += mesh->getVertexStride(0);
    }

    IndexBuffer& ib = (*mesh->getIndexBuffer());
    unsigned short vertexOffset = chunk->offset/mesh->getVertexStride(0);
    unsigned short index = 0;

    for (int i = 0; i < textLength; ++i)
    {
        // 0
        index = vertexOffset;
        ib.append<unsigned short>(index);

        // 3
        index = vertexOffset + 3;
        ib.append<unsigned short>(index);

        // 2
        index = vertexOffset + 2;
        ib.append<unsigned short>(index);

        // 0
        index = vertexOffset;
        ib.append<unsigned short>(index);

        // 2
        index = vertexOffset + 2;
        ib.append<unsigned short>(index);

        // 1
        index = vertexOffset + 1;
        ib.append<unsigned short>(index);

        vertexOffset += 4;
    }
}
