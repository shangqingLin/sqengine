import WebGLDevice from "./WebGLDevice";
import { IGLGPUShader, IGLGPUVAO } from "./WebGLGPUObjectDefine";

/* debug:start */
import GameStat from "../../profiler/GameStat";
/* debug:end */

import { GLEnumMap, GLEnumPrimitive, initializeBaseAPI, registerCommand, registerGFXFormatToWebGLFormat, registerGFXFormatToWebGLInternalFormat, registerGFXFormatToWebGLType } from "../webglbase/BaseWebGLAPI";
import { JSAPAGLEnum, JSAPIMap } from "../webglbase/WebGLAPIDefine";
import { Format } from "../base/define";
import { WebGLEXT } from "./WebGLDefine";
import { Byte } from "../../core";

function GFXFormatToWebGLType(format: Format, gl: WebGLRenderingContext): GLenum {
    switch (format) {
        case Format.R8: return gl.UNSIGNED_BYTE;
        case Format.R8SN: return gl.BYTE;
        case Format.R8UI: return gl.UNSIGNED_BYTE;
        case Format.R8I: return gl.BYTE;
        case Format.A8: return gl.UNSIGNED_BYTE;
        // case Format.R16F: return WebGLEXT.HALF_FLOAT_OES;
        case Format.R16UI: return gl.UNSIGNED_SHORT;
        case Format.R16I: return gl.SHORT;
        case Format.R32F: return gl.FLOAT;
        case Format.R32UI: return gl.UNSIGNED_INT;
        case Format.R32I: return gl.INT;

        case Format.RG8: return gl.UNSIGNED_BYTE;
        case Format.RG8SN: return gl.BYTE;
        case Format.RG8UI: return gl.UNSIGNED_BYTE;
        case Format.RG8I: return gl.BYTE;
        // case Format.RG16F: return WebGLEXT.HALF_FLOAT_OES;
        case Format.RG16UI: return gl.UNSIGNED_SHORT;
        case Format.RG16I: return gl.SHORT;
        case Format.RG32F: return gl.FLOAT;
        case Format.RG32UI: return gl.UNSIGNED_INT;
        case Format.RG32I: return gl.INT;

        case Format.RGB8: return gl.UNSIGNED_BYTE;
        case Format.SRGB8: return gl.UNSIGNED_BYTE;
        case Format.RGB8SN: return gl.BYTE;
        case Format.RGB8UI: return gl.UNSIGNED_BYTE;
        case Format.RGB8I: return gl.BYTE;
        // case Format.RGB16F: return WebGLEXT.HALF_FLOAT_OES;
        case Format.RGB16UI: return gl.UNSIGNED_SHORT;
        case Format.RGB16I: return gl.SHORT;
        case Format.RGB32F: return gl.FLOAT;
        case Format.RGB32UI: return gl.UNSIGNED_INT;
        case Format.RGB32I: return gl.INT;

        case Format.BGRA8: return gl.UNSIGNED_BYTE;
        case Format.RGBA8: return gl.UNSIGNED_BYTE;
        case Format.SRGB8_A8: return gl.UNSIGNED_BYTE;
        case Format.RGBA8SN: return gl.BYTE;
        case Format.RGBA8UI: return gl.UNSIGNED_BYTE;
        case Format.RGBA8I: return gl.BYTE;
        // case Format.RGBA16F: return WebGLEXT.HALF_FLOAT_OES;
        case Format.RGBA16UI: return gl.UNSIGNED_SHORT;
        case Format.RGBA16I: return gl.SHORT;
        case Format.RGBA32F: return gl.FLOAT;
        case Format.RGBA32UI: return gl.UNSIGNED_INT;
        case Format.RGBA32I: return gl.INT;

        case Format.R5G6B5: return gl.UNSIGNED_SHORT_5_6_5;
        case Format.R11G11B10F: return gl.FLOAT;
        case Format.RGB5A1: return gl.UNSIGNED_SHORT_5_5_5_1;
        case Format.RGBA4: return gl.UNSIGNED_SHORT_4_4_4_4;
        case Format.RGB10A2: return gl.UNSIGNED_BYTE;
        case Format.RGB10A2UI: return gl.UNSIGNED_INT;
        case Format.RGB9E5: return gl.UNSIGNED_BYTE;

        case Format.DEPTH: return gl.UNSIGNED_INT;
        // case Format.DEPTH_STENCIL: return WebGLEXT.UNSIGNED_INT_24_8_WEBGL;

        case Format.BC1: return gl.UNSIGNED_BYTE;
        case Format.BC1_SRGB: return gl.UNSIGNED_BYTE;
        case Format.BC2: return gl.UNSIGNED_BYTE;
        case Format.BC2_SRGB: return gl.UNSIGNED_BYTE;
        case Format.BC3: return gl.UNSIGNED_BYTE;
        case Format.BC3_SRGB: return gl.UNSIGNED_BYTE;
        case Format.BC4: return gl.UNSIGNED_BYTE;
        case Format.BC4_SNORM: return gl.BYTE;
        case Format.BC5: return gl.UNSIGNED_BYTE;
        case Format.BC5_SNORM: return gl.BYTE;
        case Format.BC6H_SF16: return gl.FLOAT;
        case Format.BC6H_UF16: return gl.FLOAT;
        case Format.BC7: return gl.UNSIGNED_BYTE;
        case Format.BC7_SRGB: return gl.UNSIGNED_BYTE;

        case Format.ETC_RGB8: return gl.UNSIGNED_BYTE;
        case Format.ETC2_RGB8: return gl.UNSIGNED_BYTE;
        case Format.ETC2_SRGB8: return gl.UNSIGNED_BYTE;
        case Format.ETC2_RGB8_A1: return gl.UNSIGNED_BYTE;
        case Format.ETC2_SRGB8_A1: return gl.UNSIGNED_BYTE;
        case Format.EAC_R11: return gl.UNSIGNED_BYTE;
        case Format.EAC_R11SN: return gl.BYTE;
        case Format.EAC_RG11: return gl.UNSIGNED_BYTE;
        case Format.EAC_RG11SN: return gl.BYTE;

        case Format.PVRTC_RGB2: return gl.UNSIGNED_BYTE;
        case Format.PVRTC_RGBA2: return gl.UNSIGNED_BYTE;
        case Format.PVRTC_RGB4: return gl.UNSIGNED_BYTE;
        case Format.PVRTC_RGBA4: return gl.UNSIGNED_BYTE;
        case Format.PVRTC2_2BPP: return gl.UNSIGNED_BYTE;
        case Format.PVRTC2_4BPP: return gl.UNSIGNED_BYTE;

        case Format.ASTC_RGBA_4X4:
        case Format.ASTC_RGBA_5X4:
        case Format.ASTC_RGBA_5X5:
        case Format.ASTC_RGBA_6X5:
        case Format.ASTC_RGBA_6X6:
        case Format.ASTC_RGBA_8X5:
        case Format.ASTC_RGBA_8X6:
        case Format.ASTC_RGBA_8X8:
        case Format.ASTC_RGBA_10X5:
        case Format.ASTC_RGBA_10X6:
        case Format.ASTC_RGBA_10X8:
        case Format.ASTC_RGBA_10X10:
        case Format.ASTC_RGBA_12X10:
        case Format.ASTC_RGBA_12X12:
        case Format.ASTC_SRGBA_4X4:
        case Format.ASTC_SRGBA_5X4:
        case Format.ASTC_SRGBA_5X5:
        case Format.ASTC_SRGBA_6X5:
        case Format.ASTC_SRGBA_6X6:
        case Format.ASTC_SRGBA_8X5:
        case Format.ASTC_SRGBA_8X6:
        case Format.ASTC_SRGBA_8X8:
        case Format.ASTC_SRGBA_10X5:
        case Format.ASTC_SRGBA_10X6:
        case Format.ASTC_SRGBA_10X8:
        case Format.ASTC_SRGBA_10X10:
        case Format.ASTC_SRGBA_12X10:
        case Format.ASTC_SRGBA_12X12:
            return gl.UNSIGNED_BYTE;
        default: {
            /* debug:start */
            console.warn("不支持数据格式 " + format + ",转换为UNSIGNED_BYTE");
            /* debug:end */
            return gl.UNSIGNED_BYTE;
        }
    }
}

