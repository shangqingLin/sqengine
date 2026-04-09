import Device from "../base/Device";
import { IWebGLExtensions } from "script/engine/sq/gfx/webgl/WebGLDefine"
import { WebGLCmdFuncCreateShader, runCommand } from "./WebGLCommand";
import { API } from "../base/define";
import { initialize as initializeAPI } from "./WebGLAPI";
import { IGLGPUShader } from "./WebGLGPUObjectDefine";
import { Byte } from "../../core/index";
import ProgramUtils from "../../scene/ProgramUtil";

/* debug:start */
import WebGLGPUPerformance from "./WebGLGPUPerformance";
import WebGLShaderDebug from "../webglbase/WebGLShaderDebug";
import WebGLDeviceRenderInfo from "../webglbase/WebGLDeviceRenderInfo";
/* debug:end */

function getContext(canvas: HTMLCanvasElement): WebGLRenderingContext | null {
    let context: WebGLRenderingContext | null = null;
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
    context = canvas.getContext('webgl', webGLCtxAttribs);
    // console.info(context);
    return context;
}



function getExtension(gl: WebGLRenderingContext, ext: string): any {
    const prefixes = ['', 'WEBKIT_', 'MOZ_'];
    for (let i = 0; i < prefixes.length; ++i) {
        const _ext = gl.getExtension(prefixes[i] + ext);
        if (_ext) {
            return _ext;
        }
    }
    return null;
}


function getExtensions(gl: WebGLRenderingContext): IWebGLExtensions {

    // https://developer.mozilla.org/en-US/docs/Web/API/WebGL_API

    const res: IWebGLExtensions = {
        EXT_texture_filter_anisotropic: getExtension(gl, 'EXT_texture_filter_anisotropic'),
        EXT_blend_minmax: getExtension(gl, 'EXT_blend_minmax'),
        EXT_float_blend: getExtension(gl, "EXT_float_blend"),
        EXT_frag_depth: getExtension(gl, 'EXT_frag_depth'),
        EXT_shader_texture_lod: getExtension(gl, 'EXT_shader_texture_lod'),
        EXT_sRGB: getExtension(gl, 'EXT_sRGB'),
        EXT_color_buffer_float: getExtension(gl, "EXT_color_buffer_float"),
        EXT_color_buffer_half_float: getExtension(gl, "EXT_color_buffer_half_float"),
        EXT_disjoint_timer_query_webgl: getExtension(gl, "EXT_disjoint_timer_query_webgl"),
        OES_vertex_array_object: getExtension(gl, 'OES_vertex_array_object'),
        WEBGL_color_buffer_float: getExtension(gl, 'WEBGL_color_buffer_float'),
        WEBGL_compressed_texture_etc1: getExtension(gl, 'WEBGL_compressed_texture_etc1'),
        WEBGL_compressed_texture_etc: getExtension(gl, 'WEBGL_compressed_texture_etc'),
        WEBGL_compressed_texture_pvrtc: getExtension(gl, 'WEBGL_compressed_texture_pvrtc'),
        WEBGL_compressed_texture_s3tc: getExtension(gl, 'WEBGL_compressed_texture_s3tc'),
        WEBGL_compressed_texture_s3tc_srgb: getExtension(gl, 'WEBGL_compressed_texture_s3tc_srgb'),
        WEBGL_debug_shaders: getExtension(gl, 'WEBGL_debug_shaders'),

        //启用FBO支持多ColorAttachment
        WEBGL_draw_buffers: getExtension(gl, 'WEBGL_draw_buffers'),
        WEBGL_lose_context: getExtension(gl, 'WEBGL_lose_context'),
        WEBGL_depth_texture: getExtension(gl, 'WEBGL_depth_texture'),

        OES_texture_half_float: getExtension(gl, 'OES_texture_half_float'),
        OES_texture_half_float_linear: getExtension(gl, 'OES_texture_half_float_linear'),
        OES_texture_float: getExtension(gl, 'OES_texture_float'),
        OES_texture_float_linear: getExtension(gl, 'OES_texture_float_linear'),
        OES_standard_derivatives: getExtension(gl, 'OES_standard_derivatives'),
        OES_element_index_uint: getExtension(gl, 'OES_element_index_uint'),
        ANGLE_instanced_arrays: getExtension(gl, 'ANGLE_instanced_arrays'),
        WEBGL_debug_renderer_info: getExtension(gl, 'WEBGL_debug_renderer_info'),
        WEBGL_multi_draw: null,
        WEBGL_compressed_texture_astc: null,
        destroyShadersImmediately: true,
        noCompressedTexSubImage2D: false,
        PARALLEL_SHADER_COMPLIE: gl.getExtension("KHR_parallel_shader_compile"),
        isLocationActive: (glLoc: unknown): glLoc is WebGLUniformLocation => !!glLoc,
        useVAO: false,
    };
    return res;
}

