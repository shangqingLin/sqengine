#ifndef _SHADER_H_
#define _SHADER_H_

#include "define.h"

class Shader{
    public:
        virtual void initialize (ShaderInfo&) = 0;
        virtual ~Shader() = default;
};

#endif