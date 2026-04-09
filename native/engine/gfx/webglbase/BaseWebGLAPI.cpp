#include "BaseWebGLAPI.h"
#include "../../core/common/number_to_string.h"

namespace GLAPI
{

        BaseWebGLAPI::BaseWebGLAPI() : jsObject(bridge::NativeObjectType::gl)
        {
                jsObject.enableWriteRepeat(true);
        }

        GLBuffer BaseWebGLAPI::createBuffer()
        {
                GLBuffer id = idGen.get();
                jsObject.beginOp(toNumber(JSAPIMap::createBuffer));
                jsObject.writeOpArg<GLBuffer>(id);
                jsObject.endOp();
#ifdef DEBUG_WEBGL
                printf("createBuffer %d %d\n", toNumber(JSAPIMap::createBuffer), id);
#endif
                return id;
        }

        void BaseWebGLAPI::bindBuffer(GLenum target, GLBuffer buffer)
        {
                jsObject.beginOp(toNumber(JSAPIMap::bindBuffer));
                jsObject.writeOpArg<GLenum>(target);
                jsObject.writeOpArg<GLBuffer>(buffer);
                jsObject.endOp();
#ifdef DEBUG_WEBGL
                printf("bindBuffer %d %u %d\n", toNumber(JSAPIMap::bindBuffer), target, buffer);
#endif
        }

        void BaseWebGLAPI::bufferData(GLenum target, GLsizei size, GLenum usage)
        {
                jsObject.beginOp(toNumber(JSAPIMap::bufferData));
                jsObject.writeOpArg<GLenum>(target);
                jsObject.writeOpArg<GLsizei>(size);
                jsObject.writeOpArg<GLenum>(usage);
                jsObject.endOp();
#ifdef DEBUG_WEBGL
                printf("bufferData %d %u %d %u\n", toNumber(JSAPIMap::bufferData), target, size, usage);
#endif
        }

        void BaseWebGLAPI::bufferSubData(GLenum target, GLsizei offset, char *data, GLsizei updateSize)
        {
                jsObject.beginOp(toNumber(JSAPIMap::bufferSubData));
                jsObject.writeOpArg<GLenum>(target);
                jsObject.writeOpArg(offset);
                unsigned char *adress = reinterpret_cast<unsigned char *>(data);
                jsObject.writeOpArg(adress);
                jsObject.writeOpArg(updateSize);
                jsObject.endOp();

                //   if(target == 2){
                //     printf(">>>>>>>>>>>>>>>>>>>>>> %d %d %d %d %d %d \n",*((unsigned short*)data),*((unsigned short*)(data+2)),*((unsigned short*)(data+4)),
                //       *((unsigned short*)(data+6)),*((unsigned short*)(data+8)),*((unsigned short*)(data+10)));
                //   }

#ifdef DEBUG_WEBGL
                printf("bufferSubData %d %u %d %u %d\n", toNumber(JSAPIMap::bufferSubData), target, offset, adress, updateSize);
#endif
        }

        void BaseWebGLAPI::bufferDataFromJs(GLenum target, int bufferNativeId)
        {
                jsObject.beginOp(toNumber(JSAPIMap::bufferDataFromJs));
                jsObject.writeOpArg<GLenum>(target);
                jsObject.writeOpArg<int>(bufferNativeId);
                jsObject.endOp();
        }

        GLShader BaseWebGLAPI::createShader(GLenum glShaderType) { return 0; }

        void BaseWebGLAPI::shaderSource(GLShader shader, int source) {}

        void BaseWebGLAPI::compileShader(GLShader shader) {}

        GLProgram BaseWebGLAPI::createProgram()
        {
                GLProgram id = idGen.get();
                jsObject.beginOp(toNumber(JSAPIMap::createProgram));
                jsObject.writeOpArg<int>(id);
                jsObject.endOp();
                // printf("createProgram %d %d\n",toNumber(JSAPIMap::createProgram),id);
                return id;
        }

        void BaseWebGLAPI::attachShader(GLProgram program, GLShader shader)
        {
        }

        void BaseWebGLAPI::linkProgram(GLProgram program)
        {
        }

        void BaseWebGLAPI::detachShader(GLProgram glProgram, GLShader glShader)
        {
        }

        void BaseWebGLAPI::deleteShader(GLShader glShader)
        {
        }

