#ifndef _OBJECT_OBJECT_H_
#define _OBJECT_OBJECT_H_

#include "../../engine/core/common/ArrayBuffer.h"
#include "../define.h"

namespace bridge{
  class JsToNativeObjectBridge{
    private:
       void dispatch(ArrayBuffer& buffer,ObjectType type); 
    public:
      friend class DispatchManager;
      virtual void processDispatch(ArrayBuffer& buffer,int nativeId,unsigned int op,ObjectType type) = 0;
  };
}

#endif