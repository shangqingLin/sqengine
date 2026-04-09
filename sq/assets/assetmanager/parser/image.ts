import Asset from "../../assets/Asset";
import { ImageAsset } from "../../assets/ImageAsset";
import { Texture2d } from "../../assets/Texture2d";
import { AssetInfo } from "../config";

export function parseImage(url:string,configAssetInfo:AssetInfo,imageAsset:ImageAsset,onComplete:(error:Error|null,asset:Asset)=>void) : void{
    let texture2d = new Texture2d();
    texture2d.setMipmaps(imageAsset);
    onComplete(null,texture2d);
}