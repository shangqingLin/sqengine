#ifndef _BRIDGE_PYHSICS_SYSTEM_H_
#define _BRIDGE_PYHSICS_SYSTEM_H_
#include "../../bindings/binding.h"

namespace physics2d {
    class PyhsicsSystemBridge : public bridge::JsToNativeObjectBridge{
        public:    
            virtual void processDispatch(ArrayBuffer& buffer,int nativeId,unsigned int op,bridge::ObjectType type);
    };
}

#endif