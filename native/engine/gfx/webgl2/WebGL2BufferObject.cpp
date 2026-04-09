#include "WebGL2BufferObject.h"
#include "WebGL2Command.h"
#include "../DeviceManager.h"
#include "../../core/base/config.h"
#include "../webglbase/BaseWebGLCommand.h"

WebGL2BufferObject::WebGL2BufferObject()
{
}

void WebGL2BufferObject::initialize(BufferInfo &info)
{
   bufferData.memUsage = info.usage;
   bufferData.size = info.size;
   bufferData.type = info.type;
   bufferData.bufferView = info.bufferView;
   bufferData.sysToJsByCopy = info.webglSysToJsByCopy;
   //  if( bufferData.type == BufferType::VERTEX){
   //    printf(" WebGL2BufferObject %p %d \n",bufferData.attrs,info.attrs->size());
   //    for(int i = 0; i < bufferData.attrs->size() ; ++i){
   //       printf("WebGL2BufferObject %i %p %s\n",i, &(*bufferData.attrs)[i],(*bufferData.attrs)[i].name.c_str());
   //    }
   //  }
}

void WebGL2BufferObject::resize(int size)
{
   SQ_ASSERT(size);
   WebGL2Device *device = dynamic_cast<WebGL2Device *>(DeviceManager::getInstance()->device);
   if (bufferData.glBuffer == 0 && bufferData.buffer == 0)
   {
      // printf("WebGL2BufferObject create buffer %d \n",bufferData.type);
      WebGL2CmdFuncCreateBuffer(device, &bufferData);
   }

   if (size != bufferData.size)
   {
      WebGL2CmdFuncResizeBuffer(device, &bufferData, size);
   }
}

void WebGL2BufferObject::update(char *data, int updateSize)
{

   SQ_ASSERT(updateSize);

   WebGL2Device *device = dynamic_cast<WebGL2Device *>(DeviceManager::getInstance()->device);

   // printf("WebGL2BufferObject update buffer %d %p %p \n",bufferData.type,bufferData.glBuffer,bufferData.buffer);

   if (bufferData.glBuffer == 0 && bufferData.buffer == 0)
   {
      // printf("WebGL2BufferObject create buffer %d \n",bufferData.type);
      WebGL2CmdFuncCreateBuffer(device, &bufferData);
   }

   if (updateSize > bufferData.size)
   {
      WebGL2CmdFuncResizeBuffer(device, &bufferData, updateSize);
   }

   if (data == nullptr)
      return;

   int offset = 0;
   WebGL2CmdFuncUpdateBuffer(
       device,
       &bufferData,
       data,
       offset,
       updateSize);
}

void WebGL2BufferObject::updateFromJs(int nativeId, int updateSize)
{
   WebGL2Device *device = dynamic_cast<WebGL2Device *>(DeviceManager::getInstance()->device);

   // printf("WebGLBufferObject update buffer %d %p %p \n",bufferData.type,bufferData.glBuffer,bufferData.buffer);

   if (bufferData.glBuffer == 0 && bufferData.buffer == 0)
   {
      // printf("WebGLBufferObject create buffer %d \n",bufferData.type);
      WebGL2CmdFuncCreateBuffer(device, &bufferData);
   }

   if (updateSize > bufferData.size)
   {
      WebGL2CmdFuncResizeBuffer(device, &bufferData, updateSize);
   }

   BaseWebGLCmdUpdateBufferObject(device->getGLContext(), bufferData.glTarget, nativeId);
}

WebGL2BufferObject::~WebGL2BufferObject()
{
   if (bufferData.buffer)
   {
      delete bufferData.buffer;
      bufferData.buffer = NULL;
   }
}