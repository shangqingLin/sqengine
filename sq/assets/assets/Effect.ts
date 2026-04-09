/**
 * 渲染管线的状态控制
 */
import Asset from "./Asset";
import {
    Type, Format, Uniform,
    RasterizerState, DepthStencilState, BlendState,
    PrimitiveMode
} from "../../gfx/index";
import { Byte, CommonUtils, js, sqclass } from "../../core/index";
import { ShaderPrecision } from "../../rendering";

export type MacroRecord = Record<string, number | boolean | string>;

export interface IPassStates {
    primitive?: PrimitiveMode;
    rasterizerState?: RasterizerState;
    depthStencilState?: DepthStencilState;
    blendState?: BlendState;
}

/**
 * 顶点着色器声明的顶点属性
 */
export interface IAttributeInfo {
    /**
     * 属性名字
     */
    name: string;
    /**
     * 属性数据类型
     */
    format: Format;
    location: number;
}


/**
 * 着色器中声明的Uniform块
 */
export interface IBlockInfo {
    binding?: number;
    name?: string;
    members?: Uniform[];
    /**
     * 是否为内置的块
     * 如果是内置block，则上面的可以不用设置
     */
    buildIn?: boolean;
}


export interface ISamplerTextureInfo {
    binding?: number;
    name: string;
    type: Type;
    count?: number;
    /**
     * 是否为内置声明的纹理
     */
    buildin?: boolean;
}


export interface IShaderDefine {

    attributes?: Array<IAttributeInfo>;
    blocks?: Array<IBlockInfo>;
    samplerTextures?: Array<ISamplerTextureInfo>;

    /**
     * 着色器中出现的所有的宏都必须记录在这里，否则无法做着色器变体
     */
    macros?: MacroRecord;

    /**
     * 
     * 设置默认的浮点数精度。精度越低，性能越好，但质量越低
     * 默认为MEDIUMP
     */
    float_precision: ShaderPrecision;

    //默认的整数类型精度。 默认为MEDIUMP
    int_precision: ShaderPrecision;


    /**
    * 着色器源代码
    * vert : 顶点着色器源代码
    * frag ： 片元着色器源代码
    * compute ： 计算着色器源代码
    */
    glsl: { vert: string, frag: string, compute?: string };
}

export interface IPassDefine {
    shaderIndex: number;
    states?: IPassStates;
}

export interface IEffectDefine {
    shaders: Array<IShaderDefine>;
    passes: Array<IPassDefine>;
}

/**
 * 代表着色器资源
 */
@sqclass("sq.Effect")
export class Effect extends Asset {

    public readonly shaders: IShaderDefine[];

    /** 
     * 传统的做法是通过将宏名字拼接起来作为Shader变体唯一的key，这造成这个Key可能非常长浪费内存，
     * 而且与C++交互也不好操作，所以这里会为每个宏名字动态生成一个唯一的数字的key
    */
    private shaderMacroFlags: Array<{ [key: string]: number }>;

    constructor() {
        super();
        this.native = new window.Module.EffectAsset(this.id);
    }



