
#include "b2TrackedBlock.h"
#include <stddef.h>
#include <stdint.h>
#include <new>

using namespace phxy;

// Initialize this block with a reference to "this".
b2TrackedBlock::b2TrackedBlock()
{
	b2TrackedBlock **pointerToThis =
		(b2TrackedBlock **)((unsigned char *)GetMemory() - sizeof(b2TrackedBlock **));
	*pointerToThis = this;
}

/// Get the allocated memory associated with this block.
void *b2TrackedBlock::GetMemory() const
{
	// The size of data in this without padding.
	static const unsigned int kSizeOfThisWithNoPadding =
		sizeof(*this) - sizeof(m_padding) + sizeof(b2TrackedBlock **);

	// Make sure b2_mallocAlignment is base2.
	SQ_ASSERT(((b2_mallocAlignment - 1) & b2_mallocAlignment) == 0);

	// Round the pointer following data in this to b2_mallocAlignment.
	unsigned char *const aligned = (unsigned char *)(((uintptr_t)this + kSizeOfThisWithNoPadding + b2_mallocAlignment - 1) &
													 ~((uintptr_t)b2_mallocAlignment - 1));
	// Verify offset doesn't overlap data in this.
	SQ_ASSERT((uintptr_t)aligned - (uintptr_t)this >= kSizeOfThisWithNoPadding);
	return aligned;
}

/// Allocate a b2TrackedBlock returning a pointer to memory of size
/// bytes that can be used by the caller.
void *b2TrackedBlock::Allocate(unsigned int size)
{
	void *memory = (b2TrackedBlock *)malloc(sizeof(b2TrackedBlock) + size);
	if (!memory)
	{
		return NULL;
	}
	return (new (memory) b2TrackedBlock)->GetMemory();
}

/// Get a b2TrackedBlock from a pointer to memory returned by
/// b2TrackedBlock::Allocate().
b2TrackedBlock *b2TrackedBlock::GetFromMemory(void *memory)
{
	unsigned char *const aligned = (unsigned char *)memory;
	SQ_ASSERT(memory);
	b2TrackedBlock **blockPtr = (b2TrackedBlock **)(aligned -
													sizeof(b2TrackedBlock **));

	SQ_ASSERT(*blockPtr);
	return *blockPtr;
}

/// Free a block of memory returned by b2TrackedBlock::Allocate()
void b2TrackedBlock::Free(void *memory)
{
	Free(GetFromMemory(memory));
}

/// Free a b2TrackedBlock.
void b2TrackedBlock::Free(b2TrackedBlock *block)
{
	SQ_ASSERT(block);
	block->~b2TrackedBlock();
	free(block);
}

/// Allocate a block of size bytes using b2TrackedBlock::Allocate().
void *b2TrackedBlockAllocator::Allocate(unsigned int size)
{
	void *memory = b2TrackedBlock::Allocate(size);
	m_blocks.InsertBefore(b2TrackedBlock::GetFromMemory(memory));
	return memory;
}

/// Free a block returned by Allocate().
void b2TrackedBlockAllocator::Free(void *memory)
{
	b2TrackedBlock::Free(memory);
}

/// Free all allocated blocks.
void b2TrackedBlockAllocator::FreeAll()
{
	while (!m_blocks.IsEmpty())
	{
		b2TrackedBlock::Free(m_blocks.GetNext());
	}
}
