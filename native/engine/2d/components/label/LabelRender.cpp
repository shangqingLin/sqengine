
#include "LabelRender.h"
#include "../Transform2DComponent.h"
#include "../../../core/math/Mat3.h"
#include "../../render/vertex-format.h"
#include "../../render/BatcherRenderData.h"
#include "./LabelComponent.h"
#include <algorithm>

LabelRender::LabelRender(LabelComponent *copmonent)
    : component(copmonent)
{
}

int LabelRender::countValidChar()
{
    std::vector<unsigned int> &text = component->getTextCode();
    int textLength = text.size();
    int count = 0;
    for (int i = 0; i < textLength; ++i)
    {
        // 行换符号、空格符、\t、\b
        if (text[i] != 0xA && text[i] != 0x20 && text[i] != 0x9 && text[i] != 0x8)
        {
            ++count;
        }
    }
    return count;
}

void LabelRender::render()
{
    if (!_drity)
        return;

    Font *font = component->getFont();
    if (!font)
        return;

    FontFace *face = (FontFace *)font->getFontFace();
    if (!face)
        return;

    _drity = false;

    validCharNum = countValidChar();
    if (validCharNum == 0)
        return;

    std::vector<unsigned int> &text = component->getTextCode();

    if (component->vertData)
    {
        component->vertData->recycle();
    }
    Mesh *mesh;
    BatcherRenderData::getInstance()->allocateMeshChunk(vfmtPosUvColor, validCharNum << 4, mesh, component->vertData); // validCharNum * 4
    component->setMesh(mesh);
    VertexBuffer *vertexBuffer = mesh->getVertexBufer(0);
    vertexBuffer->setDirty(true);
    sqstd::ByteBlockChunk *chunk = component->vertData;

    Transform2DComponent *transform = component->node->getComponent<Transform2DComponent>();
    Mat3 &worldMatrix = transform->getWorldTransform();

    float offsetX = component->getOffsetX();
    float offsetY = component->getOffsetY();

    float maxWidth = 0;
    float maxFontHeight = 0;
    float x = 0, y = 0;
    float l, r, t, b;

    Vec2 point;
    float fTextureIndex = 0.f;

    unsigned int color = component->getColorNum();

    // printf("========================================\n");
    // Date date1;
    // date1.reset();

    // Date date;
    // date.reset();
    for (int i = 0; i < text.size(); ++i)
    {
        // 换行符"\n"
        if (text[i] == 0xa)
        {
            x = 0;
            y -= maxFontHeight;
            maxFontHeight = 0;
            component->contextHeight = -y;
            continue;
        }

        // date.reset();

        const FontGlyph *plygph = face->getGlyph(text[i], component->fontSize);

        // if(date.get() != 0)
        //     printf(">>>>>>>>>>>>>>>>LabelRender %d %ld\n",text[i],date.get());

        if (text[i] == 0x20 || text[i] == 0x9 || text[i] == 0x8)
        {
            x += plygph->width + component->spacingX;
            continue;
        }

        if (component->_enableWrapText)
        {
            if (component->getWidth() > 0 && (x + plygph->width) > component->getWidth())
            {
                // 换行
                x = 0;
                y -= maxFontHeight;
                maxFontHeight = 0;
                component->contextHeight = -y;
            }
        }

        maxFontHeight = component->lineHeight != 0 ? component->lineHeight : std::max(maxFontHeight, (float)plygph->height);
        component->setTexture(face->getTexture(plygph->page));
        
        l = x - offsetX;
        r = l + plygph->width;
        t = y - offsetY;
        b = t - plygph->height;

        point.set(l, t);
        // printf("   point %f %f %f %f\n",point.x,point.y,plygph->u0,plygph->v1);
        worldMatrix.transformPoint(point, point);
        chunk->buffer.append(point.x);
        chunk->buffer.append(point.y);
        chunk->buffer.append(plygph->u0);
        chunk->buffer.append(plygph->v0);
        chunk->buffer.append(color);
        chunk->buffer.append(fTextureIndex);

        point.set(r, t);
        //  printf("   point %f %f %f %f\n",point.x,point.y,plygph->u1,plygph->v1);
        worldMatrix.transformPoint(point, point);
        chunk->buffer.append(point.x);
        chunk->buffer.append(point.y);
        chunk->buffer.append(plygph->u1);
        chunk->buffer.append(plygph->v0);
        chunk->buffer.append(color);
        chunk->buffer.append(fTextureIndex);

        point.set(r, b);
        //  printf("   point %f %f %f %f\n",point.x,point.y,plygph->u1,plygph->v0);
        worldMatrix.transformPoint(point, point);
        chunk->buffer.append(point.x);
        chunk->buffer.append(point.y);
        chunk->buffer.append(plygph->u1);
        chunk->buffer.append(plygph->v1);
        chunk->buffer.append(color);
        chunk->buffer.append(fTextureIndex);

        point.set(l, b);
        //  printf("   point %f %f %f %f\n",point.x,point.y,plygph->u0,plygph->v0);
        worldMatrix.transformPoint(point, point);
        chunk->buffer.append(point.x);
        chunk->buffer.append(point.y);
        chunk->buffer.append(plygph->u0);
        chunk->buffer.append(plygph->v1);
        chunk->buffer.append(color);
        chunk->buffer.append(fTextureIndex);

        // 保证行最后一个字不会加上spacingX
        x += plygph->width;
        maxWidth = std::max(maxWidth, x);

        x += component->spacingX;
    }

    // printf("========================================%ld\n",date1.get());

    component->contextHeight += maxFontHeight;
    component->contextWidth = maxWidth;
}