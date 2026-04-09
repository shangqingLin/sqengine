#ifndef _WEBGL_SHADER_H_
#define _WEBGL_SHADER_H_
#include "../base/Shader.h"
#include "WebGL2GPUObjectDefine.h"
class WebGL2Shader : public Shader
{
private:
  IGLGPUShader *gpuShader;

public:
  WebGL2Shader();
  virtual ~WebGL2Shader();
  virtual void initialize(ShaderInfo &);
  IGLGPUShader &getGPUShader();
};

#endif