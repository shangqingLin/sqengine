/**
 * Astc图片压缩格式解析
 */

import Asset from "../../assets/Asset";
import Font from "../../assets/Font";
import { AssetInfo } from "../config";

export default function parseFont(url:string,configAssetInfo:AssetInfo,data:ArrayBuffer,onComplete:(error:Error|null,asset:Asset)=>void) : void{
    let font = new Font();
    let native:Module.FreeTypeFont = font.getNative() as Module.FreeTypeFont;
    let nativeBuffer = window._malloc(data.byteLength);
    window.Module.HEAPU8.set(new Uint8Array(data),nativeBuffer);
    native.loadFontFile(nativeBuffer,data.byteLength);
    //这个Font的内容不能释放，FreeType内部会一直持有的
    // window._free(nativeBuffer);
    onComplete(null,font);
}