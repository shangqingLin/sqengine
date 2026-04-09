import type WebGLDevice from "../webgl/WebGLDevice";
import type WebGL2Device from "../webgl2/WebGL2Device";
import { IGLGPUShader, IGLGPUBuffer, IGLGPUTexture, IGLGPUFramebuffer, IGLGPURenderbuffer, IGLGPUVAO } from "./BaseGPUObjectDefine";
import { PrimitiveMode, Type, Format, StencilOp, ComparisonFunc } from "../base/define";
import { AssetManager } from "../../assets/assetmanager/AssetManager";
import { Texture2d } from "../../assets/assets/Texture2d";
import { IMemoryImageSource } from "../../assets/assets/ImageAsset";

/* debug:start */
import GameStat from "../../profiler/GameStat";
/* debug:end */
import { JSAPAGLEnum, JSAPIMap } from "./WebGLAPIDefine";
import { dispatch, NativeObjectType } from "../../native_binding";
import NativeToJsWebGLCommandBridge from "./NativeToJsWebGLCommandBridge";
import type GraphicsBuffer from "../../scene/grahics/GraphicsBuffer";
import { Byte } from "../../core";

export const commandMap: { [key: number]: (device: WebGLDevice | WebGL2Device, byte: Byte) => void } = Object.create(null);
export const GLEnumMap: { [key: number]: number } = Object.create(null);
export const GLEnumPrimitive: { [key: number]: number } = Object.create(null);
export const GLStencilOpMap: { [key: number]: number } = Object.create(null);
export const GLComparisonFuncMap: { [key: number]: number } = Object.create(null);

type GFXFormatToWebGLFunc = (format: Format, gl: WebGLRenderingContext | WebGL2RenderingContext) => GLenum;
let GFXFormatToWebGLType: GFXFormatToWebGLFunc;

//Format转换为外部格式
let GFXFormatToWebGLFormat: GFXFormatToWebGLFunc;

//Format转换为内部格式
let GFXFormatToInternalWebGLFormat: GFXFormatToWebGLFunc;

function viewport(device: WebGLDevice | WebGL2Device, byte: Byte) {
    let x = byte.readInt32();
    let y = byte.readInt32();
    let width = byte.readInt32();
    let height = byte.readInt32();
    device.gl.viewport(x, y, width, height);

    /* debug:start */
    if (width === 0 && height === 0) {
        console.warn("viewport的宽高为0");
    }
    /* debug:end */

    // console.info("runCommand viewport", x, y, width, height);
}

function scissor(device: WebGLDevice | WebGL2Device, byte: Byte) {
    let x = byte.readInt32();
    let y = byte.readInt32();
    let width = byte.readInt32();
    let height = byte.readInt32();
    device.gl.scissor(x, y, width, height);

    // console.info("runCommand scissor", x, y, width, height);
}

function clear(device: WebGLDevice | WebGL2Device, byte: Byte) {
    let b = byte.readUint32();
    let clear = 0;
    if (JSAPAGLEnum.DEPTH_BUFFER_BIT & b) {
        clear |= GLEnumMap[JSAPAGLEnum.DEPTH_BUFFER_BIT];
    }
    if (JSAPAGLEnum.COLOR_BUFFER_BIT & b) {
        clear |= GLEnumMap[JSAPAGLEnum.COLOR_BUFFER_BIT];
    }
    if (JSAPAGLEnum.STENCIL_BUFFER_BIT & b) {
        clear |= GLEnumMap[JSAPAGLEnum.STENCIL_BUFFER_BIT];
    }
    device.gl.clear(clear);
    // console.info("runCommand clear", clear);
}

function clearColor(device: WebGLDevice | WebGL2Device, byte: Byte) {
    let red = byte.readFloat32();
    let green = byte.readFloat32();
    let blue = byte.readFloat32();
    let alpha = byte.readFloat32();
    device.gl.clearColor(red, green, blue, alpha);
    // console.info("runCommand clearColor", red, green, blue, alpha);
}

function clearDepth(device: WebGLDevice | WebGL2Device, byte: Byte) {
    let b = byte.readFloat32();
    device.gl.clearDepth(b);
    // console.info("runCommand clearDepth");
}

function clearStencil(device: WebGLDevice | WebGL2Device, byte: Byte) {
    let s = byte.readInt32();
    device.gl.clearStencil(s);
    // console.info("runCommand clearStencil");
}

function colorMask(device: WebGLDevice | WebGL2Device, byte: Byte) {
    let red: any = byte.readUint8();
    let green: any = byte.readUint8();
    let blue: any = byte.readUint8();
    let alpha: any = byte.readUint8();
    device.gl.colorMask(red, green, blue, alpha);
    // console.info("runCommand colorMask",red, green, blue, alpha);
}

function createBuffer(device: WebGLDevice | WebGL2Device, byte: Byte) {
    let id = byte.readInt32();
    let obj: IGLGPUBuffer = Object.create(null);
    obj.buffer = device.gl.createBuffer();
    device.addGpuObject(id, obj);
    // console.info("runCommand createBuffer", id);
}


function bindBuffer(device: WebGLDevice | WebGL2Device, byte: Byte) {
    let target = byte.readUint32();
    let id = byte.readInt32();
    if (id === 0) {
        device.gl.bindBuffer(GLEnumMap[target], null);
    } else {
        let obj: IGLGPUBuffer = device.getGpuObject(id) as IGLGPUBuffer;
        device.gl.bindBuffer(GLEnumMap[target], obj.buffer);
    }
    /* debug:start */
    ++GameStat.bindBuffer;
    /* debug:end */
    // console.info("runCommand bindBuffer", target, id);
}

function bufferData(device: WebGLDevice | WebGL2Device, byte: Byte) {
    let target = byte.readUint32();
    let size = byte.readInt32();
    let usage = byte.readUint32();
    device.gl.bufferData(GLEnumMap[target], size, GLEnumMap[usage]);

    /* debug:start */
    ++GameStat.bufferUpdate;
    /* debug:end */
    // console.info("runCommand bufferData", target, size, usage);
}

function bufferSubData(device: WebGLDevice | WebGL2Device, byte: Byte) {
    let target = byte.readUint32();
    let offset = byte.readUint32();
    let data = byte.readDataAddress();
    let size = byte.readInt32();

    // console.info("runCommand bufferSubData", target, offset, data, size);

    // if (target == 1) {
    //     var a = new Float32Array(window.Module.HEAP8.buffer, data, size / 4);
    //     console.info(a.join(","));
    // } else if (target == 2) {
    //     // var c = new Int8Array(Module.HEAP8.buffer,data,size);
    //     var b = new Uint16Array(window.Module.HEAP8.buffer, data, size / 2);
    //     console.info(b.join(","));
    // }

    device.gl.bufferSubData(GLEnumMap[target], offset, window.Module.HEAP8.subarray(data, data + size));

    /* debug:start */
    ++GameStat.bufferUpdate;
    /* debug:end */
}

