#pragma once
#include <new>
#include <stdio.h>
#include <algorithm>
#include "./os/memory.h"
#include "StackTempArenaAllocator.h"
#include "../base/config.h"

namespace sqstd
{

    class StackTempArenaAllocator;

    /**
     * 必须注意：
     *  外部不要存储指针，因为Array是一个可变的数组，当大小补足时会重新分配内存
     *  那么旧的数据的所有地址都改变了，如果外部还持有旧的地址，那么拿到肯定是错误的数据
     *  所以，外部应该持有数组的index，而不是数据的指针
     *
     */
    template <typename T>
    class Array
    {
    private:
        StackTempArenaAllocator *allocator = nullptr;

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
        Array() {};
        Array(StackTempArenaAllocator *allocator) : allocator(allocator) {};

        ~Array()
        {
            freeData();
            allocator = nullptr;
        }
        void freeData()
        {
            if (capacity == 0)
                return;

            SQ_ASSERT(data);
            // printf("+++ delete array %p\n", this);
            if (constexpr bool has_destructor_v = std::is_destructible<T>::value)
            {
                for (int i = 0; i < capacity; ++i)
                {
                    (data + i)->~T();
                }
            }

            if (allocator)
            {
                allocator->freeChunk(data);
            }
            else
            {
                free(data);
            }
            data = nullptr;
            count = 0;
            capacity = 0;
        }
        inline void setTempAllocator(StackTempArenaAllocator *allocator) { this->allocator = allocator; };
        inline const T *getData() const { return data; };

        void setData(T *data, int count)
        {
            this->data = data;
            this->count = count;
        };

        inline void clear() noexcept __attribute__((always_inline)) { count = 0; }
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

            if (allocator)
            {

                void *newData = allocator->allocateChunk(newCapacity * sizeof(T));

                // printf("resize %p new %d %p old %d %d %p\n", this, newCapacity * sizeof(T), newData, oldCacity, sizeof(T), data);
                if (data)
                {
                    memcpy(newData, (void *)data, oldCacity * sizeof(T));
                    allocator->freeChunk(data);
                }
                data = (T *)newData;
            }
            else
            {

                T *oldData = data;
                data = (T *)GrowAlloc(data, oldCacity * sizeof(T), newCapacity * sizeof(T));

                // printf("GrowAlloc %p new %d %p old %d %d %p\n", this, newCapacity * sizeof(T), data, oldCacity, sizeof(T), oldData);
            }

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

        T &operator[](int index) const
        {
            SQ_ASSERT(0 <= index && index < count);
            return data[index];
        }

        T *getLast() const
        {
            SQ_ASSERT(count > 0);
            return data + (count - 1);
        }

        int push(const T &value)
        {
            if (count == capacity)
            {
                // 在原来的基础上2倍增长
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
            int movedIndex = -1;
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
                freeData();
            }
        }

        /**
         * 删除数组中满足条件的元素
         */
        template <class UnaryPredicate>
        T *removeIf(UnaryPredicate pred)
        {
            T *newEnd = std::remove_if(data, data + count, pred);
            Shorten(newEnd);
            return newEnd;
        }

        /**
         * 对数组元素排序
         */
        template <typename Callback>
        void sort(Callback compare)
        {
            if (count == 0)
                return;

            /**
             * 返回false : b 在 a 的前面
             * 返回true : a 在 b 的前面
             *  */
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

        /**
         * 去掉重复元素
         */
        template <class BinaryPredicate>
        T *unique(BinaryPredicate pred)
        {
            T *newEnd = std::unique(data, data + count, pred);
            Shorten(newEnd);
            return newEnd;
        }

        /**
         * 移除指定位置上的一个元素
         * 但内存并不会释放，只是把删除的Index挪到最后，后面的元素往前挪
         */
        void removeAt(int index)
        {
            if (index < count)
            {
                int elementCount = 0;
                auto newEnd = std::remove_if(data, data + count, [&elementCount, index](T &)
                                             {
                                                 elementCount++;
                                                 return elementCount == index; });
                Shorten(newEnd);
            }
        }

        void insert(int pos, const T &value)
        {
            if (count == capacity)
            {
                int newCapacity = capacity < 2 ? 2 : capacity + (capacity >> 1);
                resize(newCapacity);
            }

            // 如果是简单类型，使用memmove（更高效）
            if constexpr (std::is_trivially_copyable_v<T>)
            {

                // 将位置pos后面的元素全部往后复制
                std::memmove(data + pos + 1, data + pos, (count - pos) * sizeof(T));

                data[pos] = value;
            }
            // 如果是复杂类型，逐个移动构造
            else
            {
                // 先在末尾构造新元素
                new (data + count) T(value);

                // 从末尾（新插入的元素）开始一个一个后前挪
                for (size_t i = count; i > pos; --i)
                {
                    std::swap(data[i], data[i - 1]);
                }
            }

            ++count;
        }
    };
}