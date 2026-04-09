import { sqclass } from "../../core/index";
import { AssetManager } from "../assetmanager/AssetManager";
import Asset from "./Asset";
import { Texture2d } from "./Texture2d";

@sqclass("sq.SpineAsset")
export default class Spine extends Asset {

    private textures: Array<Texture2d>;
    private animations: Array<string>;
    constructor() {
        super();
        this.native = new window.Module.Spine(this.id);
    }

    public getAllAnimationNames(): Array<string> {
        if (!this.animations) {
            let count = (this.native as Module.Spine).getAnimatoinCount();
            let namesAddress = (this.native as Module.Spine).getAllAnimationNames();
            this.animations = new Array(count);
            for (let i = 0; i < count; ++i) {
                this.animations[i] = window.Module.UTF8ToString(window.Module.getValue(namesAddress, "*"));
                namesAddress += 4;
            }
        }
        return this.animations;
    }

    public getAnimationIndex(name: string): number {
        let animations = this.getAllAnimationNames();
        for (let i = 0, n = animations.length; i < n; ++i) {
            if (animations[i] === name) {
                return i;
            }
        }
        /* debug:start */
        console.error(name + "动画不存在");
        /* debug:end */
        return -1;
    }

    public getAnimationName(index: number): string {
        let animations = this.getAllAnimationNames();
        return animations[index];
    }

    public override onLoad(): void {
        let textuerCount: number = this.assetInfo.dependAssets.length;
        if (textuerCount === 0) return;
        this.textures = new Array(textuerCount);
        let texuterIds = new Int32Array(textuerCount);
        for (let i = 0; i < textuerCount; ++i) {
            let texture = AssetManager.getInstance().getAssetByUUId(this.assetInfo.dependAssets[i]) as Texture2d;

            /* debug:start */
            if (!texture) {
                console.error("Spine关联的图片没有加载成功");
            }
            /* debug:end */

            this.textures[i] = texture;
            texuterIds[i] = texture.getId();
        }

        let b = new Int8Array(texuterIds.buffer);
        let bb = window._malloc(b.byteLength);
        window.Module.HEAP8.set(b, bb);
        (this.native as Module.Spine).setTextures(bb, textuerCount);
        window._free(bb);
    }

    override destroy(): void {
        super.destroy();
        if (this.textures) {
            for (let i = 0, n = this.textures.length; i < n; ++i) {
                AssetManager.getInstance().destroyAsset(this.textures[i]);
            }
            this.textures = null;
        }
    }
}