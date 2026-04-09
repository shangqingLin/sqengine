import WebGL2Device from "./WebGL2Device";
import { runCommand as runWebGLCommad } from "../webglbase/BaseWebGLAPI";
import { Byte } from "../../core";
export function runCommand(device: WebGL2Device,commadType:number,byte:Byte):void{
    runWebGLCommad(device,commadType,byte);
}
