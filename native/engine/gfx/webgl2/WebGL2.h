#ifndef __GL2_H__
#define __GL2_H__

#include "../webglbase/BaseWebGLAPI.h"

namespace GLAPI
{
    class WebGL2 : public BaseWebGLAPI
    {
    public:
        void drawElementsInstanced(GLenum primtive, GLsizei, GLenum, unsigned int, int);
        GLVAO createVertexArray();
        void bindVertexArray(GLVAO vao);
        void deleteVertexArray(GLVAO vao);
        GLSampler createSampler();
        void samplerParameteri(GLSampler, GLenum target, GLenum value);
        void bindSampler(int, GLSampler);
        void deleteSampler(GLSampler);
    };
}

#endif