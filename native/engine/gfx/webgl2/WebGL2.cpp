#include "WebGL2.h"

namespace GLAPI
{
      GLVAO WebGL2::createVertexArray()
      {
            int vao = idGen.get();
            jsObject.beginOp(toNumber(JSAPIMap::createVertexArray));
            jsObject.writeOpArg(vao);
            jsObject.endOp();
            return vao;
      }

      void WebGL2::bindVertexArray(GLVAO vao)
      {
            jsObject.beginOp(toNumber(JSAPIMap::bindVertexArray));
            jsObject.writeOpArg(vao);
            jsObject.endOp();
#ifdef DEBUG_WEBGL
            printf("bind VAO %d \n", vao);
#endif
      }

      void WebGL2::deleteVertexArray(GLVAO vao)
      {
            idGen.recvoery(vao);
            jsObject.beginOp(toNumber(JSAPIMap::deleteVertexArray));
            jsObject.writeOpArg(vao);
            jsObject.endOp();
      }

      void WebGL2::drawElementsInstanced(GLenum primtive, GLsizei indexCount, GLenum type, unsigned int offset, int instanceCount)
      {
            jsObject.beginOp(toNumber(JSAPIMap::drawElementsInstanced));
            jsObject.writeOpArg(primtive);
            jsObject.writeOpArg(indexCount);
            jsObject.writeOpArg(type);
            jsObject.writeOpArg(offset);
            jsObject.writeOpArg(instanceCount);
            jsObject.endOp();
#ifdef DEBUG_WEBGL
            printf("drawElementsInstanced %d %u %d %u %u\n", toNumber(JSAPIMap::drawElements), primtive, indexCount, type, offset);
#endif
      }

      GLSampler WebGL2::createSampler()
      {
            int sampler = idGen.get();
            jsObject.beginOp(toNumber(JSAPIMap::createSampler));
            jsObject.writeOpArg(sampler);
            jsObject.endOp();
            return sampler;
      }

      void WebGL2::samplerParameteri(GLSampler sampler, GLenum target, GLenum value)
      {
            jsObject.beginOp(toNumber(JSAPIMap::samplerParameteri));
            jsObject.writeOpArg(sampler);
            jsObject.writeOpArg(target);
            jsObject.writeOpArg(value);
            jsObject.endOp();
      }

      void WebGL2::bindSampler(int texUnit, GLSampler sampler)
      {
            jsObject.beginOp(toNumber(JSAPIMap::bindSampler));
            jsObject.writeOpArg(sampler);
            jsObject.writeOpArg(texUnit);
            jsObject.endOp();
      }

      void WebGL2::deleteSampler(GLSampler sampler)
      {
            jsObject.beginOp(toNumber(JSAPIMap::deleteSampler));
            jsObject.writeOpArg(sampler);
            jsObject.endOp();
      }

}