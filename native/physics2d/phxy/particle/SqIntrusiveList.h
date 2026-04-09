#pragma once

// Whether to enable b2IntrusiveList::ValidateList().
// Be careful when enabling this since this changes the size of
// SqIntrusiveListNode so make sure *all* projects that include Box2D.h
// also define this value in the same way to avoid data corruption.
#ifndef B2_INTRUSIVE_LIST_VALIDATE
#define B2_INTRUSIVE_LIST_VALIDATE 0
#endif // B2_INTRUSIVE_LIST_VALIDATE

#include "../common/SqCore.h"

namespace phxy
{

	/// SqIntrusiveListNode is used to implement an intrusive doubly-linked
	/// list.
	///
	/// For example:
	///
	/// class MyClass {
	/// public:
	/// 	MyClass(const char *msg) : m_msg(msg) {}
	/// 	const char* GetMessage() const { return m_msg; }
	/// 	B2_INTRUSIVE_LIST_GET_NODE(m_node);
	/// 	B2_INTRUSIVE_LIST_NODE_GET_CLASS(MyClass, m_node);
	/// private:
	/// 	SqIntrusiveListNode m_node;
	/// 	const char *m_msg;
	/// };
	///
	/// int main(int argc, char *argv[]) {
	/// 	SqIntrusiveListNode list; // NOTE: type is NOT MyClass
	/// 	MyClass a("this");
	/// 	MyClass b("is");
	/// 	MyClass c("a");
	/// 	MyClass d("test");
	/// 	list.InsertBefore(a.GetListNode());
	/// 	list.InsertBefore(b.GetListNode());
	/// 	list.InsertBefore(c.GetListNode());
	/// 	list.InsertBefore(d.GetListNode());
	/// 	for (SqIntrusiveListNode* node = list.GetNext();
	/// 		 node != list.GetTerminator(); node = node->GetNext()) {
	/// 		MyClass *cls = MyClass::GetInstanceFromListNode(node);
	/// 		printf("%s\n", cls->GetMessage());
	/// 	}
	/// 	return 0;
	/// }
	class SqIntrusiveListNode
	{
	public:
		/// Initialize the node.
		SqIntrusiveListNode()
		{
			Initialize();
#if B2_INTRUSIVE_LIST_VALIDATE
			m_magic = k_magic;
#endif // B2_INTRUSIVE_LIST_VALIDATE
		}

		/// If the node is in a list, remove it from the list.
		~SqIntrusiveListNode()
		{
			Remove();
#if B2_INTRUSIVE_LIST_VALIDATE
			m_magic = 0;
#endif // B2_INTRUSIVE_LIST_VALIDATE
		}

		/// Insert this node after the specified node.
		void InsertAfter(SqIntrusiveListNode *const node)
		{
			SQ_ASSERT(!node->InList());
			node->m_next = m_next;
			node->m_prev = this;
			m_next->m_prev = node;
			m_next = node;
		}

		/// Insert this node before the specified node.
		void InsertBefore(SqIntrusiveListNode *const node)
		{
			SQ_ASSERT(!node->InList());
			node->m_next = this;
			node->m_prev = m_prev;
			m_prev->m_next = node;
			m_prev = node;
		}

		/// Get the terminator of the list.
		const SqIntrusiveListNode *GetTerminator() const
		{
			return this;
		}

		/// Remove this node from the list it's currently in.
		SqIntrusiveListNode *Remove()
		{
			m_prev->m_next = m_next;
			m_next->m_prev = m_prev;
			Initialize();
			return this;
		}

		/// Determine whether this list is empty or the node isn't in a list.
		bool IsEmpty() const
		{
			return GetNext() == this;
		}

		/// Determine whether this node is in a list or the list contains nodes.
		bool InList() const
		{
			return !IsEmpty();
		}

