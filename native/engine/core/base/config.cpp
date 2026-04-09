#include "config.h"
#include <emscripten.h>

void TRACE()
{
    EM_ASM("console.trace();");
}