function bufferDataFromJs(device: WebGLDevice, byte: Byte) {
    let target = byte.readUint32();
    let bufferNativeId = byte.readInt32();
    let nativeObj = dispatch.getJsToNativeObject(bufferNativeId);
    let buffer: GraphicsBuffer = nativeObj.getCustomData() as GraphicsBuffer;
    device.gl.bufferSubData(GLEnumMap[target], 0, buffer.getData());

    //todo 可以清理，估计外面不会再用了
    buffer.clearData();

    /* debug:start */
    ++GameStat.bufferUpdate;
    /* debug:end */
}

function createProgram(device: WebGLDevice | WebGL2Device, byte: Byte) {
    let programeId = byte.readInt32();
    let shaderObject = device.getShaderFromTemp(programeId);
    device.addGpuObject(programeId, shaderObject);
}

function useProgram(device: WebGLDevice | WebGL2Device, byte: Byte) {
    let programeId = byte.readInt32();
    device.GPUStateCache.glProgram = programeId;
    if (programeId === 0) {
        device.gl.useProgram(null);
    } else {
        let obj: IGLGPUShader = device.getGpuObject(programeId) as IGLGPUShader;
        device.gl.useProgram(obj.glProgram);
    }
    // console.info("runCommand useProgram", programeId);
    /* debug:start */
    ++GameStat.useShader;
    /* debug:end */
}

function deleteProgram(device: WebGLDevice | WebGL2Device, byte: Byte) {
    let programeId = byte.readInt32();
    let obj: IGLGPUShader = device.getGpuObject(programeId) as IGLGPUShader;
    device.gl.deleteProgram(obj.glProgram);
    device.deleteGpuObject(programeId);

    if (device.GPUStateCache.glProgram === programeId) {
        device.GPUStateCache.glProgram = 0;
    }

    //console.info("runCommand deleteProgram", programeId);
}

function disable(device: WebGLDevice | WebGL2Device, byte: Byte) {
    let target = byte.readUint32();
    device.gl.disable(GLEnumMap[target]);
    //   if(target == 13)
    // console.info("runCommand disable", target);
}

function enable(device: WebGLDevice | WebGL2Device, byte: Byte) {
    let target = byte.readUint32();
    device.gl.enable(GLEnumMap[target]);

    // if(target == 13)
    // console.info("runCommand enable", target);
}

function cullFace(device: WebGLDevice | WebGL2Device, byte: Byte) {
    let target = byte.readUint32();
    device.gl.cullFace(GLEnumMap[target]);

    // console.info("runCommand cullFace", target);
}

function frontFace(device: WebGLDevice | WebGL2Device, byte: Byte) {
    let target = byte.readUint32();
    device.gl.frontFace(GLEnumMap[target]);

    // console.info("runCommand frontFace", target);
}

function polygonOffset(device: WebGLDevice | WebGL2Device, byte: Byte) {
    let depthBias = byte.readFloat32();
    let depthBiasSlop = byte.readFloat32();
    device.gl.polygonOffset(depthBias, depthBiasSlop);

    //console.info("runCommand polygonOffset", depthBias, depthBiasSlop);
}

function lineWidth(device: WebGLDevice | WebGL2Device, byte: Byte) {
    let width = byte.readFloat32();
    device.gl.lineWidth(width);
    //console.info("runCommand lineWidth", width);
}

function depthMask(device: WebGLDevice, byte: Byte) {
    let b: any = byte.readUint8();
    device.gl.depthMask(b);
    //console.info("runCommand depthMask", b);
}

function depthFunc(device: WebGLDevice | WebGL2Device, byte: Byte) {
    let target = byte.readUint32();
    device.gl.depthFunc(GLEnumMap[target]);
    //console.info("runCommand depthFunc", target);
}

function stencilFuncSeparate(device: WebGLDevice | WebGL2Device, byte: Byte) {
    let front = byte.readUint32();
    let func = byte.readUint32();
    let ref = byte.readUint32();
    let mask = byte.readUint32();
    device.gl.stencilFuncSeparate(GLEnumMap[front], GLComparisonFuncMap[func], ref, mask);
    // console.info("runCommand stencilFuncSeparate", "frace", front, "fuc", func, "ref", ref, "masks", mask);
}

function stencilOpSeparate(device: WebGLDevice | WebGL2Device, byte: Byte) {
    let front = byte.readUint32();
    let sfail = byte.readUint32();
    let dpfail = byte.readUint32();
    let dppass = byte.readUint32();
    device.gl.stencilOpSeparate(GLEnumMap[front], GLStencilOpMap[sfail], GLStencilOpMap[dpfail], GLStencilOpMap[dppass]);
    // console.info("runCommand stencilOpSeparate","frace", front, "sfail", sfail, "dpfail", dpfail, "dppass", dppass);
}

function stencilMaskSeparate(device: WebGLDevice | WebGL2Device, byte: Byte) {
    let front = byte.readUint32();
    let mask = byte.readUint32();
    device.gl.stencilMaskSeparate(GLEnumMap[front], mask);
    // console.info("runCommand stencilMaskSeparate", "frace", front, "mask", mask);
}

function blendColor(device: WebGLDevice | WebGL2Device, byte: Byte) {
    let r = byte.readFloat32();
    let g = byte.readFloat32();
    let b = byte.readFloat32();
    let a = byte.readFloat32();
    device.gl.blendColor(r, g, b, a);
    //console.info("runCommand stencilMaskSeparate", r, g, b, a);
}

function blendFuncSeparate(device: WebGLDevice | WebGL2Device, byte: Byte) {
    let srcRGB = byte.readUint32();
    let destRGB = byte.readUint32();
    let srcAlpha = byte.readUint32();
    let destAlpha = byte.readUint32();
    device.gl.blendFuncSeparate(
        GLEnumMap[srcRGB], GLEnumMap[destRGB], GLEnumMap[srcAlpha], GLEnumMap[destAlpha]);

    // console.info("runCommand blendFuncSeparate", srcRGB, destRGB, srcAlpha, destAlpha);
}