        void BaseWebGLAPI::deleteProgram(GLProgram program)
        {
                jsObject.beginOp(toNumber(JSAPIMap::deleteProgram));
                jsObject.writeOpArg<GLProgram>(program);
                jsObject.endOp();
#ifdef DEBUG_WEBGL
                printf("deleteProgram %d %d\n", toNumber(JSAPIMap::deleteProgram), program);
#endif
        }

        void BaseWebGLAPI::useProgram(GLProgram program)
        {
                jsObject.beginOp(toNumber(JSAPIMap::useProgram));
                jsObject.writeOpArg<GLProgram>(program);
                jsObject.endOp();
#ifdef DEBUG_WEBGL
                printf("useProgram %d %d\n", toNumber(JSAPIMap::useProgram), program);
#endif
        }

        void BaseWebGLAPI::disable(GLenum v)
        {
                jsObject.beginOp(toNumber(JSAPIMap::disable));
                jsObject.writeOpArg<GLenum>(v);
                jsObject.endOp();
#ifdef DEBUG_WEBGL
                printf("disable %d %d\n", toNumber(JSAPIMap::disable), v);
#endif
        }

        void BaseWebGLAPI::enable(GLenum v)
        {
                jsObject.beginOp(toNumber(JSAPIMap::enable));
                jsObject.writeOpArg<GLenum>(v);
                jsObject.endOp();
#ifdef DEBUG_WEBGL
                printf("enable %d %d\n", toNumber(JSAPIMap::disable), v);
#endif
        }

        void BaseWebGLAPI::cullFace(GLenum v)
        {
                jsObject.beginOp(toNumber(JSAPIMap::cullFace));
                jsObject.writeOpArg<GLenum>(v);
                jsObject.endOp();
#ifdef DEBUG_WEBGL
                printf("cullFace %d %d\n", toNumber(JSAPIMap::cullFace), v);
#endif
        }

        void BaseWebGLAPI::frontFace(GLenum v)
        {
                jsObject.beginOp(toNumber(JSAPIMap::frontFace));
                jsObject.writeOpArg<GLenum>(v);
                jsObject.endOp();
#ifdef DEBUG_WEBGL
                printf("frontFace %d %d\n", toNumber(JSAPIMap::frontFace), v);
#endif
        }

        void BaseWebGLAPI::polygonOffset(GLfloat depthBias, GLfloat depthBiasSlop)
        {
                jsObject.beginOp(toNumber(JSAPIMap::polygonOffset));
                jsObject.writeOpArg<GLfloat>(depthBias);
                jsObject.writeOpArg<GLfloat>(depthBiasSlop);
                jsObject.endOp();
#ifdef DEBUG_WEBGL
                printf("polygonOffset %d %f %f\n", toNumber(JSAPIMap::polygonOffset), depthBias, depthBiasSlop);
#endif
        }

        void BaseWebGLAPI::lineWidth(GLfloat width)
        {
                jsObject.beginOp(toNumber(JSAPIMap::lineWidth));
                jsObject.writeOpArg<GLfloat>(width);
                jsObject.endOp();
#ifdef DEBUG_WEBGL
                printf("lineWidth %d %f\n", toNumber(JSAPIMap::polygonOffset), width);
#endif
        }

        void BaseWebGLAPI::depthMask(GLboolean mask)
        {
                jsObject.beginOp(toNumber(JSAPIMap::depthMask));
                jsObject.writeOpArg<GLboolean>(mask);
                jsObject.endOp();
#ifdef DEBUG_WEBGL
                printf("depthMask %d %d\n", toNumber(JSAPIMap::depthMask), mask);
#endif
        }

        void BaseWebGLAPI::depthFunc(GLenum d)
        {
                jsObject.beginOp(toNumber(JSAPIMap::depthFunc));
                jsObject.writeOpArg<GLenum>(d);
                jsObject.endOp();
#ifdef DEBUG_WEBGL
                printf("depthFunc %d %u\n", toNumber(JSAPIMap::depthFunc), d);
#endif
        }

        void BaseWebGLAPI::stencilFuncSeparate(GLenum frontType, GLenum func, GLuint ref, GLuint mask)
        {
                jsObject.beginOp(toNumber(JSAPIMap::stencilFuncSeparate));
                jsObject.writeOpArg(frontType);
                jsObject.writeOpArg(func);
                jsObject.writeOpArg(ref);
                jsObject.writeOpArg(mask);
                jsObject.endOp();
#ifdef DEBUG_WEBGL
                printf("stencilFuncSeparate %d %u %u %d %u\n", toNumber(JSAPIMap::stencilFuncSeparate), frontType, func, ref, mask);
#endif
        }

