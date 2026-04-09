#include "JsToNativeObjectBridge.h"
using namespace bridge;

void JsToNativeObjectBridge::dispatch(ArrayBuffer& buffer,ObjectType type){
    int& nativeId = *buffer.popp<int>();
	unsigned int& op = *buffer.popp<unsigned int>();

    // printf("        native process %d %d %d \n",type,nativeId,op);

    processDispatch(buffer,nativeId,op,type);
}