function GFXFormatToWebGLFormat(format: Format, gl: WebGLRenderingContext): GLenum {
    switch (format) {
        case Format.A8: return gl.ALPHA;
        case Format.L8: return gl.LUMINANCE;
        case Format.LA8: return gl.LUMINANCE_ALPHA;
        case Format.RGB8: return gl.RGB;
        case Format.RGB16F: return gl.RGB;
        case Format.RGB32F: return gl.RGB;
        case Format.BGRA8: return gl.RGBA;
        case Format.RGBA8: return gl.RGBA;
        case Format.SRGB8_A8: return gl.RGBA;
        case Format.RGBA16F: return gl.RGBA;
        case Format.RGBA32F: return gl.RGBA;
        case Format.R5G6B5: return gl.RGB;
        case Format.RGB5A1: return gl.RGBA;
        case Format.RGBA4: return gl.RGBA;
        case Format.DEPTH: return gl.DEPTH_COMPONENT;
        case Format.DEPTH_STENCIL: return gl.DEPTH_STENCIL;
        // case Format.BC1: return WebGLEXT.COMPRESSED_RGB_S3TC_DXT1_EXT;
        // case Format.BC1_ALPHA: return WebGLEXT.COMPRESSED_RGBA_S3TC_DXT1_EXT;
        // case Format.BC1_SRGB: return WebGLEXT.COMPRESSED_SRGB_S3TC_DXT1_EXT;
        // case Format.BC1_SRGB_ALPHA: return WebGLEXT.COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT;
        // case Format.BC2: return WebGLEXT.COMPRESSED_RGBA_S3TC_DXT3_EXT;
        // case Format.BC2_SRGB: return WebGLEXT.COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT;
        // case Format.BC3: return WebGLEXT.COMPRESSED_RGBA_S3TC_DXT5_EXT;
        // case Format.BC3_SRGB: return WebGLEXT.COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT;

        // case Format.ETC_RGB8: return WebGLEXT.COMPRESSED_RGB_ETC1_WEBGL;
        // case Format.ETC2_RGB8: return WebGLEXT.COMPRESSED_RGB8_ETC2;
        // case Format.ETC2_SRGB8: return WebGLEXT.COMPRESSED_SRGB8_ETC2;
        // case Format.ETC2_RGB8_A1: return WebGLEXT.COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2;
        // case Format.ETC2_SRGB8_A1: return WebGLEXT.COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2;
        // case Format.ETC2_RGBA8: return WebGLEXT.COMPRESSED_RGBA8_ETC2_EAC;
        // case Format.ETC2_SRGB8_A8: return WebGLEXT.COMPRESSED_SRGB8_ALPHA8_ETC2_EAC;
        // case Format.EAC_R11: return WebGLEXT.COMPRESSED_R11_EAC;
        // case Format.EAC_R11SN: return WebGLEXT.COMPRESSED_SIGNED_R11_EAC;
        // case Format.EAC_RG11: return WebGLEXT.COMPRESSED_RG11_EAC;
        // case Format.EAC_RG11SN: return WebGLEXT.COMPRESSED_SIGNED_RG11_EAC;

        // case Format.PVRTC_RGB2: return WebGLEXT.COMPRESSED_RGB_PVRTC_2BPPV1_IMG;
        // case Format.PVRTC_RGBA2: return WebGLEXT.COMPRESSED_RGBA_PVRTC_2BPPV1_IMG;
        // case Format.PVRTC_RGB4: return WebGLEXT.COMPRESSED_RGB_PVRTC_4BPPV1_IMG;
        // case Format.PVRTC_RGBA4: return WebGLEXT.COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;

        // case Format.ASTC_RGBA_4X4: return WebGLEXT.COMPRESSED_RGBA_ASTC_4x4_KHR;
        // case Format.ASTC_RGBA_5X4: return WebGLEXT.COMPRESSED_RGBA_ASTC_5x4_KHR;
        // case Format.ASTC_RGBA_5X5: return WebGLEXT.COMPRESSED_RGBA_ASTC_5x5_KHR;
        // case Format.ASTC_RGBA_6X5: return WebGLEXT.COMPRESSED_RGBA_ASTC_6x5_KHR;
        // case Format.ASTC_RGBA_6X6: return WebGLEXT.COMPRESSED_RGBA_ASTC_6x6_KHR;
        // case Format.ASTC_RGBA_8X5: return WebGLEXT.COMPRESSED_RGBA_ASTC_8x5_KHR;
        // case Format.ASTC_RGBA_8X6: return WebGLEXT.COMPRESSED_RGBA_ASTC_8x6_KHR;
        // case Format.ASTC_RGBA_8X8: return WebGLEXT.COMPRESSED_RGBA_ASTC_8x8_KHR;
        // case Format.ASTC_RGBA_10X5: return WebGLEXT.COMPRESSED_RGBA_ASTC_10x5_KHR;
        // case Format.ASTC_RGBA_10X6: return WebGLEXT.COMPRESSED_RGBA_ASTC_10x6_KHR;
        // case Format.ASTC_RGBA_10X8: return WebGLEXT.COMPRESSED_RGBA_ASTC_10x8_KHR;
        // case Format.ASTC_RGBA_10X10: return WebGLEXT.COMPRESSED_RGBA_ASTC_10x10_KHR;
        // case Format.ASTC_RGBA_12X10: return WebGLEXT.COMPRESSED_RGBA_ASTC_12x10_KHR;
        // case Format.ASTC_RGBA_12X12: return WebGLEXT.COMPRESSED_RGBA_ASTC_12x12_KHR;

        // case Format.ASTC_SRGBA_4X4: return WebGLEXT.COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR;
        // case Format.ASTC_SRGBA_5X4: return WebGLEXT.COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR;
        // case Format.ASTC_SRGBA_5X5: return WebGLEXT.COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR;
        // case Format.ASTC_SRGBA_6X5: return WebGLEXT.COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR;
        // case Format.ASTC_SRGBA_6X6: return WebGLEXT.COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR;
        // case Format.ASTC_SRGBA_8X5: return WebGLEXT.COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR;
        // case Format.ASTC_SRGBA_8X6: return WebGLEXT.COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR;
        // case Format.ASTC_SRGBA_8X8: return WebGLEXT.COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR;
        // case Format.ASTC_SRGBA_10X5: return WebGLEXT.COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR;
        // case Format.ASTC_SRGBA_10X6: return WebGLEXT.COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR;
        // case Format.ASTC_SRGBA_10X8: return WebGLEXT.COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR;
        // case Format.ASTC_SRGBA_10X10: return WebGLEXT.COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR;
        // case Format.ASTC_SRGBA_12X10: return WebGLEXT.COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR;
        // case Format.ASTC_SRGBA_12X12: return WebGLEXT.COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR;

        default: {
            /* debug:start */
            console.warn("不支持纹理格式 " + format + ",转换为RGBA");
            /* debug:end */
            return gl.RGBA;
        }
    }
}

