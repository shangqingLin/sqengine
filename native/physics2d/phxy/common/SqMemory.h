#pragma once
#include "SqCore.h"

#include <string.h>
#if defined( SQ_COMPILER_MSVC )
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <stdlib.h>
#else
#include <stdlib.h>
#endif


// 使用了32为内存字节对齐方式。因为这样更好地和 256 bit 的 SIMD 工作.
#define SQ_ALIGNMENT 32

namespace phxy
{

    void* SqAlloc(int size);
    void SqFree(void *mem, int size);
    void* SqGrowAlloc( void* oldMem, int oldSize, int newSize );
}