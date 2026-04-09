#include "FixedSetAllocator.h"
#include "b2ParticleSetting.h"
#include "SqParticle.h"
#include <string>
#include <algorithm>

using namespace phxy;

// Associate a memory allocator with this object.
FixedSetAllocator::FixedSetAllocator(sqstd::StackTempArenaAllocator *allocator) : m_buffer(NULL), m_valid(NULL), m_count(0), m_allocator(allocator)
{
    SQ_ASSERT(allocator);
}

// Allocate internal storage for this object.
int FixedSetAllocator::Allocate(const int itemSize, const int count)
{
    Clear();
    if (count > 0)
    {
        // 正确计算需要的总内存大小：元素数据 + 有效性标志数组
        std::size_t dataSize = itemSize * count;
        std::size_t validSize = sizeof(char) * count;
        std::size_t totalSize = dataSize + validSize;

        // 分配总内存
        m_buffer = m_allocator->allocateChunk(totalSize);
        SQ_ASSERT(m_buffer);

        // 有效性标志数组在元素数据之后
        m_valid = static_cast<char *>(m_buffer) + dataSize;

        // 初始化有效性标志为 1（全部有效）
        memset(m_valid, 1, validSize);

        m_count = count;
    }
    return m_count;
}

// Deallocate the internal buffer if it's allocated.
void FixedSetAllocator::Clear()
{
    if (m_buffer)
    {
        m_allocator->freeChunk(m_buffer);
        m_buffer = NULL;
        m_count = 0;
    }
}

//=================================================================
// Search set for item returning the index of the item if it's found, -1
// otherwise.
template <typename T>
static int FindItemIndexInFixedSet(const TypedFixedSetAllocator<T> &set, const T &item)
{
    if (set.GetCount())
    {
        const T *buffer = set.GetBuffer();
        const T *last = buffer + set.GetCount();
        const T *found = std::lower_bound(buffer, buffer + set.GetCount(), item, T::Compare);
        // if (found != last)
        // {
        //     return set.GetIndex(found);
        // }
        if (found != buffer + set.GetCount() && !T::Compare(item, *found) && !T::Compare(*found, item))
        {
            return set.GetIndex(found); // 说明找到了完全相等的项
        }
        else
        {
            return -1; // 没找到
        }
    }
    return -1;
}

//========================================================
// Initialize from a set of particle contacts.
void b2ParticlePairSet::Initialize(const SqParticleContact *const contacts, const int numContacts, const unsigned int *const particleFlagsBuffer)
{
    Clear();
    if (Allocate(numContacts))
    {
        ParticlePair *set = GetBuffer();
        int insertedContacts = 0;
        for (int i = 0; i < numContacts; ++i)
        {
            ParticlePair *const pair = &set[i];
            const SqParticleContact &contact = contacts[i];
            if (contact.GetIndexA() == sq_invalidParticleIndex ||
                contact.GetIndexB() == sq_invalidParticleIndex ||
                !((particleFlagsBuffer[contact.GetIndexA()] |
                   particleFlagsBuffer[contact.GetIndexB()]) &
                  sq_particleContactListenerParticle))
            {
                continue;
            }
            pair->first = contact.GetIndexA();
            pair->second = contact.GetIndexB();
            insertedContacts++;
        }
        SetCount(insertedContacts);
        std::sort(set, set + insertedContacts, ParticlePair::Compare);
    }
}

// Find the index of a particle pair in the set or -1 if it's not present.
int b2ParticlePairSet::Find(const ParticlePair &pair) const
{
    int index = FindItemIndexInFixedSet(*this, pair);
    if (index < 0)
    {
        ParticlePair swapped;
        swapped.first = pair.second;
        swapped.second = pair.first;
        index = FindItemIndexInFixedSet(*this, swapped);
    }
    return index;
}
