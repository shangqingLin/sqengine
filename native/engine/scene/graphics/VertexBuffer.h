#ifndef _GRAPHICS_VERTEX_BUFFER_H_
#define _GRAPHICS_VERTEX_BUFFER_H_
#include <vector>
#include "../../core/sqstd/ByteBlock.h"
#include "Buffer.h"
#include "../../../bindings/binding.h"

/**
 * 代表顶点属性数据
 */
class VertexBuffer : public Buffer, public bridge::JsToNativeObject
{
private:
    sqstd::ByteBlock *byteBlock = NULL;
    void _createBufferObject(int size, BufferUsage);
    int jsBufferSize = 0;

public:
    VertexBuffer();
    VertexBuffer(sqstd::Byte *buffer, bool needDeleteBuffer);
    VertexBuffer(BufferUsage);

    void resizeBufferFromJs(int byteSize);

    virtual ~VertexBuffer();

    sqstd::ByteBlock *getByteBlock();

    virtual void upload() override;
};

#endif