        void BaseWebGLAPI::stencilOpSeparate(GLenum frontType, GLenum sfail, GLenum dpfail, GLenum dppass)
        {
                jsObject.beginOp(toNumber(JSAPIMap::stencilOpSeparate));
                jsObject.writeOpArg(frontType);
                jsObject.writeOpArg(sfail);
                jsObject.writeOpArg(dpfail);
                jsObject.writeOpArg(dppass);
                jsObject.endOp();
#ifdef DEBUG_WEBGL
                printf("stencilOpSeparate %d %u %u %u %u\n", toNumber(JSAPIMap::stencilOpSeparate), frontType, sfail, dpfail, dppass);
#endif
        }

        void BaseWebGLAPI::stencilMaskSeparate(GLenum frontType, GLuint mask)
        {
                jsObject.beginOp(toNumber(JSAPIMap::stencilMaskSeparate));
                jsObject.writeOpArg(frontType);
                jsObject.writeOpArg(mask);
                jsObject.endOp();
#ifdef DEBUG_WEBGL
                printf("stencilMaskSeparate %d %u %u\n", toNumber(JSAPIMap::stencilOpSeparate), frontType, mask);
#endif
        }

        void BaseWebGLAPI::blendColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
        {
                jsObject.beginOp(toNumber(JSAPIMap::blendColor));
                jsObject.writeOpArg<GLfloat>(r);
                jsObject.writeOpArg<GLfloat>(g);
                jsObject.writeOpArg<GLfloat>(b);
                jsObject.writeOpArg<GLfloat>(a);
                jsObject.endOp();
#ifdef DEBUG_WEBGL
                printf("blendColor %d %f %f %f %f\n", toNumber(JSAPIMap::blendColor), r, g, b, a);
#endif
        }

        void BaseWebGLAPI::blendFuncSeparate(GLenum srcRGB, GLenum destRGB, GLenum srcAlpha, GLenum destAlpha)
        {
                jsObject.beginOp(toNumber(JSAPIMap::blendFuncSeparate));
                jsObject.writeOpArg<GLenum>(srcRGB);
                jsObject.writeOpArg<GLenum>(destRGB);
                jsObject.writeOpArg<GLenum>(srcAlpha);
                jsObject.writeOpArg<GLenum>(destAlpha);
                jsObject.endOp();
#ifdef DEBUG_WEBGL
                printf("blendFuncSeparate %d %u %u %u %u\n", toNumber(JSAPIMap::blendFuncSeparate), srcRGB, destRGB, srcAlpha, destAlpha);
#endif
        }

        void BaseWebGLAPI::blendEquationSeparate(GLenum modeRGB, GLenum modeAlpha)
        {
                jsObject.beginOp(toNumber(JSAPIMap::blendEquationSeparate));
                jsObject.writeOpArg<GLenum>(modeRGB);
                jsObject.writeOpArg<GLenum>(modeAlpha);
                jsObject.endOp();

#ifdef DEBUG_WEBGL
                printf("blendEquationSeparate %d %u %u\n", toNumber(JSAPIMap::blendEquationSeparate), modeRGB, modeAlpha);
#endif
        }

        GLTexture BaseWebGLAPI::createTexture()
        {
                GLTexture id = idGen.get();
                jsObject.beginOp(toNumber(JSAPIMap::createTexture));
                jsObject.writeOpArg<GLTexture>(id);
                jsObject.endOp();
#ifdef DEBUG_WEBGL
                printf("createTexture %d %d\n", toNumber(JSAPIMap::createTexture), id);
#endif
                return id;
        }

        void BaseWebGLAPI::bindTexture(GLenum target, GLTexture texture)
        {
                jsObject.beginOp(toNumber(JSAPIMap::bindTexture));
                jsObject.writeOpArg<GLenum>(target);
                jsObject.writeOpArg<GLTexture>(texture);
                jsObject.endOp();
#ifdef DEBUG_WEBGL
                printf("bindTexture %d %u %d\n", toNumber(JSAPIMap::createTexture), target, texture);
#endif
        }

