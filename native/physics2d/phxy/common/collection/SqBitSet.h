#pragma once
#include <stdint.h>
#include <stdio.h>
#include "../math/SqMath.h"
#include "../../../../engine/core/sqstd/StackTempArenaAllocator.h"
namespace phxy
{

    /**
     * 在程序中我们总是遇到需要保存一个int\uint的数组，传统的做法就是直接声明一个int的数组，然后将int数值直接存储进去
     * 但是大多数的时候我存储的int值根本达不到32位，这就浪费了很多内存，而这里的BitSet就是为了解决此问题的
     */
    class SqBitSet
    {
    private:
        uint64_t *bits = nullptr;
        uint32_t blockCapacity = 0;
        uint32_t blockCount = 0;
        sqstd::StackTempArenaAllocator *alloctor = nullptr;
        uint64_t *allocData(int);
        void freeData(void *, int);

    public:
        SqBitSet();
        SqBitSet(int bitCapacity);
        ~SqBitSet();

        /**
         * 销毁内存
         */
        void clearData();

        /**
         * 清除存储的所有的value，不会销毁内存
         */
        void clearValue();
        void setAlloc(sqstd::StackTempArenaAllocator *);
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
        bool getNext(T &result)
        {
            if (!bits || blockCount == 0)
                return 0;

            for (int i = 0; i < blockCount; ++i)
            {
                uint64_t value = bits[i];
                if (value != 0)
                {
                    uint32_t ctz = sqCTZ64(value);
                    value = value & (value - 1); // 取消这个位
                    bits[i] = value;
                    result = (T)(64 * i + ctz);
                    return true;
                }
            }
            return false;
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
                    uint32_t ctz = sqCTZ64(value);
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

        template <typename T, typename Callback>
        void iterate_count(int numCheck , const Callback &callback) const
        {
            if (!bits || blockCount == 0)
            {
                return;
            }

            int num = 0;

            T valueResult[8];
            for(int i = 0; i < 8 ; ++i){
                valueResult[i] = -1;
            }

            for (int i = 0; i < blockCount; ++i)
            {
                uint64_t value = bits[i];
                while (value)
                {
                    uint32_t ctz = sqCTZ64(value);
                    value = value & (value - 1);
                    T index = (T)(64 * i + ctz);

                    valueResult[num] = index;
                    ++num;

                    if(num == numCheck){
                       
                        if constexpr (std::is_same_v<std::invoke_result_t<Callback, T*>, bool>)
                        {
                            if (!callback(valueResult))
                                return;
                        }
                        else
                        {
                            callback(valueResult);
                        }
                        
                        num = 0;
                        for(int i = 0; i < numCheck ; ++i){
                            valueResult[i] = -1;
                        }
                    }
                }
            }

            if(num > 0){
                if constexpr (std::is_same_v<std::invoke_result_t<Callback, T*>, bool>)
                {
                    if (!callback(valueResult))
                        return;
                }
                else
                {
                    callback(valueResult);
                }
            }
        }

        /**
         * 将setB合并到setA中
         */
        static void inPlaceUnion(const SqBitSet *setA, const SqBitSet *setB);
    };
}