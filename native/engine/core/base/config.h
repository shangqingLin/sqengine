#pragma once

#if ENABLE_ASSERTS
#include <assert.h>
#define SQ_ASSERT( condition ) assert(condition) 																										 
#else
	#define SQ_ASSERT( ... ) ( (void)0 )
#endif

void TRACE();