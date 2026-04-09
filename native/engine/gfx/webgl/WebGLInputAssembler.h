#ifndef _GFX_WEBGL_INPUT_ASSEMBLER_H_
#define _GFX_WEBGL_INPUT_ASSEMBLER_H_
#include "../base/InputAssembler.h"
#include "./WebGLGPUObjectDefine.h"
#include <optional>

class WebGLInputAssembler : public InputAssembler
{
public:
    /**
     * 1、同一个Mesh可以被多个Shader使用
     * 2、同一个Shader可以绑定到不同的Mesh上
     * 即一个Mesh可以对应多个Shader或一个Shader可以对应多个Mesh，由此可以对应多个VAO，每一个VAO实例针对一个Shader进行顶点属性分布设置。
     * 因此这里存储了Shader->VAO
     */
    std::unordered_map<GLAPI::GLProgram, GLAPI::GLVAO> shaderToVAOMap;
    virtual ~WebGLInputAssembler();
};

#endif