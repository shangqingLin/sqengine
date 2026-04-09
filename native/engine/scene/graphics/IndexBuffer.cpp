#include "./IndexBuffer.h"

IndexBuffer::IndexBuffer(int numIndices)
    : Buffer()
{
    // 使用无符号16位数值表示index,每个数字2个字节
    int bytes = numIndices << 1; // 乘以2
    createArrayBuffer(bytes);
    createBufferObject(bytes, BufferType::INDEX, BufferUsage::DYNAMIC_DRAW);
}

IndexBuffer::IndexBuffer(sqstd::Byte *buffer, bool needDeleteBuffer)
    : Buffer()
{
    createArrayBuffer(buffer, needDeleteBuffer);
    createBufferObject(buffer->getBuffSize(), BufferType::INDEX, BufferUsage::DYNAMIC_DRAW);
}

IndexBuffer::IndexBuffer(BufferUsage usage, int numIndices)
{
    int bytes = numIndices << 1; // 乘以2
    createArrayBuffer(bytes);
    createBufferObject(bytes, BufferType::INDEX, usage);
}

IndexBuffer::IndexBuffer()
{
    createArrayBuffer(0);
    createBufferObject(0, BufferType::INDEX, BufferUsage::DYNAMIC_DRAW);
}

int IndexBuffer::getCount()
{
    // return buffer->getDataSize() * 0.5;
    return buffer->getDataSize() >> 1;
}

void IndexBuffer::upload()
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
        gfxBufferObj->updateFromJs(nativeId,jsBufferSize);
    }
}

void IndexBuffer::resizeBufferFromJs(int byteSize)
{
    if (this->jsBufferSize != byteSize)
    {
        this->jsBufferSize = byteSize;
        setDirty(true);
    }
}
