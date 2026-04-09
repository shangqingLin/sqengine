#pragma once

#include <string>
#include "../../../engine/core/sqstd/StackTempArenaAllocator.h"
#include "../common/SqCore.h"
namespace phxy
{
	template <typename T>
	class b2StackQueue
	{

	public:
		b2StackQueue(sqstd::StackTempArenaAllocator *allocator, int capacity)
		{
			m_allocator = allocator;
			m_buffer = (T *)m_allocator->allocateChunk(sizeof(T) * capacity);
			m_front = 0;
			m_back = 0;
			m_capacity = capacity;
		}

		~b2StackQueue()
		{
			m_allocator->freeChunk(m_buffer);
		}

		void Push(const T &item)
		{
			if (m_back >= m_capacity)
			{
				for (int i = m_front; i < m_back; i++)
				{
					m_buffer[i - m_front] = m_buffer[i];
				}
				m_back -= m_front;
				m_front = 0;
				if (m_back >= m_capacity)
				{
					int oldSize = m_capacity;
					if (m_capacity > 0)
					{
						m_capacity *= 2;
					}
					else
					{
						m_capacity = 1;
					}
					T *oldBuffer = m_buffer;
					m_buffer = (T *)m_allocator->allocateChunk(sizeof(T) * m_capacity);
					memcpy(m_buffer, oldBuffer, oldSize);
					m_allocator->freeChunk(oldBuffer);
				}
			}
			m_buffer[m_back] = item;
			m_back++;
		}

		void Pop()
		{
			SQ_ASSERT(m_front < m_back);
			m_front++;
		}

		bool Empty() const
		{
			SQ_ASSERT(m_front <= m_back);
			return m_front == m_back;
		}

		const T &Front() const
		{
			return m_buffer[m_front];
		}

	private:
		sqstd::StackTempArenaAllocator *m_allocator;
		T *m_buffer;
		unsigned int m_front;
		int m_back;
		int m_capacity;
	};

}
