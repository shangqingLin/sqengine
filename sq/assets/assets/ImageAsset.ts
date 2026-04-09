import { Format } from "../../gfx/index";
import Asset from "./Asset";


/**
 * 直接存储在内存中的图像数据
 */
export interface IMemoryImageSource {
    _data: ArrayBufferView | null;
    _compressed?: boolean;
    width: number;
    height: number;
    format?: number;
    mipmapLevelDataSize?: number[];
}

export type ImageSource = HTMLCanvasElement | HTMLImageElement | IMemoryImageSource | ImageBitmap;

/**
 * 代表图像的数据源对象
 */
export class ImageAsset extends Asset {

    private nativeData: ImageSource;

    private _format: Format.RGBA8;

    constructor(source?: ImageSource) {
        super();
        if (source) {
            this.source = source;
        }
    }

    public set source(source: ImageSource) {
        this.nativeData = source;
    }

    public get source(): ImageSource {
        return this.nativeData;
    }

    /**
     * @en The pixel width of the image.
     * @zh 此图像资源的像素宽度。
     */
    get width(): number {
        return this.nativeData.width;
    }

    /**
     * @en The pixel height of the image.
     * @zh 此图像资源的像素高度。
     */
    get height(): number {
        return this.nativeData.height;
    }

    /**
     * @en The pixel format of the image.
     * @zh 此图像资源的像素格式。
     */
    get format(): Format {
        return this._format;
    }

    /**
     * @en Whether the image is in compressed texture format.
     * @zh 此图像资源是否为压缩像素格式。
     */
    // get isCompressed (): boolean {
    //     return (this._format >= PixelFormat.RGB_ETC1 && this._format <= PixelFormat.RGBA_ASTC_12x12)
    //     || (this._format >= PixelFormat.RGB_A_PVRTC_2BPPV1 && this._format <= PixelFormat.RGBA_ETC1);
    // }

    public override destroy(): void {
        super.destroy();
        if (this.nativeData) {
            if (this.nativeData && this.nativeData instanceof HTMLImageElement) {
                this.nativeData.src = "";
            } else if (this.nativeData instanceof ImageBitmap) {
                this.nativeData.close();
            }
            this.nativeData = null;
        }
    }
}