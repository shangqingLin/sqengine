#ifndef _WEBGL2_BUFFER_H_
#define _WEBGL2_BUFFER_H_
#include "../base/BufferObject.h"
#include "WebGL2GPUObjectDefine.h"

class WebGL2BufferObject : public BufferObject
{
public:
    IGLGPUBuffer bufferData;
    WebGL2BufferObject();
    ~WebGL2BufferObject();
    virtual void initialize(BufferInfo &info);
    virtual void resize(int size);
    virtual void update(char* data, int size);
    virtual void updateFromJs(int nativeId,int size);
};

#endif