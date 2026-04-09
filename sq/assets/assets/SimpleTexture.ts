import TextureBase from "./TextureBase";

export default abstract class SimpleTexture extends TextureBase{
    protected _mipmapLevel = 1;
    protected _setMipmapLevel (value: number): void {
        this._mipmapLevel = value < 1 ? 1 : value;
    }
}