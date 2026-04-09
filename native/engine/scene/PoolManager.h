#pragma once
#include "../core/sqstd/Array.h"
#include "../core/sqstd/BitSet.h"

/**
 * 为了避免频繁创建和销毁对象，造成过多的内存碎片和频繁的内存申请，这里提供了一个对象池
 */

template <typename T>
class PoolManager
{
private:
    /**
     * 每次触发分配时，则分配多少个对象
     */
    int append = 300;

    /**
     * 当不够对象触发分配时，不销毁原来内存区域，保持原来的不变（这样也是为了避免频繁销毁和创建、数据复制的操作）。
     * 不够时只是重新分配一个数组来存储append个对象，所以这里是一个二维数组
     */
    sqstd::Array<sqstd::Array<T>> objects;
    sqstd::BitSet pool;
    static PoolManager<T> _inst = nullptr;

public:
    inline void setAppend(int append) { this->append = append; }

    static PoolManager<T> *getInstance() { return PoolManager<T>::_inst; };

    T *get()
    {
        T *obj = nullptr;

        unsigned int saveIndex = pool.getNext<unsigned int>();
        if (saveIndex != 0)
        {
            unsigned int arrayIndex = (saveIndex >> 16) - 1u;
            unsigned int objectIndex = saveIndex & 65535u;
            obj = objects.get(arrayIndex)->get(objectIndex);
        }

        if (!obj)
        {
            sqstd::Array<T> *last = objects.getLast();
            if (last->getCount() < last->getCapacity())
            {
                obj = last->Add();
            }
        }

        if (!obj)
        {
            sqstd::Array<T> *last = objects.Add();
            last->resize(append);
            obj = last->Add();
        }

        return obj;
    }

    void recovery(T *obj)
    {
        SQ_ASSERT(obj->poolArrayIndex != 0 && obj->poolObjectIndex != 0);
        obj->onRecoveryFromPool();

        unsigned int poolArrayIndex = obj->poolArrayIndex + 1u;
        unsigned int poolObjectIndex = obj->poolObjectIndex;
        unsigned int activeIndex = poolArrayIndex << 16 | poolObjectIndex;
        pool.addValue(activeIndex);
    }
};

template <typename T>
class PoolObject
{
private:
    unsigned int poolArrayIndex = 0;
    unsigned int poolObjectIndex = 0;

public:
    friend class PoolManager<T>;
    virtual void onActionFromPool() {};
    virtual void onRecoveryFromPool() {};
};
