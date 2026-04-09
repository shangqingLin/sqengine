export default class WasmByte {

    private _pointer: number = null;
    private _pos: number = 0;
    private _bufferSize: number = 0;
    private _u16int: Uint16Array;
    private _16int: Int16Array;
    private _u32int: Uint32Array;
    private _32int: Int32Array;
    private _32f: Float32Array;
    private _64f: Float64Array;
    private _byteArray: Uint8Array;
    private _byteBefor2Array: Uint8Array;
    private _byteBefor4Array: Uint8Array;
    public onResizeHandler: (address: number, size: number) => void;

    constructor() {
        const buffer = new ArrayBuffer(8);
        this._u16int = new Uint16Array(buffer);
        this._16int = new Int16Array(buffer);
        this._u32int = new Uint32Array(buffer);
        this._32int = new Int32Array(buffer);
        this._32f = new Float32Array(buffer);
        this._64f = new Float64Array(buffer);
        this._byteArray = new Uint8Array(buffer);
        this._byteBefor2Array = this._byteArray.subarray(0, 2);
        this._byteBefor4Array = this._byteArray.subarray(0, 4);
    }

    resize(size: number) {
        let newPointer = window._malloc(size);
        if (this._pointer) {
            window.Module.HEAP8.set(window.Module.HEAP8.subarray(this._pointer, this._pointer + this._bufferSize), newPointer);
            window._free(this._pointer);
        }
        this._pointer = newPointer;
        this._bufferSize = size;
        if (this.onResizeHandler) {
            this.onResizeHandler(this._pointer, size);
        }
    }

    setExternalBuffer(pointer: number, size: number) {
        this._pointer = pointer;
        this._bufferSize = size;
    }

    checkSize(size: number) {
        if (this._bufferSize < size) {
            this.resize(size);
        }
    }

    clearData() {
        this._pos = 0;
    }

    writeInt8(value: number) {
        this.checkSize(this._pos + 1);
        window.Module.HEAP8[this._pointer + this._pos] = value;
        ++this._pos;
    }

    writeUint8(value: number) {
        this.checkSize(this._pos + 1);
        window.Module.HEAPU8[this._pointer + this._pos] = value;
        ++this._pos;
    }

    writeByte(value: number) {
        this.writeInt8(value);
    }

    writeInt16(value: number) {
        this.checkSize(this._pos + 2);
        this._16int[0] = value;
        window.Module.HEAPU8.set(this._byteBefor2Array, this._pointer + this._pos);
        this._pos += 2;
    }

    writeUint16(value: number) {
        this.checkSize(this._pos + 2);
        this._u16int[0] = value;
        window.Module.HEAPU8.set(this._byteBefor2Array, this._pointer + this._pos);
        this._pos += 2;
    }
    writeInt32(value: number) {
        this.checkSize(this._pos + 4);
        this._32int[0] = value;
        window.Module.HEAPU8.set(this._byteBefor4Array, this._pointer + this._pos);
        this._pos += 4;
    }

    writeUint32(value: number) {
        this.checkSize(this._pos + 4);
        this._u32int[0] = value;
        window.Module.HEAPU8.set(this._byteBefor4Array, this._pointer + this._pos);
        this._pos += 4;
    }

    writeFloat32(value: number) {
        this.checkSize(this._pos + 4);
        this._32f[0] = value;
        window.Module.HEAPU8.set(this._byteBefor4Array, this._pointer + this._pos);
        this._pos += 4;
    }

    writeFloat64(value: number) {
        this.checkSize(this._pos + 8);
        this._64f[0] = value;
        window.Module.HEAPU8.set(this._byteArray, this._pointer + this._pos);
        this._pos += 8;
    }


    writeUTFBytes(value: string) {
        value = value + "";
        for (var i = 0, sz = value.length; i < sz; i++) {
            var c = value.charCodeAt(i);

            // console.info("unicode ",c,value[i]);

            //小于等于127，只用一个字节即可存储这个字符
            if (c <= 0x7F) {
                this.writeUint8(c);
            }
            else if (c <= 0x7FF) { //0x7FF = 2047
                this.writeUint8(0xC0 | (c >> 6));
                this.writeUint8(0x80 | (c & 0x3F));
            }
            else if (c >= 0xD800 && c <= 0xDBFF) { //0xD800 = 55296 0xDBFF = 56319
                i++;
                const c2 = value.charCodeAt(i);
                if (!Number.isNaN(c2) && c2 >= 0xDC00 && c2 <= 0xDFFF) {
                    const _p1 = (c & 0x3FF) + 0x40;
                    const _p2 = c2 & 0x3FF;
                    const _b1 = 0xF0 | ((_p1 >> 8) & 0x3F);
                    const _b2 = 0x80 | ((_p1 >> 2) & 0x3F);
                    const _b3 = 0x80 | ((_p1 & 0x3) << 4) | ((_p2 >> 6) & 0xF);
                    const _b4 = 0x80 | (_p2 & 0x3F);
                    this.writeUint8(_b1);
                    this.writeUint8(_b2);
                    this.writeUint8(_b3);
                    this.writeUint8(_b4);
                }
            }
            else if (c <= 0xFFFF) { //0xFFFF = 65535
                this.writeUint8(0xE0 | (c >> 12));
                this.writeUint8(0x80 | ((c >> 6) & 0x3F));
                this.writeUint8(0x80 | (c & 0x3F));
            }
            else {
                this.writeUint8(0xF0 | (c >> 18));
                this.writeUint8(0x80 | ((c >> 12) & 0x3F));
                this.writeUint8(0x80 | ((c >> 6) & 0x3F));
                this.writeUint8(0x80 | (c & 0x3F));
            }
        }
    }

    writeUTFString(value: string) {
        var tPos = this._pos;

        //记录字符串占用字节的长度
        this.writeUint16(1);

        this.writeUTFBytes(value);

        var curPos = this._pos;

        //得到字符串字节长度
        var size = this.pos - tPos - 2;

        // console.info("writeUTFString value :", value, tPos,size);

        this._pos = tPos;
        this.writeUint16(size);
        this._pos = curPos;
    }


    getDataSize() {
        return this._pos;
    }

    set pos(pos: number) {
        this._pos = pos;
    }

    get pos(): number {
        return this._pos;
    }

    get bufferSize() {
        return this._bufferSize;
    }

    get pointer() {
        return this._pointer;
    }

    readInt8() {
        let v = window.Module.HEAP8[this._pointer + this._pos];
        ++this._pos;
        return v;
    }

    readUint8() {
        let v = window.Module.HEAPU8[this._pointer + this._pos];
        ++this._pos;
        return v;
    }
    readByte() {
        return this.readInt8();
    }

    readInt16() {
        let p = this._pointer + this._pos;
        this._byteArray.set(window.Module.HEAPU8.subarray(p, p + 2), 0);
        this._pos += 2;
        return this._16int[0];
    }
    readUint16() {
        let p = this._pointer + this._pos;
        this._byteArray.set(window.Module.HEAPU8.subarray(p, p + 2), 0);
        this._pos += 2;
        return this._u16int[0];
    }
    readInt32() {
        let p = this._pointer + this._pos;
        this._byteArray.set(window.Module.HEAPU8.subarray(p, p + 4), 0);
        this._pos += 4;
        return this._32int[0];
    }

    readUint32() {
        let p = this._pointer + this._pos;
        this._byteArray.set(window.Module.HEAPU8.subarray(p, p + 4), 0);
        this._pos += 4;
        return this._u32int[0];
    }
    readFloat32() {
        let p = this._pointer + this._pos;
        this._byteArray.set(window.Module.HEAPU8.subarray(p, p + 4), 0);
        this._pos += 4;
        return this._32f[0];
    }
    readFloat64() {
        let p = this._pointer + this._pos;
        this._byteArray.set(window.Module.HEAPU8.subarray(p, p + 8), 0);
        this._pos += 8;
        return this._64f[0];
    }

    readDataAddress(): number {
        let pointer = this.readUint32();
        return pointer;
    }

    readBuffer(size: number): Uint8Array {
        let p = this._pointer + this._pos;
        let data = window.Module.HEAPU8.subarray(p, p + size);
        this._pos += size;
        return data;
    }

    readFloat32Array(numCount: number): Float32Array {
        let p = this._pointer + this._pos;

        let data: Float32Array;
        let size = numCount * 4;

        //读取Float32地址必须是4字节对其的，即地址为4的倍数
        //但这里记录的数据是紧凑的，不是4字节对齐，所以对于不是4字节对齐
        //则需要复制数据，然后才能解析成浮点型
        if (p % 4 === 0) {
            data = window.Module.HEAPF32.subarray(p >> 2, (p + size) >> 2);
            this._pos += size;
        } else {
            // 从原始 buffer 中读取非对齐的数据
            const rawBytes = this.readBuffer(size); // 16 字节，假设包含 4 个 float

            // 创建一个新的对齐的 ArrayBuffer
            const alignedBuffer = new ArrayBuffer(size);
            new Uint8Array(alignedBuffer).set(rawBytes); // 拷贝数据

            // 现在可以安全地用 Float32Array 读取了
            data = new Float32Array(alignedBuffer);
        }
        return data;
    }

    destroy() {
        if (this._pointer) {
            window._free(this._pointer);
            this._pointer = NaN;
        }
    }
}
