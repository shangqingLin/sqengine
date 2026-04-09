#include "BufferView.h"
#include "../base/config.h"

void ArrayBufferChunk::initialize()
{
    buffer.setExternalBuffer(srcBuffer->getBuffer() + offset, length);
    buffer.clearData();
}

void ArrayBufferChunk::recycle()
{
    bufferView->recycleChunk(this);
}

//=======================================
ArrayBufferView::ArrayBufferView(ArrayBuffer *buffer) : buffer(buffer),
                                                        needDeleteBuffer(false)
{
    initialize();
}

ArrayBufferView::ArrayBufferView(ArrayBuffer *buffer, bool needDeleteBuffer) : buffer(buffer),
                                                                               needDeleteBuffer(needDeleteBuffer)
{
    initialize();
}

void ArrayBufferView::initialize()
{
    ArrayBufferEntry *entry = new ArrayBufferEntry();
    entry->offset = 0;
    entry->length = buffer->getBuffSize();
    freeLists.push_back(entry);
}

bool ArrayBufferView::hasEnoughSpace(int needSize)
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

ArrayBufferChunk *ArrayBufferView::allocateChunk(int size)
{
    SQ_ASSERT(size > 0);
    ArrayBufferEntry *entry = NULL;
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

    ArrayBufferChunk *chunk = new ArrayBufferChunk();
    chunk->bufferView = this;
    chunk->offset = offset;
    chunk->length = size;
    chunk->srcBuffer = buffer;
    chunk->initialize();

    // printf("======entry %p offset %d needsize %d datasize %d bufferSize %d \n", entry, offset, size, chunk->buffer.getDataSize(), chunk->buffer.getBuffSize());

    // 剩余还有多少内存没有被使用
    int remaining = entry->length - size;

    if (remaining == 0)
    {
        // 表示这段内存刚好够使用，那么就没有剩余可以使用的Entry，则删除当前Entry
        freeLists.erase(freeLists.begin() + entryIndex);
        delete entry;
    }
    else
    {
        // 还有剩余空间，则记录一下留给下一个chunk使用
        entry->offset += size;
        entry->length = remaining;
    }

    updateUseMemory();

    return chunk;
}

void ArrayBufferView::recycleChunk(ArrayBufferChunk *chunk)
{

    // 如果回收的空间和前后可用的空间是连续的，则合并一下，减少内存碎片

    // 查找这段需要回收的内存在哪两个Entry之间。查找这段内存所在的区间
    ArrayBufferEntry *prev = NULL;
    ArrayBufferEntry *next = NULL;
    int nextIndex = -1;
    int max = freeLists.size() - 1;
    bool recycled = false;
    for (int i = 0; i < freeLists.size(); ++i)
    {
        ArrayBufferEntry *current = freeLists[i];
        if (current->offset < chunk->offset)
        {
            nextIndex = i + 1;
            if (nextIndex <= max)
            {
                ArrayBufferEntry *nextCurrent = freeLists[nextIndex];
                if (nextCurrent->offset > chunk->offset)
                {
                    prev = current;
                    next = nextCurrent;
                    break;
                }
            }
            else
            {
                prev = current;
            }
        }
        else
        {
            next = current;
            break;
        }
    }

    // 判断是否是连续的内存，如果是连续的，则合并
    if (prev && next)
    {
        // 看看是否可以合并前后

        if (prev->offset + prev->length == chunk->offset)
        {
            prev->length += chunk->length;
            recycled = true;

            // 看看后面是否可以合并
            if (prev->offset + prev->length == next->offset)
            {
                prev->length += next->length;
                freeLists.erase(freeLists.begin() + nextIndex);
                delete next;
            }
        }
        else if (chunk->offset + chunk->length == next->offset)
        {
            // 前面不可以合并，看看是否可以合并后面
            next->offset = chunk->offset;
            next->length = chunk->length + next->length;
            recycled = true;
        }
    }
    else if (prev)
    {
        if (prev->offset + prev->length == chunk->offset)
        {
            prev->length = chunk->offset + chunk->length;
            recycled = true;
        }
    }
    else if (next)
    {
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
        ArrayBufferEntry *entry = new ArrayBufferEntry();
        entry->offset = chunk->offset;
        entry->length = chunk->length;

        // 保证entry根据offset从小到大排序，确保上面查找区间的逻辑正确
        int insertIndex = -1;
        for (int i = freeLists.size() - 1; i >= 0; --i)
        {
            ArrayBufferEntry *current = freeLists[i];
            if (chunk->offset < current->offset)
            {
                insertIndex = i;
                break;
            }
        }

        if (insertIndex != -1)
        {
            auto it = freeLists.begin() + insertIndex;
            freeLists.insert(it, entry);
        }
        else
        {
            freeLists.push_back(entry);
        }
    }
    updateUseMemory();
    delete chunk;
}

void ArrayBufferView::updateUseMemory()
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
        ArrayBufferEntry *lastEntry = freeLists[freeLists.size() - 1];
        buffer->m_nDataSize = lastEntry->offset;
    }
}

ArrayBufferView::~ArrayBufferView()
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
}