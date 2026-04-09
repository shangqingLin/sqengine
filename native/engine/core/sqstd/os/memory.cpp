#include "memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../base/config.h"

using namespace sqstd;

void *operator new(size_t p_size, const char *p_description)
{
	return Memory::alloc_static(p_size, false);
}

void *operator new(size_t p_size, void *(*p_allocfunc)(size_t p_size))
{
	return p_allocfunc(p_size);
}

inline bool is_power_of_2(size_t x) { return x && ((x & (x - 1U)) == 0U); }

void *Memory::alloc_aligned_static(size_t p_bytes, size_t p_alignment)
{
	void *p1, *p2;
	if ((p1 = (void *)malloc(p_bytes + p_alignment - 1 + sizeof(uint32_t))) == nullptr)
	{
		return nullptr;
	}

	p2 = (void *)(((uintptr_t)p1 + sizeof(uint32_t) + p_alignment - 1) & ~((p_alignment)-1));
	*((uint32_t *)p2 - 1) = (uint32_t)((uintptr_t)p2 - (uintptr_t)p1);
	return p2;
}

void *Memory::realloc_aligned_static(void *p_memory, size_t p_bytes, size_t p_prev_bytes, size_t p_alignment)
{
	if (p_memory == nullptr)
	{
		return alloc_aligned_static(p_bytes, p_alignment);
	}

	void *ret = alloc_aligned_static(p_bytes, p_alignment);
	memcpy(ret, p_memory, p_prev_bytes);
	free_aligned_static(p_memory);
	return ret;
}

void Memory::free_aligned_static(void *p_memory)
{
	uint32_t offset = *((uint32_t *)p_memory - 1);
	void *p = (void *)((uint8_t *)p_memory - offset);
	free(p);
}

void *Memory::alloc_static(size_t p_bytes, bool p_pad_align)
{
	void *mem = malloc(p_bytes + (p_pad_align ? DATA_OFFSET : 0));

	if (p_pad_align)
	{
		// 前面的字节记录一下当前分配的内存大小
		uint8_t *s8 = (uint8_t *)mem;
		uint64_t *s = (uint64_t *)(s8 + SIZE_OFFSET);
		*s = p_bytes;

		// 真正外部记录数据的地址
		return s8 + DATA_OFFSET;
	}
	else
	{
		return mem;
	}
}

void *Memory::realloc_static(void *p_memory, size_t p_bytes, bool p_pad_align)
{
	if (p_memory == nullptr)
	{
		return alloc_static(p_bytes, p_pad_align);
	}

	uint8_t *mem = (uint8_t *)p_memory;
	bool prepad = p_pad_align;

	if (prepad)
	{
		mem -= DATA_OFFSET;
		uint64_t *s = (uint64_t *)(mem + SIZE_OFFSET);
		if (p_bytes == 0)
		{
			free(mem);
			return nullptr;
		}
		else
		{
			*s = p_bytes;
			mem = (uint8_t *)realloc(mem, p_bytes + DATA_OFFSET);
			s = (uint64_t *)(mem + SIZE_OFFSET);

			*s = p_bytes;

			return mem + DATA_OFFSET;
		}
	}
	else
	{
		mem = (uint8_t *)realloc(mem, p_bytes);
		return mem;
	}
}

void Memory::free_static(void *p_ptr, bool p_pad_align)
{
	uint8_t *mem = (uint8_t *)p_ptr;
	bool prepad = p_pad_align;
	if (prepad)
	{
		mem -= DATA_OFFSET;
		free(mem);
	}
	else
	{
		free(mem);
	}
}

uint64_t Memory::get_mem_available()
{
	return -1; // 0xFFFF...
}

uint64_t Memory::get_mem_usage()
{
	return 0;
}

uint64_t Memory::get_mem_max_usage()
{
	return 0;
}

void sqstd::postinitialize_handler(void *)
{
}

void *sqstd::GrowAlloc(void *oldMem, int oldSize, int newSize)
{
	SQ_ASSERT(newSize > oldSize);
	void *newMem = malloc(newSize);
	if (oldMem && oldSize > 0)
	{
		memcpy(newMem, oldMem, oldSize);
		free(oldMem);
	}
	return newMem;
}