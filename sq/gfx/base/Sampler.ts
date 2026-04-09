import { TextureFilter, TextureWrapMode } from "./define";

/**
 * 纹理采样器参数设置对象
 * Sampler Objects
 */
export default class Smapler {

    public minFilter = TextureFilter.LINEAR;
    public magFilter = TextureFilter.LINEAR;
    public mipFilter = TextureFilter.NONE;
    public wrapS = TextureWrapMode.CLAMP_EDGE;
    public wrapT = TextureWrapMode.CLAMP_EDGE;
    public wrapR = TextureWrapMode.CLAMP_EDGE;

    static computeHash(info: Readonly<Smapler>): number {
        let hash = info.minFilter;
        hash |= (info.magFilter << 4);
        hash |= (info.wrapS << 8);
        hash |= (info.wrapT << 12);
        hash |= (info.wrapR << 16);
        return hash;
    }
}