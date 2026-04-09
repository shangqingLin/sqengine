import Device from "../base/Device";
import { API, GPUShader } from "../base/define";
import WasmByte from "../../native_binding/common/WasmByte";
import { initialize as initializeAPI, WebGL2CmdFuncCreateShader } from "./WebGL2API";
import { runCommand } from "./WebGL2Command";
import { IGLGPUShader } from "../webgl/WebGLGPUObjectDefine";
import { Byte } from "../../core";
import type { IWebGL2Extensions } from "./WenGL2Define";
import ProgramUtils from "../../scene/ProgramUtil";

/* debug:start */
import WebGL2GPUPerformance from "./WebGL2GPUPerformance";
import WebGLShaderDebug from "../webglbase/WebGLShaderDebug";
import WebGLDeviceRenderInfo from "../webglbase/WebGLDeviceRenderInfo";
/* debug:end */


function getExtension(gl: WebGL2RenderingContext, ext: string): any {
    const prefixes = ['', 'WEBKIT_', 'MOZ_'];
    for (let i = 0; i < prefixes.length; ++i) {
        const _ext = gl.getExtension(prefixes[i] + ext);
        if (_ext) {
            return _ext;
        }
    }
    return null;
}

function getExtensions(gl: WebGL2RenderingContext): IWebGL2Extensions {

    // https://developer.mozilla.org/en-US/docs/Web/API/WebGL_API

    const res: IWebGL2Extensions = {
        EXT_texture_filter_anisotropic: getExtension(gl, 'EXT_texture_filter_anisotropic'),
        EXT_color_buffer_half_float: getExtension(gl, 'EXT_color_buffer_half_float'),
        EXT_color_buffer_float: getExtension(gl, 'EXT_color_buffer_float'),
        EXT_float_blend: getExtension(gl, "EXT_float_blend"),
        EXT_disjoint_timer_query_webgl2: getExtension(gl, "EXT_disjoint_timer_query_webgl2"),
        WEBGL_compressed_texture_etc1: getExtension(gl, 'WEBGL_compressed_texture_etc1'),
        WEBGL_compressed_texture_etc: getExtension(gl, 'WEBGL_compressed_texture_etc'),
        WEBGL_compressed_texture_pvrtc: getExtension(gl, 'WEBGL_compressed_texture_pvrtc'),
        WEBGL_compressed_texture_astc: getExtension(gl, 'WEBGL_compressed_texture_astc'),
        WEBGL_compressed_texture_s3tc: getExtension(gl, 'WEBGL_compressed_texture_s3tc'),
        WEBGL_compressed_texture_s3tc_srgb: getExtension(gl, 'WEBGL_compressed_texture_s3tc_srgb'),
        WEBGL_debug_shaders: getExtension(gl, 'WEBGL_debug_shaders'),
        WEBGL_lose_context: getExtension(gl, 'WEBGL_lose_context'),
        WEBGL_debug_renderer_info: getExtension(gl, 'WEBGL_debug_renderer_info'),
        WEBGL_color_buffer_float: getExtension(gl, 'WEBGL_color_buffer_float'),
        OES_texture_half_float_linear: getExtension(gl, 'OES_texture_half_float_linear'),
        OES_texture_float_linear: getExtension(gl, 'OES_texture_float_linear'),
        useVAO: true,

        //是否支持着色器的并行编译，避免在触发编译着色器的时候阻塞主线程造成卡顿，现在可以通过此扩展实现异步编译不阻塞主线程
        PARALLEL_SHADER_COMPLIE: gl.getExtension("KHR_parallel_shader_compile")
    };
    return res;
}


function getContext(canvas: HTMLCanvasElement): WebGL2RenderingContext | null {
    let context: WebGL2RenderingContext | null = null;
    const webGLCtxAttribs: WebGLContextAttributes = {
        alpha: false,
        antialias: true,
        depth: true,
        stencil: true,
        premultipliedAlpha: false,
        preserveDrawingBuffer: false,
        powerPreference: 'default',
        failIfMajorPerformanceCaveat: false,
    };
    context = canvas.getContext('webgl2', webGLCtxAttribs);
    return context;
}


function initState(gl: WebGL2RenderingContext) {
    gl.pixelStorei(gl.PACK_ALIGNMENT, 1);

    //OpenGL要求所有的纹理都是4字节对齐的，即纹理的大小永远是4字节的倍数
    gl.pixelStorei(gl.UNPACK_ALIGNMENT, 1);

    gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, false);

    gl.enable(gl.SCISSOR_TEST);
    gl.frontFace(gl.CCW);
    gl.disable(gl.POLYGON_OFFSET_FILL);
    gl.polygonOffset(0.0, 0.0);

    gl.disable(gl.DEPTH_TEST);

    gl.disable(gl.STENCIL_TEST);
    gl.disable(gl.SAMPLE_ALPHA_TO_COVERAGE);

    //设置Blend的默认值。默认不开启，各自的材质决定是否开启
    gl.disable(gl.BLEND);
    gl.blendEquationSeparate(gl.FUNC_ADD, gl.FUNC_ADD);
    gl.blendFuncSeparate(gl.SRC_ALPHA, gl.ONE_MINUS_SRC_ALPHA, gl.SRC_ALPHA, gl.ONE_MINUS_SRC_ALPHA);
    gl.blendColor(0.0, 0.0, 0.0, 0.0);

    gl.colorMask(true, true, true, true);
    gl.clearColor(1.0, 1.0, 1.0, 1.0);
}

