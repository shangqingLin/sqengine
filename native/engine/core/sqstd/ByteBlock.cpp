#include "ByteBlock.h"
#include "../base/config.h"
#include "Array.h"

using namespace sqstd;

void ByteBlockChunk::initialize()
{
    buffer.setExternalBuffer(srcBuffer->getBuffer() + offset, length);
    buffer.clearData();
}

void *ByteBlockChunk::getData()
{
    return buffer.getBuffer();
}

void ByteBlockChunk::recycle()
{
    bufferView->recycleChunk(this);
}

ByteBlockChunk::~ByteBlockChunk()
{
    // printf("+++++++++++++++++delete ByteBlockChunk \n");
}

//=======================================

ByteBlock::ByteBlock() : buffer(nullptr), needDeleteBuffer(false)
{
}

ByteBlock::ByteBlock(Byte *buffer) : buffer(buffer),
                                     needDeleteBuffer(false)
{
    initialize();
}

ByteBlock::ByteBlock(Byte *buffer, bool needDeleteBuffer) : buffer(buffer),
                                                            needDeleteBuffer(needDeleteBuffer)
{
    initialize();
}

void ByteBlock::setByte(Byte *byte, bool needDeleteBuffer)
{
    clear();
    buffer = byte;
    this->needDeleteBuffer = needDeleteBuffer;
    this->initialize();
}

void ByteBlock::initialize()
{
    ByteBlockEntry *entry = new ByteBlockEntry();
    entry->offset = 0;
    entry->length = buffer->getBuffSize();
    freeLists.push_back(entry);
}

bool ByteBlock::hasEnoughSpace(int needSize)
{
    SQ_ASSERT(needSize > 0);
    for (int e = 0; e < freeLists.size(); ++e)
    {
        if (freeLists[e]->length >= needSize)
        {
            return true;
        }
    }
    return false;
}

ByteBlockChunk *ByteBlock::allocateChunk(int size)
{
    SQ_ASSERT(size > 0);
    ByteBlockEntry *entry = NULL;
    int offset, length, entryIndex;
    for (int e = 0; e < freeLists.size(); ++e)
    {
        // 只要长度足够就可以使用的
        if (freeLists[e]->length >= size)
        {
            entry = freeLists[e];
            offset = entry->offset;
            length = entry->length;
            entryIndex = e;
            break;
        }
    }

    // 按照道理来说entry不会为空，除非没有剩余空间了
    SQ_ASSERT(entry);

    ByteBlockChunk *chunk = nullptr;
    if (chunkPool.size() > 0)
    {
        chunk = chunkPool[chunkPool.size() - 1];
        chunkPool.pop_back();
    }
    else
    {
        chunk = new ByteBlockChunk();
    }

    activeChunks.push_back(chunk);
    chunk->bufferView = this;
    chunk->offset = offset;
    chunk->length = size;
    chunk->srcBuffer = buffer;
    chunk->initialize();

    // printf("===allocateChunk : entry %p offset %d needsize %d  bufferSize %d \n", entry, offset, size, buffer->getBuffSize());

    // 剩余还有多少内存没有被使用
    int remaining = entry->length - size;

    if (remaining == 0)
    {
        // 表示这段内存刚好够使用，那么就没有剩余可以使用的Entry，则删除当前Entry
        freeLists.erase(freeLists.begin() + entryIndex);
        entryPool.push_back(entry);
    }
    else
    {
        // 还有剩余空间，则记录一下留给下一个chunk使用
        entry->offset += size;
        entry->length = remaining;
    }

    updateUseMemory();

    // for (int i = freeLists.size() - 1; i >= 0; --i)
    // {
    //     ByteBlockEntry *current = freeLists[i];
    //     printf(" after alloc entry %d %d %d \n", i, current->offset, current->length);
    // }
    return chunk;
}

