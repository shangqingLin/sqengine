#ifndef _WEBGLAPI_H_
#define _WEBGLAPI_H_
#include "../../../bindings/nativetojs/NativeToJsObject.h"
#include "../../core/common/IDGen.h"
#include "WebGLAPIDefine.h"

namespace GLAPI
{
    class BaseWebGLAPI
    {
    protected:
        bridge::NativeToJsObject jsObject;
        IDGen idGen;

    public:
        BaseWebGLAPI();
        const GLenum ARRAY_BUFFER = 1;
        const GLenum ELEMENT_ARRAY_BUFFER = 2;
        const GLenum DYNAMIC_DRAW = 3;
        const GLenum STATIC_DRAW = 4;
        const GLenum VERTEX_SHADER = 5;
        const GLenum FRAGMENT_SHADER = 6;
        const GLenum CULL_FACE = 7;
        const GLenum FRONT = 8;
        const GLenum BACK = 9;
        const GLenum CCW = 10;
        const GLenum CW = 11;
        const GLenum DEPTH_TEST = 12;
        const GLenum STENCIL_TEST = 13;
        const GLenum BLEND = 14;

        const GLenum TEXTURE_2D = 15;
        const GLenum REPEAT = 16;
        const GLenum CLAMP_TO_EDGE = 17;
        const GLenum LINEAR = 18;
        const GLenum TEXTURE_WRAP_S = 19;
        const GLenum TEXTURE_WRAP_T = 20;
        const GLenum TEXTURE_MAG_FILTER = 21;
        const GLenum TEXTURE_MIN_FILTER = 22;
        const GLenum NEAREST = 23;
        const GLenum MIRRORED_REPEAT = 24;
        const GLenum NEAREST_MIPMAP_NEAREST = 25;
        const GLenum NEAREST_MIPMAP_LINEAR = 26;
        const GLenum LINEAR_MIPMAP_LINEAR = 27;

        const GLenum UNSIGNED_BYTE = 200;
        const GLenum UNSIGNED_SHORT = 201;
        const GLenum UNSIGNED_INT = 202;
        const GLenum BYTE = 203;
        const GLenum SHORT = 204;
        const GLenum FLOAT = 205;
        const GLenum INT = 206;

        // Blend
        const GLenum ZERO = 207;
        const GLenum ONE = 208;
        const GLenum SRC_COLOR = 209;
        const GLenum DST_COLOR = 210;
        const GLenum ONE_MINUS_SRC_COLOR = 211;
        const GLenum ONE_MINUS_DST_COLOR = 212;
        const GLenum SRC_ALPHA = 213;
        const GLenum DST_ALPHA = 214;
        const GLenum ONE_MINUS_DST_ALPHA = 215;
        const GLenum CONSTANT_COLOR = 216;
        const GLenum ONE_MINUS_CONSTANT_COLOR = 217;
        const GLenum CONSTANT_ALPHA = 218;
        const GLenum ONE_MINUS_CONSTANT_ALPHA = 219;
        const GLenum ONE_MINUS_SRC_ALPHA = 220;
        const GLenum SRC_ALPHA_SATURATE = 221;
        const GLenum FUNC_ADD = 222;
        const GLenum FUNC_SUBTRACT = 223;
        const GLenum FUNC_REVERSE_SUBTRACT = 224;
        const GLenum MIN = 225;
        const GLenum MAX = 226;

        const GLenum DEPTH_BUFFER_BIT = 0x00000100;   // 256 这么大的数是为了方面做位运算
        const GLenum STENCIL_BUFFER_BIT = 0x00000400; // 1024
        const GLenum COLOR_BUFFER_BIT = 0x00004000;   // 16384

        const GLenum FRAMEBUFFER = 300;
        const GLenum RENDERBUFFER = 301;
        const GLenum COLOR_ATTACHMENT0 = 302;
        const GLenum COLOR_ATTACHMENT1 = 303;
        const GLenum COLOR_ATTACHMENT2 = 304;
        const GLenum COLOR_ATTACHMENT3 = 305;
        const GLenum COLOR_ATTACHMENT4 = 306;
        const GLenum COLOR_ATTACHMENT5 = 307;
        const GLenum COLOR_ATTACHMENT6 = 308;
        const GLenum COLOR_ATTACHMENT7 = 309;
        const GLenum DEPTH_STENCIL_ATTACHMENT = 340;
        const GLenum DEPTH_ATTACHMENT = 341;

