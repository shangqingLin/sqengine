import { sqclass } from "../../core/index";
import { AssetManager } from "../assetmanager/AssetManager";
import Asset from "./Asset";
import type SpriteFrame from "./SpriteFrame";
import { Texture2d } from "./Texture2d";

/**
 * 图集资源
 */
@sqclass("sq.SpriteAtlas")
export default class SpriteAtlas extends Asset {
    /**
     * 图集关联哪张图片
     */
    public texture: Texture2d;

    /**
     * 图集中所有的精灵图片
     */
    public sprites: Array<SpriteFrame>;

    //Unity中的图集才有这些属性
    public pixelsPerUnit?: number;

    getSpriteFrame(name: string): SpriteFrame | null {
        for (let i = 0; i < this.sprites.length; ++i) {
            if (this.sprites[i].name === name) {
                return this.sprites[i];
            }
        }
        return null;
    }

    override destroy(): void {
        super.destroy();
        for (let i = 0, n = this.sprites.length; i < n; ++i) {
            AssetManager.getInstance().destroyAsset(this.sprites[i]);
        }
        AssetManager.getInstance().destroyAsset(this.texture);
        this.sprites = null;
        this.texture = null;
    }

}