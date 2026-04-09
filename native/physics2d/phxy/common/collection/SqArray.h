#pragma once
#include "../SqMemory.h"
#include "../constants-define.h"
#include <new>
#include <stdio.h>
#include <algorithm>

namespace phxy
{

    /**
     * 必须注意：
     *  外部不要存储指针，因为Array是一个可变的数组，当大小补足时会重新分配内存
     *  那么旧的数据的所有地址都改变了，如果外部还持有旧的地址，那么拿到肯定是错误的数据
     *  所以，外部应该持有数组的index，而不是数据的指针
     *
     */
    template <typename T>
    class SqArray
    {
    private:
        T *data = nullptr;

        /**
         * 数组中的元素个数
         */
        int count = 0;

        /**
         * 当前数组的内存空间大小。不是以字节为单位的，是元素为大小，即可以存储元素的个数
         */
        int capacity = 0;

        void Shorten(const T *newEnd)
        {
            SQ_ASSERT(newEnd >= data);
            count = (int)(newEnd - data);
        }

    public:
        ~SqArray()
        {
            SqFree(data, getByteCount());
        }

        inline const T *getData() const { return data; };
        inline void clear() { count = 0; };
        inline int getByteCount() const { return capacity * sizeof(T); };
        inline int getCount() const { return count; };
        inline int getCapacity() const { return capacity; }
        void resize(int newCapacity)
        {
            if (newCapacity <= capacity)
            {
                return;
            }

            int oldCacity = capacity;
            data = (T *)SqGrowAlloc(data, capacity * sizeof(T), newCapacity * sizeof(T));
            capacity = newCapacity;

            // 如果不是trivially constructible，调用构造函数，初始化类
            if constexpr (!std::is_trivially_constructible<T>::value)
            {
                for (int i = oldCacity; i < capacity; ++i)
                {
                    new (data + i) T();
                }
            }
        }

        T *pop()
        {
            SQ_ASSERT(count > 0);
            --count;
            return data + count;
        }

        void set(int index, T value) const
        {
            SQ_ASSERT(0 <= index && index < count);
            data[index] = value;
        }

        T *get(int index) const
        {
            SQ_ASSERT(0 <= index && index < count);
            return data + index;
        }

        int push(T value)
        {
            if (count == capacity)
            {
                // 在原来的基础上2呗增长
                int newCapacity = capacity < 2 ? 2 : capacity + (capacity >> 1);
                resize(newCapacity);
            }

            data[count] = value;
            count += 1;
            return count;
        }

        /**
         * 总是拿数组最后的元素覆盖index处的元素，达到删除index，有填补当前空间，保持连续
         */
        int removeSwap(int index)
        {
            SQ_ASSERT(0 <= index && index < count);
            int movedIndex = SQ_NULL_INDEX;
            if (index != count - 1)
            {
                movedIndex = count - 1;
                data[index] = data[movedIndex];
            }
            count -= 1;
            return movedIndex;
        }

        /**
         * 有就从缓存中拿，没有就重新创建一个
         */
        T *Add()
        {
            if (count == capacity)
            {
                int newCapacity = capacity < 2 ? 2 : capacity + (capacity >> 1);
                resize(newCapacity);
            }
            count += 1;
            return data + (count - 1);
        }

        inline T *begin() const
        {
            return data;
        }

        inline T *end() const
        {
            return data + count;
        }

        void shrink()
        {
            if (data)
            {
                SqFree(data, getByteCount());
                data = nullptr;
                count = 0;
                capacity = 0;
            }
        }

        template <class UnaryPredicate>
        T *removeIf(UnaryPredicate pred)
        {
            T *newEnd = std::remove_if(data, data + count, pred);
            Shorten(newEnd);
            return newEnd;
        }

        void sort(bool (*compare)(const T &, const T &))
        {
            if (count == 0)
                return;
            std::sort(begin(), end(), compare);
        }

        /**
         * 调用之前记得调用sort排序，因为这里使用二分查找
         * @compare 排序函数
         * @equals 基于二分法查找
         */
        T *findWithBinarySearch(T *item, bool (*compare)(const T &, const T &), bool (*equals)(const T &, const T &))
        {
            if (count == 0)
                return nullptr;
            const T *found = std::lower_bound(begin(), end(), *item, compare);
            if (found != end() && equals(*item, *found))
            {
                return (T *)found;
            }
            return nullptr;
        }

        template <class BinaryPredicate>
        T *unique(BinaryPredicate pred)
        {
            T *newEnd = std::unique(data, data + count, pred);
            Shorten(newEnd);
            return newEnd;
        }
    };
}