        void viewport(GLint, GLint, GLsizei, GLsizei);
        void scissor(GLint, GLint, GLsizei, GLsizei);

        void clear(GLbitfield);
        void clearColor(GLclampf, GLclampf, GLclampf, GLclampf);
        void clearDepth(GLclampf);
        void clearStencil(GLint);
        void colorMask(GLboolean, GLboolean, GLboolean, GLboolean);

        GLBuffer createBuffer();
        void bindBuffer(GLenum target, GLBuffer buffer);
        void bufferData(GLenum target, GLsizei size, GLenum usage);
        void bufferSubData(GLenum target, GLsizei size, char *data, GLsizei updateSize);
        void bufferDataFromJs(GLenum target, int bufferNativeId);
        GLShader createShader(GLenum glShaderType);

        void shaderSource(GLShader shader, int source);
        void compileShader(GLShader shader);
        void attachShader(GLProgram program, GLShader shader);
        void detachShader(GLProgram glProgram, GLShader glShader);
        void deleteShader(GLShader glShader);
        GLProgram createProgram();
        void useProgram(GLProgram);
        void linkProgram(GLProgram program);
        void deleteProgram(GLProgram program);

        void disable(GLenum);
        void enable(GLenum);

        void cullFace(GLenum);
        void frontFace(GLenum);
        void polygonOffset(GLfloat, GLfloat);
        void lineWidth(GLfloat);

        void depthMask(GLboolean);
        void depthFunc(GLenum);

        void stencilFuncSeparate(GLenum, GLenum, GLuint, GLuint);
        void stencilOpSeparate(GLenum, GLenum, GLenum, GLenum);
        void stencilMaskSeparate(GLenum, GLuint);

        void blendColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
        void blendFuncSeparate(GLenum srcRGB, GLenum destRGB, GLenum srcAlpha, GLenum destAlpha);
        void blendEquationSeparate(GLenum modeRGB, GLenum modeAlpha);

        GLTexture createTexture();
        void bindTexture(GLenum target, GLTexture texture);
        void deleteTexture(GLTexture texture);
        void texParameteri(GLenum target, GLenum, GLenum);
        void texImage2D(GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, void *);

        void texSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLenum format, GLenum type, int sourceAssetId);
        void texSubImage2D(GLenum target, GLint level, GLint xoffset, GLsizei yoffset, GLsizei width, GLint height, GLenum format, GLenum type, int sourceAssetId);
        void texSubImage2D(GLenum target, GLint level, GLint xoffset, GLsizei yoffset, GLsizei width, GLint height, GLenum format, GLenum type, const unsigned char *buffer, unsigned int size, bool copyBuffer);
        void activeTexture(GLint);

        void uniform1i(unsigned short, int);
        void uniform2i(unsigned short, int, int);
        void uniform3i(unsigned short, int, int, int);
        void uniform4i(unsigned short, int, int, int, int);
        void uniform1iv(unsigned short, int, char *, bool copyBuffer = false);

        void uniform1f(unsigned short, float);
        void uniform2f(unsigned short, float, float);
        void uniform3f(unsigned short, float, float, float);
        void uniform4f(unsigned short, float, float, float, float);
        void uniform4fv(unsigned short location, char *data);
        void uniformMatrix4f(unsigned short, char *);
        void uniformMatrix2fv(unsigned short, char *);
        void uniformMatrix3fv(unsigned short, char *);
        void uniformMatrix4fv(unsigned short, char *);

        void enableVertexAttribArray(unsigned short);
        void vertexAttribPointer(unsigned short, GLint, GLenum, GLsizei, GLsizei);
        void vertexAttribDivisor(unsigned short, GLint);
        void drawElements(GLenum primtive, GLsizei, GLenum, unsigned int);
        GLFramebuffer createFramebuffer();
        void bindFramebuffer(GLenum target, GLFramebuffer framebuffer);
        void deleteFramebuffer(GLFramebuffer framebuffer);

        GLRenderbuffer createRenderbuffer();
        void deleteRenderbuffer(GLRenderbuffer renderbuffer);
        void bindRenderbuffer(GLenum target, GLRenderbuffer renderbuffer);
        void renderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);

        void framebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLRenderbuffer renderbuffer);
        void framebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLTexture texture, GLint level);
        void drawBuffers(int num);
    };
}
#endif