        void BaseWebGLAPI::deleteTexture(GLTexture texture)
        {
                idGen.recvoery(texture);
                jsObject.beginOp(toNumber(JSAPIMap::deleteTexture));
                jsObject.writeOpArg<GLTexture>(texture);
                jsObject.endOp();

#ifdef DEBUG_WEBGL
                printf("deleteTexture %d %d\n", toNumber(JSAPIMap::deleteTexture), texture);
#endif
        }

        void BaseWebGLAPI::texParameteri(GLenum target, GLenum type1, GLenum type2)
        {
                jsObject.beginOp(toNumber(JSAPIMap::texParameteri));
                jsObject.writeOpArg<GLenum>(target);
                jsObject.writeOpArg<GLenum>(type1);
                jsObject.writeOpArg<GLenum>(type2);
                jsObject.endOp();

#ifdef DEBUG_WEBGL
                printf("texParameteri %d %u %u %u\n", toNumber(JSAPIMap::deleteTexture), target, type1, type2);
#endif
        }

        void BaseWebGLAPI::texImage2D(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, void *)
        {
                jsObject.beginOp(toNumber(JSAPIMap::texImage2D));
                jsObject.writeOpArg(target);
                jsObject.writeOpArg(level);
                jsObject.writeOpArg(internalFormat);
                jsObject.writeOpArg(width);
                jsObject.writeOpArg(height);
                jsObject.writeOpArg(border);
                jsObject.writeOpArg(format);
                jsObject.writeOpArg(type);
                jsObject.endOp();
#ifdef DEBUG_WEBGL
                printf("texImage2D %d %u %d %d %d %d %d %u %u\n", toNumber(JSAPIMap::deleteTexture), target, level, internalFormat, width, height, border, format, type);
#endif
        }

        void BaseWebGLAPI::activeTexture(GLint index)
        {
                jsObject.beginOp(toNumber(JSAPIMap::activeTexture));
                jsObject.writeOpArg<GLint>(index);
                jsObject.endOp();

#ifdef DEBUG_WEBGL
                printf("activeTexture %d %d\n", toNumber(JSAPIMap::activeTexture), index);
#endif
        }

        void BaseWebGLAPI::texSubImage2D(GLenum target, GLint level, GLint xoffset, GLsizei yoffset,
                                         GLsizei width, GLint height, GLenum format, GLenum type, int sourceAssetId)
        {

                jsObject.beginOp(toNumber(JSAPIMap::texSubImage2D));
                jsObject.writeOpArg(target);
                jsObject.writeOpArg(level);
                jsObject.writeOpArg(xoffset);
                jsObject.writeOpArg(yoffset);
                jsObject.writeOpArg(width);
                jsObject.writeOpArg(height);
                jsObject.writeOpArg(format);
                jsObject.writeOpArg(type);
                jsObject.writeOpArg(sourceAssetId);
                jsObject.endOp();

#ifdef DEBUG_WEBGL
                printf("texSubImage2D %d %u %d %d %d %d %d %u %u %d\n", toNumber(JSAPIMap::texSubImage2D), target, level,
                       xoffset, yoffset, width, height, format, type, sourceAssetId);
#endif
        }