export function GFXFormatToWebGLInternalFormat(format: Format, gl: WebGLRenderingContext): GLenum {
    switch (format) {
        case Format.R5G6B5: return gl.RGB565;
        case Format.RGB5A1: return gl.RGB5_A1;
        case Format.RGBA4: return gl.RGBA4;
        case Format.RGBA16F: return WebGLEXT.RGBA16F_EXT;
        case Format.RGBA32F: return gl.RGBA;// WebGLEXT.RGBA32F_EXT;
        case Format.SRGB8_A8: return WebGLEXT.SRGB8_ALPHA8_EXT;
        case Format.DEPTH: return gl.DEPTH_COMPONENT16;
        case Format.DEPTH_STENCIL: return gl.DEPTH_STENCIL;
        case Format.RGBA8 : return gl.RGBA;
        default: {
            /* debug:start */
            console.warn("不支持纹理格式 " + format + ",转换为RGBA");
            /* debug:end */
            return gl.RGBA;
        }
    }
}


function vertexAttribDivisorANGLE(device: WebGLDevice, byte: Byte) {
    let shader: IGLGPUShader = device.getGpuObject(device.GPUStateCache.glProgram) as IGLGPUShader;
    let location = byte.readUint16();
    let stride = byte.readInt32();
    device.extensions.ANGLE_instanced_arrays.vertexAttribDivisorANGLE(shader.glAttributes[location].glLoc, stride);

    // console.info("run vertexAttribDivisorANGLE ",location,stride);
}

