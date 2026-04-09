

import { GPUObject, GPUShader } from "../base/define";


export interface IGLGPUUniform {
    /**
     * 变量名字。调试使用
     */
    name?: string;
    glLoc: WebGLUniformLocation;
}

export interface IGLGPUAttribute {
    name?: string;
    glLoc: GLint;
}


export interface IGLGPUBuffer extends GPUObject {
    buffer: WebGLBuffer;
}

export interface IGLGPUShader extends GPUShader {
    glProgram: WebGLProgram;
    glUniforms: Array<IGLGPUUniform>;
    glAttributes: Array<IGLGPUAttribute>;
}

export interface IGLGPUTexture extends GPUObject {
    texture: WebGLTexture;
}

export interface IGLGPUFramebuffer extends GPUObject {
    frameBuffer: WebGLFramebuffer;
}

export interface IGLGPURenderbuffer extends GPUObject {
    renderBuffer: WebGLRenderbuffer
}

export interface IGLGPUVAO extends GPUObject {
    vao: WebGLVertexArrayObjectOES;
}

export interface IGSampler extends GPUObject {
    sampler: WebGLSampler;
}