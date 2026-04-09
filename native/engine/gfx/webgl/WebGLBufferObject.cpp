#include "WebGLBufferObject.h"
#include "WebGLCommand.h"
#include "../DeviceManager.h"
#include "../../core/base/config.h"
#include "../webglbase/BaseWebGLCommand.h"

WebGLBufferObject::WebGLBufferObject()
{
}

void WebGLBufferObject::initialize(BufferInfo &info)
{
   bufferData.memUsage = info.usage;
   bufferData.size = info.size;
   bufferData.type = info.type;
   bufferData.bufferView = info.bufferView;
   bufferData.sysToJsByCopy = info.webglSysToJsByCopy;
   //  if( bufferData.type == BufferType::VERTEX){
   //    printf(" WebGLBufferObject %p %d \n",bufferData.attrs,info.attrs->size());
   //    for(int i = 0; i < bufferData.attrs->size() ; ++i){
   //       printf("WebGLBufferObject %i %p %s\n",i, &(*bufferData.attrs)[i],(*bufferData.attrs)[i].name.c_str());
   //    }
   //  }©
}

void WebGLBufferObject::resize(int size)
{
   SQ_ASSERT(size);
   WebGLDevice *device = dynamic_cast<WebGLDevice *>(DeviceManager::getInstance()->device);
   if (bufferData.glBuffer == 0 && bufferData.buffer == 0)
   {
      // printf("WebGL2BufferObject create buffer %d \n",bufferData.type);
      WebGLCmdFuncCreateBuffer(device, &bufferData);
   }

   if (size != bufferData.size)
   {
      WebGLCmdFuncResizeBuffer(device, &bufferData, size);
   }
}
void WebGLBufferObject::update(char *data, int updateSize)
{

   SQ_ASSERT(updateSize);

   WebGLDevice *device = dynamic_cast<WebGLDevice *>(DeviceManager::getInstance()->device);

   // printf("WebGLBufferObject update buffer %d %p %p \n",bufferData.type,bufferData.glBuffer,bufferData.buffer);

   if (bufferData.glBuffer == 0 && bufferData.buffer == 0)
   {
      // printf("WebGLBufferObject create buffer %d \n",bufferData.type);
      WebGLCmdFuncCreateBuffer(device, &bufferData);
   }

   if (updateSize > bufferData.size)
   {
      WebGLCmdFuncResizeBuffer(device, &bufferData, updateSize);
   }

   int offset = 0;
   WebGLCmdFuncUpdateBuffer(
       device,
       &bufferData,
       data,
       offset,
       updateSize);
}

void WebGLBufferObject::updateFromJs(int nativeId, int updateSize)
{
   WebGLDevice *device = dynamic_cast<WebGLDevice *>(DeviceManager::getInstance()->device);

   // printf("WebGLBufferObject update buffer %d %p %p \n",bufferData.type,bufferData.glBuffer,bufferData.buffer);

   if (bufferData.glBuffer == 0 && bufferData.buffer == 0)
   {
      // printf("WebGLBufferObject create buffer %d \n",bufferData.type);
      WebGLCmdFuncCreateBuffer(device, &bufferData);
   }

   if (updateSize > bufferData.size)
   {
      WebGLCmdFuncResizeBuffer(device, &bufferData, updateSize);
   }

   BaseWebGLCmdUpdateBufferObject(device->getGLContext(), bufferData.glTarget, nativeId);
}

WebGLBufferObject::~WebGLBufferObject()
{
   if (bufferData.buffer)
   {
      delete bufferData.buffer;
      bufferData.buffer = NULL;
   }
}