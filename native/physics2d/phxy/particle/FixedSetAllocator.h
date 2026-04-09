#pragma once

#include "SqParticleContact.h"
#include "../../../engine/core/sqstd/StackTempArenaAllocator.h"
#include <type_traits>
#include <stdio.h>
#include "../common/SqCore.h"

namespace phxy
{
    // *Very* lightweight pair implementation.
    template <typename A, typename B>
    struct LightweightPair
    {
        A first;
        B second;

        // Compares the value of two FixtureParticle objects returning
        // true if left is a smaller value than right.
        static bool Compare(const LightweightPair &left, const LightweightPair &right)
        {
            // return left.first < right.first && left.second < right.second;
            return (left.first < right.first) || (!(right.first < left.first) && left.second < right.second);
        }

        // static bool ComparePartilceAndShape(const LightweightPair &left, const LightweightPair &right)
        // {
        //     return ((b2ShapeId)left.first).index1 < ((b2ShapeId)right.first).index1 && left.second < right.second;
        // }
    };

    // Associates a fixture with a particle index.
    typedef LightweightPair<int, int> ParticlePair;

    // Allocator for a fixed set of items.
    class FixedSetAllocator
    {
    public:
        // Associate a memory allocator with this object.
        FixedSetAllocator(sqstd::StackTempArenaAllocator *allocator);
        // Deallocate storage for this class.
        ~FixedSetAllocator()
        {
            Clear();
        }

        // Allocate internal storage for this object returning the size.
        int Allocate(const int itemSize, const int count);

        // Deallocate the internal buffer if it's allocated.
        void Clear();

        // Get the number of items in the set.
        int GetCount() const { return m_count; }

        // Invalidate an item from the set by index.
        void Invalidate(const int itemIndex)
        {
            SQ_ASSERT(m_valid);
            m_valid[itemIndex] = 0;
        }

        // Get the buffer which indicates whether items are valid in the set.
        const char *GetValidBuffer() const { return m_valid; }

    protected:
        // Get the internal buffer.
        void *GetBuffer() const { return m_buffer; }
        void *GetBuffer() { return m_buffer; }

        // Reduce the number of items in the set.
        void SetCount(int count)
        {
            SQ_ASSERT(count <= m_count);
            m_count = count;
        }

    private:
        // Set buffer.
        void *m_buffer;
        // Array of size m_count which indicates whether an item is in the
        // corresponding index of m_set (1) or the item is invalid (0).
        char *m_valid;
        // Number of items in m_set.
        int m_count;
        // Allocator used to allocate / free the set.
        sqstd::StackTempArenaAllocator *m_allocator;
    };

    // Allocator for a fixed set of objects.
    template <typename T>
    class TypedFixedSetAllocator : public FixedSetAllocator
    {
    public:
        // Initialize members of this class.
        TypedFixedSetAllocator(sqstd::StackTempArenaAllocator *allocator) : FixedSetAllocator(allocator) {}

        // Allocate a set of objects, returning the new size of the set.
        int Allocate(const int numberOfObjects)
        {
            Clear();
            return FixedSetAllocator::Allocate(sizeof(T), numberOfObjects);
        }

        // Get the index of an item in the set if it's valid return an index
        // >= 0, -1 otherwise.
        int GetIndex(const T *item) const
        {
            if (item)
            {
                SQ_ASSERT(item >= GetBuffer() && item < GetBuffer() + GetCount());
                const int index = (int)(((unsigned char *)item - (unsigned char *)GetBuffer()) / sizeof(*item));
                if (GetValidBuffer()[index])
                {
                    return index;
                }
            }
            return -1;
        }

        // Get the internal buffer.
        const T *GetBuffer() const
        {
            return (const T *)FixedSetAllocator::GetBuffer();
        }
        T *GetBuffer() { return (T *)FixedSetAllocator::GetBuffer(); }
    };

    // Set of particle / particle pairs.
    class b2ParticlePairSet : public TypedFixedSetAllocator<ParticlePair>
    {
    public:
        // Initialize members of this class.
        b2ParticlePairSet(sqstd::StackTempArenaAllocator *allocator) : TypedFixedSetAllocator<ParticlePair>(allocator) {}

        // Initialize from a set of particle contacts.
        void Initialize(const SqParticleContact *const contacts,
                        const int numContacts,
                        const unsigned int *const particleFlagsBuffer);

        // Find the index of a particle pair in the set or -1
        // if it's not present.
        // NOTE: This was not written as a template function to avoid
        // exposing any dependencies via this header.
        int Find(const ParticlePair &pair) const;
    };
}