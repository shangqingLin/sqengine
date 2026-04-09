#include "Font.h"
#include "FontTexture.h"

Font::Font(int id) : Asset(id)
{
}

Texture2d* FontFace::getTexture(unsigned int page){
     return _textures[page]->getTexture();
}