export default class WebGL2Device extends Device {
    private native: Module.WebGL2Device;
    private context: WebGL2RenderingContext | null = null;
    private _extensions: IWebGL2Extensions;

    /* debug:start */
    private gpuPerfomace: WebGL2GPUPerformance;
    private shaderDebug: WebGLShaderDebug;
    /* debug:end */

    constructor() {
        super();
        this._gfxAPI = API.WEBGL2;
        this.native = new window.Module.WebGL2Device();
    }

    override initialize(canvas: HTMLCanvasElement): void {
        this.canvas = canvas;
        this.context = getContext(this.canvas);
        if (!this.context) throw Error("WebGL2.0 Create Fail");
        let gl = this.context;

        // 进一步检查渲染器信息（某些设备会伪装支持 WebGL 2.0）
        console.log("Renderer:", gl.getParameter(gl.RENDERER));
        console.log("Version:", gl.getParameter(gl.VERSION));
        console.log('着色器版本:', gl.getParameter(gl.SHADING_LANGUAGE_VERSION));

        const exts = getExtensions(gl);
        this._extensions = exts;
        console.info(exts);

        initState(gl);
        initializeAPI(this);
        this.initializeCapability();
        this.initMarcos();

        /* debug:start */
        // if(this.extensions.EXT_disjoint_timer_query_webgl2) this.gpuPerfomace = new WebGL2GPUPerformance(this);
        // this.shaderDebug = new WebGLShaderDebug(this);
        new WebGLDeviceRenderInfo(this).print();
        /* debug:end */
    }


    private initializeCapability() {

        let gl = this.context;
        let caps = this.caps;
        caps.maxVertexAttributes = gl.getParameter(gl.MAX_VERTEX_ATTRIBS);
        caps.maxVertexUniformVectors = gl.getParameter(gl.MAX_VERTEX_UNIFORM_VECTORS);
        caps.maxFragmentUniformVectors = gl.getParameter(gl.MAX_FRAGMENT_UNIFORM_VECTORS);
        caps.maxTextureUnits = gl.getParameter(gl.MAX_TEXTURE_IMAGE_UNITS);
        caps.maxVertexTextureUnits = gl.getParameter(gl.MAX_VERTEX_TEXTURE_IMAGE_UNITS);
        caps.maxTextureSize = gl.getParameter(gl.MAX_TEXTURE_SIZE);
        caps.maxCubeMapTextureSize = gl.getParameter(gl.MAX_CUBE_MAP_TEXTURE_SIZE);
        caps.support16FloatTexture = true;
        caps.support32FloatTexture = true;
        caps.support32FloatRenderTexture = !!(this.extensions.EXT_color_buffer_float || this.extensions.WEBGL_color_buffer_float);
        caps.support16FloatRenderTexture = !!(this.extensions.EXT_color_buffer_half_float || this.extensions.WEBGL_color_buffer_float);
        caps.supportBlendMaxMinEquation = true;
        caps.supportBlendWith32FloatRenderTexture = !!(this.extensions.EXT_float_blend);

        // console.info(caps);

        let bufferSize: number = 4;
        let buffer = window._malloc(bufferSize);
        let byte = new Byte();
        byte.setData(window.Module.HEAPU8.subarray(buffer, buffer + bufferSize));
        byte.writeByte(1);
        byte.writeByte(1);
        byte.writeByte(1);
        byte.writeByte(0);

        this.native.initializeCapability(buffer);
        window._free(buffer);
    }

    private initMarcos() {
        ProgramUtils.registerBuildInMacros("#version", "300 es", 1);
        ProgramUtils.registerBuildInMacros("WEBGL2", 1, 1);
    }

    get gl(): WebGL2RenderingContext {
        return this.context!;
    }

    get extensions(): IWebGL2Extensions {
        return this._extensions;
    }

    override createShader(program: number, effectId: number, shaderIndex: number, macroFlags: number): GPUShader {
        let shader: IGLGPUShader = Object.create(null);
        // this.addGpuObject(program, shader);
        this.addShaderTmp(program,shader);
        WebGL2CmdFuncCreateShader(this, effectId, shaderIndex, shader, macroFlags);
        return shader;
    }

    override runCommand(commadType: number, byte: Byte): void {

        /* debug:start */
        if (this.gpuPerfomace) {
            this.gpuPerfomace.beginQuery();
        }
        /* debug:end */

        runCommand(this, commadType, byte);

        /* debug:start */
        if (this.gpuPerfomace) {
            this.gpuPerfomace.endQuery();
        }
        /* debug:end */
    }
}