function drawElementsInstancedANGLE(device: WebGLDevice, byte: Byte) {
    let primitive = byte.readUint32();
    let indexCount = byte.readInt32();
    let type = byte.readUint32();
    let offset = byte.readUint32();
    let instanceCount = byte.readInt32();
    device.extensions.ANGLE_instanced_arrays.drawElementsInstancedANGLE(GLEnumPrimitive[primitive], indexCount, GLEnumMap[type], offset, instanceCount);
    /* debug:start */
    ++GameStat.instanceDrawCall;
    /* debug:end */
}

function createVertexArrayOES(device: WebGLDevice, byte: Byte) {
    let id = byte.readInt32();
    let obj: IGLGPUVAO = Object.create(null);
    obj.vao = device.extensions.OES_vertex_array_object.createVertexArrayOES();
    device.addGpuObject(id, obj);
}

function bindVertexArrayOES(device: WebGLDevice, byte: Byte) {
    let vaoId = byte.readUint32();
    if (vaoId === 0) {
        device.extensions.OES_vertex_array_object.bindVertexArrayOES(null);
    } else {
        let obj: IGLGPUVAO = device.getGpuObject(vaoId) as IGLGPUVAO;
        device.extensions.OES_vertex_array_object.bindVertexArrayOES(obj.vao);
    }
    // console.info("bind vao",vaoId);
}

