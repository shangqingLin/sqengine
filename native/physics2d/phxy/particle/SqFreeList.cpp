#include "SqFreeList.h"

using namespace phxy;

/// Allocate an item from the freelist.
SqIntrusiveListNode* SqFreeList::Allocate()
{
	if (m_free.IsEmpty()) return nullptr;
	SqIntrusiveListNode * const node = m_free.GetNext();
	node->Remove();
	m_allocated.InsertBefore(node);
	return node;
}

void SqFreeList::Free(SqIntrusiveListNode* node)
{
	SQ_ASSERT(node);
#if B2_FREE_LIST_CHECK_ALLOCATED_ON_FREE
	SQ_ASSERT(m_allocated.FindNodeInList(node));
#endif // B2_FREE_LIST_CHECK_ALLOCATED_ON_FREE
	node->Remove();
	m_free.InsertAfter(node);
}

void SqFreeList::AddToFreeList(SqIntrusiveListNode* node)
{
	SQ_ASSERT(node);
	SQ_ASSERT(!node->InList());
	m_free.InsertBefore(node);
}

void SqFreeList::RemoveAll()
{
	while (!m_allocated.IsEmpty()) {
		m_allocated.GetNext()->Remove();
	}
	while (!m_free.IsEmpty()) {
		m_free.GetNext()->Remove();
	}
}
