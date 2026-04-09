import { Byte } from "../core";
import { NativeToJsObjectBridge } from "../native_binding";
import { Tween } from "./Tween";
import TweenSystem from "./TweenSystem";

export default class NativeToJsDispatchTweenBridge extends NativeToJsObjectBridge
{
    public override dispatch(byte: Byte): void {
        let op:number = byte.readUint8();
        let id:number = byte.readInt32();
        let tween:Tween = TweenSystem.instance.getTween(id);
        switch(op){
            case 1:
                //@ts-ignore
                tween.callComplete();
                break;
            case 2:
                //@ts-ignore
                tween.callStart();
                break;    
            case 3:
                //@ts-ignore
                tween.callUpdate(byte.readFloat32());
                break    
        }
    }
    
}