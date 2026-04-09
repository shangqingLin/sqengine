#include "WebGLCommandBridge.h"
#include <emscripten.h>
#include <engine/core/common/ArrayBuffer.h>

namespace bridge
{
    static WebGLCommandBridge* ins = new WebGLCommandBridge();

    WebGLCommandBridge* WebGLCommandBridge::getInstance(){
        return ins;
    }
    
    void WebGLCommandBridge::createShader(int program,
            int effectId,
            unsigned int macroFlags,
            int shaderIndex,
            GPUShaderInfo& info
    ){

         void* dataPointer = EM_ASM_PTR({
            return NativeToJsWebGLCommandBridge.getInstance().createShader($0,$1,$2,$3);
        }, program,effectId,macroFlags,shaderIndex);
        
        unsigned int& dataSize = *(unsigned int*)dataPointer;
        ArrayBuffer data((char*)dataPointer,dataSize,true);
        data.popp<unsigned int>();

        unsigned short& uniformSize = *data.popp<unsigned short>();
        unsigned short& attributeSize = *data.popp<unsigned short>();

        if(attributeSize > 0){
            info.activeAttributes.assign(attributeSize,ActiveAttribute());
            for(int i = 0 ; i < attributeSize ; ++i){
                ActiveAttribute& attribute = info.activeAttributes[i];
                attribute.location = *data.popp<unsigned short>();
                attribute.index =  *data.popp<unsigned short>();
            }
        }
      
        ActiveGPUUniform uniform;
        for(int i = 0; i < uniformSize ; ++i){
            char type = *data.popp<char>();
            uniform.blockIndex = *data.popp<unsigned short>();
            uniform.memberIndex = *data.popp<unsigned short>();
            uniform.location = *data.popp<unsigned short>();
            if(type == 2){ 
               info.activeSampler.push_back(uniform);
            }else if(type == 3){
               info.activeUniform.push_back(uniform);
            }
        }
    }
}