function blendEquationSeparate(device: WebGLDevice | WebGL2Device, byte: Byte) {
    let modeRGB = byte.readUint32();
    let modeAlpha = byte.readUint32();
    device.gl.blendEquationSeparate(GLEnumMap[modeRGB], GLEnumMap[modeAlpha]);
    //console.info("runCommand blendEquationSeparate", modeRGB, modeAlpha);
}

function createTexture(device: WebGLDevice | WebGL2Device, byte: Byte) {
    let id = byte.readInt32();
    let obj: IGLGPUTexture = Object.create(null);
    obj.texture = device.gl.createTexture();
    device.addGpuObject(id, obj);

    // console.info("runCommand createTexture", id);
}


function activeTexture(device: WebGLDevice | WebGL2Device, byte: Byte) {
    let index = byte.readInt32();
    device.gl.activeTexture(device.gl.TEXTURE0 + index);

    // console.info("runCommand activeTexture", index);
}

// var bind: any;
function bindTexture(device: WebGLDevice | WebGL2Device, byte: Byte) {
    let target = byte.readUint32();
    let id = byte.readInt32();
    if (id === 0) {
        device.gl.bindTexture(GLEnumMap[target], null);
    } else {
        let obj: IGLGPUTexture = device.getGpuObject(id) as IGLGPUTexture;
        device.gl.bindTexture(GLEnumMap[target], obj.texture);
    }
    /* debug:start */
    ++GameStat.bindTextureCall;
    /* debug:end */

    // if(id == 36)
    // console.info("runCommand bindTexture", "target", target, "id", id);
}

function deleteTexture(device: WebGLDevice | WebGL2Device, byte: Byte) {
    let id = byte.readInt32();
    let obj: IGLGPUTexture = device.getGpuObject(id) as IGLGPUTexture;
    device.gl.deleteTexture(obj.texture);
    device.deleteGpuObject(id);
    //console.info("runCommand deleteTexture", id);
}

function texParameteri(device: WebGLDevice | WebGL2Device, byte: Byte) {
    let target = byte.readUint32();
    let v1 = byte.readUint32();
    let v2 = byte.readUint32();
    device.gl.texParameteri(GLEnumMap[target], GLEnumMap[v1], GLEnumMap[v2]);

    // console.info("runCommand texParameteri", target, v1, v2, GLEnumMap[v1], GLEnumMap[v2]);
}

function texImage2D(device: WebGLDevice | WebGL2Device, byte: Byte) {
    let target = byte.readUint32();
    let level = byte.readInt32();
    let internalFormat = byte.readInt32();
    let width = byte.readInt32();
    let height = byte.readInt32();
    let border = byte.readInt32();
    let format = byte.readUint32();
    let type = byte.readUint32();
    device.gl.texImage2D(
        GLEnumMap[target],
        level,
        GFXFormatToInternalWebGLFormat(internalFormat, device.gl),
        width,
        height,
        border,
        GFXFormatToWebGLFormat(format, device.gl),
        GFXFormatToWebGLType(type, device.gl),
        null
    );
    /* debug:start */
    ++GameStat.updateTexture;
    /* debug:end */
    // console.info("runCommand texImage2D", target, level, internalFormat, width, height, border, format, type);
}

function texSubImage2D(device: WebGLDevice | WebGL2Device, byte: Byte) {

    //从一个数组中加载纹理数据

    let target = byte.readUint32();
    let level = byte.readInt32();
    let xOffset = byte.readInt32();
    let yOfset = byte.readInt32();
    let width = byte.readInt32();
    let height = byte.readInt32();
    let format = byte.readInt32();
    let type = byte.readInt32();
    let textureAssetId = byte.readInt32();
    let asset: Texture2d = AssetManager.getInstance().getAssetById(textureAssetId) as Texture2d;
    let bufferView = (asset.image.source as IMemoryImageSource)._data;
    //支持直接使用Buffer或数组填充图片数据
    device.gl.texSubImage2D(
        GLEnumMap[target],
        level,
        xOffset, yOfset,
        width, height,
        GFXFormatToWebGLFormat(format, device.gl),
        GFXFormatToWebGLType(type, device.gl),
        bufferView);

    //节省内存，加载之后直接清除掉
    asset.clear();
    /* debug:start */
    ++GameStat.updateTexture;
    /* debug:end */
    // console.info("runCommand texSubImage2D", target, level, xOffset, yOfset, width, height, format);

}

function texSubImage2D_2(device: WebGLDevice | WebGL2Device, byte: Byte) {

    //从一个纹理资源加载纹理数据

    let target = byte.readUint32();
    let level = byte.readInt32();
    let xOffset = byte.readInt32();
    let yOfset = byte.readInt32();
    let format = byte.readUint32();
    let type = byte.readUint32();
    let textureAssetId = byte.readInt32();
    let asset: Texture2d = AssetManager.getInstance().getAssetById(textureAssetId) as Texture2d;

    // console.info(bind, asset);

    /**
      * 对于纹理数据来源为HTMLCanvasElement、HTMLImageElement、HTMLVideoElement来说，图片原点是在左上角的；
      * 但是UV坐标系原点是在左下角的，所以在将数据读入到显存中的时候，必须将纹理进行Y轴翻转过来，否则读取纹理数据肯定错误的
      * 将改参数设置为true，则就可以进行上下翻转。
      * 
      * 但对于其他的纹理数据就不用了，比如你自己创建的ArrayBuffer,ImageBitmap等
      * 底层直接从UV坐标翻转
      */
    //device.gl.pixelStorei(device.gl.UNPACK_FLIP_Y_WEBGL, true);

    let source = asset.image.source as IMemoryImageSource;
    let isByteBuffer = source._data && (Array.isArray(source._data) || ArrayBuffer.isView(source._data));
    if (isByteBuffer) {
        device.gl.texSubImage2D(
            GLEnumMap[target],
            level,
            xOffset, yOfset,
            asset.image.width, asset.image.height,
            GFXFormatToWebGLFormat(format, device.gl),
            GFXFormatToWebGLType(type, device.gl),
            source._data);
    } else {
        //支持Image等HTML标签的图片数据
        device.gl.texSubImage2D(
            GLEnumMap[target],
            level,
            xOffset, yOfset,
            GFXFormatToWebGLFormat(format, device.gl),
            GFXFormatToWebGLType(type, device.gl),
            asset.image.source as HTMLImageElement
        );
    }

    // device.gl.pixelStorei(device.gl.UNPACK_FLIP_Y_WEBGL, false);

    //节省内存，加载之后直接清除掉
    asset.clear();
    /* debug:start */
    ++GameStat.updateTexture;
    /* debug:end */
    // console.info("runCommand texSubImage2D_2", target, level, xOffset, yOfset, format, type, textureAssetId);
}

