import JsToNativeObjectBridge from "../../native_binding/jstonativebridge/JsToNativeObjectBridge";
import Node from "../../scene/Node"
import { EventDispatch, sqclass } from "../../core/index"

@sqclass("sq.Component")
export default abstract class Component extends EventDispatch {

    protected nativeType: number;

    public readonly node: Node;

    constructor(nativeType?: number) {
        super();
        this.nativeType = nativeType;
    }

    protected onInitialize(): void { }
    protected onEnabled(): void { };
    protected onDisabled(): void { };
    protected onRemove() {
        this.node.offAll(this);
        //@ts-ignore
        this.node = null;
    }

    protected nativeBeginOp(op: number, ignoreRepeat: boolean = false): number {

        /* debug:start */
        if (!this.nativeType) throw new Error("没有指定NativeType");
        /* debuge:end */

        //@ts-ignore
        let nativeObject: JsToNativeObjectBridge = this.node.nativeObject;

        //因为Node中，所有的Component都使用同一个NativeObject,
        //所以需要使用componentType+op组合成唯一的键标记是哪个组件的哪个操作
        op = op << 16;
        let componentType = this.nativeType << 24;
        return nativeObject.beginOp(8, op | componentType, ignoreRepeat);
    }

    protected nativeBeginOpSysc(op: number) {

        /* debug:start */
        if (!this.nativeType) throw new Error("没有指定NativeType");
        /* debuge:end */

        //@ts-ignore
        let nativeObject: JsToNativeObjectBridge = this.node.nativeObject;

        //因为Node中，所有的Component都使用同一个NativeObject,
        //所以需要使用componentType+op组合成唯一的键标记是哪个组件的哪个操作
        op = op << 16;
        let componentType = this.nativeType << 24;
        return nativeObject.beginOpSync(8, op | componentType);
    }

    protected getNativeObject(): JsToNativeObjectBridge {
        //@ts-ignore
        return this.node.nativeObject;
    }

    protected nativeWriteOpArg(type: string, value: any): number {
        //@ts-ignore
        let nativeObject: JsToNativeObjectBridge = this.node.nativeObject;
        return nativeObject.writeOpArg(type, value);
    }

    protected nativeWriteOpArgOverride(type: string, value: any, pos: any): void {
        //@ts-ignore
        let nativeObject: JsToNativeObjectBridge = this.node.nativeObject;
        nativeObject.rewriteArg(pos, type, value);
    }

    protected nativeEndOp() {
        //@ts-ignore
        let nativeObject: JsToNativeObjectBridge = this.node.nativeObject;
        nativeObject.endOp();
    }

}
