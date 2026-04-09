import JsToNativeObjectBridge from "./JsToNativeObjectBridge";
import WasmByte from "../common/WasmByte";

export default class JsToNativeDispatch {
    private _byte: WasmByte;
    private _objects: { [key: number]: JsToNativeObjectBridge };
    private _lock: boolean = false;
    constructor() {
        this._byte = new WasmByte();

        /**
         * 第一个4字节记录同步调用数据的开始地址
         * 第二个4字节记录同步调用数据的大小
         * 第三个4字节记录异步调用的dt
         * 第四个4字节记录异步调用的数据大小
         */
        this._byte.pos = 16;
        this._byte.onResizeHandler = this.onBufferResize.bind(this);
        this._objects = Object.create(null);
    }

    private onBufferResize(pointer: number, size: number) {
        window.Module.ccall("resizeDispatchMemeory", null, ["number", "number"], [pointer, size]);
    }

    resize(size: number): void {
        this._byte.resize(size);
    }

    set pos(p) {
        this._byte.pos = p;
    }

    get pos() {
        return this._byte.pos;
    }

    get dataSize() {
        return this.pos;
    }

    get bufferSize() {
        return this._byte.bufferSize;
    }


    writeUInt8(v: number): number {
        let p1 = this._byte.pos;
        this._byte.writeUint8(v);
        return p1;
    }

    writeInt8(v: number) {
        let p1 = this._byte.pos;
        this._byte.writeByte(v);
        return p1;
    }

    writeUInt16(v: number) {
        let p1 = this._byte.pos;
        this._byte.writeUint16(v);
        return p1;
    }

    writeInt16(v: number) {
        let p1 = this._byte.pos;
        this._byte.writeInt16(v);
        return p1;
    }


    writeInt32(v: number) {
        let p1 = this._byte.pos;
        this._byte.writeInt32(v);
        return p1;
    }

    writeUInt32(v: number) {
        let p1 = this._byte.pos;
        this._byte.writeInt32(v);
        return p1;
    }

    writeFloat32(v: number) {
        let p1 = this._byte.pos;
        this._byte.writeFloat32(v);
        return p1;
    }

    writeFloat64(v: number) {
        let p1 = this._byte.pos;
        this._byte.writeFloat64(v);
        return p1;
    }

    writeUTFString(v: string) {
        let p1 = this._byte.pos;
        this._byte.writeUTFString(v);
        return p1;
    }

    lock() {
        /* debug:start */
        if (this._lock) {
            throw new Error("当前正在执行操作，不能再继续其他操作");
        }
        /* debug:end */
        this._lock = true;
    }

    unLock() {
        this._lock = false;
    }

    addObject(obj: JsToNativeObjectBridge) {
        //@ts-ignore
        this._objects[obj.nativeId] = obj;
    }

    removeObject(obj: JsToNativeObjectBridge) {
        //@ts-ignore
        this._objects[obj.nativeId] = null;
    }

    frameBegin(dt: number) {

        let pos = this._byte.pos;

        this._byte.pos = 8;

        //将帧时间传递到Native中
        this._byte.writeFloat32(dt);

        //记录数据大小
        this._byte.writeUint32(0);

        this._byte.pos = pos;
    }

    /**
     * 立刻执行C++端的调用
     */
    flush() {
        /* debug:start */
        if (this._lock) {
            throw new Error("当前正在执行操作，不能再继续其他操作");
        }
        /* debug:end */

        /* debug:start */
        //不清除为什么必须使用try catch才能在控制台输出Error错误，否则报错了也没有任何反应
        try {
            /* debug:end */
            let size = this._byte.pos;
            if (size > 16) {
                this._byte.pos = 12;
                this._byte.writeUint32(size);
                this._byte.pos = 16;
                window.Module.ccall("jsToNativeDispatch");
                this.clear();
            }
            /* debug:start */
        } catch (e) {
            console.error(e);
        }
        /* debug:end */

    }

    private clear() {
        //清空临时数据
        let nativeIds: Array<any> = Object.keys(this._objects);
        for (let i = 0, n = nativeIds.length; i < n; ++i) {
            let obj: JsToNativeObjectBridge = this._objects[nativeIds[i]];
            if (obj) {
                obj.reset();
            }
        }
    }

    dispatch() {
        //记录数据大小 
        let size = this._byte.pos;
        this._byte.pos = 12;
        this._byte.writeUint32(size);
        this._byte.pos = 16;
        this.clear();
    }


    frameEnd() {

    }
}