    initialize(effectDefine: IEffectDefine): void {

        //@ts-ignore
        this.shaders = effectDefine.shaders;

        let nativeEffectInfo = new window.Module.IEffectDefine();

        for (let i = 0, n = effectDefine.shaders.length; i < n; ++i) {
            let shader: IShaderDefine = effectDefine.shaders[i];
            let nativeShaderInfo = new window.Module.IShaderDefine();
            nativeEffectInfo.pushShaderDefine(nativeShaderInfo);

            if (shader.attributes) {
                for (let a = 0; a < shader.attributes.length; ++a) {
                    let nativeAttr = new window.Module.IAttributeInfo();
                    let attribute = shader.attributes[a];
                    nativeAttr.format = attribute.format;
                    nativeAttr.setName(attribute.name);
                    nativeShaderInfo.pushAttribute(nativeAttr);
                }
            }

            if (shader.blocks) {
                for (let a = 0; a < shader.blocks.length; ++a) {
                    let nativeBlock = new window.Module.IBlockInfo();
                    let block = shader.blocks[a];
                    nativeBlock.binding = block.binding;
                    nativeShaderInfo.pushBlock(nativeBlock);
                    nativeBlock.setBlockName(block.name);
                    if (block.buildIn) {
                        nativeBlock.buildin = true;
                    } else {
                        for (let n = 0, nn = block.members.length; n < nn; ++n) {
                            let me = block.members[n];
                            let uniform = new window.Module.IUniform();
                            uniform.setName(me.name);
                            uniform.count = me.count;
                            uniform.type = me.type;
                            nativeBlock.pushMembers(uniform);
                        }
                    }
                }
            }

            if (shader.samplerTextures) {
                for (let a = 0; a < shader.samplerTextures.length; ++a) {
                    let samplerTexture = shader.samplerTextures[a];
                    let b = new window.Module.ISamplerTextureInfo();
                    b.setName(samplerTexture.name);
                    if (samplerTexture.buildin) {
                        b.buildin = true;
                    } else {
                        b.binding = samplerTexture.binding;
                        b.count = samplerTexture.count;
                        b.type = samplerTexture.type;
                    }
                    nativeShaderInfo.pushSamplerTexture(b);
                }
            }

            if (shader.macros) {
                if (!this.shaderMacroFlags) this.shaderMacroFlags = [];
                let info = js.createMap();
                this.shaderMacroFlags[i] = info;
                let names = Object.keys(shader.macros);
                for (let macroId = 0; macroId < names.length; ++macroId) {
                    let flag = macroId + 1;
                    info[names[macroId]] = flag;
                    nativeShaderInfo.pushMacro(names[macroId], flag);
                }
            }
        }

        let nativeBuffer = [];
        for (let i = 0, n = effectDefine.passes.length; i < n; ++i) {
            let passInfo = effectDefine.passes[i];
            let pass = new window.Module.IPassInfo();
            pass.shaderIndex = passInfo.shaderIndex || 0;
            if (passInfo.states) {
                let buffer = writePassToNative(passInfo.states);
                pass.passStateBuffer = buffer;
                nativeBuffer.push(buffer);
            }
            nativeEffectInfo.pushPassInfo(pass);
        }

        (this.native as Module.EffectAsset).initialize(nativeEffectInfo);
        window.Module.destroy(nativeEffectInfo);
        for (let i = 0; i < nativeBuffer.length; ++i) {
            window._free(nativeBuffer[i]);
        }
    }

    public generateEffectInMarcors(flags: number, shaderIndex: number): string {
        let shaderInfo = this.shaders[shaderIndex];
        if (!shaderInfo.macros) return "";
        let names: Array<string> = Object.keys(shaderInfo.macros);
        let code = "";
        for (let i = 0; i < names.length; ++i) {
            let flag: number = this.shaderMacroFlags[shaderIndex][names[i]];
            let v: number = 1 << flag;
            if (v & flags) {
                code += "#define " + names[i] + " 1 \n";
            }
        }
        return code;
    }
}

function writeValue(byte: Byte, value: any, state: number, bit: number): number {
    if (CommonUtils.isDefine(value)) {
        state |= 1 << bit;
        byte.writeUint8(value);
    }
    return state;
}

function writePassToNative(info: IPassStates) {
    let byte = new Byte();
    /**
     * PrimitiveMode : 1
     * Blend: 
     */
    byte.length = 1;
    byte.writeByte(info.primitive !== undefined ? info.primitive : -1);

    let state = 0;
    let statePos = byte.pos;
    byte.writeUint32(state);

    if (info.blendState) {
        state |= 1 << 1;

        let blendState = 0;
        let blendStatePos = byte.pos;
        byte.writeUint32(blendState);

        if (info.blendState.blendTarget) {
            blendState |= 1 << 9;
            blendState = writeValue(byte, info.blendState.blendTarget.blend, blendState, 1);
            blendState = writeValue(byte, info.blendState.blendTarget.blendSrc, blendState, 2);
            blendState = writeValue(byte, info.blendState.blendTarget.blendSrcAlpha, blendState, 3);
            blendState = writeValue(byte, info.blendState.blendTarget.blendDst, blendState, 4);
            blendState = writeValue(byte, info.blendState.blendTarget.blendDstAlpha, blendState, 5);
            blendState = writeValue(byte, info.blendState.blendTarget.blendEq, blendState, 6);
            blendState = writeValue(byte, info.blendState.blendTarget.blendAlphaEq, blendState, 7);
            blendState = writeValue(byte, info.blendState.blendTarget.blendColorMask, blendState, 8);
        } else if (CommonUtils.isDefine(info.blendState.blendMode)) {
            blendState |= 1 << 10;
            blendState = writeValue(byte, info.blendState.blendMode, blendState, 1);
        }

        let p = byte.pos;
        byte.pos = blendStatePos;
        byte.writeUint32(blendState);
        byte.pos = p;
    }

    if (info.depthStencilState) {
        state |= 1 << 2;
    }

    if (info.rasterizerState) {
        state |= 1 << 3;

    }

    let p = byte.pos;
    byte.pos = statePos;
    byte.writeUint32(state);
    byte.pos = p;


    byte.length = byte.pos;
    let buffer = window._malloc(byte.length);
    window.Module.HEAP8.set(byte.byteData, buffer);
    return buffer;
}