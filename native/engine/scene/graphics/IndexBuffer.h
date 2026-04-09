#pragma once
#include "../../core/common/ArrayBuffer.h"
#include "Buffer.h"
#include "../../../bindings/binding.h"

class IndexBuffer : public Buffer, public bridge::JsToNativeObject
{
private:
    int jsBufferSize = 0;
    
public:
    IndexBuffer();
    IndexBuffer(sqstd::Byte *buffer, bool needDeleteBuffer);
    IndexBuffer(int numIndices);
    IndexBuffer(BufferUsage, int numIndices);

    void resizeBufferFromJs(int byteSize);

    /**
     * 获取当前存储了多少个索引
     */
    int getCount();
    virtual void upload();
};
