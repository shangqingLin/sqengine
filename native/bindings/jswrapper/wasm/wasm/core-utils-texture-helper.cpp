#include <engine/core/utils/texture-hepler.h>
#include <emscripten.h>
#include <stdio.h>

extern "C"
{
    EMSCRIPTEN_KEEPALIVE void calculateTextureSize(int pixlesNum, int *result, int widthMultiple, bool powerOfTwo)
    {
        if (widthMultiple > 0)
        {
            utils::calculateTextureSize(pixlesNum, result, widthMultiple);
        }
        else
        {
            utils::calculateTextureSize(pixlesNum, result, powerOfTwo);
        }        
    }
}