		/// Calculate the length of the list.
		unsigned int GetLength() const
		{
			unsigned int length = 0;
			const SqIntrusiveListNode *const terminator = GetTerminator();
			for (const SqIntrusiveListNode *node = GetNext();
				 node != terminator; node = node->GetNext())
			{
				length++;
			}
			return length;
		}

		/// Get the next node in the list.
		SqIntrusiveListNode *GetNext() const
		{
			return m_next;
		}

		/// Get the previous node in the list.
		SqIntrusiveListNode *GetPrevious() const
		{
			return m_prev;
		}

		/// If B2_INTRUSIVE_LIST_VALIDATE is 1 perform a very rough validation
		/// of all nodes in the list.
		bool ValidateList() const
		{
#if B2_INTRUSIVE_LIST_VALIDATE
			if (m_magic != k_magic)
				return false;
			const SqIntrusiveListNode *const terminator = GetTerminator();
			for (SqIntrusiveListNode *node = GetNext(); node != terminator;
				 node = node->GetNext())
			{
				if (node->m_magic != k_magic)
					return false;
			}
#endif // B2_INTRUSIVE_LIST_VALIDATE
			return true;
		}

		/// Determine whether the specified node is present in this list.
		bool FindNodeInList(SqIntrusiveListNode *const nodeToFind) const
		{
			const SqIntrusiveListNode *const terminator = GetTerminator();
			for (SqIntrusiveListNode *node = GetNext(); node != terminator;
				 node = node->GetNext())
			{
				if (nodeToFind == node)
					return true;
			}
			return false;
		}

	private:
		/// Initialize the list node.
		void Initialize()
		{
			m_next = this;
			m_prev = this;
		}

	private:
#if B2_INTRUSIVE_LIST_VALIDATE
		uint32 m_magic;
#endif // B2_INTRUSIVE_LIST_VALIDATE
		/// The next node in the list.
		SqIntrusiveListNode *m_prev;
		/// The previous node in the list.
		SqIntrusiveListNode *m_next;

	private:
#if B2_INTRUSIVE_LIST_VALIDATE
		static const uint32 k_magic = 0x7157ac01;
#endif // B2_INTRUSIVE_LIST_VALIDATE
	};

/// Declares the member function GetListNode() of Class to retrieve a pointer
/// to NodeMemberName.
/// See #B2_INTRUSIVE_LIST_NODE_GET_CLASS_ACCESSOR()
#define SQ_INTRUSIVE_LIST_GET_NODE(NodeMemberName)                 \
	SqIntrusiveListNode *GetListNode() { return &NodeMemberName; } \
	const SqIntrusiveListNode *GetListNode() const { return &NodeMemberName; }

/// Declares the member function FunctionName of Class to retrieve a pointer
/// to a Class instance from a list node pointer.   NodeMemberName references
/// the name of the SqIntrusiveListNode member of Class.
#define SQ_INTRUSIVE_LIST_NODE_GET_CLASS_ACCESSOR(                         \
	Class, NodeMemberName, FunctionName)                                   \
	static Class *FunctionName(SqIntrusiveListNode *node)                  \
	{                                                                      \
		Class *cls = nullptr;                                                 \
		/* This effectively performs offsetof(Class, NodeMemberName) */    \
		/* which ends up in the undefined behavior realm of C++ but in */  \
		/* practice this works with most compilers. */                     \
		return reinterpret_cast<Class *>((uint8 *)(node) -                 \
										 (uint8 *)(&cls->NodeMemberName)); \
	}                                                                      \
                                                                           \
	static const Class *FunctionName(const SqIntrusiveListNode *node)      \
	{                                                                      \
		return FunctionName(const_cast<SqIntrusiveListNode *>(node));      \
	}

