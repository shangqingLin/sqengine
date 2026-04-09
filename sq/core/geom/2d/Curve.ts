import { ImageAsset, IMemoryImageSource, Texture2d } from "../../../assets";
import ShapePrimitive from "./ShapePrimitive";
import { Vec2 } from "../../math";
import { Format, TextureFilter } from "../../../gfx";

export default class Curve extends ShapePrimitive {
    override sampleStrokePoints(dist: number, normal?: boolean, normalType?: number): Float32Array {
        throw new Error("Method not implemented.");
    }

    public addPoint(x: number, y: number) {
        this.points.push(new Vec2(x, y));
    }

    override buildFillForTexture(): Texture2d {
        let texture: Texture2d = new Texture2d();

        let pixlesNum: number = Math.ceil(this.points.length / 2);
        // let size = new Float32Array(2);
        // let size_data = new Uint8Array(size.buffer);

        let length = 8;
        let buffer = window._malloc(length);
        window.Module.ccall("calculateTextureSize", null, ["number", "number", "number", "bool"], [pixlesNum, buffer, 0, false]);
        let output = Module.HEAP32.subarray(buffer / 4, buffer / 4 + 2);

        let size = output;
        let data = new Float32Array(pixlesNum * 4);
        let index = 0;
        for (let i = 0, n = this.points.length; i < n; ++i) {
            data[index++] = this.points[i].x;
            data[index++] = this.points[i].y;
            // data[index++] = 0;
            // data[index++] = 0;
        }


        let imgSource = new ImageAsset();
        imgSource.source = {
            _data: data,
            width: size[0],
            height: size[1]
        } as IMemoryImageSource;

        texture.setFilters(TextureFilter.NEAREST, TextureFilter.NEAREST);
        texture.create(size[0], size[1], Format.RGBA32F);
        texture.setBufferData(imgSource);
        return texture;
    }

}