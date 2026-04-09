import { js } from "../../../core/index";
import type Asset from "../../assets/Asset";
import type { AssetInfo } from "../config";
import Prefab from "../../assets/Prefab";

export function parsePrefab(url: string, configAssetInfo: AssetInfo, data: any, onComplete: (error: Error | null, asset: Asset) => void): void {
    // let asset;
    // if (configAssetInfo.ext === "scene") {
    //     asset = new SceneAsset();
    // } else {
    //     asset = new Prefab();
    // }
    // asset.data = data;
    // onComplete(null, asset);

    let asset:Prefab = new Prefab();
    asset.data = data;
    onComplete(null, asset);
}