void ByteBlock::recycleChunk(ByteBlockChunk *chunk)
{

    {
        bool find = false;
        for (int i = 0; i < activeChunks.size(); ++i)
        {
            if (activeChunks[i] == chunk)
            {
                find = true;
                activeChunks.erase(activeChunks.begin() + i);
                break;
            }
        }
        SQ_ASSERT(find);
    }
    // 如果回收的空间和前后可用的空间是连续的，则合并一下，减少内存碎片

    // 查找这段需要回收的内存在哪两个Entry之间。查找这段内存所在的区间
    ByteBlockEntry *prev = NULL;
    ByteBlockEntry *next = NULL;
    int nextIndex = -1;

    int max = freeLists.size() - 1;
    bool recycled = false;

    // printf("====recycleChunk  : entry %d chunk offset %d length %d \n", freeLists.size(), chunk->offset, chunk->length);

    for (int i = 0; i < freeLists.size(); ++i)
    {
        ByteBlockEntry *entry = freeLists[i];

        // printf("check %d entry offset %d len %d chunk off %d len %d \n", i, entry->offset, entry->length, chunk->offset, chunk->length);
        if (entry->offset < chunk->offset)
        {
            nextIndex = i + 1;
            if (nextIndex <= max)
            {
                ByteBlockEntry *nextCurrent = freeLists[nextIndex];
                if (nextCurrent->offset > chunk->offset)
                {
                    prev = entry;
                    next = nextCurrent;
                    break;
                }
            }
            else
            {
                prev = entry;
            }
        }
        else
        {
            next = entry;
            nextIndex = i;
            break;
        }
    }

    // 判断是否是连续的内存，如果是连续的，则合并
    if (prev && next)
    {
        // 回收的内存段在 两个Entry之间

        SQ_ASSERT(chunk->offset + chunk->length <= next->offset);
        SQ_ASSERT(chunk->offset >= prev->offset + prev->length);

        if (prev->offset + prev->length == chunk->offset)
        {
            // 与前面那一个Entry内存连续
            prev->length += chunk->length;
            recycled = true;

            // 看看是否也与后面的Entry也连续，一起合并了。
            if (prev->offset + prev->length == next->offset)
            {
                prev->length += next->length;

                SQ_ASSERT(nextIndex != -1);

                // 删除这个Next Entry，不要了
                freeLists.erase(freeLists.begin() + nextIndex);
                entryPool.push_back(next);
            }
        }
        else if (chunk->offset + chunk->length == next->offset)
        {
            // 与后面那一个Entry内存连续
            next->offset = chunk->offset;
            next->length = chunk->length + next->length;
            recycled = true;
        }
    }
    else if (prev)
    {
        // 表示回收的内存段位于 prev 内存段之后

        // 检查一下是否重叠了。
        SQ_ASSERT(prev->offset + prev->length <= chunk->offset);

        if (prev->offset + prev->length == chunk->offset)
        {
            // prev->length = chunk->offset + chunk->length;
            prev->length += chunk->length;
            recycled = true;
        }
    }
    else if (next)
    {

        // 检查一下是否重叠了。按照道理来说不会
        SQ_ASSERT(chunk->offset + chunk->length <= next->offset);

        // 表示回收的内存段位于 next 内存段之前
        if (chunk->offset + chunk->length == next->offset)
        {
            next->offset = chunk->offset;
            next->length = chunk->length + next->length;
            recycled = true;
        }
    }

    // 表示找不到连续可以合并的空间，则插入一个Entry标记这段空间可用
    // 经过这里的操作之后，可用的空间不再是连续的了
    if (!recycled)
    {

        ByteBlockEntry *newEntry = nullptr;
        if (entryPool.size() > 0)
        {
            newEntry = entryPool[entryPool.size() - 1];
            entryPool.pop_back();
        }
        else
        {
            newEntry = new ByteBlockEntry();
        }
        newEntry->offset = chunk->offset;
        newEntry->length = chunk->length;

        // 保证entry根据offset从小到大排序，确保上面查找区间的逻辑正确

        // 基于二分法查找对应的位置
        auto it = std::lower_bound(freeLists.begin(), freeLists.end(), newEntry,
                                   [](ByteBlockEntry *a, ByteBlockEntry *b)
                                   {
                                       return a->offset < b->offset;
                                   });
        freeLists.insert(it, newEntry);
    }
    updateUseMemory();

    // for (int i = freeLists.size() - 1; i >= 0; --i)
    // {
    //     ByteBlockEntry *current = freeLists[i];
    //     printf(" after recycleChunk entry %d %d %d \n", i, current->offset, current->length);
    // }

    chunkPool.push_back(chunk);
}

void ByteBlock::updateUseMemory()
{
    /**
     * 此处记录一下Buffer中有多少内存被使用了
     */
    if (freeLists.empty())
    {
        // 没有空闲的内存片段，表示所有内存使用完毕
        buffer->m_nDataSize = buffer->getBuffSize();
    }
    else
    {
        ByteBlockEntry *lastEntry = freeLists[freeLists.size() - 1];
        buffer->m_nDataSize = lastEntry->offset;
    }
}

void ByteBlock::clear()
{
    if (needDeleteBuffer)
    {
        delete buffer;
        buffer = NULL;
    }

    for (int e = 0; e < freeLists.size(); ++e)
    {
        delete freeLists[e];
    }
    freeLists.clear();

    for (int e = 0; e < entryPool.size(); ++e)
    {
        delete entryPool[e];
    }
    entryPool.clear();

    for (int i = 0; i < activeChunks.size(); ++i)
    {
        delete activeChunks[i];
    }
    activeChunks.clear();

    for (int i = 0; i < chunkPool.size(); ++i)
    {
        delete chunkPool[i];
    }
    chunkPool.clear();
}

ByteBlock::~ByteBlock()
{
    clear();
}