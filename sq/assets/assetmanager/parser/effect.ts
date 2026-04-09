import { API, DeviceManager } from "../../../gfx/index";
import Asset from "../../assets/Asset";
import { Effect } from "../../assets/Effect";
import { AssetInfo } from "../config";

export function parseEffect(url:string,configAssetInfo:AssetInfo,data:any,onComplete:(error:Error|null,asset:Asset)=>void) : void{
    let effect = new Effect();
    let deviceApi:API = DeviceManager.ins.device.getAPI();
    for(let i = 0, n = data.shaders.length ; i < n ; ++i){
        let shaderInfo =  data.shaders[i];
        switch(deviceApi){
            case API.WEBGL:
            case API.GLES2:
                shaderInfo.glsl = shaderInfo.glsl1;
                break;      
            case API.WEBGL2:
            case API.GLES3:
                shaderInfo.glsl = shaderInfo.glsl3;
                break;                      
        }    

        delete shaderInfo.glsl1;
        delete shaderInfo.glsl3;
        delete shaderInfo.glsl4;
    }
    // effect.shaders = data.shaders;
    // effect.passes = data.passes;
    // effect.initialize();
    onComplete(null,effect);
}