#include "VertexBuffer.h"

VertexBuffer::VertexBuffer() : Buffer()
{
    _createBufferObject(0, BufferUsage::DYNAMIC_DRAW);
    createArrayBuffer(0);
}

VertexBuffer::VertexBuffer(sqstd::Byte *buffer, bool needDeleteBuffer)
    : Buffer()
{
    _createBufferObject(buffer->getBuffSize(), BufferUsage::DYNAMIC_DRAW);
    createArrayBuffer(buffer, needDeleteBuffer);
}

VertexBuffer::VertexBuffer(BufferUsage usage)
{
    _createBufferObject(0, usage);
    createArrayBuffer(0);
}

void VertexBuffer::_createBufferObject(int size, BufferUsage usage)
{
    BufferInfo info;
    info.usage = usage;
    info.type = BufferType::VERTEX;
    info.size = size;
    createBufferObject(info);
}

 sqstd::ByteBlock *VertexBuffer::getByteBlock()
{
    if (!byteBlock)
    {
        byteBlock = new sqstd::ByteBlock(buffer);
    }
    return byteBlock;
}

VertexBuffer::~VertexBuffer()
{
    if (byteBlock)
    {
        delete byteBlock;
        byteBlock = NULL;
    }
}

void VertexBuffer::upload()
{
    if (!_dirty)
        return;

    if (nativeId == -1)
    {
        Buffer::upload();
    }
    else
    {
        _dirty = false;
        SQ_ASSERT(this->jsBufferSize);

        // 表示这个Buffer是JS端构建的，则数据在JS端，调用不同的命令来更新数据
        gfxBufferObj->updateFromJs(nativeId, jsBufferSize);
    }
}

void VertexBuffer::resizeBufferFromJs(int byteSize)
{
    if (this->jsBufferSize != byteSize)
    {
        this->jsBufferSize = byteSize;
        setDirty(true);
    }
}
