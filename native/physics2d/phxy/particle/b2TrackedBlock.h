#pragma once

#include "SqIntrusiveList.h"

namespace phxy
{
	/// Alignment (in bytes) of user memory associated with b2TrackedBlock.
	const unsigned int b2_mallocAlignment = 32;

	/// Allocated block of memory that can be tracked in a b2IntrusiveList.
	class b2TrackedBlock : public b2TypedIntrusiveListNode<b2TrackedBlock>
	{
	private:
		// Initialize this block with a reference to "this".
		b2TrackedBlock();
		// Remove the block from the list.
		~b2TrackedBlock() {}

	public:
		/// Get the allocated memory associated with this block.
		void *GetMemory() const;

	private:
		// Padding required to align the pointer to user memory in the block
		// to b2_mallocAlignment.
		unsigned char m_padding[b2_mallocAlignment + sizeof(b2TrackedBlock **)];

	public:
		/// Allocate a b2TrackedBlock returning a pointer to memory of size
		/// bytes that can be used by the caller.
		static void *Allocate(unsigned int size);

		/// Get a b2TrackedBlock from a pointer to memory returned by
		/// b2TrackedBlock::Allocate().
		static b2TrackedBlock *GetFromMemory(void *memory);

		/// Free a block of memory returned by b2TrackedBlock::Allocate()
		static void Free(void *memory);

		/// Free a b2TrackedBlock.
		static void Free(b2TrackedBlock *block);
	};

	/// Allocator of blocks which are tracked in a list.
	class b2TrackedBlockAllocator
	{
	public:
		/// Initialize.
		b2TrackedBlockAllocator() {}
		/// Free all allocated blocks.
		~b2TrackedBlockAllocator()
		{
			FreeAll();
		}

		/// Allocate a block of size bytes using b2TrackedBlock::Allocate().
		void *Allocate(unsigned int size);

		/// Free a block returned by Allocate().
		void Free(void *memory);

		/// Free all allocated blocks.
		void FreeAll();

		// Get the list of allocated blocks.
		const b2TypedIntrusiveListNode<b2TrackedBlock> &GetList() const
		{
			return m_blocks;
		}

	private:
		b2TypedIntrusiveListNode<b2TrackedBlock> m_blocks;
	};

}
