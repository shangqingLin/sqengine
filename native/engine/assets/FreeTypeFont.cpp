#include "FreeTypeFont.h"
#include "FontTexture.h"
#include "../core/common/Date.h"

#include <cmath>
#include <ctime>

struct FTLibrary
{
    FTLibrary()
    {
        FT_Error error = FT_Init_FreeType(&lib);
        if (error)
        {
            printf("FreeType init failed, error code: %d \n", error);
        }
    }

    ~FTLibrary()
    {
        if (lib)
        {
            FT_Error error = FT_Done_FreeType(lib);
            if (error)
            {
                printf("FreeType exit failed, error code: %d \n", error);
            }

            lib = nullptr;
        }
    }

    FT_Library lib{nullptr};
};

struct FTFace
{

    unsigned char *buffer;
    explicit FTFace(FT_Face f)
        : face(f), buffer(nullptr)
    {
    }

    ~FTFace()
    {
        if (face)
        {
            FT_Done_Face(face);
            face = nullptr;
        }

        if (buffer)
        {
            free(buffer);
            buffer = nullptr;
        }
    }

    FT_Face face{nullptr};
};

// 整个应用中，只需要一个FreeType实例即可
static FTLibrary *library = nullptr;

//====================================
void FreeTypeFontFace::init(unsigned char *fontFileData, int size)
{
    /**
     * 一个字体库称为一个FT_Face实例
     */
    FT_Face face{nullptr};
    FT_Error error = FT_New_Memory_Face(library->lib, fontFileData, size, 0, &face);

    if (error)
    {
        printf("FT_New_Memory_Face failed, error code: %d.\n", error);
        return;
    }
    _face = std::make_unique<FTFace>(face);
    _face->buffer = fontFileData;
}

const FontGlyph *FreeTypeFontFace::getGlyph(unsigned int code, int fontSize)
{

    sqstd::hash_t _hash = 666;
    sqstd::hash_combine(_hash, code);
    sqstd::hash_combine(_hash, fontSize);

    auto iter = _glyphs.find(_hash);
    if (iter != _glyphs.end())
    {
        // std::srand(static_cast<unsigned int>(std::time(nullptr)));
        // int a = std::rand() % 80 + 1;
        // FT_Face &face = _face->face;
        // FT_Set_Pixel_Sizes(face, 0, a);
        // FT_Error error = FT_Load_Char(face, code, FT_LOAD_RENDER);
        // iter->second.width = face->glyph->bitmap.width;
        // iter->second.height = face->glyph->bitmap.rows;

        iter->second.time = getDateNow();
        return &iter->second;
    }

    FontGlyph glpyh;
    if (loadGlyph(code, fontSize, glpyh))
    {
        glpyh.time = getDateNow();
        _glyphs[_hash] = glpyh;
        return &_glyphs[_hash];
    }
    return nullptr;
}
bool FreeTypeFontFace::loadGlyph(unsigned int code, int fontSize, FontGlyph &glyph)
{

    FT_Face &face = _face->face;

    FT_Error error;

    if (this->fontSize != fontSize)
    {
        this->fontSize = fontSize;
        FT_Set_Pixel_Sizes(face, 0, fontSize);
        if (error)
        {
            printf("FT_Set_Pixel_Sizes failed, error code: %d.\n", error);
            return false;
        }
    }

    error = FT_Load_Char(face, code, FT_LOAD_RENDER);
    if (error)
    {
        printf("FT_Load_Char failed, error code: %d, character: %u.\n", error, code);
        return false;
    }

    glyph.width = face->glyph->bitmap.width;
    glyph.height = face->glyph->bitmap.rows;

    bool add = false;
    if (glyph.width > 0U && glyph.height > 0U)
    {
        if (_textures.empty())
        {
            createTexture(2048, 2048);
        }

        int page = static_cast<int>(_textures.size() - 1);
        FontTexture *texture = _textures[page];

        add = texture->addFontGlyph(glyph, face->glyph->bitmap.buffer);
        if (!add)
        {
            clearGlyph();

            for (int i = _textures.size() - 1; i >= 0; --i)
            {
                texture = _textures[i];
                if (texture->addFontGlyph(glyph, face->glyph->bitmap.buffer))
                {
                    page = i;
                    add = true;
                    break;
                }
            }

            if (!add)
            {
                createTexture(2048, 2048);
                page = static_cast<int>(_textures.size() - 1);
                texture = _textures[page];
                add = texture->addFontGlyph(glyph, face->glyph->bitmap.buffer);
            }
        }
        glyph.page = page;
    }
    return add;
}

void FreeTypeFontFace::clearGlyph()
{
    std::unordered_map<unsigned int, FontGlyph>::iterator it = _glyphs.begin();
    long now = getDateNow();
    std::vector<int> updateTexture;
    updateTexture.reserve(20);

    bool find = false;
    while (it != _glyphs.end())
    {
        if (now - it->second.time > 20000)
        {
            find = false;
            for (int i = 0; i < updateTexture.size(); ++i)
            {
                if (updateTexture[i] == it->second.page)
                {
                    find = true;
                    break;
                }
            }

            if (!find)
            {
                updateTexture.push_back(it->second.page);
            }

            _textures[it->second.page]->removeFontGlyph(it->second);
            _glyphs.erase(it);
        }
        ++it;
    }

    for (int i = 0; i < updateTexture.size(); ++i)
    {
        _textures[updateTexture[i]]->repeack();
    }
}

void FreeTypeFontFace::createTexture(unsigned int width, unsigned int height)
{
    FontTexture *texture = new FontTexture(width, height);
    _textures.push_back(texture);
}

//=================================

FreeTypeFont::FreeTypeFont(int id) : Font(id)
{

    if (!library)
    {
        library = new FTLibrary();
    }
}

void FreeTypeFont::loadFontFile(char *data, int size)
{

    FreeTypeFontFace *_face = new FreeTypeFontFace();
    _face->init((unsigned char *)data, size);
    face = _face;
}