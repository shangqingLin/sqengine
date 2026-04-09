import Asset from "../../assets/Asset";
import { Effect } from "../../assets/Effect";
import { IMaterialInfo, Material } from "../../assets/Material";
import { AssetManager } from "../AssetManager";
import { AssetInfo } from "../config";


function parse(data:any,shader:Effect,onComplete:(error:Error|null,asset:Asset)=>void){
    let material = new Material();
    var info:IMaterialInfo = {
        shaderAsset:shader
    }
    material.initialize(info);
    onComplete(null,material);
}

export function parseMaterial(url:string,configAssetInfo:AssetInfo,data:any,onComplete:(error:Error|null,asset:Asset)=>void) : void{
    AssetManager.getInstance().load(data.effect,(error:Error|null,shader:Asset)=>{
        if(error) return onComplete(error,null);
        parse(data,shader as Effect,onComplete);
    });
}