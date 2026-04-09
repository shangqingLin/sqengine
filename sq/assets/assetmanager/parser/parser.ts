import Asset from "../../assets/Asset";
import { parseImage } from "./image";
import { js } from "../../../core/index";
import { parseMaterial } from "./material";
import { parsePrefab } from "./prefab";
import { parseEffect } from "./effect";
import parseSpine from "./spine";
import { AssetInfo } from "../config";
import parseAstc from "./astc";
import parseFont from "./font";
import { parseBlueprint } from "./blueprint";
import parseAtlas from "./atlas";

var parserRegister: { [key: string]: (url: string, configAssetInfo: AssetInfo, data: any, complete: (error: Error | null, asset: Asset) => void) => void } = {
    "png": parseImage,
    "jpg": parseImage,
    "jpeg": parseImage,
    "mat": parseMaterial,
    "prefab": parsePrefab,
    "scene": parsePrefab,
    "tm":parsePrefab,
    "eff": parseEffect,
    "skel": parseSpine,
    "astc": parseAstc,
    "ttf": parseFont,
    "bp": parseBlueprint,
    "atlas":parseAtlas
}

class Parser {
    /**
   * 当前正在加载的资源
   * @private
   */
    private parsingAssetMap: { [key: string]: Array<(error: Error | null, asset: Asset) => void> } = js.createMap();

    parse(url: string, configAssetInfo: AssetInfo, fileData: any, onComplete: (error: Error | null, asset: Asset) => void) {
        let parse = parserRegister[configAssetInfo.ext];
        if (!parse) {
            let asset = new Asset();
            //@ts-ignore
            asset.file = fileData;
            onComplete(null, asset);
            return;
        }

        var loadingAsset: Array<(error: Error | null, asset: Asset) => void> = this.parsingAssetMap[url];
        if (loadingAsset) {
            loadingAsset.push(onComplete);
            return;
        }
        loadingAsset = [];
        loadingAsset.push(onComplete);
        this.parsingAssetMap[url] = loadingAsset;

        parse(url, configAssetInfo, fileData, (error: Error | null, asset: Asset) => {
            for (let i = 0; i < loadingAsset.length; ++i) {
                loadingAsset[i](error, asset);
            }
            delete this.parsingAssetMap[url];
        });
    }
}

export default new Parser()