function deleteVertexArrayOES(device: WebGLDevice, byte: Byte) {
    let vaoId = byte.readUint32();
    let obj: IGLGPUVAO = device.getGpuObject(vaoId) as IGLGPUVAO;
    device.extensions.OES_vertex_array_object.deleteVertexArrayOES(obj.vao);
    device.deleteGpuObject(vaoId);
}

function drawBuffers(device: WebGLDevice, byte: Byte) {
    let num = byte.readInt32();
    let attachments = new Array(num);
    for (let i = 0; i < num; ++i) {
        attachments[i] = device.gl.COLOR_ATTACHMENT0 + i;
    }
    device.extensions.WEBGL_draw_buffers.drawBuffersWEBGL(attachments);
}

export function initialize(device: WebGLDevice) {
    initializeBaseAPI(device);

    if (device.extensions.EXT_blend_minmax) {
        GLEnumMap[JSAPAGLEnum.MIN] = device.extensions.EXT_blend_minmax.MIN_EXT;
        GLEnumMap[JSAPAGLEnum.MAX] = device.extensions.EXT_blend_minmax.MAX_EXT;
    }

    registerCommand(JSAPIMap.vertexAttribDivisor, vertexAttribDivisorANGLE);
    registerCommand(JSAPIMap.drawElementsInstancedANGLE, drawElementsInstancedANGLE);
    registerCommand(JSAPIMap.createVertexArrayOES, createVertexArrayOES);
    registerCommand(JSAPIMap.bindVertexArrayOES, bindVertexArrayOES);
    registerCommand(JSAPIMap.deleteVertexArrayOES, deleteVertexArrayOES);
    registerCommand(JSAPIMap.drawBuffers, drawBuffers);
    registerGFXFormatToWebGLFormat(GFXFormatToWebGLFormat);
    registerGFXFormatToWebGLType(GFXFormatToWebGLType);
    registerGFXFormatToWebGLInternalFormat(GFXFormatToWebGLInternalFormat);
}


