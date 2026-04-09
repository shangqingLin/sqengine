import JsToNativeDispatch from "./jstonativebridge/JsToNativeDispatch";
import { NativeObjectType } from "./define";
import NativeToJsDispatch from "./nativetojsbridge/NativeToJsDispatch";
import NativeToJsObjectBridge from "./nativetojsbridge/NativeToJsObjectBridge";
import JsToNativeObjectBridge from "./jstonativebridge/JsToNativeObjectBridge";
import { Application } from "../framework/Application";

/**
 *  
 */
class DispatchManager {

    private nativeToJsByte: NativeToJsDispatch = new NativeToJsDispatch();
    private jsToNativeDispatch = new JsToNativeDispatch();

    private jsToNativeObjPool: Array<JsToNativeObjectBridge> = [];
    private jsToNativeObjWaitToRemove: Array<JsToNativeObjectBridge> = [];
    private jsToNativeObjMap: { [key: number]: JsToNativeObjectBridge } = Object.create(null);
    public initialize() {
        //1024 x 1024 = 1M;
        this.jsToNativeDispatch.resize(1048576);
    }

    /**
     * 当Native底层Buffer大小更改时由底层调用
     * @param pointer 
     */
    private onResizeNativeToJsBuffer(pointer: number, size: number): void {
        this.nativeToJsByte.onResizeNativeToJsBuffer(pointer, size);
    }

    public registerNativeToJsObject(type: NativeObjectType, obj: NativeToJsObjectBridge) {
        this.nativeToJsByte.addDispatchHandler(type, obj);
    }

    removeNativeToJsOject(obj: NativeToJsObjectBridge)
    {   
        this.nativeToJsByte.removeNativeToJsOject(obj);
    }

    public createJsToNativeObject(type: number, customData?: any): JsToNativeObjectBridge {
        let obj: JsToNativeObjectBridge = this.jsToNativeObjPool.pop();
        if (!obj) {
            obj = new JsToNativeObjectBridge();
        }
        //@ts-ignore
        obj.customData = customData;

        //@ts-ignore
        obj.type = type;
        this.jsToNativeDispatch.addObject(obj);
        this.jsToNativeObjMap[obj.getId()] = obj;
        return obj
    }

    removeJsToNativeObj(obj: JsToNativeObjectBridge) {
        this.jsToNativeDispatch.removeObject(obj);
        this.jsToNativeObjMap[obj.getId()] = null;
        this.jsToNativeObjWaitToRemove.push(obj)
        //因为nativeId是重用的，这就是会出现同一个nativeId立刻销毁又立刻重用了
        //造成有问题的，销毁的Object延迟一下再使用
        Application.ins.timer.callLater(this, this.onDelayPushPool);
    }

    private onDelayPushPool() {
        for (let i = 0; i < this.jsToNativeObjWaitToRemove.length; ++i) {
            this.jsToNativeObjPool.push(this.jsToNativeObjWaitToRemove[i]);
        }
        this.jsToNativeObjWaitToRemove.length = 0;
    }

    getJsToNativeObject(nativeId: number): JsToNativeObjectBridge {
        return this.jsToNativeObjMap[nativeId];
    }

    public frameBegin(dt: number) {
        this.jsToNativeDispatch.frameBegin(dt);
    }

    public dispatchJsToNative() {
        this.jsToNativeDispatch.dispatch();
    }

    public getNativeToJsByte(): NativeToJsDispatch {
        return this.nativeToJsByte;
    }

    public flushJsToNative() {
        this.jsToNativeDispatch.flush();
    }

    public dispatchNativeToJs() {
        this.nativeToJsByte.dispatch();
    }

    public dispatchNativeToJsLock(): boolean {
        return this.nativeToJsByte.isLock;
    }

    public frameEnd() {
        this.jsToNativeDispatch.frameEnd();
    }
}

let dispatch = new DispatchManager();

//@ts-ignore 暴露全局，给Native端读取
window.nativeDispatchManager = dispatch;
export default dispatch;