import WebGLDevice from "./WebGLDevice"
import { runCommand as runWebGLCommand } from "../webglbase/BaseWebGLAPI";
import type { IGLGPUShader } from "./WebGLGPUObjectDefine";
import { CmdFuncCreateShader } from "../webglbase/BaseWebGLCommand";
import { Byte } from "../../core";

export function WebGLCmdFuncCreateShader(device: WebGLDevice, shaderAssetId: number, programName: number, shader: IGLGPUShader,macroFlags:number): void {
    CmdFuncCreateShader(device, shaderAssetId, programName, shader,macroFlags);
}

export function runCommand(device: WebGLDevice, commadType: number, byte: Byte): void {
    runWebGLCommand(device, commadType, byte);
}
