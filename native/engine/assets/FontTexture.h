#ifndef _FONT_TEXTURE_H_
#define _FONT_TEXTURE_H_
#include "Texture2d.h"
#include "Font.h"
#include "../core/algorithm/MaxRectPacker.h"

/**
 * 将字形存储在一个纹理中
 */
class FontTexture
{
private:
    /**
     * 纹理大小，像素单位
     */
    const int textureWidth{0U};
    const int textureHeight{0U};
    algorithm::MaxRectPacker packer;
    int removeGlyphCount = 0;

    /**
     * 因为文字需要逐个字更新，那么有两种做法
     * 1、每次调用WebGL接口直接更新显存中的纹理数据，这样造成调用WebGL API太多，卡顿
     * 2、在应用端保存纹理数据，每次只更新textureData数据。然后统一最后一次调用WebGL API更新
     */
    unsigned char* textureData;

    Texture2d *texture;

    bool dirty = false;

    void addFontBuffer(FontGlyph &,unsigned char *fontAlphaBuffer, int offsetX, int offsetY);
    bool allocate(FontGlyph &glyph);
    void updateTexture(float dt);
public:
    FontTexture(int maxWidth, int maxHeight);
    ~FontTexture();
    bool addFontGlyph(FontGlyph &glyph, unsigned char *buffer);
    void removeFontGlyph(FontGlyph &glyph);
    void repeack();
    inline Texture2d* getTexture(){ return texture; };
    void showDebugSprite(float posX,float posY);
};

#endif