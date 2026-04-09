#pragma once

#include "b2BlockAllocator.h"
#include <string.h>
#include <memory.h>
#include <algorithm>
#include "b2ParticleSetting.h"
#include "../common/SqCore.h"

namespace phxy
{
	/// A simple array-like container, similar to std::vector.
	/// If we ever start using stl, we should replace this with std::vector.
	template <typename T>
	class b2GrowableBuffer
	{
	public:
		b2GrowableBuffer(b2BlockAllocator &allocator) : data(nullptr),
														count(0),
														capacity(0),
														allocator(&allocator)
		{
			// b2ParticleAssembly.neon.s assumes these values are at fixed offsets.
			// If this assert fails, be sure to update the assembly offsets!
			// ldr r3, [r9, #0] @ r3 = out = contacts.data
			// ldr r6, [r9, #8] @ r6 = contacts.capacity
			SQ_ASSERT((intptr_t)&data - (intptr_t)(this) == 0 && (intptr_t)&capacity - (intptr_t)(this) == 8);
		}

		b2GrowableBuffer(const b2GrowableBuffer<T> &rhs) : data(nullptr),
														   count(rhs.count),
														   capacity(rhs.capacity),
														   allocator(rhs.allocator)
		{
			if (rhs.data != nullptr)
			{
				data = (T *)allocator->Allocate(sizeof(T) * capacity);
				memcpy(data, rhs.data, sizeof(T) * count);
			}
		}

		~b2GrowableBuffer()
		{
			Free();
		}

		T &Append()
		{
			if (count >= capacity)
			{
				Grow();
			}
			return data[count++];
		}

		void Reserve(int newCapacity)
		{
			if (capacity >= newCapacity)
				return;

			// Reallocate and copy.
			T *newData = (T *)allocator->Allocate(sizeof(T) * newCapacity);
			if (data)
			{
				memcpy(newData, data, sizeof(T) * count);
				allocator->Free(data, sizeof(T) * capacity);
			}

			// Update pointer and capacity.
			capacity = newCapacity;
			data = newData;
		}

		void Grow()
		{
			// Double the capacity.
			int newCapacity = capacity ? 2 * capacity : sq_minParticleSystemBufferCapacity;
			SQ_ASSERT(newCapacity > capacity);
			Reserve(newCapacity);
		}

		void Free()
		{
			if (data == nullptr)
				return;

			allocator->Free(data, sizeof(data[0]) * capacity);
			data = nullptr;
			capacity = 0;
			count = 0;
		}

		void Shorten(const T *newEnd)
		{
			SQ_ASSERT(newEnd >= data);
			count = (int)(newEnd - data);
		}

		T &operator[](int i)
		{
			return data[i];
		}

		const T &operator[](int i) const
		{
			return data[i];
		}

		T *Data()
		{
			return data;
		}

		const T *Data() const
		{
			return data;
		}

		T *Begin()
		{
			return data;
		}

		const T *Begin() const
		{
			return data;
		}

		T *End()
		{
			return &data[count];
		}

		const T *End() const
		{
			return &data[count];
		}

		int GetCount() const
		{
			return count;
		}

		void SetCount(int newCount)
		{
			SQ_ASSERT(0 <= newCount && newCount <= capacity);
			count = newCount;
		}

		int GetCapacity() const
		{
			return capacity;
		}

		template <class UnaryPredicate>
		T *RemoveIf(UnaryPredicate pred)
		{
			T *newEnd = std::remove_if(data, data + count, pred);
			Shorten(newEnd);
			return newEnd;
		}

		template <class BinaryPredicate>
		T *Unique(BinaryPredicate pred)
		{
			T *newEnd = std::unique(data, data + count, pred);
			Shorten(newEnd);
			return newEnd;
		}

		void sort(bool (*compare)(const T &, const T &))
		{
			if (count == 0)
				return;
			std::sort(this->Begin(), this->End(), compare);
		}

		/**
		 * 调用之前记得调用sort排序，这里使用二分查找
		 * @param 要超值的值
		 * @param 比较函数
		 */
		T *findWithBinarySearch(T *item, bool (*compare)(const T &, const T &), bool (*equals)(const T &, const T &))
		{
			if (count == 0)
				return nullptr;

			const T *found = std::lower_bound(this->Begin(), this->End(), *item, compare);
			if (found != this->End() && equals(*item, *found))
			{
				return (T *)found;
			}
			return nullptr;
		}

	private:
		T *data;
		int count;
		int capacity;
		b2BlockAllocator *allocator;
	};

}
