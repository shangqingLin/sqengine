import { Byte } from "../../core";
import { AttachmentOp, Format } from "../../gfx/index";
import { Texture2d } from "./Texture2d";


export interface RenderTextureDefine {
    /**
     * 大小是必须设置的
     */
    width: number,
    height: number,
    format?: Format, //默认是RGBA8
    needDepthStencil?: boolean //是否需要记录深度和模板缓冲区的值
    clearOp?: AttachmentOp;
}

export class RenderTexutre extends Texture2d {


    protected override createNative(): void {
        this.native = new window.Module.RenderTexture(this.id);
    }

    public initiliaze(define: RenderTextureDefine): void {
        this._width = define.width;
        this._height = define.height;

        let bufferSize: number = 14;
        let buffer = window._malloc(bufferSize);
        let byte = new Byte();
        byte.setData(window.Module.HEAPU8.subarray(buffer, buffer + bufferSize));
        byte.writeInt32(this._width);
        byte.writeInt32(this._height);
        byte.writeUint32(define.format ? define.format : Format.RGBA8);
        byte.writeByte(define.needDepthStencil ? 1 : 0);
        byte.writeByte(define.clearOp !== undefined ? define.clearOp : AttachmentOp.CLEAR);
        (this.native as Module.RenderTexture).initiliazeFormJS(buffer);
        window._free(buffer);
    }

    public reset(define: RenderTextureDefine) {
        this.initiliaze(define);
    }

    // public resize(width: number, height: number): void {
    //     this._width = width;
    //     this._height = height;
    //     this.fire('resize');
    // }
}