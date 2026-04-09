#ifndef _WEBGL_SHADER_H_
#define _WEBGL_SHADER_H_
#include "../base/Shader.h"
#include "WebGLGPUObjectDefine.h"
class WebGLShader : public Shader
{
    private:
      IGLGPUShader *gpuShader;
    public:
      WebGLShader();
      virtual ~WebGLShader();
      virtual void initialize (ShaderInfo&);
      IGLGPUShader& getGPUShader();
};

#endif