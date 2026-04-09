#include "FontTexture.h"
#include "../scene/Node.h"
#include "../2d/components/Transform2DComponent.h"
#include "../2d/components/sprite/SpriteComponent.h"
#include "../framework/Application.h"
#include "../core/base/config.h"

FontTexture::FontTexture(int maxWidth, int maxHeight)
    : textureWidth(maxWidth),
      textureHeight(maxHeight),
      packer(maxWidth, maxHeight)
{
    texture = new Texture2d();
    ITexture2DCreateInfo info;
    info.format = toNumber(Format::RGBA8);
    // info.format = toNumber(Format::A8);
    info.width = maxWidth;
    info.height = maxHeight;
    texture->create(&info);
    textureData = new unsigned char[maxWidth * maxHeight * 4];
}

void FontTexture::updateTexture(float dt)
{
    dirty = false;
    BufferTextureCopyRegion regions;
    regions.texOffsetX = 0;
    regions.texOffsetY = 0;
    regions.texWidth = textureWidth;
    regions.texHeight = textureHeight;
    regions.buffCopyToNative = false;
    texture->updateData(textureData, regions);
}

void FontTexture::addFontBuffer(FontGlyph &glyph, unsigned char *fontAlphaBuffer, int offsetX, int offsetY)
{
    // FreeType计算得到的像素数据只是一个Alpha值，可见处值不为零，通过Alpha值来控制
    // 可见性，从而实现绘制字形。所以fontAlphaBuffer每个像素只有一个字节
    // 但为了使用LabelComponent像SpriteComponet一样渲染（为了合并drawcall），纹理还是保存为RGBA格式
    // 所以这里我们需要插入RGB颜色数据

    // printf( "=======================glyph code %u %u %u \n", code,glyph.width, glyph.height);

    int index = 0;
    int dataIndex = 0;
    for (int r = offsetY, rr = offsetY + glyph.height; r < rr; ++r)
    {
        for (int c = offsetX, cc = offsetX + glyph.width; c < cc; ++c)
        {
            index = r * textureWidth + c;
            index *= 4;

            textureData[index] = 255;
            textureData[index + 1] = 255;
            textureData[index + 2] = 255;
            textureData[index + 3] = fontAlphaBuffer[dataIndex];
            ++dataIndex;
        }
    }
    // printf("================== %d\n",index);

    if (!dirty)
    {
        dirty = true;
        Application::getInstance()->timer.frameOnce(1, this, &FontTexture::updateTexture, TIMER_UPDATE_STAGE::AFTER_UPDATE);
    }
}

bool FontTexture::addFontGlyph(FontGlyph &glyph, unsigned char *buffer)
{
    if (allocate(glyph))
    {
        int x = glyph.textureNode->x;
        int y = glyph.textureNode->y;

        addFontBuffer(glyph, buffer, x, y);

        // 两个操作必须有一个是浮点数，那么结果才是浮点数
        float texWdith = static_cast<float>(textureWidth);
        float texHeight = static_cast<float>(textureHeight);
        glyph.u0 = x / texWdith;
        glyph.v0 = y / texHeight;
        glyph.u1 = (x + glyph.width) / texWdith;
        glyph.v1 = (y + glyph.height) / texHeight;
        return true;
    }
    return false;
}
void FontTexture::removeFontGlyph(FontGlyph &glyph)
{
    SQ_ASSERT(glyph.textureNode);
    ++removeGlyphCount;
    glyph.textureNode->packer->remove(glyph.textureNode);
    glyph.textureNode = nullptr;
}
void FontTexture::repeack()
{
    if(removeGlyphCount == 0) return;

    if (removeGlyphCount > 10)
    {
        packer.repack();
    }
    else
    {
        packer.pruneFreeList();
    }
    removeGlyphCount = 0;
}

bool FontTexture::allocate(FontGlyph &glyph)
{
    algorithm::MaxRectPackerNode *node = packer.add(glyph.width + 1, glyph.height + 1);
    glyph.textureNode = node;
    return node != NULL;
}

void FontTexture::showDebugSprite(float posX, float posY)
{
    Node *node = new Node();
    Transform2DComponent *t = node->addComponent<Transform2DComponent>();
    t->setX(posX);
    t->setY(posY);

    SpriteComponent *sprite = node->addComponent<SpriteComponent>();

    SpriteFrame frame;
    frame.texture = texture;
    sprite->setSpriteFrame(frame);
    sprite->setShaderMaterialFromRes("build-in/material/sprite_normal.mat");

    Scene *scene = Application::getInstance()->getRunScene();
    // Node *canvasNode = scene->getChildHasComponent<Canvas>();
    // canvasNode->addChild(node);
}

FontTexture::~FontTexture()
{
    delete texture;
    texture = NULL;

    delete[] textureData;
    textureData = NULL;

    if (dirty)
    {
        dirty = false;
        Application::getInstance()->timer.off(this, &FontTexture::updateTexture);
    }
}