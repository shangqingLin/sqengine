import { path, Rect, Vec2 } from "../../../core";
import Asset from "../../assets/Asset";
import SpriteAtlas from "../../assets/SpriteAtlas";
import SpriteFrame from "../../assets/SpriteFrame";
import { AssetInfo } from "../config";
import { AssetManager } from "../AssetManager";
import type { Texture2d } from "../../assets/Texture2d";


interface Sprite {
    name: string; //图片名字
    border: Array<number>; //9宫格的设置
    pivot: { x: number, y: number };
    rotated: boolean;
    rect: {
        x: number;
        y: number;
        width: number;
        height: number;
    };
}

interface AtlasInfo {
    path: string;
    textureWidth: number;
    textureHeight: number;
    pixelsPerUnit: number;
    sprites: Array<Sprite>;
}

export default function parseAtlas(url: string, configAssetInfo: AssetInfo, data: AtlasInfo, onComplete: (error: Error | null, asset: Asset) => void) {
    AssetManager.getInstance().load(data.path, (error: Error, texture: Asset) => {
        let atlas = new SpriteAtlas();
        atlas.texture = texture as Texture2d;
        atlas.pixelsPerUnit = data.pixelsPerUnit;
        let dirPath = path.getDirectory(configAssetInfo.gameUrl);
        atlas.sprites = new Array(data.sprites.length);
        let rect = new Rect();
        for (let i = 0, n = data.sprites.length; i < n; ++i) {
            let s = data.sprites[i];
            let sprite = new SpriteFrame();
            atlas.sprites[i] = sprite;
            rect.x = s.rect.x;
            rect.y = s.rect.y;
            rect.width = s.rect.width;
            rect.height = s.rect.height;
            sprite.rect = rect;
            sprite.rotated = s.rotated;
            sprite.name = s.name;
            if (s.pivot) sprite.pivot = new Vec2(s.pivot.x, s.pivot.y);
            let gameUrl: string = dirPath + "/" + s.name;
            let spriteAssetInfo = AssetManager.getInstance().getAssetConfig().getInfoByGameUrl(gameUrl);
            sprite.setKeyUrl(gameUrl);
            //@ts-ignore
            sprite.uuid = spriteAssetInfo.uuid;
            sprite.texture = atlas;
            AssetManager.getInstance().addAsset(sprite);
        }
        onComplete(null, atlas);
    });
}
