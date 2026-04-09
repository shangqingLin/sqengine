#include "SqHashSet.h"
#include "../SqCore.h"
#include "../SqMemory.h"
#include "../math/SqMath.h"

using namespace phxy;

// 我需要一个好的哈希，因为键是由成对的递增整数构成的。
// 像 hash = (integer1 XOR integral2) 这样的简单哈希存在许多冲突,容易重复。
// https://lemire.me/blog/2018/08/15/fast-strongly-universal-64-bit-hashing-everywhere/
// https://preshing.com/20130107/this-hash-set-is-faster-than-a-judy-array/
// todo try: https://www.jandrewrogers.com/2019/02/12/fast-perfect-hashing/
// todo try: https://probablydance.com/2018/06/16/fibonacci-hashing-the-optimization-that-the-world-forgot-or-a-better-alternative-to-integer-modulo/
static uint32_t GeneratorKeyHash(uint64_t key)
{
    // Murmur hash
    uint64_t h = key;
    h ^= h >> 33;
    h *= 0xff51afd7ed558ccduLL;
    h ^= h >> 33;
    h *= 0xc4ceb9fe1a85ec53uLL;
    h ^= h >> 33;

    return (uint32_t)h;
}

SqHashSet::SqHashSet(int capacity)
{
    resize(capacity);
}

void SqHashSet::resize(int newCapacity)
{
    if(capacity >= newCapacity) return;

    if (capacity > 16)
    {
        capacity = roundUpPowerOf2(capacity);
    }
    else
    {
        capacity = 16;
    }

    count = 0;
    items = (SqSetItem*)SqAlloc(capacity * sizeof(SqSetItem));
    memset(items, 0, capacity * sizeof(SqSetItem));
}

SqHashSet::~SqHashSet()
{
    SqFree(items, capacity * sizeof(SqSetItem));
    items = NULL;
    count = 0;
    capacity = 0;
}

void SqHashSet::clear()
{
    count = 0;
    memset(items, 0, capacity * sizeof(SqSetItem));
}

void SqHashSet::growTable()
{
    uint32_t oldCount = count;

    uint32_t oldCapacity = capacity;
    SqSetItem *oldItems = items;

    count = 0;
    // Capacity must be a power of 2
    capacity = 2 * oldCapacity;
    items = (SqSetItem *)SqAlloc(capacity * sizeof(SqSetItem));
    memset(items, 0, capacity * sizeof(SqSetItem));

    // 将原来的数据存储新的数组中
    for (uint32_t i = 0; i < oldCapacity; ++i)
    {
        SqSetItem *item = oldItems + i;
        if (item->hash == 0)
        {
            // 如果为0则表示这个item还没有被使用
            continue;
        }
        addKeyHaveCapacity(item->key, item->hash);
    }

    SQ_ASSERT(count == oldCount);
    SqFree(oldItems, oldCapacity * sizeof(SqSetItem));
}

void SqHashSet::addKeyHaveCapacity(uint64_t key, uint32_t hash)
{
    int index = findSlot(key, hash);
    SQ_ASSERT(items[index].hash == 0);

    items[index].key = key;
    items[index].hash = hash;
    count += 1;
}

int SqHashSet::findSlot(uint64_t key, uint32_t hash)
{
    // 尽量使用hash值作为index，这样能够快速找到需要的index
    // 比如hash值为1，capacity>0 任何值都是返回1
    // 比如hash值为2，capacity>0 任何值都是返回2
    int index = hash & (capacity - 1);
    while (items[index].hash != 0 && items[index].key != key)
    {
        index = (index + 1) & (capacity - 1);
    }
    return index;
}

/**
 * 添加一个key，如果key存在，则返回true，否则返回false
 */
bool SqHashSet::addKey(uint64_t key)
{

    SQ_ASSERT(key != 0);

    uint32_t hash = GeneratorKeyHash(key);
    SQ_ASSERT(hash != 0);

    int index = findSlot(key, hash);
    if (items[index].hash != 0)
    {
        // 表示这个key已经在当前Set中了，没必要在添加
        SQ_ASSERT(items[index].hash == hash && items[index].key == key);
        return true;
    }

    if (2 * count >= capacity)
    {
        growTable();
    }

    addKeyHaveCapacity(key, hash);
    return false;
}

bool SqHashSet::removeKey(uint64_t key)
{
    uint32_t hash = GeneratorKeyHash(key);
    int i = findSlot(key, hash);

    if (items[i].hash == 0)
    {
        // Not in set
        return false;
    }

    // Mark item i as unoccupied
    items[i].key = 0;
    items[i].hash = 0;

    SQ_ASSERT(count > 0);
    count -= 1;

    // Attempt to fill item i
    int j = i;
    for (;;)
    {
        j = (j + 1) & (capacity - 1);
        if (items[j].hash == 0)
        {
            break;
        }

        // k is the first item for the hash of j
        int k = items[j].hash & (capacity - 1);

        // determine if k lies cyclically in (i,j]
        // i <= j: | i..k..j |
        // i > j: |.k..j  i....| or |....j     i..k.|
        if (i <= j)
        {
            if (i < k && k <= j)
            {
                continue;
            }
        }
        else
        {
            if (i < k || k <= j)
            {
                continue;
            }
        }

        // Move j into i
        items[i] = items[j];

        // Mark item j as unoccupied
        items[j].key = 0;
        items[j].hash = 0;

        i = j;
    }

    return true;
}

bool SqHashSet::containsKey(uint64_t key)
{
    SQ_ASSERT(key != 0);
    uint32_t hash = GeneratorKeyHash(key);
    int index = findSlot(key, hash);
    return items[index].key == key;
}

int SqHashSet::getHashSetBytes()
{
    return capacity * (int)sizeof(SqSetItem);
}