import { ImageAsset } from "./ImageAsset";
import SimpleTexture from "./SimpleTexture";
import { DeviceManager, Format } from "../../gfx/index";
import { sqclass } from "../../core/index";
import Smapler from "../../gfx/base/Sampler";
import { dispatch } from "../../native_binding";

@sqclass("sq.Texture2d")
export class Texture2d extends SimpleTexture {

    private _mipmaps: ImageAsset[] = [];
    private clearWhenLoaded: boolean = true;
    constructor() {
        super();
        this.createNative();
    }

    protected createNative() {
        this.native = new window.Module.Texture2d(this.id);
    }

    /**
     * 设置图片的数据源。
     * 支持设置多个mipmaps的数据源，如果当前这张图片没有minmaps,只需要设置一个即可
     * @param value 
     */
    public setMipmaps(value: Array<ImageAsset> | ImageAsset) {
        this._mipmaps = Array.isArray(value) ? value : [value];
        this._setMipmapLevel(this._mipmaps.length);
        if (this._mipmaps.length > 0) {
            let imageAsset: ImageAsset = this._mipmaps[0];
            this.create(
                imageAsset.width,
                imageAsset.height,
                imageAsset.format
            );
            (this.native as Module.Texture2d).uploadData();
        } else {

        }
    }

    public setBufferData(value: ImageAsset | ImageAsset, clearWhenLoaded: boolean = true) {
        this._mipmaps = [value];
        this.clearWhenLoaded = clearWhenLoaded;
        if (value.width > this._width || value.height > this._height) {
            this._width = value.width;
            this._height = value.height;
            (this.native as Module.Texture2d).resize(value.width, value.height);
        }
        (this.native as Module.Texture2d).uploadData();
    }

    public create(width: number, height: number, format = Format.RGBA8, mipmapLevel = 1): void {
        const maxSize = Math.max(width, height);
        if (maxSize > DeviceManager.ins.device.caps.maxTextureSize) {
            throw new Error("纹理大小超出了限制，最大为：" + DeviceManager.ins.device.caps.maxTextureSize);
        }

        this._width = width;
        this._height = height;
        this._format = format;

        let info = new window.Module.ITexture2DCreateInfo();
        info.format = format;
        info.height = this._height;
        info.width = this._width;
        info.mipmapLevel = mipmapLevel;
        info.samplerHash = this._gfxSampler ? Smapler.computeHash(this._gfxSampler) : 0;
        (this.native as Module.Texture2d).create(info);
        window.Module.destroy(info);
    }

    get image(): ImageAsset | null {
        return this._mipmaps[0];
    }

    public override clear() {
        if (this.clearWhenLoaded) {
            this._mipmaps = [];
        }
    }
}