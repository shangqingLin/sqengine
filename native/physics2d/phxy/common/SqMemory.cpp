#include "SqMemory.h"
#include "SqCore.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

void *phxy::SqAlloc(int size)
{
	if (size == 0)
	{
		return NULL;
	}

	// Allocation must be a multiple of 32 or risk a seg fault
	// https://en.cppreference.com/w/c/memory/aligned_alloc
	int size32 = ((size - 1) | 0x1F) + 1;
#ifdef SQ_PLATFORM_WINDOWS
	void *ptr = _aligned_malloc(size32, SQ_ALIGNMENT);
#elif defined(SQ_PLATFORM_ANDROID)
	void *ptr = NULL;
	if (posix_memalign(&ptr, SQ_ALIGNMENT, size32) != 0)
	{
		// allocation failed, exit the application
		exit(EXIT_FAILURE);
	}
#else
	void *ptr = aligned_alloc(SQ_ALIGNMENT, size32);
	// void *ptr = malloc(size32);
#endif
	// printf("========alloc %p %d\n",ptr,size);
	SQ_ASSERT(ptr != NULL);
	SQ_ASSERT(((uintptr_t)ptr & 0x1F) == 0);
	return ptr;
}

void phxy::SqFree(void *mem, int size)
{
	if (mem == NULL)
	{
		return;
	}

	// printf("========free %p %d \n",mem,size);

#ifdef SQ_PLATFORM_WINDOWS
	_aligned_free(mem);
#else
	free(mem);
#endif
}

void* phxy::SqGrowAlloc( void* oldMem, int oldSize, int newSize )
{
	SQ_ASSERT( newSize > oldSize );
	void* newMem = SqAlloc( newSize );
	if ( oldMem && oldSize > 0 )
	{
		memcpy( newMem, oldMem, oldSize );
		SqFree( oldMem, oldSize );
	}
	return newMem;
}