function texSubImage2DBuffer(device: WebGLDevice | WebGL2Device, byte: Byte) {
    let target = byte.readUint32();
    let level = byte.readInt32();
    let xOffset = byte.readInt32();
    let yOfset = byte.readInt32();
    let width = byte.readInt32();
    let height = byte.readInt32();
    let format = byte.readUint32();
    let type = byte.readUint32();
    let bufferSize = byte.readUint32();
    let nativeBuffer = byte.readByte();
    let bufferData: Uint8Array | Float32Array;
    let glType = GFXFormatToWebGLType(type, device.gl);
    if (glType === device.gl.FLOAT) {
        if (nativeBuffer) {
            bufferData = byte.readFloat32Array(bufferSize >> 2); //除以4
            // console.info(bufferData.join());
        } else {
            let address = byte.readDataAddress();
            bufferData = window.Module.HEAPF32.subarray(address >> 2, (address + bufferSize) >> 2);
        }
    } else {
        if (nativeBuffer) {
            bufferData = byte.readBuffer(bufferSize);
        } else {
            let address = byte.readDataAddress();
            bufferData = window.Module.HEAPU8.subarray(address, address + bufferSize);
            // console.info("++++bufferSIze",bufferSize);
        }
    }

    device.gl.texSubImage2D(
        GLEnumMap[target],
        level,
        xOffset,
        yOfset,
        width,
        height,
        GFXFormatToWebGLFormat(format, device.gl),
        glType,
        bufferData
    );
    // console.info("run texSubImage2DBuffer ",target,type);

    /* debug:start */
    ++GameStat.updateTexture;
    /* debug:end */
}


function enableVertexAttribArray(device: WebGLDevice | WebGL2Device, byte: Byte) {
    let shader: IGLGPUShader = device.getGpuObject(device.GPUStateCache.glProgram) as IGLGPUShader;
    let location = byte.readUint16();
    device.gl.enableVertexAttribArray(shader.glAttributes[location].glLoc);
    // console.info("run enableVertexAttribArray ",location);
}

function vertexAttribPointer(device: WebGLDevice | WebGL2Device, byte: Byte) {
    let shader: IGLGPUShader = device.getGpuObject(device.GPUStateCache.glProgram) as IGLGPUShader;
    let location = byte.readUint16();
    let count = byte.readInt32();

    // let testType = byte.readUint32();
    // let type: GLenum = GFXFormatToWebGLType(testType, device.gl);
    let type: GLenum = GFXFormatToWebGLType(byte.readUint32(), device.gl);
    let stride = byte.readInt32();
    let offset = byte.readInt32();
    device.gl.vertexAttribPointer(shader.glAttributes[location].glLoc, count, type, false, stride, offset);
    // console.info("run vertexAttribPointer ", location,count,type,stride,offset);
}

function drawElements(device: WebGLDevice | WebGL2Device, byte: Byte) {
    let primitive = byte.readUint32();
    let indexCount = byte.readInt32();
    let type = byte.readUint32();
    let offset = byte.readUint32();
    device.gl.drawElements(GLEnumPrimitive[primitive], indexCount, GLEnumMap[type], offset);

    // console.info("runcommand drawElements indexCount ", indexCount, "type", type, "offset", offset)

    /* debug:start */
    ++GameStat.drawCall;
    /* debug:end */
}

function createFramebuffer(device: WebGLDevice | WebGL2Device, byte: Byte) {
    let id = byte.readInt32();
    let obj: IGLGPUFramebuffer = Object.create(null);
    obj.frameBuffer = device.gl.createFramebuffer();
    device.addGpuObject(id, obj);
}

function bindFramebuffer(device: WebGLDevice | WebGL2Device, byte: Byte) {
    let target = byte.readUint32();
    let frameBuffer = byte.readInt32();

    // console.info("bindFramebuffer", target, frameBuffer);

    if (frameBuffer === 0) {
        device.gl.bindFramebuffer(GLEnumMap[target], null);
    } else {
        let obj: IGLGPUFramebuffer = device.getGpuObject(frameBuffer) as IGLGPUFramebuffer;
        device.gl.bindFramebuffer(GLEnumMap[target], obj.frameBuffer);
    }
    /* debug:start */
    ++GameStat.bindFrameBufferCall;
    /* debug:end */
}

function deleteFramebuffer(device: WebGLDevice | WebGL2Device, byte: Byte) {
    let frameBuffer = byte.readInt32();
    let obj: IGLGPUFramebuffer = device.getGpuObject(frameBuffer) as IGLGPUFramebuffer;
    device.gl.deleteFramebuffer(obj.frameBuffer);
    device.deleteGpuObject(frameBuffer);
}

function createRenderbuffer(device: WebGLDevice | WebGL2Device, byte: Byte) {
    let id = byte.readInt32();
    let renderBuffer: IGLGPURenderbuffer = Object.create(null);
    renderBuffer.renderBuffer = device.gl.createRenderbuffer();
    device.addGpuObject(id, renderBuffer);
}

function deleteRenderbuffer(device: WebGLDevice | WebGL2Device, byte: Byte) {
    let id = byte.readInt32();
    let obj: IGLGPURenderbuffer = device.getGpuObject(id) as IGLGPURenderbuffer;
    device.gl.deleteFramebuffer(obj.renderBuffer);
    device.deleteGpuObject(id);
}

function bindRenderbuffer(device: WebGLDevice | WebGL2Device, byte: Byte) {
    let target = byte.readUint32();
    let id = byte.readInt32();
    let obj: IGLGPURenderbuffer = device.getGpuObject(id) as IGLGPURenderbuffer;
    device.gl.bindRenderbuffer(GLEnumMap[target], obj.renderBuffer);
}


function renderbufferStorage(device: WebGLDevice | WebGL2Device, byte: Byte) {
    let target = byte.readUint32();
    let internalformat = byte.readUint32();
    let width = byte.readInt32();
    let height = byte.readInt32();
    device.gl.renderbufferStorage(GLEnumMap[target], GFXFormatToInternalWebGLFormat(internalformat, device.gl), width, height);
}

