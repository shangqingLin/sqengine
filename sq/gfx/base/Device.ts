import { Byte } from "../../core";
import { IGLGPUShader } from "../webglbase/BaseGPUObjectDefine";
import { DeviceCaps, API, GPUShader, GPUObject } from "./define"

export default abstract class Device {
    public canvas: HTMLCanvasElement;
    public readonly caps: DeviceCaps = new DeviceCaps();

    /**
     * 记录当前图形API的版本
     */
    protected _gfxAPI: API = API.UNKNOWN;

    private gpuObjectMap: { [key: number]: GPUObject } = Object.create(null);
    private tempShaderObjectMap: { [key: number]: IGLGPUShader } = Object.create(null);
    public readonly GPUStateCache: {
        glProgram: number
    } = Object.create(null);

    public getAPI(): API {
        return this._gfxAPI;
    }
    abstract initialize(canvas: HTMLCanvasElement): void;
    abstract createShader(program: number, effectId: number, shaderIndex: number, macroFlags: number): GPUShader;
    abstract runCommand(commadType: number, byte: Byte): void;
    public addGpuObject(id: number, obj: GPUObject) {
        this.gpuObjectMap[id] = obj;
    }
    public getGpuObject(id: number): GPUObject {
        return this.gpuObjectMap[id];
    }
    public deleteGpuObject(id: number) {
        this.gpuObjectMap[id] = null;
    }


    /**
     * 因为Object ID是重用的
     * 比如有一个ID为A：
     * 1、首先A被Texture使用
     * 2、然后delele Texture,A被回收
     * 3、这时创建一个Shader，Shader拿到A这个ID,由于创建Shader是与C++同步，这时在JS端已经调用
     *    addGpuObject存储了A这个对象了  
     * 4、接下来执行Command Buffer，这时找到delete Texture，然后deleteTexture调用deleteGpuObject 把A删除了，这时A是一个Shader，不是Texture
     * 
     * 所以这里不立刻将Shader Object存储进去，而是放到一个临时对象中，等createProgram再放进去
     */
    public addShaderTmp(id: number, shader: IGLGPUShader) {
        this.tempShaderObjectMap[id] = shader;
    }
    public getShaderFromTemp(id: number): IGLGPUShader {
        let t = this.tempShaderObjectMap[id];
        this.tempShaderObjectMap[id] = null;
        return t;
    }
}