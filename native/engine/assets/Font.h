#ifndef _ENGINE_ASSETS_FONT_H_
#define _ENGINE_ASSETS_FONT_H_
/**
 * 通用字体定义
 */

#include "./Asset.h"
#include <unordered_map>
#include "./Texture2d.h"
#include "../core/algorithm/MaxRectPacker.h"
#include "../core/core.h"

class FontTexture;
struct FontGlyph {
    unsigned int width{0U};
    unsigned int height{0U};
    unsigned int page{0U};
    float u0,v0;
    float u1,v1;

    //保存一个时间戳，字形太久没有使用就清理掉
    long time = 0;
    algorithm::MaxRectPackerNode* textureNode = NULL;
};

class FontFace {
    protected:
        std::vector<FontTexture*> _textures;
        std::unordered_map<sqstd::hash_t, FontGlyph> _glyphs;
    public:
        virtual const FontGlyph *getGlyph(unsigned int code,int fontSiz) = 0;
        Texture2d* getTexture(unsigned int page);
};

class Font : public Asset
{
    protected:
        FontFace *face = nullptr;
    public:
        Font(int id);
        inline const FontFace* getFontFace() { return face; };
};

#endif