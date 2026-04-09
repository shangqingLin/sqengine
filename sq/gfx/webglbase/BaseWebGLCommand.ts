import { AssetManager } from "../../assets";
import { Effect, IShaderDefine } from "../../assets/assets/Effect";
import { Byte } from "../../core";
import { ShaderPrecision } from "../../rendering";
import ProgramUtils from "../../scene/ProgramUtil";
import WebGLDevice from "../webgl/WebGLDevice";
import { IGLGPUShader, IGLGPUAttribute, IGLGPUUniform } from "../webgl/WebGLGPUObjectDefine";
import WebGL2Device from "../webgl2/WebGL2Device";

export function CmdFuncCreateShader(device: WebGLDevice | WebGL2Device, shaderAssetId: number, shaderIndex: number, shader: IGLGPUShader, macroFlags: number): void {
    let effect = AssetManager.getInstance().getAssetById(shaderAssetId) as Effect;
    let shaderInfo: IShaderDefine = effect.shaders[shaderIndex];
    let buildInVertexMacros = ProgramUtils.generateBuildInMarcors(2);
    let buildInFragMacros = ProgramUtils.generateBuildInMarcors(3);
    let shaderMacros = effect.generateEffectInMarcors(macroFlags, shaderIndex);
    buildInVertexMacros += shaderMacros;
    buildInFragMacros += shaderMacros;
    let gl = device.gl;
    let glsl = shaderInfo.glsl;

    let precisionDefine: string = "\nprecision " + (shaderInfo.float_precision || ShaderPrecision.MEDIUMP) + " float;";
    precisionDefine += "\nprecision " + (shaderInfo.float_precision || ShaderPrecision.MEDIUMP) + " int;\n";

    var verShaderCode = buildInVertexMacros + precisionDefine + glsl.vert;
    let verShader = gl.createShader(gl.VERTEX_SHADER);
    gl.shaderSource(verShader, verShaderCode);
    gl.compileShader(verShader);

    /* debug:start */
    var b = false;// glsl.vert.indexOf("getParticlePos") !== -1;
    if (b) {
        console.info("++++++++++++++++++++++++++++++++++++++++++++++++++++create shader");
        console.info("====vert code");
        console.info(verShaderCode);
    }
    /* debug:end */


    /* debug:start */
    if (!gl.getShaderParameter(verShader, gl.COMPILE_STATUS)) {
        console.error(gl.getShaderInfoLog(verShader));
        return;
    }

    //@ts-ignore
    if (device.shaderDebug) {
        //@ts-ignore
        device.shaderDebug.printShaderCode(verShader);
    }
    /* debug:end */

    let fragCode = buildInFragMacros + precisionDefine + glsl.frag;
    let fragShader = gl.createShader(gl.FRAGMENT_SHADER);
    gl.shaderSource(fragShader, fragCode);
    gl.compileShader(fragShader);

    /* debug:start */
    if (b) {
        console.info("====frag code");
        console.info(fragCode);
    }

    //@ts-ignore
    if (device.shaderDebug) {
        //@ts-ignore
        device.shaderDebug.printShaderCode(fragShader);
    }
    /* debug:end */

    /* debug1:start */
    if (!gl.getShaderParameter(fragShader, gl.COMPILE_STATUS)) {
        console.error(gl.getShaderInfoLog(fragShader));
        return;
    }
    /* debug1:end */

    let glProgram = gl.createProgram();
    shader.glProgram = glProgram;

    gl.attachShader(glProgram, verShader);
    gl.attachShader(glProgram, fragShader);
    gl.linkProgram(glProgram);

    /* debug1:start */
    if (!gl.getProgramParameter(glProgram, gl.LINK_STATUS)) {
        console.error(gl.getProgramInfoLog(glProgram));
        return;
    }
    /* debug1:end */

    const activeAttribCount: number = gl.getProgramParameter(glProgram, gl.ACTIVE_ATTRIBUTES);
    const activeUniformCount = gl.getProgramParameter(glProgram, gl.ACTIVE_UNIFORMS);

    let size = activeUniformCount * 7; //1 + 2 + 2 + 2;
    size += activeAttribCount * 4;
    size += 8; //数据总大小 + uniform个数 + Attribute个数

    let pointer = _malloc(size);
    shader.nativePointer = pointer;
    let data = new Uint8Array(window.Module.HEAP8.buffer, pointer, size);
    let byte = new Byte();
    byte.setData(data);
    byte.writeUint32(size);
    byte.writeUint16(activeUniformCount);
    byte.writeUint16(activeAttribCount);

    for (let i = 0; i < activeAttribCount; ++i) {
        if (!shader.glAttributes) {
            shader.glAttributes = [];
        }
        const attribInfo: WebGLActiveInfo = gl.getActiveAttrib(glProgram, i);
        let varName: string;
        const nameOffset = attribInfo.name.indexOf('[');
        if (nameOffset !== -1) {
            varName = attribInfo.name.substr(0, nameOffset);
        } else {
            varName = attribInfo.name;
        }

        const glLoc = gl.getAttribLocation(glProgram, varName);

        // if(b)
        // console.info("===atrribute：", glLoc, attribInfo.name, attribInfo.size);

        let attribute: IGLGPUAttribute = Object.create(null) as IGLGPUAttribute;
        attribute.glLoc = glLoc;
        /* debug:start */
        attribute.name = varName;
        /* debug:end */

        //@ts-ignore
        attribute.name = varName;
        shader.glAttributes.push(attribute);
        byte.writeUint16(shader.glAttributes.length - 1);
        let find = false;
        for (let n = 0; n < shaderInfo.attributes.length; ++n) {
            if (shaderInfo.attributes[n].name === varName) {
                byte.writeUint16(n);
                find = true;
                break;
            }
        }
        /* debug:start */
        if (!find) {
            console.error("找不到顶点属性 : " + varName);
        }
        /* debug:end */
    }


    for (let i = 0; i < activeUniformCount; ++i) {
        const uniformInfo: WebGLActiveInfo = gl.getActiveUniform(glProgram, i);

        let varName: string;
        const nameOffset = uniformInfo.name.indexOf('[');
        if (nameOffset !== -1) {
            varName = uniformInfo.name.substr(0, nameOffset);
        } else {
            varName = uniformInfo.name;
        }

        if (!shader.glUniforms) {
            shader.glUniforms = [];
        }

        let glLoc = gl.getUniformLocation(glProgram, uniformInfo.name);
        if (uniformInfo.type === gl.SAMPLER_2D || uniformInfo.type === gl.SAMPLER_CUBE) {

            let find = false;
            //type
            byte.writeByte(2);
            for (let j = 0; j < shaderInfo.samplerTextures.length; ++j) {
                if (shaderInfo.samplerTextures[j].name === varName) {
                    find = true;
                    byte.writeUint16(0); //没有block，记录为0
                    byte.writeUint16(j); //index
                    break;
                }
            }

            /* debug:start */
            if (!find) {
                console.error("找不到Sampler属性:" + varName);
            }
            /* debug:end */

            //location
            byte.writeUint16(shader.glUniforms.length);
            let sampler: IGLGPUUniform = Object.create(null);
            sampler.glLoc = glLoc;
            /* debug:start */
            sampler.name = varName;
            /* debug:end */
            shader.glUniforms.push(sampler);

        } else {
            byte.writeByte(3);
            let find = false;
            for (let j = 0; j < shaderInfo.blocks.length; j++) {
                const block = shaderInfo.blocks[j];
                for (let k = 0; k < block.members.length; k++) {
                    const uniform = block.members[k];
                    if (uniform.name === varName) {
                        byte.writeUint16(j); //block index
                        byte.writeUint16(k); //member index
                        find = true;
                        break;
                    }
                }
                if (find) {
                    break;
                }
            }

            /* debug:start */
            if (!find) {
                console.error("找不到Uniform属性" + varName);
            }
            /* debug:end */

            byte.writeUint16(shader.glUniforms.length);
            let uniform: IGLGPUUniform = Object.create(null);
            uniform.glLoc = glLoc;
            /* debug:start */
            uniform.name = varName;
            /* debug:end */
            shader.glUniforms.push(uniform);
        }
    }
}

