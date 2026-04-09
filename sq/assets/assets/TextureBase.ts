import Smapler from "../../gfx/base/Sampler";
import { TextureFilter, Format, TextureWrapMode } from "../../gfx/index";
import Asset from "./Asset";

/**
 * 代表图片资源的基类
 */
export default class TextureBase extends Asset {

    protected _format = Format.RGBA8;

    protected _width = 1;

    protected _height = 1;

    protected _gfxSampler: Smapler;

    get width(): number {
        return this._width;
    }

    get height(): number {
        return this._height;
    }

    public setWrapMode(wrapS: TextureWrapMode, wrapT: TextureWrapMode, wrapR?: TextureWrapMode): void {
        if (wrapR === undefined) wrapR = wrapS;

        //默认值就不用设置了
        if (wrapS === TextureWrapMode.CLAMP_EDGE && wrapT === TextureWrapMode.CLAMP_EDGE && wrapR === TextureWrapMode.CLAMP_EDGE) {
            return;
        }

        let _this = this;
        if (_this._gfxSampler && _this._gfxSampler.wrapS === wrapS && _this._gfxSampler.wrapS === wrapT && _this._gfxSampler.wrapS == wrapR) {
            return;
        }

        if (!_this._gfxSampler) {
            _this._gfxSampler = new Smapler();
        }
        _this._gfxSampler.wrapS = wrapS;
        _this._gfxSampler.wrapT = wrapT;
        _this._gfxSampler.wrapR = wrapR;
        
        if (_this.native) {
            //@ts-ignore
            _this.native.setSamplerFromJs(Smapler.computeHash(_this._gfxSampler))
        }
    }

    public setFilters(minFilter: TextureFilter, magFilter: TextureFilter): void {
        if (minFilter === TextureFilter.LINEAR && magFilter === TextureFilter.LINEAR) {
            return;
        }

        let _this = this;
        if (_this._gfxSampler && _this._gfxSampler.minFilter === minFilter && _this._gfxSampler.magFilter === magFilter) {
            return;
        }

        if (!_this._gfxSampler) {
            _this._gfxSampler = new Smapler();
        }
        _this._gfxSampler.minFilter = minFilter;
        _this._gfxSampler.magFilter = magFilter;

        if (_this.native) {
            //@ts-ignore
            _this.native.setSamplerFromJs(Smapler.computeHash(_this._gfxSampler))
        }
    }
}