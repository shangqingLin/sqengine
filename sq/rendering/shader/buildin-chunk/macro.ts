import { DeviceManager } from "../../../gfx";


export function getAttributeDefine():string
{
    return "#define attribute " + (DeviceManager.ins.apiType() === 2 ? "in" : "attribute");
}

export function getVaryingDefine(input:boolean):string
{
    if(input){
        return "#define varying " + (DeviceManager.ins.apiType() === 2 ? "in" : "varying");
    }else{
        return "#define varying " + (DeviceManager.ins.apiType() === 2 ? "out" : "varying");
    }
}

export function webgl1CommonDefine()
{
    return "#define texture texture2D\n";
}