#include "WebGL.h"

namespace GLAPI
{
      GLVAO WebGL::createVertexArrayOES()
      {
            int vao = idGen.get();
            jsObject.beginOp(toNumber(JSAPIMap::createVertexArrayOES));
            jsObject.writeOpArg(vao);
            jsObject.endOp();
            return vao;
      }

      void WebGL::bindVertexArrayOES(GLVAO vao)
      {
            jsObject.beginOp(toNumber(JSAPIMap::bindVertexArrayOES));
            jsObject.writeOpArg(vao);
            jsObject.endOp();
      }

      void WebGL::deleteVertexArrayOES(GLVAO vao)
      {
            idGen.recvoery(vao);
            jsObject.beginOp(toNumber(JSAPIMap::deleteVertexArrayOES));
            jsObject.writeOpArg(vao);
            jsObject.endOp();
      }

      void WebGL::drawElementsInstancedANGLE(GLenum primtive, GLsizei indexCount, GLenum type, unsigned int offset, int instanceCount)
      {
            jsObject.beginOp(toNumber(JSAPIMap::drawElementsInstancedANGLE));
            jsObject.writeOpArg(primtive);
            jsObject.writeOpArg(indexCount);
            jsObject.writeOpArg(type);
            jsObject.writeOpArg(offset);
            jsObject.writeOpArg(instanceCount);
            jsObject.endOp();
#ifdef DEBUG_WEBGL
            printf("drawElementsInstancedANGLE %d %u %d %u %u\n", toNumber(JSAPIMap::drawElements), primtive, indexCount, type, offset);
#endif
      }
}