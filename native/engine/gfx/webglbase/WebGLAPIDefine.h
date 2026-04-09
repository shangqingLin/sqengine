
#ifndef _WEBGL_DEFINE_H_
#define _WEBGL_DEFINE_H_
#include "../../core/core.h"

namespace GLAPI
{

    typedef unsigned int GLenum;
    typedef unsigned char GLboolean;
    typedef unsigned int GLbitfield;
    typedef signed char GLbyte;
    typedef short GLshort;
    typedef int GLint;
    typedef int GLsizei;
    typedef unsigned char GLubyte;
    typedef unsigned short GLushort;
    typedef unsigned int GLuint;
    typedef float GLfloat;
    typedef float GLclampf;
    typedef double GLdouble;
    typedef double GLclampd;
    typedef void GLvoid;
    typedef int GLBuffer;
    typedef int GLProgram;
    typedef int GLShader;
    typedef int GLTexture;
    typedef int GLFramebuffer;
    typedef int GLRenderbuffer;
    typedef int GLVAO;

    typedef int GLSampler; // webgl2.0

    /**
     * WebGL1.0和WebGL2.0同用一个，没必要搞两个，就是一个数值而已
     */
    enum class JSAPIMap : unsigned char
    {
        createBuffer,
        bindBuffer,
        bufferData,
        bufferSubData,
        bufferDataFromJs,
        createShader,
        shaderSource,
        compileShader,
        attachShader,
        detachShader,
        deleteShader,
        createProgram,
        useProgram,
        linkProgram,
        deleteProgram,
        disable,
        enable,
        cullFace,
        frontFace,
        polygonOffset,
        lineWidth,
        depthMask,
        depthFunc,
        stencilFuncSeparate,
        stencilOpSeparate,
        stencilMaskSeparate,
        blendColor,
        blendFuncSeparate,
        blendEquationSeparate,
        createTexture,
        bindTexture,
        deleteTexture,
        texParameteri,
        texImage2D,
        texSubImage2D,
        texSubImage2D_2,
        texSubImage2D_buffer,
        activeTexture,
        uniform1i,
        uniform2i,
        uniform3i,
        uniform4i,
        uniform1iv,
        uniform1f,
        uniform2f,
        uniform3f,
        uniform4f,
        uniform4fv,
        uniformMatrix4f,
        uniformMatrix2fv,
        uniformMatrix3fv,
        uniformMatrix4fv,
        enableVertexAttribArray,
        vertexAttribPointer,
        vertexAttribDivisor,
        drawElements,
        drawElementsInstancedANGLE, // WEBGL1.0
        viewport,
        scissor,
        clear,
        clearColor,
        clearDepth,
        clearStencil,
        colorMask,
        createFramebuffer,
        bindFramebuffer,
        deleteFramebuffer,
        createRenderbuffer,
        deleteRenderbuffer,
        bindRenderbuffer,
        renderbufferStorage,
        framebufferRenderbuffer,
        framebufferTexture2D,
        beginWriteTempData,
        createVertexArrayOES,  // WEBGL1.0
        bindVertexArrayOES,    // WEBGL1.0
        deleteVertexArrayOES,  // WEBGL1.0
        createVertexArray,     // WEBGL2.0
        bindVertexArray,       // WEBGL2.0
        deleteVertexArray,     // WEBGL2.0
        drawElementsInstanced, // WEBGL2.0
        drawBuffers,
        createSampler,     // WEBGL2.0
        samplerParameteri, // WEBGL2.0
        bindSampler,       // WEBGL2.0
        deleteSampler,    
        COUNT
    };

    SQ_ENUM_CONVERSION_OPERATOR(JSAPIMap);
}
#endif