function framebufferRenderbuffer(device: WebGLDevice | WebGL2Device, byte: Byte) {
    let target = byte.readUint32();
    let attachment = byte.readUint32();
    let renderbuffertarget = byte.readUint32();
    let renderbufferId = byte.readInt32();
    let renderBuffer: IGLGPURenderbuffer = device.getGpuObject(renderbufferId) as IGLGPURenderbuffer;
    device.gl.framebufferRenderbuffer(GLEnumMap[target], GLEnumMap[attachment], GLEnumMap[renderbuffertarget], renderBuffer.renderBuffer);
}

function framebufferTexture2D(device: WebGLDevice | WebGL2Device, byte: Byte) {
    let target = byte.readUint32();
    let attachment = byte.readUint32();
    let textarget = byte.readUint32();
    let texture = byte.readInt32();
    let level = byte.readInt32();
    let textureObj: IGLGPUTexture = device.getGpuObject(texture) as IGLGPUTexture;
    device.gl.framebufferTexture2D(GLEnumMap[target], GLEnumMap[attachment], GLEnumMap[textarget], textureObj.texture, level);

    /* debug:start  */
    const status = device.gl.checkFramebufferStatus(device.gl.FRAMEBUFFER);
    if (status !== device.gl.FRAMEBUFFER_COMPLETE) {
        console.error("Framebuffer not complete:", status);
    }
    /* debug:end */
}

function uniform1i(device: WebGLDevice | WebGL2Device, byte: Byte) {
    let shader: IGLGPUShader = device.getGpuObject(device.GPUStateCache.glProgram) as IGLGPUShader;
    let location = byte.readUint16();
    let v = byte.readInt32();
    device.gl.uniform1i(shader.glUniforms[location].glLoc, v);
    /* debug:start */
    ++GameStat.unfiromCall;
    /* debug:end */
}

function uniform2i(device: WebGLDevice | WebGL2Device, byte: Byte) {
    let shader: IGLGPUShader = device.getGpuObject(device.GPUStateCache.glProgram) as IGLGPUShader;
    let location = byte.readUint16();
    let v = byte.readInt32();
    let v2 = byte.readInt32();
    device.gl.uniform2i(shader.glUniforms[location].glLoc, v, v2);
    /* debug:start */
    ++GameStat.unfiromCall;
    /* debug:end */
}

function uniform3i(device: WebGLDevice | WebGL2Device, byte: Byte) {
    let shader: IGLGPUShader = device.getGpuObject(device.GPUStateCache.glProgram) as IGLGPUShader;
    let location = byte.readUint16();
    let v = byte.readInt32();
    let v2 = byte.readInt32();
    let v3 = byte.readInt32();
    device.gl.uniform3i(shader.glUniforms[location].glLoc, v, v2, v3);
    /* debug:start */
    ++GameStat.unfiromCall;
    /* debug:end */
}

function uniform4i(device: WebGLDevice | WebGL2Device, byte: Byte) {
    let shader: IGLGPUShader = device.getGpuObject(device.GPUStateCache.glProgram) as IGLGPUShader;
    let location = byte.readUint16();
    let v = byte.readInt32();
    let v2 = byte.readInt32();
    let v3 = byte.readInt32();
    let v4 = byte.readInt32();
    device.gl.uniform4i(shader.glUniforms[location].glLoc, v, v2, v3, v4);
    /* debug:start */
    ++GameStat.unfiromCall;
    /* debug:end */
}

function uniform1iv(device: WebGLDevice | WebGL2Device, byte: Byte) {
    let shader: IGLGPUShader = device.getGpuObject(device.GPUStateCache.glProgram) as IGLGPUShader;
    let location = byte.readUint16();
    let size = byte.readInt32();
    let copy = byte.readByte();
    let data: Int32Array;
    if (copy) {
        let uData = byte.readBuffer(size * 4);
        data = new Int32Array(uData);
    } else {
        let dataAddress = byte.readDataAddress();
        data = new Int32Array(window.Module.HEAP32.buffer, dataAddress, size);
    }
    // console.info(data);
    // console.info("++++++++++ data ", data.join(","), data.length, size, shader.glUniforms[location].name);
    device.gl.uniform1iv(shader.glUniforms[location].glLoc, data);
    /* debug:start */
    ++GameStat.unfiromCall;
    /* debug:end */
}

function uniform1f(device: WebGLDevice | WebGL2Device, byte: Byte) {
    let shader: IGLGPUShader = device.getGpuObject(device.GPUStateCache.glProgram) as IGLGPUShader;
    let location = byte.readUint16();
    let v = byte.readFloat32();
    device.gl.uniform1f(shader.glUniforms[location].glLoc, v);
    /* debug:start */
    ++GameStat.unfiromCall;
    /* debug:end */
}

function uniform2f(device: WebGLDevice | WebGL2Device, byte: Byte) {
    let shader: IGLGPUShader = device.getGpuObject(device.GPUStateCache.glProgram) as IGLGPUShader;
    let location = byte.readUint16();
    let v = byte.readFloat32();
    let v2 = byte.readFloat32();
    device.gl.uniform2f(shader.glUniforms[location].glLoc, v, v2);
    /* debug:start */
    ++GameStat.unfiromCall;
    /* debug:end */
}

function uniform3f(device: WebGLDevice | WebGL2Device, byte: Byte) {
    let shader: IGLGPUShader = device.getGpuObject(device.GPUStateCache.glProgram) as IGLGPUShader;
    let location = byte.readUint16();
    let v = byte.readFloat32();
    let v2 = byte.readFloat32();
    let v3 = byte.readFloat32();
    device.gl.uniform3f(shader.glUniforms[location].glLoc, v, v2, v3);
    /* debug:start */
    ++GameStat.unfiromCall;
    /* debug:end */
}

function uniform4f(device: WebGLDevice | WebGL2Device, byte: Byte) {
    let shader: IGLGPUShader = device.getGpuObject(device.GPUStateCache.glProgram) as IGLGPUShader;
    let location = byte.readUint16();
    let v = byte.readFloat32();
    let v2 = byte.readFloat32();
    let v3 = byte.readFloat32();
    let v4 = byte.readFloat32();
    device.gl.uniform4f(shader.glUniforms[location].glLoc, v, v2, v3, v4);

    // console.info("uniform4f", v, v2, v3, v4);
    /* debug:start */
    ++GameStat.unfiromCall;
    /* debug:end */
}

function uniform4fv(device: WebGLDevice | WebGL2Device, byte: Byte) {
    let shader: IGLGPUShader = device.getGpuObject(device.GPUStateCache.glProgram) as IGLGPUShader;
    let location = byte.readUint16();
    let data = byte.readDataAddress();
    device.gl.uniform4fv(shader.glUniforms[location].glLoc, new Float32Array(window.Module.HEAP8.buffer, data, 4));

    // console.info("uniform4f", v, v2, v3, v4);
    /* debug:start */
    ++GameStat.unfiromCall;
    /* debug:end */
}


