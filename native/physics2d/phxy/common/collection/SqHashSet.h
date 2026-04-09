#pragma once
#include <stdint.h>

namespace phxy

{

#define SQ_SHAPE_PAIR_KEY(K1, K2) K1 < K2 ? (uint64_t)K1 << 32 | (uint64_t)K2 : (uint64_t)K2 << 32 | (uint64_t)K1

    /**
     * 哈希表。记录在其中的数据不会重复
     */

    struct SqSetItem
    {
        uint64_t key;  // 哈希值对应的key
        uint32_t hash; // 哈希值。
    };

    class SqHashSet
    {
    private:
        SqSetItem *items = nullptr;

        /**
         * items数组当前容量
         */
        uint32_t capacity = 0;

        /**
         * 当前已经使用的SqSetItem的数量
         */
        uint32_t count = 0;

        void addKeyHaveCapacity(uint64_t key, uint32_t hash);
        void growTable();
        int findSlot(uint64_t key, uint32_t hash);

    public:
        SqHashSet(int);
        ~SqHashSet();

        void resize(int);
        void clear();
        bool addKey(uint64_t key);
        bool removeKey(uint64_t key);
        bool containsKey(uint64_t key);
        int getHashSetBytes();
        inline int getCount() const { return count; };
    };

}