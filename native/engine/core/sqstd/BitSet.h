#pragma once
#include <stdint.h>
#include <stdio.h>
#include "../math/math.h"

namespace sqstd
{

    /**
     * 在程序中我们总是遇到需要保存一个int\uint的数组，传统的做法就是直接声明一个int的数组，然后将int数值直接存储进去
     * 但是大多数的时候我存储的int值根本达不到32位，这就浪费了很多内存，而这里的BitSet就是为了解决此问题的
     */
    class BitSet
    {
    private:
        uint64_t *bits = nullptr;
        uint32_t blockCapacity = 0;
        uint32_t blockCount = 0;
        uint64_t *allocData(int);
        void freeData(void *, int);

    public:
        BitSet();
        BitSet(int bitCapacity);
        ~BitSet();

        /**
         * 销毁内存
         */
        void clearData();

        /**
         * 清除存储的所有的value，不会销毁内存
         */
        void clearValue();
        void removeValue(uint32_t bitIndex);
        void addValue(uint32_t bitIndex);
        bool hasValue(uint32_t bitIndex);
        bool isEmpty();
        uint32_t getValue(uint32_t blockIndex);
        int getBitSetBytes();
        void growBitSet(uint32_t blockCount);
        void setBitCountAndClear(uint32_t bitCount);
        inline uint32_t getBlockCount() { return blockCount; };
        inline uint64_t *getData() { return bits; };

        template <typename T>
        T getNext()
        {
            if (!bits || blockCount == 0)
                return 0;

            for (int i = 0; i < blockCount; ++i)
            {
                uint64_t value = bits[i];
                if (value != 0)
                {
                    uint32_t ctz = Math::CTZ64(value);
                    value = value & (value - 1); // 取消这个位
                    bits[i] = value;
                    return (T)(64 * i + ctz);
                }
            }
            return 0;
        }

        template <typename T, typename Callback>
        void iterate(const Callback &callback) const
        {
            if (!bits || blockCount == 0)
            {
                return;
            }

            for (int i = 0; i < blockCount; ++i)
            {
                uint64_t value = bits[i];
                while (value)
                {
                    uint32_t ctz = Math::CTZ64(value);
                    value = value & (value - 1);
                    T index = (T)(64 * i + ctz);
                    // 如果callback返回bool，则检查返回值；如果返回void，则直接调用并继续
                    if constexpr (std::is_same_v<std::invoke_result_t<Callback, T>, bool>)
                    {
                        if (!callback(index))
                            return;
                    }
                    else
                    {
                        callback(index);
                    }
                }
            }
        }

        /**
         * 将setB合并到setA中
         */
        static void inPlaceUnion(const BitSet *setA, const BitSet *setB);
    };
}