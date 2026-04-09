#include "Buffer.h"

Buffer::Buffer(sqstd::Byte *buffer, bool needDeleteBuffer)
    : buffer(buffer),
      needDeleteBuffer(needDeleteBuffer)
{
}

Buffer::Buffer(int bufferSize)
{
    createArrayBuffer(bufferSize);
}

Buffer::Buffer()
{
}

void Buffer::createArrayBuffer(sqstd::Byte *buffer, bool needDeleteBuffer)
{
    this->buffer = buffer;
    this->needDeleteBuffer = needDeleteBuffer;
}

void Buffer::createArrayBuffer(int bufferSize)
{
    buffer = new sqstd::Byte();
    if (bufferSize > 0)
        buffer->resize(bufferSize);
    needDeleteBuffer = true;
}

void Buffer::createBufferObject(BufferInfo &info)
{
    gfxBufferObj = DeviceManager::getInstance()->device->createBuffer(info);
}

void Buffer::createBufferObject(int size, BufferType type, BufferUsage usage)
{
    BufferInfo info;
    info.size = size;
    info.type = type;
    info.usage = usage;
    gfxBufferObj = DeviceManager::getInstance()->device->createBuffer(info);
}

void Buffer::upload()
{
    if (_dirty)
    {
        _dirty = false;
        if (buffer->getDataSize() > 0)
        {
            gfxBufferObj->update(buffer->getBuffer(), buffer->getDataSize());
        }
    }
}

void Buffer::resizeBuffer(int size, bool allocate)
{

    // 有些需求不需要对本地内存做分配,因为可能内存不来自本Buffer分配而是别的地方分配的
    // 这里需求只需要对显存进行分配
    if (allocate)
    {
        buffer->resize(size);
    }
    else
    {
        buffer->_setDataSize(size);
        buffer->setBufferSize(size);
    }

    gfxBufferObj->resize(size);
}

void Buffer::clearData()
{
    buffer->clearData();
}

void Buffer::setExternalBuffer(sqstd::Byte *ebuffer)
{
    if (needDeleteBuffer && buffer)
    {
        delete buffer;
    }
    needDeleteBuffer = false;
    buffer = ebuffer;
}

void Buffer::setExternalBuffer(char *p, int size)
{
    if (needDeleteBuffer && buffer)
    {
        delete buffer;
        buffer = nullptr;
    }
    needDeleteBuffer = true;

    if (!buffer)
        buffer = new sqstd::Byte;
    buffer->setExternalBuffer(p, size);
}

Buffer::~Buffer()
{
    if (needDeleteBuffer)
    {
        delete buffer;
    }
    buffer = nullptr;

    if (gfxBufferObj)
    {
        delete gfxBufferObj;
        gfxBufferObj = nullptr;
    }
}