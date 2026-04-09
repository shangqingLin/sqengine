
/**
 * 使用这个工具可以直接在代码中编写Shader Code。并且可以根据当前是WebGL1.0还是2.0生成兼容性代码
 * 可以不使用effect资源，方便一些内置的Shader编写
 */

import { IAttributeInfo, IBlockInfo, IEffectDefine, IPassDefine, ISamplerTextureInfo, IShaderDefine, MacroRecord } from "../../assets/assets/Effect";
import { js } from "../../core";
import { DeviceManager, Format, Type, Uniform } from "../../gfx/index";
import { BuildInBlock, samplerTextures } from "./buildin-block";
import { getAttributeDefine, getVaryingDefine, webgl1CommonDefine } from "./buildin-chunk/macro";
import { FragMainWebGL1, FragMainWebGL2, VertexMainWebGL1, VertexMainWebGL2 } from "./buildin-chunk/shader_main_chunk";
import { texture_array_define, texture_array_func, texture_array_varying } from "./buildin-chunk/texture_array_frag";
import { ShaderPrecision } from "./define";

export default class Shader {

    private attributes: Array<IAttributeInfo> = [];
    private varying: Array<IAttributeInfo> = [];
    private blocks: Array<IBlockInfo> = [];
    private samplerTexture: Array<ISamplerTextureInfo> = [];
    private pass: Array<IPassDefine> = [];
    private blockMap: { [key: string]: IBlockInfo } = js.createMap();
    private blockBinding: number = 0;
    private vertexCode: string;
    private fragCode: string;
    private cBlock: IBlockInfo;
    private defines: MacroRecord;

    addSamplerTexture(type: Type, name: string, count?: number) {
        this.addSamplerTextureInfo({
            name: name,
            type: type,
            count: count || 1
        });
    }

    /**
     * 
     * @param stage 1 是顶点着色器，其他值为片元着色器
     * @param texture 
     */
    addSamplerTextureInfo(texture: ISamplerTextureInfo) {
        texture.binding = this.blockBinding++;
        this.samplerTexture.push(texture);
    }

    addBuildInBlock(blockInfo: BuildInBlock) {
        let block: IBlockInfo = this.blockMap[blockInfo.block.name];
        if (block) {
            throw new Error(blockInfo.block.name + " Uniform Block Exsit");
        }
        block = blockInfo.block;
        this.blockMap[blockInfo.block.name] = block;
        this.blocks.push(block);
    }

    addDefine(name: string, value?: boolean | string | number) {
        if (!this.defines) {
            this.defines = js.createMap();
        }
        this.defines[name] = value;
    }

    beginBlock(blockName: string) {
        let block: IBlockInfo = this.blockMap[blockName];

        /* debug:start */
        if (block)
            throw new Error("block重复" + blockName);
        /* debug:end */

        block = js.createMap();
        block.binding = this.blockBinding++;
        block.name = blockName;
        block.members = [];
        this.blockMap[blockName] = block;
        this.blocks.push(block);
        this.cBlock = block;
    }

    endBlock() {
        this.cBlock = null;
    }

    addUniformToBlock(type: Type, name: string, precisoin: ShaderPrecision = ShaderPrecision.HIGHP) {

        /* debug:start */
        if (!this.cBlock) throw new Error("请先调用beginBlock");
        if (type === Type.SAMPLER2D) new Error("纹理不能在block中声明");
        /* debug:end */

        let member: Uniform = js.createMap();
        member.name = name;
        member.type = type;
        member.count = 1;
        this.cBlock.members.push(member);
    }

    addAttribute(type: Format, name: string, precisoin: ShaderPrecision = ShaderPrecision.HIGHP) {
        for (let i = 0, n = this.attributes.length; i < n; ++i) {
            if (this.attributes[i].name === name) {
                throw new Error(name + " Attribute Exist");
            }
        }
        let attribute: IAttributeInfo = js.createMap();
        attribute.name = name;
        attribute.format = type;
        this.attributes.push(attribute);
    }

    addVarying(type: Format, name: string) {
        for (let i = 0, n = this.varying.length; i < n; ++i) {
            if (this.varying[i].name === name) {
                throw new Error(name + " Varing Exist");
            }
        }
        let attribute: IAttributeInfo = js.createMap();
        attribute.name = name;
        attribute.format = type;
        this.varying.push(attribute);
    }

    setShaderCode(code: { vert: string, frag: string }) {
        this.vertexCode = code.vert;
        this.fragCode = code.frag;
    }

    toCode(): { vert: string, frag: string } {
        let apiType: number = DeviceManager.ins.apiType();
        let vertCode: Array<string> = [];
        let fragCode: Array<string> = [];

        vertCode.push(this.vertexCode);
        vertCode.push(apiType === 2 ? VertexMainWebGL2 : VertexMainWebGL1);

        fragCode.push(this.fragCode);
        fragCode.push(apiType === 2 ? FragMainWebGL2 : FragMainWebGL1);

        return {
            vert: vertCode.join("\n"),
            frag: fragCode.join("\n")
        }
    }

    addPass(info: IPassDefine) {
        this.pass.push(info);
    }

    createEffectDefine(): IEffectDefine {
        let define: IEffectDefine = js.createMap();
        define.shaders = [
            {
                blocks: this.blocks,
                attributes: this.attributes,
                samplerTextures: this.samplerTexture,
                macros: this.defines,
                glsl: this.toCode()
            } as IShaderDefine
        ];


        if (this.pass.length > 0) {
            define.passes = this.pass;
        } else {
            define.passes = [
                {
                    shaderIndex: 0
                }
            ]
        }

        return define;
    }
}


