import Asset from "../../assets/Asset";
import Spine from "../../assets/Spine";
import { AssetInfo } from "../config";

export default function parseSpine(url:string,configAssetInfo:AssetInfo,data:ArrayBuffer,onComplete:(error:Error|null,asset:Asset)=>void):void
{
    let asset = new Spine();
    let spineNativeSpine:Module.Spine = asset.getNative() as Module.Spine;
    let buffer = window._malloc(data.byteLength);
    window.Module.HEAP8.set(new Int8Array(data),buffer);
    spineNativeSpine.parseBuffer(buffer,data.byteLength);
    window._free(buffer);
    onComplete(null,asset);
}