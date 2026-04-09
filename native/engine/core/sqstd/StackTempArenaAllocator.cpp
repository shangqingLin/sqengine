#include "StackTempArenaAllocator.h"
#include <stdio.h>
#include <string>
#include "../base/config.h"
#include "Array.h"

using namespace sqstd;

static StackTempArenaAllocator *_inst = nullptr;
void StackTempArenaAllocator::preAllocation(int size)
{
    if (!_inst)
    {
        _inst = new StackTempArenaAllocator(size, size);
    }
}

StackTempArenaAllocator *StackTempArenaAllocator::getInstance()
{
    return _inst;
}

StackTempArenaAllocator::StackTempArenaAllocator(int capacity, int maxCapacity)
{
    SQ_ASSERT(capacity <= maxCapacity);
    data.resize(capacity);
    dataBlock.setByte(&data, false);
    chunks = new Array<StackTempChunk>();
}

void *StackTempArenaAllocator::allocateChunk(int size)
{

    /**
     * 举个例子：
     *   假设size = 32:
     *   size - 1 = 31 -> 31 | 0x1F = 31 -> 31 + 1 = 32 -> 得到32。
     *
     *   假设size = 33:
     *   size - 1 = 32 -> 32 | 0x1F = 63 -> 63 + 1 = 64 -> 得到64。
     *
     *   假设size = 1:
     *       size - 1 = 0 -> 0 | 0x1F = 31 -> 31 + 1 = 32 -> 得到32。
     *
     * 这时因为分配的内存最好事内存对齐可以提高访问效率，尤其是对于32字节对齐的内存，在一些体系结构（如使用SIMD指令）中，可以提高性能。
     */
    int size32 = ((size - 1) | 0x1F) + 1;

    StackTempChunk *temp = nullptr;
    if (dataBlock.hasEnoughSpace(size32))
    {
        temp = chunks->Add();
        temp->chunk = dataBlock.allocateChunk(size32);
        temp->data = nullptr;
    }
    else
    {
        // 如果当前的内存空间不够了，就重新分配一块,但是这块内存不会缓存，用完freeChunk之后会立刻销毁
        temp = chunks->Add();
        temp->data = (char *)malloc(size32);
        temp->chunk = nullptr;
        SQ_ASSERT(((uintptr_t)temp->data & 0x1F) == 0);

        // printf(" warn: StackTempArenaAllocator alloc %d use memeory %d total %d\n", size32, allocation,dataBlock.buffer->getBuffSize());
        // for (int e = 0; e < dataBlock.freeLists.size(); ++e)
        // {
        //     printf("block %d %d %d \n",e,dataBlock.freeLists[e]->offset,dataBlock.freeLists[e]->length);
        // }
        // SQ_ASSERT(false);
    }

    temp->size = size32;
    allocation += size32;
    return temp->data ? temp->data : temp->chunk->getData();
}

void StackTempArenaAllocator::freeChunk(void *mem)
{

    int entryCount = chunks->getCount();
    SQ_ASSERT(entryCount > 0);

    bool find = false;

    for (int i = 0; i < chunks->getCount(); ++i)
    {
        StackTempChunk *temp = chunks->get(i);
        if (temp->chunk && temp->chunk->getData() == mem)
        {

            find = true;
            allocation -= temp->size;
            temp->chunk->recycle();
            temp->chunk = nullptr;
            temp->size = 0;
            chunks->removeSwap(i);
            break;
        }

        if (temp->data && mem == temp->data)
        {
            allocation -= temp->size;
            temp->size = 0;
            temp->data = nullptr;
            find = true;
            chunks->removeSwap(i);
            free(mem);
            break;
        }
    }
    SQ_ASSERT(find);
}

StackTempArenaAllocator::~StackTempArenaAllocator()
{
    for (int i = 0; i < chunks->getCount(); ++i)
    {
        StackTempChunk *temp = chunks->get(i);
        if (temp->data)
        {
            free(temp->data);
        }
    }
    delete chunks;
    chunks = nullptr;
}