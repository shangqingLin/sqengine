import { dispatch, JsToNativeObjectBridge, ObjectType } from "../../native_binding";
import GraphicsBuffer from "./GraphicsBuffer";

export default class IndexBuffer extends GraphicsBuffer {

    private nativeObject: JsToNativeObjectBridge;
    constructor() {
        super();
        this.nativeObject = dispatch.createJsToNativeObject(ObjectType.IndexBuffer, this);
        this.nativeObject.beginOp(1);
        this.nativeObject.endOp();
    }

    override setData(data: Uint8Array): void {
        if (!this.getData() || this.getData().length != data.length) {
            this.nativeObject.beginOp(4);
            this.nativeObject.writeOpArg("i32", data.length);
            this.nativeObject.endOp();
        }
        super.setData(data);
    }

    override setDirty(): void {
        this.nativeObject.beginOp(3);
        this.nativeObject.endOp();
    }

    destroy() {
        this.nativeObject.beginOp(2);
        this.nativeObject.endOp();
        this.nativeObject.destroy();
    }
}