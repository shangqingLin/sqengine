#ifndef __GL_H__
#define __GL_H__

#include "../webglbase/BaseWebGLAPI.h"

namespace GLAPI
{
    class WebGL : public BaseWebGLAPI
    {
    public:
        void drawElementsInstancedANGLE(GLenum primtive, GLsizei, GLenum, unsigned int, int);
        GLVAO createVertexArrayOES();
        void bindVertexArrayOES(GLVAO vao);
        void deleteVertexArrayOES(GLVAO vao);
    };
}

#endif