        void BaseWebGLAPI::texSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLenum format, GLenum type, int sourceAssetId)
        {
                jsObject.beginOp(toNumber(JSAPIMap::texSubImage2D_2));
                jsObject.writeOpArg(target);
                jsObject.writeOpArg(level);
                jsObject.writeOpArg(xoffset);
                jsObject.writeOpArg(yoffset);
                jsObject.writeOpArg(format);
                jsObject.writeOpArg(type);
                jsObject.writeOpArg(sourceAssetId);
                jsObject.endOp();
#ifdef DEBUG_WEBGL
                printf("texSubImage2D_2 %d %u %d %d %d %u %u %d\n", toNumber(JSAPIMap::texSubImage2D), target, level,
                       xoffset, yoffset, format, type, sourceAssetId);
#endif
        }

        void BaseWebGLAPI::texSubImage2D(GLenum target, GLint level, GLint xoffset, GLsizei yoffset, GLsizei width, GLint height, GLenum format, GLenum type,
                                         const unsigned char *buffer, unsigned int bufferSize, bool copyBuffer)
        {
                jsObject.beginOp(toNumber(JSAPIMap::texSubImage2D_buffer));
                jsObject.writeOpArg(target);
                jsObject.writeOpArg(level);
                jsObject.writeOpArg(xoffset);
                jsObject.writeOpArg(yoffset);
                jsObject.writeOpArg(width);
                jsObject.writeOpArg(height);
                jsObject.writeOpArg(format);
                jsObject.writeOpArg(type);
                jsObject.writeOpArg(bufferSize);
                if (copyBuffer)
                {
                        jsObject.writeOpArg<char>(1);
                        buffer = (unsigned char *)jsObject.writeBuffer(buffer, bufferSize);
                }
                else
                {
                        jsObject.writeOpArg<char>(0);
                        const unsigned char *address = reinterpret_cast<const unsigned char *>(buffer);
                        jsObject.writeOpArg(address);
                }
                jsObject.endOp();
        }

        void BaseWebGLAPI::uniform1i(unsigned short location, int v)
        {
                jsObject.beginOp(toNumber(JSAPIMap::uniform1i));
                jsObject.writeOpArg(location);
                jsObject.writeOpArg(v);
                jsObject.endOp();
#ifdef DEBUG_WEBGL
                printf("uniform1i %d %u %d\n", toNumber(JSAPIMap::uniform1i), location, v);
#endif
        }

        void BaseWebGLAPI::uniform2i(unsigned short location, int v1, int v2)
        {
                jsObject.beginOp(toNumber(JSAPIMap::uniform2i));
                jsObject.writeOpArg(location);
                jsObject.writeOpArg(v1);
                jsObject.writeOpArg(v2);
                jsObject.endOp();

#ifdef DEBUG_WEBGL
                printf("uniform2i %d %d %d %d\n", toNumber(JSAPIMap::uniform2i), location, v1, v2);
#endif
        }

        void BaseWebGLAPI::uniform3i(unsigned short location, int v1, int v2, int v3)
        {
                jsObject.beginOp(toNumber(JSAPIMap::uniform3i));
                jsObject.writeOpArg(location);
                jsObject.writeOpArg(v1);
                jsObject.writeOpArg(v2);
                jsObject.writeOpArg(v3);
                jsObject.endOp();

#ifdef DEBUG_WEBGL
                printf("uniform3i %d %d %d %d %d\n", toNumber(JSAPIMap::uniform3i), location, v1, v2, v3);
#endif
        }

        void BaseWebGLAPI::uniform4i(unsigned short location, int v1, int v2, int v3, int v4)
        {
                jsObject.beginOp(toNumber(JSAPIMap::uniform4i));
                jsObject.writeOpArg(location);
                jsObject.writeOpArg(v1);
                jsObject.writeOpArg(v2);
                jsObject.writeOpArg(v3);
                jsObject.writeOpArg(v4);
                jsObject.endOp();

#ifdef DEBUG_WEBGL
                printf("uniform4i %d %d %d %d %d %d\n", toNumber(JSAPIMap::uniform4i), location, v1, v2, v3, v4);
#endif
        }

        void BaseWebGLAPI::uniform1iv(unsigned short location, int size, char *data, bool copyBuffer)
        {
                jsObject.beginOp(toNumber(JSAPIMap::uniform1iv));
                jsObject.writeOpArg(location);
                jsObject.writeOpArg(size);
                if (copyBuffer)
                {
                        jsObject.writeOpArg<char>(1);
                        jsObject.writeBuffer(data, size * 4);
                }
                else
                {
                        jsObject.writeOpArg<char>(0);
                        unsigned char *adress = reinterpret_cast<unsigned char *>(data);
                        jsObject.writeOpArg(adress);
                }

                jsObject.endOp();
        }

        void BaseWebGLAPI::uniform1f(unsigned short location, float v1)
        {
                jsObject.beginOp(toNumber(JSAPIMap::uniform1f));
                jsObject.writeOpArg(location);
                jsObject.writeOpArg(v1);
                jsObject.endOp();

#ifdef DEBUG_WEBGL
                printf("uniform1f %d %d %f\n", toNumber(JSAPIMap::uniform1f), location, v1);
#endif
        }

        void BaseWebGLAPI::uniform2f(unsigned short location, float v1, float v2)
        {
                jsObject.beginOp(toNumber(JSAPIMap::uniform2f));
                jsObject.writeOpArg(location);
                jsObject.writeOpArg(v1);
                jsObject.writeOpArg(v2);
                jsObject.endOp();
#ifdef DEBUG_WEBGL
                printf("uniform2f %d %d %f %f\n", toNumber(JSAPIMap::uniform2f), location, v1, v2);
#endif
        }

        void BaseWebGLAPI::uniform3f(unsigned short location, float v1, float v2, float v3)
        {
                jsObject.beginOp(toNumber(JSAPIMap::uniform3f));
                jsObject.writeOpArg(location);
                jsObject.writeOpArg(v1);
                jsObject.writeOpArg(v2);
                jsObject.writeOpArg(v3);
                jsObject.endOp();

#ifdef DEBUG_WEBGL
                printf("uniform3f %d %d %f %f %f\n", toNumber(JSAPIMap::uniform3f), location, v1, v2, v3);
#endif
        }

        void BaseWebGLAPI::uniform4f(unsigned short location, float v1, float v2, float v3, float v4)
        {
                jsObject.beginOp(toNumber(JSAPIMap::uniform4f));
                jsObject.writeOpArg(location);
                jsObject.writeOpArg(v1);
                jsObject.writeOpArg(v2);
                jsObject.writeOpArg(v3);
                jsObject.writeOpArg(v4);
                jsObject.endOp();

#ifdef DEBUG_WEBGL
                printf("uniform4f %d %d %f %f %f %f\n", toNumber(JSAPIMap::uniform4f), location, v1, v2, v3, v4);
#endif
        }

        void BaseWebGLAPI::uniform4fv(unsigned short location, char *data)
        {
                jsObject.beginOp(toNumber(JSAPIMap::uniform4fv));
                jsObject.writeOpArg(location);
                unsigned char *adress = reinterpret_cast<unsigned char *>(data);
                jsObject.writeOpArg(adress);
                jsObject.endOp();

#ifdef DEBUG_WEBGL
                printf("uniformMatrix2fv %d %d %f %f %f %f\n", toNumber(JSAPIMap::uniformMatrix2fv), location, (float)data[0], (float)data[4], (float)data[8], (float)data[12]);
#endif
        }

        void BaseWebGLAPI::uniformMatrix2fv(unsigned short location, char *data)
        {
                jsObject.beginOp(toNumber(JSAPIMap::uniformMatrix2fv));
                jsObject.writeOpArg(location);
                unsigned char *adress = reinterpret_cast<unsigned char *>(data);
                jsObject.writeOpArg(adress);
                jsObject.endOp();

#ifdef DEBUG_WEBGL
                printf("uniformMatrix2fv %d %d %f %f %f %f\n", toNumber(JSAPIMap::uniformMatrix2fv), location, (float)data[0], (float)data[4], (float)data[8], (float)data[12]);
#endif
        }

        void BaseWebGLAPI::uniformMatrix3fv(unsigned short location, char *data)
        {
                jsObject.beginOp(toNumber(JSAPIMap::uniformMatrix3fv));
                jsObject.writeOpArg(location);
                unsigned char *adress = reinterpret_cast<unsigned char *>(data);
                jsObject.writeOpArg(adress);
                jsObject.endOp();

#ifdef DEBUG_WEBGL
                printf("uniformMatrix3fv %d %p\n", location, adress);
#endif
        }

        void BaseWebGLAPI::uniformMatrix4f(unsigned short location, char *data)
        {
                jsObject.beginOp(toNumber(JSAPIMap::uniformMatrix4f));
                jsObject.writeOpArg(location);
                jsObject.writeBuffer(data, 64);
                jsObject.endOp();

#ifdef DEBUG_WEBGL
                float *dataf = (float *)data;
                printf("uniformMatrix4f %d %p %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f \n", location, data,
                       dataf[0], dataf[1], dataf[2], dataf[3],
                       dataf[4], dataf[5], dataf[6], dataf[7],
                       dataf[8], dataf[9], dataf[10], dataf[11],
                       dataf[12], dataf[13], dataf[14], dataf[15]);
#endif
        }

        void BaseWebGLAPI::uniformMatrix4fv(unsigned short location, char *data)
        {
                jsObject.beginOp(toNumber(JSAPIMap::uniformMatrix4fv));
                jsObject.writeOpArg(location);
                unsigned char *adress = reinterpret_cast<unsigned char *>(data);
                jsObject.writeOpArg(adress);
                jsObject.endOp();

#ifdef DEBUG_WEBGL
                printf("uniformMatrix4fv %d %p %p\n", location, adress, data);
#endif
        }

        void BaseWebGLAPI::enableVertexAttribArray(unsigned short location)
        {
                jsObject.beginOp(toNumber(JSAPIMap::enableVertexAttribArray));
                jsObject.writeOpArg(location);
                jsObject.endOp();
#ifdef DEBUG_WEBGL
                printf("enableVertexAttribArray %d %d\n", toNumber(JSAPIMap::enableVertexAttribArray), location);
#endif
        }

        void BaseWebGLAPI::vertexAttribPointer(unsigned short location, GLint count, GLenum type, GLsizei stride, GLsizei offset)
        {
                jsObject.beginOp(toNumber(JSAPIMap::vertexAttribPointer));
                jsObject.writeOpArg(location);
                jsObject.writeOpArg(count);
                jsObject.writeOpArg(type);
                jsObject.writeOpArg(stride);
                jsObject.writeOpArg(offset);
                jsObject.endOp();
#ifdef DEBUG_WEBGL
                printf("vertexAttribPointer %d %d %d %u %d %d\n", toNumber(JSAPIMap::vertexAttribPointer), location, count, type, stride, offset);
#endif
        }

        void BaseWebGLAPI::vertexAttribDivisor(unsigned short location, GLint num)
        {
                jsObject.beginOp(toNumber(JSAPIMap::vertexAttribDivisor));
                jsObject.writeOpArg(location);
                jsObject.writeOpArg(num);
                jsObject.endOp();
#ifdef DEBUG_WEBGL
                printf("vertexAttribDivisor %d %d %d\n", toNumber(JSAPIMap::vertexAttribPointer), location, num);
#endif
        }

        void BaseWebGLAPI::drawElements(GLenum primtive, GLsizei indexCount, GLenum type, unsigned int offset)
        {
                jsObject.beginOp(toNumber(JSAPIMap::drawElements));
                jsObject.writeOpArg(primtive);
                jsObject.writeOpArg(indexCount);
                jsObject.writeOpArg(type);
                jsObject.writeOpArg(offset);
                jsObject.endOp();
#ifdef DEBUG_WEBGL
                printf("drawElements %d %u %d %u %u\n", toNumber(JSAPIMap::drawElements), primtive, indexCount, type, offset);
#endif
        }

        void BaseWebGLAPI::viewport(GLint x, GLint y, GLsizei width, GLsizei height)
        {
                jsObject.beginOp(toNumber(JSAPIMap::viewport)); // 2
                jsObject.writeOpArg(x);                         // 4
                jsObject.writeOpArg(y);                         // 4
                jsObject.writeOpArg(width);                     // 4
                jsObject.writeOpArg(height);                    // 4
                jsObject.endOp();
#ifdef DEBUG_WEBGL
                printf("viewport %d %d %d %d %d\n", toNumber(JSAPIMap::viewport), x, y, width, height);
#endif
        }

        void BaseWebGLAPI::scissor(GLint x, GLint y, GLsizei width, GLsizei height)
        {
                jsObject.beginOp(toNumber(JSAPIMap::scissor)); // 2
                jsObject.writeOpArg(x);                        // 4
                jsObject.writeOpArg(y);                        // 4
                jsObject.writeOpArg(width);                    // 4
                jsObject.writeOpArg(height);                   // 4
                jsObject.endOp();

#ifdef DEBUG_WEBGL
                printf("scissor %d %d %d %d %d\n", toNumber(JSAPIMap::scissor), x, y, width, height);
#endif
        }

        void BaseWebGLAPI::clear(GLbitfield b)
        {
                jsObject.beginOp(toNumber(JSAPIMap::clear)); // 2
                jsObject.writeOpArg(b);                      // 4
                jsObject.endOp();
#ifdef DEBUG_WEBGL
                printf("clear %d %d\n", toNumber(JSAPIMap::clear), b);
#endif
        }

        void BaseWebGLAPI::clearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
        {
                jsObject.beginOp(toNumber(JSAPIMap::clearColor));
                jsObject.writeOpArg(red);
                jsObject.writeOpArg(green);
                jsObject.writeOpArg(blue);
                jsObject.writeOpArg(alpha);
                jsObject.endOp();
#ifdef DEBUG_WEBGL
                printf("clearColor %d %f %f %f %f \n", toNumber(JSAPIMap::clearColor), red, green, blue, alpha);
#endif
        }

        void BaseWebGLAPI::clearDepth(GLclampf d)
        {
                jsObject.beginOp(toNumber(JSAPIMap::clearDepth));
                jsObject.writeOpArg(d);
                jsObject.endOp();
#ifdef DEBUG_WEBGL
                printf("clearDepth %d \n", toNumber(JSAPIMap::clearDepth));
#endif
        }

        void BaseWebGLAPI::clearStencil(GLint s)
        {
                jsObject.beginOp(toNumber(JSAPIMap::clearStencil));
                jsObject.writeOpArg(s);
                jsObject.endOp();
#ifdef DEBUG_WEBGL
                printf("clearStencil %d \n", toNumber(JSAPIMap::clearStencil));
#endif
        }

        void BaseWebGLAPI::colorMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)
        {
                jsObject.beginOp(toNumber(JSAPIMap::colorMask));
                jsObject.writeOpArg<bool>(red);
                jsObject.writeOpArg<bool>(green);
                jsObject.writeOpArg<bool>(blue);
                jsObject.writeOpArg<bool>(alpha);
                jsObject.endOp();

#ifdef DEBUG_WEBGL
                printf("colorMask %d \n", toNumber(JSAPIMap::colorMask));
#endif
        }

        GLFramebuffer BaseWebGLAPI::createFramebuffer()
        {
                GLFramebuffer id = idGen.get();
                jsObject.beginOp(toNumber(JSAPIMap::createFramebuffer));
                jsObject.writeOpArg(id);
                jsObject.endOp();
                return id;
        }

        void BaseWebGLAPI::bindFramebuffer(GLenum target, GLFramebuffer framebuffer)
        {
                jsObject.beginOp(toNumber(JSAPIMap::bindFramebuffer));
                jsObject.writeOpArg(target);
                jsObject.writeOpArg(framebuffer);
                jsObject.endOp();
#ifdef DEBUG_WEBGL
                printf("bindFramebuffer %d framebuffer %d \n", toNumber(JSAPIMap::bindFramebuffer), framebuffer);
#endif
        }

        void BaseWebGLAPI::deleteFramebuffer(GLFramebuffer framebuffer)
        {
                idGen.recvoery(framebuffer);
                jsObject.beginOp(toNumber(JSAPIMap::deleteFramebuffer));
                jsObject.writeOpArg(framebuffer);
                jsObject.endOp();
        }

        GLRenderbuffer BaseWebGLAPI::createRenderbuffer()
        {
                GLRenderbuffer id = idGen.get();
                jsObject.beginOp(toNumber(JSAPIMap::createRenderbuffer));
                jsObject.writeOpArg(id);
                jsObject.endOp();
                return id;
        }

        void BaseWebGLAPI::deleteRenderbuffer(GLRenderbuffer renderbuffer)
        {
                idGen.recvoery(renderbuffer);
                jsObject.beginOp(toNumber(JSAPIMap::deleteRenderbuffer));
                jsObject.writeOpArg(renderbuffer);
                jsObject.endOp();
        }

        void BaseWebGLAPI::bindRenderbuffer(GLenum target, GLRenderbuffer renderbuffer)
        {
                jsObject.beginOp(toNumber(JSAPIMap::bindRenderbuffer));
                jsObject.writeOpArg(target);
                jsObject.writeOpArg(renderbuffer);
                jsObject.endOp();
        }

        void BaseWebGLAPI::renderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height)
        {
                jsObject.beginOp(toNumber(JSAPIMap::renderbufferStorage));
                jsObject.writeOpArg(target);
                jsObject.writeOpArg(internalformat);
                jsObject.writeOpArg(width);
                jsObject.writeOpArg(height);
                jsObject.endOp();
        }

        void BaseWebGLAPI::framebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLRenderbuffer renderbuffer)
        {
                jsObject.beginOp(toNumber(JSAPIMap::framebufferRenderbuffer));
                jsObject.writeOpArg(target);
                jsObject.writeOpArg(attachment);
                jsObject.writeOpArg(renderbuffertarget);
                jsObject.writeOpArg(renderbuffer);
                jsObject.endOp();
        }

        void BaseWebGLAPI::framebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLTexture texture, GLint level)
        {
                jsObject.beginOp(toNumber(JSAPIMap::framebufferTexture2D));
                jsObject.writeOpArg(target);
                jsObject.writeOpArg(attachment);
                jsObject.writeOpArg(textarget);
                jsObject.writeOpArg(texture);
                jsObject.writeOpArg(level);
                jsObject.endOp();
        }

        void BaseWebGLAPI::drawBuffers(int num)
        {
                jsObject.beginOp(toNumber(JSAPIMap::drawBuffers));
                jsObject.writeOpArg(num);
                jsObject.endOp();
        }

}