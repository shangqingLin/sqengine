
import DeviceManager from "../DeviceManager";
import { GPUShader } from "../index";
import NativeToJsObjectBridge from "../../native_binding/nativetojsbridge/NativeToJsObjectBridge";
import { Byte } from "../../core";

export default class NativeToJsWebGLCommandBridge extends NativeToJsObjectBridge{
   
    private static _ins:NativeToJsWebGLCommandBridge;
    static getInstance():NativeToJsWebGLCommandBridge{
        if(!this._ins){
            this._ins = new NativeToJsWebGLCommandBridge;
        }
        return this._ins;
    }

    createShader(program:number,effectId:number,macroFlags:number,shaderIndex:number):number{
        let shader:GPUShader = DeviceManager.ins.device.createShader(program,effectId,shaderIndex,macroFlags);
        let pointer = shader.nativePointer;
        delete shader.nativePointer;
        return pointer;
    }

    public override dispatch(byte: Byte): void {
        let op = byte.readUint8();
        DeviceManager.ins.device.runCommand(op,byte);
    }
}

//@ts-ignore
window.NativeToJsWebGLCommandBridge = NativeToJsWebGLCommandBridge;