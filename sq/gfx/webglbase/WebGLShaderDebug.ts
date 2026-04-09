import type WebGLDevice from "../webgl/WebGLDevice";
import type WebGL2Device from "../webgl2/WebGL2Device";

export default class WebGLShaderDebug {

    private shaderDebug: WEBGL_debug_shaders;
    private gl: WebGL2RenderingContext | WebGLRenderingContext;
    constructor(deivce: WebGLDevice | WebGL2Device) {
        this.shaderDebug = deivce.extensions.WEBGL_debug_shaders;
        this.gl = deivce.gl;
    }

    printShaderCode(shader: WebGLShader): void {
        const translatedSource = this.shaderDebug.getTranslatedShaderSource(shader);
        console.log('翻译后的着色器代码:');
        console.log(translatedSource);
    }
}