/// Declares the member function GetInstanceFromListNode() of Class to retrieve
/// a pointer to a Class instance from a list node pointer.  NodeMemberName
/// reference the name of the SqIntrusiveListNode member of Class.
#define SQ_INTRUSIVE_LIST_NODE_GET_CLASS(Class, NodeMemberName)      \
	SQ_INTRUSIVE_LIST_NODE_GET_CLASS_ACCESSOR(Class, NodeMemberName, \
											  GetInstanceFromListNode)

	/// b2TypedIntrusiveListNode which supports inserting an object into a single
	/// doubly linked list.  For objects that need to be inserted in multiple
	/// doubly linked lists, use SqIntrusiveListNode.
	///
	/// For example:
	///
	/// class IntegerItem : public b2TypedIntrusiveListNode<IntegerItem>
	/// {
	/// public:
	/// 	IntegerItem(int value) : m_value(value) { }
	/// 	~IntegerItem() { }
	/// 	int GetValue() const { return m_value; }
	/// private:
	/// 	int m_value;
	/// };
	///
	/// int main(int argc, const char *arvg[]) {
	/// 	b2TypedIntrusiveListNode<IntegerItem> list;
	/// 	IntegerItem a(1);
	/// 	IntegerItem b(2);
	/// 	IntegerItem c(3);
	/// 	list.InsertBefore(&a);
	/// 	list.InsertBefore(&b);
	/// 	list.InsertBefore(&c);
	/// 	for (IntegerItem* item = list.GetNext();
	/// 		 item != list.GetTerminator(); item = item->GetNext())
	/// 	{
	/// 		printf("%d\n", item->GetValue());
	/// 	}
	/// }
	template <typename T>
	class b2TypedIntrusiveListNode
	{
	public:
		b2TypedIntrusiveListNode() {}
		~b2TypedIntrusiveListNode() {}

		/// Insert this object after the specified object.
		void InsertAfter(T *const obj)
		{
			SQ_ASSERT(obj);
			GetListNode()->InsertAfter(obj->GetListNode());
		}

		/// Insert this object before the specified object.
		void InsertBefore(T *const obj)
		{
			SQ_ASSERT(obj);
			GetListNode()->InsertBefore(obj->GetListNode());
		}

		/// Get the next object in the list.
		/// Check against GetTerminator() before deferencing the object.
		T *GetNext() const
		{
			return GetInstanceFromListNode(GetListNode()->GetNext());
		}

		/// Get the previous object in the list.
		/// Check against GetTerminator() before deferencing the object.
		T *GetPrevious() const
		{
			return GetInstanceFromListNode(GetListNode()->GetPrevious());
		}

		/// Get the terminator of the list.
		/// This should not be dereferenced as it is a pointer to
		/// b2TypedIntrusiveListNode<T> *not* T.
		T *GetTerminator() const
		{
			return (T *)GetListNode();
		}

		/// Remove this object from the list it's currently in.
		T *Remove()
		{
			GetListNode()->Remove();
			return GetInstanceFromListNode(GetListNode());
		}

		/// Determine whether this object is in a list.
		bool InList() const
		{
			return GetListNode()->InList();
		}

		// Determine whether this list is empty.
		bool IsEmpty() const
		{
			return GetListNode()->IsEmpty();
		}

		/// Calculate the length of the list.
		unsigned int GetLength() const
		{
			return GetListNode()->GetLength();
		}

		SQ_INTRUSIVE_LIST_GET_NODE(m_node);

	private:
		// Node within an intrusive list.
		SqIntrusiveListNode m_node;

	public:
		/// Get a pointer to the instance of T that contains "node".
		static T *GetInstanceFromListNode(SqIntrusiveListNode *const node)
		{
			SQ_ASSERT(node);
			// Calculate the pointer to T from the offset.
			return (T *)((unsigned char *)node - GetNodeOffset(node));
		}

	private:
		// Get the offset of m_node within this class.
		static int GetNodeOffset(SqIntrusiveListNode *const node)
		{
			SQ_ASSERT(node);
			// Perform some type punning to calculate the offset of m_node in T.
			// WARNING: This could result in undefined behavior with some C++
			// compilers.
			T *obj = (T *)node;
			int nodeOffset = (int)((unsigned char *)&obj->m_node - (unsigned char *)obj);
			return nodeOffset;
		}
	};

}
