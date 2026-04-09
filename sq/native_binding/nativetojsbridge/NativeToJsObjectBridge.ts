import { Byte } from "../../core";

export default abstract class NativeToJsObjectBridge{
     public abstract dispatch(byte:Byte):void;
}