function uniformMatrix4f(device: WebGLDevice | WebGL2Device, byte: Byte) {
    let shader: IGLGPUShader = device.getGpuObject(device.GPUStateCache.glProgram) as IGLGPUShader;
    let location = byte.readUint16();
    let data = byte.readFloat32Array(16);
    device.gl.uniformMatrix4fv(shader.glUniforms[location].glLoc, false, data);

    // console.info("uniformMatrix4f", data.join(","));

    /* debug:start */
    ++GameStat.unfiromCall;
    /* debug:end */
}

function uniformMatrix2fv(device: WebGLDevice | WebGL2Device, byte: Byte) {
    let shader: IGLGPUShader = device.getGpuObject(device.GPUStateCache.glProgram) as IGLGPUShader;
    let location = byte.readUint16();
    let data = byte.readDataAddress();
    device.gl.uniformMatrix2fv(shader.glUniforms[location].glLoc, false, new Float32Array(window.Module.HEAP8.buffer, data, 4));
    /* debug:start */
    ++GameStat.unfiromCall;
    /* debug:end */
}


function uniformMatrix3fv(device: WebGLDevice | WebGL2Device, byte: Byte) {
    let shader: IGLGPUShader = device.getGpuObject(device.GPUStateCache.glProgram) as IGLGPUShader;
    let location = byte.readUint16();
    let data = byte.readDataAddress();
    let lData = new Float32Array(window.Module.HEAP8.buffer, data, 9);
    device.gl.uniformMatrix3fv(shader.glUniforms[location].glLoc, false, lData);
    /* debug:start */
    ++GameStat.unfiromCall;
    /* debug:end */

    // console.info("uniformMatrix3fv", lData.join(","));
}

function uniformMatrix4fv(device: WebGLDevice | WebGL2Device, byte: Byte) {
    let shader: IGLGPUShader = device.getGpuObject(device.GPUStateCache.glProgram) as IGLGPUShader;
    let location = byte.readUint16();
    let data = byte.readDataAddress();
    let lData = new Float32Array(window.Module.HEAP8.buffer, data, 16);
    device.gl.uniformMatrix4fv(shader.glUniforms[location].glLoc, false, lData);
    /* debug:start */
    ++GameStat.unfiromCall;
    /* debug:end */
    // console.info("uniformMatrix4fv", data,lData.join(","));
}

function beginWriteTempData(device: WebGLDevice, byte: Byte) {
    let dataSize = byte.readInt32();
    byte.pos += dataSize;
}


