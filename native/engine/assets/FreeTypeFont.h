#ifndef _FREE_TYPE_FONT_H_
#define _FREE_TYPE_FONT_H_
#include "Font.h"

/**
 * 这里只是导入一些宏#include宏而已，如下面的FT_FREETYPE_H
 * 通过这个这些宏，你可以选择性使用FreeType的功能
 */
#include <ft2build.h>
#include FT_FREETYPE_H //<freetype.h>
#include <memory>

struct FTLibrary;
struct FTFace;

class FreeTypeFontFace : public FontFace
{
private:
    int fontSize = 0;
    std::unique_ptr<FTFace> _face;
    bool loadGlyph(unsigned int code,int fontSize,FontGlyph& glyph);
    void createTexture(unsigned int width, unsigned int height);
    void clearGlyph();
public:
    void init(unsigned char *fontFileData, int size);
    virtual const FontGlyph *getGlyph(unsigned int code,int fontSize) override;
};

class FreeTypeFont : public Font
{
public:
    FreeTypeFont(int id);
    void loadFontFile(char *data, int size);
};

#endif