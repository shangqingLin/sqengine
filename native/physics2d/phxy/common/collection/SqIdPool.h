#pragma once
#include "SqBitSet.h"
namespace phxy
{
    class SqIdPool
    {
    private:
        SqBitSet freeList;
        int nextIndex = 0;
    public:
        int get();
        void recovery(int id);
        bool hasId(int id);
    };

}
