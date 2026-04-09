import JsToNativeObjectBridge from "./NativeToJsObjectBridge";
import WasmByte from "../common/WasmByte";
import { NativeObjectType } from "../define";
import { Byte } from "../../core";
import NativeToJsObjectBridge from "./NativeToJsObjectBridge";

export default class NativeToJsDispatch {
    private bytes: WasmByte = new WasmByte();
    private nativeObjectTypeMap: { [key: number]: JsToNativeObjectBridge } = Object.create(null);
    private lock: boolean = false;

    /**
     * 麻烦死了。在Native Dispath To Js 的过程中，外部有些逻辑有立刻去调用了C++端的东西
     * 比如说创建Texture，然后这些操作里面又立刻写入上面创建的bytes，造成数据混乱了
     * 所以这里解决办法是重新创建一个Byte，然后将bytes数据复制到nativeToJsByte里面，
     * 这样不管在dispatch过程中如何修改bytes都不会引起nativeToJsByte的错乱
     */
    private nativeToJsByte: Byte = new Byte();

    constructor() {
        this.bytes.onResizeHandler = this.onResizeNativeByte.bind(this);
    }

    addDispatchHandler(type: NativeObjectType, obj: JsToNativeObjectBridge) {
        this.nativeObjectTypeMap[type] = obj;
    }

    removeNativeToJsOject(obj: NativeToJsObjectBridge) {
        let key: any = Object.keys(this.nativeObjectTypeMap);
        for (let i = 0, n = key.length; i < n; ++i) {
            if (this.nativeObjectTypeMap[key[i]] === obj) {
                delete this.nativeObjectTypeMap[key[i]];
                break;
            }
        }
    }

    onResizeNativeToJsBuffer(pointer: number, size: number): void {
        this.bytes.setExternalBuffer(pointer, size);
        this.onResizeNativeByte();
    }

    get nativeByte(): WasmByte {
        return this.bytes;
    }

    private onResizeNativeByte() {
        if (this.nativeToJsByte.length < this.bytes.bufferSize) {
            this.nativeToJsByte.length = this.bytes.bufferSize;
        }
    }


    public beginReadSyncData(): WasmByte {
        this.bytes.pos = 0;
        let dataPos = this.bytes.readUint32();
        this.bytes.pos = dataPos;
        return this.bytes;
    }

    public endReadSyncData() {
        this.bytes.pos = 0;
    }

    get isLock(): boolean {
        return this.lock;
    }

    public dispatch() {
        this.bytes.pos = 4;
        let dataSize = this.bytes.readUint32();
        if (dataSize > 0) {

            this.nativeToJsByte.byteData.set(window.Module.HEAPU8.subarray(this.bytes.pointer, this.bytes.pointer + dataSize), 0);

            //跳过上面的datasize和前4个字节
            this.nativeToJsByte.pos = 8;

            this.lock = true;
            while (this.nativeToJsByte.pos < dataSize) {
                let type = this.nativeToJsByte.readUint8();
                //   console.info("dispatch ", type, this.nativeToJsByte.pos);
                this.nativeObjectTypeMap[type].dispatch(this.nativeToJsByte);
            }

            // while (this.bytes.pos < dataSize) {
            //     let type = this.bytes.readUint8();
            //     // console.info("dispatch ", type, this.bytes.pos);
            //     //@ts-ignore
            //     this.nativeObjectTypeMap[type].dispatch(this.bytes);
            // }
            this.lock = false;
        }
    }
}