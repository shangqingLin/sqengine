
#pragma once

#include "SqIntrusiveList.h"
#include "../common/SqCore.h"

/// When B2_FREE_LIST_CHECK_ALLOCATED_ON_FREE is 1, SqFreeList::Free() will
/// check that the deallocated node was allocated from the freelist.
#ifndef B2_FREE_LIST_CHECK_ALLOCATED_ON_FREE
#define B2_FREE_LIST_CHECK_ALLOCATED_ON_FREE 0
#endif // B2_FREE_LIST_CHECK_ALLOCATED_ON_FREE

namespace phxy
{

	/// Fast - O(1) - list based allocator for items that can be inserted into
	/// SqIntrusiveListNode lists.
	class SqFreeList
	{
	public:
		/// Construct the free list.
		SqFreeList() {}

		/// Destroy the free list.
		~SqFreeList() {}

		/// Allocate an item from the freelist.
		SqIntrusiveListNode *Allocate();

		/// Free an item from the freelist.
		void Free(SqIntrusiveListNode *node);

		/// Add an item to the freelist so that it can be allocated using
		/// SqFreeList::Allocate().
		void AddToFreeList(SqIntrusiveListNode *node);

		/// Remove all items (allocated and free) from the freelist.
		void RemoveAll();

		/// Get the list which tracks allocated items.
		const SqIntrusiveListNode &GetAllocatedList() const
		{
			return m_allocated;
		}

		/// Get the list which tracks free items.
		const SqIntrusiveListNode &GetFreeList() const
		{
			return m_free;
		}

	protected:
		/// List of allocated items.
		SqIntrusiveListNode m_allocated;
		/// List of free items.
		SqIntrusiveListNode m_free;
	};

	/// Typed SqFreeList which manages items of type T assuming T implements
	/// the GetInstanceFromListNode() and GetListNode() methods.
	template <typename T>
	class SqTypedFreeList
	{
	public:
		/// Construct the free list.
		SqTypedFreeList() {}

		/// Destroy the free list.
		~SqTypedFreeList() {}

		/// Allocate an item from the free list.
		T *Allocate()
		{
			SqIntrusiveListNode *const node = m_freeList.Allocate();
			if (!node)
				return nullptr;
			return T::GetInstanceFromListNode(node);
		}

		/// Free an item.
		void Free(T *instance)
		{
			SQ_ASSERT(instance);
			m_freeList.Free(instance->GetListNode());
		}

		/// Add an item to the freelist so that it can be allocated with
		/// SqTypedFreeList::Allocate().
		void AddToFreeList(T *instance)
		{
			SQ_ASSERT(instance);
			m_freeList.AddToFreeList(instance->GetListNode());
		}

		// Get the underlying SqFreeList.
		SqFreeList *GetFreeList() { return &m_freeList; }
		const SqFreeList *GetFreeList() const { return &m_freeList; }

	protected:
		SqFreeList m_freeList;
	};

}
