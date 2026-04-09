import { IDGen, js } from "../../core/index";
import dispatch from "../DispatchManager";
import JsToNativeDispatch from "./JsToNativeDispatch";

var idGen = new IDGen();

export default class JsToNativeObjectBridge {

    private type: number;
    private nativeId: number;
    private opMap = Object.create(null);
    private curPos: number = -1;
    private rewrite: boolean = false;
    private syncRun: boolean = false;
    private syncPrevPos: number = 0;
    private customData: any;

    constructor() {
        this.nativeId = idGen.get();
        // if(this.nativeId === 78){
        //     console.info("?????????????");
        // }
    }

    getId(): number {
        return this.nativeId;
    }

    getCustomData(): any {
        return this.customData;
    }

    /**
     * 
     * @param op 
     * @param opsub 可以组合两个操作，每个操作使用4位
     * @param ignoreRepeat 
     */
    beginOp(op: number, opsub: number = 0, ignoreRepeat: boolean = false): number {

        if (this.nativeId === -1) {
            throw new Error("对象已经被销毁");
        }

        //@ts-ignore
        let jsToNativeDispatch: JsToNativeDispatch = dispatch.jsToNativeDispatch;


        //因为外部可能在同一时刻执行大量操作，造成这里的内存不足够了
        //为了避免这种瞬时大量数据传递造成内存浪费，所以这里内存不足的时候干脆执行现存的所有操作
        if (jsToNativeDispatch.dataSize >= jsToNativeDispatch.bufferSize) {
            /* debug:start */
            console.warn("操作太多");
            /* debug:end */
            jsToNativeDispatch.flush();
        }

        jsToNativeDispatch.lock();


        // let ssop = (opsub & 0xff0000) >> 16;
        // let componentType = (opsub & 0xff000000) >> 24;
        // console.info("===========begin op ",jsToNativeDispatch.pos,op,ssop,componentType);


        //如果有子操作，则组合子操作：
        // op占用低位的16个字节
        // opsub占用高位的16个字节，在外部处理
        if (opsub) {
            op |= opsub;
        }
        let pos = this.opMap[op];
        if (!ignoreRepeat && pos && pos !== -1) {
            this.curPos = jsToNativeDispatch.pos;
            this.rewrite = true;
            jsToNativeDispatch.pos = pos;
            jsToNativeDispatch.writeUInt16(this.type);
            jsToNativeDispatch.writeInt32(this.nativeId);
        } else {
            let curPos = jsToNativeDispatch.writeUInt16(this.type);//2
            jsToNativeDispatch.writeInt32(this.nativeId);//4
            this.rewrite = false;
            if (!ignoreRepeat) {
                this.opMap[op] = curPos;
            }
        }
        jsToNativeDispatch.writeUInt32(op);//4
        return pos;
    }

    /**
     * 写完之后，立刻发送到C++端
     * @param op 
     * @param opsub 
     * @returns 
     */
    beginOpSync(op: number, opsub: number = 0) {
        if (this.nativeId === -1) {
            throw new Error("对象已经被销毁");
        }
        //@ts-ignore
        let jsToNativeDispatch: jsToNativeDispatch = dispatch.jsToNativeDispatch;

        // console.info("===========beginOpSync ",this.nativeId,jsToNativeDispatch.pos,op);
        jsToNativeDispatch.lock();
        this.syncPrevPos = jsToNativeDispatch.pos;
        if (opsub) {
            op |= opsub;
        }
        let pos = jsToNativeDispatch.writeUInt16(this.type);
        jsToNativeDispatch.writeInt32(this.nativeId);
        jsToNativeDispatch.writeUInt32(op);
        this.syncRun = true;
        return pos;
    }

    rewriteArg(pos: number, type: string, v: any): number {

        //@ts-ignore
        let jsToNativeDispatch: jsToNativeDispatch = dispatch.jsToNativeDispatch;
        let cPos = jsToNativeDispatch.pos;
        jsToNativeDispatch.pos = pos;
        this.writeOpArg(type, v);
        jsToNativeDispatch.pos = cPos;
        return cPos;
    }

    writeOpArg(type: string, v: any): number {

        //@ts-ignore
        let jsToNativeDispatch: JsToNativeDispatch = dispatch.jsToNativeDispatch;
        let pos = jsToNativeDispatch.pos;

        // console.info("writeOpArg pos :", pos, "value", v, "type", type);

        switch (type) {
            case "f32":
                jsToNativeDispatch.writeFloat32(v);
                break;
            case "i32":
                jsToNativeDispatch.writeInt32(v);
                break;
            case "i8":
                jsToNativeDispatch.writeInt8(v);
                break;
            case "ui8":
                jsToNativeDispatch.writeUInt8(v);
                break;
            case "i16":
                jsToNativeDispatch.writeInt16(v);
                break;
            case "u16i":
                jsToNativeDispatch.writeUInt16(v);
                break;

            case "ui32":
                jsToNativeDispatch.writeUInt32(v);
                break;
            case "f64":
                jsToNativeDispatch.writeFloat64(v);
                break;
            case "str":
                jsToNativeDispatch.writeUTFString(v);
                break;
            default:
                throw new Error("不支持的类型 " + type);
        }

        // console.info("writeOpArg After pos :", jsToNativeDispatch.pos);

        return pos;
    }

    endOp() {

        //@ts-ignore
        let jsToNativeDispatch: jsToNativeDispatch = dispatch.jsToNativeDispatch;

        if (this.syncRun) {
            let size = jsToNativeDispatch.pos - this.syncPrevPos;
            jsToNativeDispatch.pos = 0;
            jsToNativeDispatch.writeUInt32(this.syncPrevPos); //参数数据的开始地址
            jsToNativeDispatch.writeUInt32(size); //多少个数据
            this.syncRun = false;
            jsToNativeDispatch.pos = this.syncPrevPos;
            window.Module.ccall("jsToNativeDispatchSync");
        } else {
            if (this.rewrite) {
                jsToNativeDispatch.pos = this.curPos;
            }
            this.rewrite = false;
            this.curPos = -1;
        }
        jsToNativeDispatch.unLock();

        // console.info("===========endOp ",this.nativeId,jsToNativeDispatch.pos);
    }

    /**
     * 立刻执行所有的C++端的操作
     * 这有利于在多个操作需要同步的执行的情况下， 将多个操作使用一次的C++函数的调用。提升与C++交互性能
     */
    endOpFlush() {

        //@ts-ignore
        let jsToNativeDispatch: jsToNativeDispatch = dispatch.jsToNativeDispatch;
        jsToNativeDispatch.flush();
    }

    reset() {
        let keys = Object.keys(this.opMap);
        for (let i = 0, n = keys.length; i < n; ++i) {
            this.opMap[keys[i]] = -1;
        }
    }

    destroy() {
        dispatch.removeJsToNativeObj(this);
        this.customData = null;
    }
}