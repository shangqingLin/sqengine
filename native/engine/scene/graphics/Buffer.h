#pragma once
#include "../../gfx/gfx.h"
#include "../../core/sqstd/Byte.h"

class Buffer
{
protected:
    BufferObject *gfxBufferObj = nullptr;
    sqstd::Byte *buffer = nullptr;
    bool needDeleteBuffer = false;
    bool _dirty = false;
    void createBufferObject(BufferInfo &);
    void createBufferObject(int size, BufferType, BufferUsage);
    void createArrayBuffer(sqstd::Byte *buffer, bool needDeleteBuffer);
    void createArrayBuffer(int bufferSize);

public:
    Buffer();
    Buffer(sqstd::Byte *buffer, bool needDeleteBuffer);
    Buffer(int bufferSize);
    virtual ~Buffer();
    inline void setDirty(bool dirty) { this->_dirty = dirty; };
    inline bool isDirty() { return _dirty; };
    inline BufferObject *getBufferObject() const { return gfxBufferObj; };
    inline int getBufferSize() { return buffer->getBuffSize(); };
    inline int getDataSize() { return buffer->getDataSize(); };
    virtual void resizeBuffer(int size, bool allocate);
    void setExternalBuffer(sqstd::Byte *buffer);
    void setExternalBuffer(char *buffer, int size);
    virtual void upload();
    void clearData();
    template <class T>
    void append(T v)
    {
        _dirty = true;
        buffer->append<T>(v);
    }

    char *append(const void *pBufffer, int nSize)
    {
        _dirty = true;
        return buffer->append(pBufffer, nSize);
    }

    template <class T>
    void setValue(unsigned int pos, T v)
    {
        _dirty = true;
        buffer->setValue<T>(pos, v);
    }

    template <class T>
    T &getValue(unsigned int pos)
    {
        return buffer->get<T>(pos);
    }

    inline bool isEmpty()
    {
        return buffer && buffer->isEmpty();
    }
};