export function initializeBaseAPI(device: WebGLDevice | WebGL2Device) {

    dispatch.registerNativeToJsObject(NativeObjectType.gl, new NativeToJsWebGLCommandBridge);

    let context: WebGLRenderingContext = device.gl;

    commandMap[JSAPIMap.createBuffer] = createBuffer;
    commandMap[JSAPIMap.bindBuffer] = bindBuffer;
    commandMap[JSAPIMap.bufferData] = bufferData;
    commandMap[JSAPIMap.bufferSubData] = bufferSubData;
    commandMap[JSAPIMap.createShader] = null;
    commandMap[JSAPIMap.shaderSource] = null;
    commandMap[JSAPIMap.compileShader] = null;
    commandMap[JSAPIMap.attachShader] = null;
    commandMap[JSAPIMap.detachShader] = null;
    commandMap[JSAPIMap.deleteShader] = null;
    commandMap[JSAPIMap.createProgram] = createProgram;
    commandMap[JSAPIMap.useProgram] = useProgram;
    commandMap[JSAPIMap.linkProgram] = null;
    commandMap[JSAPIMap.deleteProgram] = deleteProgram;
    commandMap[JSAPIMap.disable] = disable;
    commandMap[JSAPIMap.enable] = enable;
    commandMap[JSAPIMap.cullFace] = cullFace;
    commandMap[JSAPIMap.frontFace] = frontFace;
    commandMap[JSAPIMap.polygonOffset] = polygonOffset;
    commandMap[JSAPIMap.lineWidth] = lineWidth;
    commandMap[JSAPIMap.depthMask] = depthMask;
    commandMap[JSAPIMap.depthFunc] = depthFunc;
    commandMap[JSAPIMap.stencilFuncSeparate] = stencilFuncSeparate;
    commandMap[JSAPIMap.stencilOpSeparate] = stencilOpSeparate;
    commandMap[JSAPIMap.stencilMaskSeparate] = stencilMaskSeparate;
    commandMap[JSAPIMap.blendColor] = blendColor;
    commandMap[JSAPIMap.blendFuncSeparate] = blendFuncSeparate;
    commandMap[JSAPIMap.blendEquationSeparate] = blendEquationSeparate;
    commandMap[JSAPIMap.createTexture] = createTexture;
    commandMap[JSAPIMap.bindTexture] = bindTexture;
    commandMap[JSAPIMap.deleteTexture] = deleteTexture;
    commandMap[JSAPIMap.texParameteri] = texParameteri;
    commandMap[JSAPIMap.texImage2D] = texImage2D;
    commandMap[JSAPIMap.texSubImage2D] = texSubImage2D;
    commandMap[JSAPIMap.texSubImage2D_2] = texSubImage2D_2;
    commandMap[JSAPIMap.texSubImage2D_buffer] = texSubImage2DBuffer;
    commandMap[JSAPIMap.activeTexture] = activeTexture;
    commandMap[JSAPIMap.uniform1i] = uniform1i;
    commandMap[JSAPIMap.uniform2i] = uniform2i;
    commandMap[JSAPIMap.uniform3i] = uniform3i;
    commandMap[JSAPIMap.uniform4i] = uniform4i;
    commandMap[JSAPIMap.uniform1iv] = uniform1iv;
    commandMap[JSAPIMap.uniform1f] = uniform1f;
    commandMap[JSAPIMap.uniform2f] = uniform2f;
    commandMap[JSAPIMap.uniform3f] = uniform3f;
    commandMap[JSAPIMap.uniform4f] = uniform4f;
    commandMap[JSAPIMap.uniform4fv] = uniform4fv;
    commandMap[JSAPIMap.uniformMatrix4f] = uniformMatrix4f;
    commandMap[JSAPIMap.uniformMatrix2fv] = uniformMatrix2fv;
    commandMap[JSAPIMap.uniformMatrix3fv] = uniformMatrix3fv;
    commandMap[JSAPIMap.uniformMatrix4fv] = uniformMatrix4fv;
    commandMap[JSAPIMap.enableVertexAttribArray] = enableVertexAttribArray;
    commandMap[JSAPIMap.vertexAttribPointer] = vertexAttribPointer;
    commandMap[JSAPIMap.drawElements] = drawElements;
    commandMap[JSAPIMap.viewport] = viewport;
    commandMap[JSAPIMap.scissor] = scissor;
    commandMap[JSAPIMap.clear] = clear;
    commandMap[JSAPIMap.clearColor] = clearColor;
    commandMap[JSAPIMap.clearDepth] = clearDepth;
    commandMap[JSAPIMap.clearStencil] = clearStencil;
    commandMap[JSAPIMap.colorMask] = colorMask;
    commandMap[JSAPIMap.createFramebuffer] = createFramebuffer;
    commandMap[JSAPIMap.bindFramebuffer] = bindFramebuffer;
    commandMap[JSAPIMap.deleteFramebuffer] = deleteFramebuffer;
    commandMap[JSAPIMap.createRenderbuffer] = createRenderbuffer;
    commandMap[JSAPIMap.deleteRenderbuffer] = deleteRenderbuffer
    commandMap[JSAPIMap.framebufferRenderbuffer] = framebufferRenderbuffer;
    commandMap[JSAPIMap.framebufferTexture2D] = framebufferTexture2D;
    commandMap[JSAPIMap.bindRenderbuffer] = bindRenderbuffer;
    commandMap[JSAPIMap.renderbufferStorage] = renderbufferStorage;
    commandMap[JSAPIMap.beginWriteTempData] = beginWriteTempData;
    commandMap[JSAPIMap.bufferDataFromJs] = bufferDataFromJs;

    GLEnumMap[JSAPAGLEnum.ARRAY_BUFFER] = context.ARRAY_BUFFER;
    GLEnumMap[JSAPAGLEnum.ELEMENT_ARRAY_BUFFER] = context.ELEMENT_ARRAY_BUFFER;
    GLEnumMap[JSAPAGLEnum.DYNAMIC_DRAW] = context.DYNAMIC_DRAW;
    GLEnumMap[JSAPAGLEnum.STATIC_DRAW] = context.STATIC_DRAW;
    GLEnumMap[JSAPAGLEnum.VERTEX_SHADER] = context.VERTEX_SHADER;
    GLEnumMap[JSAPAGLEnum.FRAGMENT_SHADER] = context.FRAGMENT_SHADER;
    GLEnumMap[JSAPAGLEnum.CULL_FACE] = context.CULL_FACE;
    GLEnumMap[JSAPAGLEnum.FRONT] = context.FRONT;
    GLEnumMap[JSAPAGLEnum.BACK] = context.BACK;
    GLEnumMap[JSAPAGLEnum.CCW] = context.CCW;
    GLEnumMap[JSAPAGLEnum.CW] = context.CW;
    GLEnumMap[JSAPAGLEnum.DEPTH_TEST] = context.DEPTH_TEST;
    GLEnumMap[JSAPAGLEnum.STENCIL_TEST] = context.STENCIL_TEST;
    GLEnumMap[JSAPAGLEnum.BLEND] = context.BLEND;
    GLEnumMap[JSAPAGLEnum.TEXTURE_2D] = context.TEXTURE_2D;
    GLEnumMap[JSAPAGLEnum.REPEAT] = context.REPEAT;
    GLEnumMap[JSAPAGLEnum.CLAMP_TO_EDGE] = context.CLAMP_TO_EDGE;
    GLEnumMap[JSAPAGLEnum.LINEAR] = context.LINEAR;
    GLEnumMap[JSAPAGLEnum.NEAREST] = context.NEAREST;
    GLEnumMap[JSAPAGLEnum.NEAREST_MIPMAP_NEAREST] = context.NEAREST_MIPMAP_NEAREST;
    GLEnumMap[JSAPAGLEnum.NEAREST_MIPMAP_LINEAR] = context.NEAREST_MIPMAP_LINEAR;
    GLEnumMap[JSAPAGLEnum.LINEAR_MIPMAP_LINEAR] = context.LINEAR_MIPMAP_LINEAR;

    GLEnumMap[JSAPAGLEnum.MIRRORED_REPEAT] = context.MIRRORED_REPEAT;
    GLEnumMap[JSAPAGLEnum.TEXTURE_WRAP_S] = context.TEXTURE_WRAP_S;
    GLEnumMap[JSAPAGLEnum.TEXTURE_WRAP_T] = context.TEXTURE_WRAP_T;
    GLEnumMap[JSAPAGLEnum.TEXTURE_MAG_FILTER] = context.TEXTURE_MAG_FILTER;
    GLEnumMap[JSAPAGLEnum.TEXTURE_MIN_FILTER] = context.TEXTURE_MIN_FILTER;

    GLEnumMap[JSAPAGLEnum.UNSIGNED_BYTE] = context.UNSIGNED_BYTE;
    GLEnumMap[JSAPAGLEnum.UNSIGNED_SHORT] = context.UNSIGNED_SHORT;
    GLEnumMap[JSAPAGLEnum.UNSIGNED_INT] = context.UNSIGNED_INT;
    GLEnumMap[JSAPAGLEnum.BYTE] = context.BYTE;
    GLEnumMap[JSAPAGLEnum.INT] = context.INT;
    GLEnumMap[JSAPAGLEnum.FLOAT] = context.FLOAT;
    GLEnumMap[JSAPAGLEnum.FRAMEBUFFER] = context.FRAMEBUFFER;
    GLEnumMap[JSAPAGLEnum.RENDERBUFFER] = context.RENDERBUFFER;
    GLEnumMap[JSAPAGLEnum.COLOR_ATTACHMENT0] = context.COLOR_ATTACHMENT0;
    GLEnumMap[JSAPAGLEnum.COLOR_ATTACHMENT1] = context.COLOR_ATTACHMENT0 + 1;
    GLEnumMap[JSAPAGLEnum.COLOR_ATTACHMENT2] = context.COLOR_ATTACHMENT0 + 2;
    GLEnumMap[JSAPAGLEnum.COLOR_ATTACHMENT3] = context.COLOR_ATTACHMENT0 + 3;
    GLEnumMap[JSAPAGLEnum.COLOR_ATTACHMENT4] = context.COLOR_ATTACHMENT0 + 4;
    GLEnumMap[JSAPAGLEnum.COLOR_ATTACHMENT5] = context.COLOR_ATTACHMENT0 + 5;
    GLEnumMap[JSAPAGLEnum.COLOR_ATTACHMENT6] = context.COLOR_ATTACHMENT0 + 6;
    GLEnumMap[JSAPAGLEnum.COLOR_ATTACHMENT7] = context.COLOR_ATTACHMENT0 + 7;
    GLEnumMap[JSAPAGLEnum.DEPTH_ATTACHMENT] = context.DEPTH_ATTACHMENT;
    GLEnumMap[JSAPAGLEnum.DEPTH_STENCIL_ATTACHMENT] = context.DEPTH_STENCIL_ATTACHMENT;
    GLEnumMap[JSAPAGLEnum.DEPTH_BUFFER_BIT] = context.DEPTH_BUFFER_BIT;
    GLEnumMap[JSAPAGLEnum.STENCIL_BUFFER_BIT] = context.STENCIL_BUFFER_BIT;
    GLEnumMap[JSAPAGLEnum.COLOR_BUFFER_BIT] = context.COLOR_BUFFER_BIT;


    GLEnumMap[JSAPAGLEnum.ZERO] = context.ZERO;
    GLEnumMap[JSAPAGLEnum.ONE] = context.ONE;
    GLEnumMap[JSAPAGLEnum.SRC_COLOR] = context.SRC_COLOR;
    GLEnumMap[JSAPAGLEnum.DST_COLOR] = context.DST_COLOR;
    GLEnumMap[JSAPAGLEnum.ONE_MINUS_SRC_COLOR] = context.ONE_MINUS_SRC_COLOR;
    GLEnumMap[JSAPAGLEnum.ONE_MINUS_DST_COLOR] = context.ONE_MINUS_DST_COLOR;
    GLEnumMap[JSAPAGLEnum.SRC_ALPHA] = context.SRC_ALPHA;
    GLEnumMap[JSAPAGLEnum.DST_ALPHA] = context.DST_ALPHA;
    GLEnumMap[JSAPAGLEnum.ONE_MINUS_DST_ALPHA] = context.ONE_MINUS_DST_ALPHA;
    GLEnumMap[JSAPAGLEnum.ONE_MINUS_SRC_ALPHA] = context.ONE_MINUS_SRC_ALPHA;
    GLEnumMap[JSAPAGLEnum.CONSTANT_COLOR] = context.CONSTANT_COLOR;
    GLEnumMap[JSAPAGLEnum.ONE_MINUS_CONSTANT_COLOR] = context.ONE_MINUS_CONSTANT_COLOR;
    GLEnumMap[JSAPAGLEnum.CONSTANT_ALPHA] = context.CONSTANT_ALPHA;
    GLEnumMap[JSAPAGLEnum.ONE_MINUS_CONSTANT_ALPHA] = context.ONE_MINUS_CONSTANT_ALPHA;
    GLEnumMap[JSAPAGLEnum.SRC_ALPHA_SATURATE] = context.SRC_ALPHA_SATURATE;
    GLEnumMap[JSAPAGLEnum.FUNC_ADD] = context.FUNC_ADD;
    GLEnumMap[JSAPAGLEnum.FUNC_SUBTRACT] = context.FUNC_SUBTRACT;
    GLEnumMap[JSAPAGLEnum.FUNC_REVERSE_SUBTRACT] = context.FUNC_REVERSE_SUBTRACT;


    GLEnumPrimitive[PrimitiveMode.POINT_LIST] = context.POINTS;
    GLEnumPrimitive[PrimitiveMode.LINE_LIST] = context.LINES;
    GLEnumPrimitive[PrimitiveMode.LINE_STRIP] = context.LINE_STRIP;
    GLEnumPrimitive[PrimitiveMode.LINE_LOOP] = context.LINE_LOOP;
    GLEnumPrimitive[PrimitiveMode.TRIANGLE_LIST] = context.TRIANGLES;
    GLEnumPrimitive[PrimitiveMode.TRIANGLE_STRIP] = context.TRIANGLE_STRIP;
    GLEnumPrimitive[PrimitiveMode.TRIANGLE_FAN] = context.TRIANGLE_FAN;

    GLStencilOpMap[StencilOp.KEEP] = context.KEEP;
    GLStencilOpMap[StencilOp.ZERO] = context.ZERO;
    GLStencilOpMap[StencilOp.REPLACE] = context.REPLACE;
    GLStencilOpMap[StencilOp.INCR] = context.INCR;
    GLStencilOpMap[StencilOp.INCR_WRAP] = context.INCR_WRAP;
    GLStencilOpMap[StencilOp.DECR] = context.DECR;
    GLStencilOpMap[StencilOp.DECR_WRAP] = context.DECR_WRAP;
    GLStencilOpMap[StencilOp.INVERT] = context.INVERT;

    GLComparisonFuncMap[ComparisonFunc.NEVER] = context.NEVER;
    GLComparisonFuncMap[ComparisonFunc.LESS] = context.LESS;
    GLComparisonFuncMap[ComparisonFunc.EQUAL] = context.EQUAL;
    GLComparisonFuncMap[ComparisonFunc.LESS_EQUAL] = context.LEQUAL;
    GLComparisonFuncMap[ComparisonFunc.GREATER] = context.GREATER;
    GLComparisonFuncMap[ComparisonFunc.NOT_EQUAL] = context.NOTEQUAL;
    GLComparisonFuncMap[ComparisonFunc.GREATER_EQUAL] = context.GEQUAL;
    GLComparisonFuncMap[ComparisonFunc.ALWAYS] = context.ALWAYS;
}

export function registerCommand(type: JSAPIMap, func: (device: WebGLDevice | WebGL2Device, byte: Byte) => void) {
    commandMap[type] = func;
}

export function runCommand(device: WebGLDevice | WebGL2Device, commandType: number, byte: Byte) {

    // console.info("js type ",commandType);
    commandMap[commandType](device, byte);
    /* debug:start */
    ++GameStat.graphicsAPICall;
    /* debug:end */
}

export function registerGFXFormatToWebGLType(func: GFXFormatToWebGLFunc) {
    GFXFormatToWebGLType = func;
}

export function registerGFXFormatToWebGLFormat(func: GFXFormatToWebGLFunc) {
    GFXFormatToWebGLFormat = func;
}

export function registerGFXFormatToWebGLInternalFormat(func: GFXFormatToWebGLFunc) {
    GFXFormatToInternalWebGLFormat = func;
}

