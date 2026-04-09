#include "IDGen.h"

int IDGen::get()
{
    if (!pool.empty())
    {
        int id = pool[pool.size() - 1];
        pool.pop_back();
        return id;
    }
    return ++id;
}

void IDGen::recvoery(int id)
{
    if (pool.capacity() <= pool.size())
    {
        pool.reserve(pool.capacity() + 100);
    }
    pool.push_back(id);
}