function initState(gl: WebGLRenderingContext) {
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

    //设置Blend的默认值
    gl.disable(gl.BLEND);
    gl.blendEquationSeparate(gl.FUNC_ADD, gl.FUNC_ADD);
    gl.blendFuncSeparate(gl.SRC_ALPHA, gl.ONE_MINUS_SRC_ALPHA, gl.SRC_ALPHA, gl.ONE_MINUS_SRC_ALPHA);
    gl.blendColor(0.0, 0.0, 0.0, 0.0);

    gl.colorMask(true, true, true, true);
    gl.clearColor(0.0, 0.0, 0.0, 0.0);
}

export default class WebGLDevice extends Device {

    private context: WebGLRenderingContext | null = null;

    private native: Module.WebGLDevice;

    private _extensions: IWebGLExtensions;

    /* debug:start */
    private gpuPerfomace: WebGLGPUPerformance;
    private shaderDebug: WebGLShaderDebug;
    /* debug:end */

    constructor() {
        super();
        this._gfxAPI = API.WEBGL;
        this.native = new window.Module.WebGLDevice();
    }

    get gl(): WebGLRenderingContext {
        return this.context!;
    }

    get extensions(): IWebGLExtensions {
        return this._extensions;
    }

    override initialize(canvas: HTMLCanvasElement): void {
        this.canvas = canvas;
        this.context = getContext(this.canvas);
        if (!this.context) throw Error("WebGL1.0 Create Fail");
        let gl = this.context;
        console.log("Renderer:", gl.getParameter(gl.RENDERER));
        console.log("Version:", gl.getParameter(gl.VERSION));
        console.log('着色器版本:', gl.getParameter(gl.SHADING_LANGUAGE_VERSION));

        initState(gl);

        this._extensions = getExtensions(gl);
        console.info(this._extensions);

        initializeAPI(this);
        this.initializeCapability();
        this.initMarcos();

        /* debug:start */
        if (this._extensions.EXT_disjoint_timer_query_webgl) this.gpuPerfomace = new WebGLGPUPerformance(this);
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
        caps.support16FloatTexture = !!(this.extensions.OES_texture_float);
        caps.support32FloatTexture = !!(this.extensions.OES_texture_half_float);
        caps.support32FloatRenderTexture = !!(this.extensions.EXT_color_buffer_float || this.extensions.WEBGL_color_buffer_float);
        caps.support16FloatRenderTexture = !!(this.extensions.EXT_color_buffer_half_float || this.extensions.WEBGL_color_buffer_float);
        caps.supportBlendMaxMinEquation = !!(this.extensions.EXT_blend_minmax);
        caps.supportBlendWith32FloatRenderTexture = !!(this.extensions.EXT_float_blend);


        let bufferSize: number = 4;
        let buffer = window._malloc(bufferSize);
        let byte = new Byte();
        byte.setData(window.Module.HEAPU8.subarray(buffer, buffer + bufferSize));

        //是否支持浮点数的纹理和浮点数纹理的线性采用，确保正读取浮点数纹理
        let supportFloatTexture = this._extensions.OES_texture_float && this.extensions.OES_texture_float_linear;

        //顶点着色器是否支持读取纹理
        let supportVertexShaderTexture = !!this._extensions.EXT_shader_texture_lod;

        //是否支持VAO    
        let supportVAO = !!this._extensions.OES_vertex_array_object;

        byte.writeByte(supportFloatTexture ? 1 : 0);
        byte.writeByte(supportVertexShaderTexture ? 1 : 0);
        byte.writeByte(supportVAO ? 1 : 0);
        byte.writeByte(this._extensions.WEBGL_depth_texture ? 1 : 0);
        this.native.initializeCapability(buffer);
        window._free(buffer);
    }

    private initMarcos() {
        ProgramUtils.registerBuildInMacros("#version", "100", 1);
        ProgramUtils.registerBuildInMacros("WEBGL1", 1, 1);
        ProgramUtils.registerBuildInMacros("texture", "texture2D", 1);
        ProgramUtils.registerBuildInMacros("in", "attribute", 2);
        ProgramUtils.registerBuildInMacros("out", "varying", 2);
        ProgramUtils.registerBuildInMacros("in", "varying", 3);
    }

    override createShader(program: number, effectId: number, shaderIndex: number, macroFlags: number): IGLGPUShader {
        let shader: IGLGPUShader = Object.create(null);
        // this.addGpuObject(program, shader);
        this.addShaderTmp(program, shader);
        WebGLCmdFuncCreateShader(this, effectId, shaderIndex, shader, macroFlags);
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

