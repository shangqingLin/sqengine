#ifndef _WEBGL_BUFFER_H_
#define _WEBGL_BUFFER_H_
#include "../base/BufferObject.h"
#include "WebGLGPUObjectDefine.h"

class WebGLBufferObject : public BufferObject
{
public:
    IGLGPUBuffer bufferData;
    WebGLBufferObject();
    ~WebGLBufferObject();
    virtual void initialize(BufferInfo &info);
    virtual void resize(int size);
    virtual void update(char *data, int size);
    virtual void updateFromJs(int nativeId, int size);
};

#endif