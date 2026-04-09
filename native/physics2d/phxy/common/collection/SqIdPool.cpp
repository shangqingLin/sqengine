#include "SqIdPool.h"
#include <stdio.h>
#include "../SqCore.h"

using namespace phxy;

int SqIdPool::get()
{
    int id = 0;
    bool has = freeList.getNext<int>(id);
    if (!has)
    {
        id = nextIndex;
        nextIndex += 1;
    }
    return id;
}

void SqIdPool::recovery(int id)
{
    SQ_ASSERT(id >= 0 && id <= nextIndex);
    freeList.addValue(id);
}

bool SqIdPool::hasId(int id)
{
    return freeList.hasValue(id);
}