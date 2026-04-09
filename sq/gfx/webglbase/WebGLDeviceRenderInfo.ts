import type WebGLDevice from "../webgl/WebGLDevice";
import type WebGL2Device from "../webgl2/WebGL2Device";

/**
 * 用来获取GPU的一些信息，我们可以通过信息来调试、故障诊断或根本不同的设备情况调整画面质量和性能之间的平衡
 */
export default class WebGLDeviceRenderInfo {

    private renderInfo: WEBGL_debug_renderer_info;
    private gl: WebGL2RenderingContext | WebGLRenderingContext;
    constructor(deivce: WebGLDevice | WebGL2Device) {
        this.renderInfo = deivce.extensions.WEBGL_debug_renderer_info;
        this.gl = deivce.gl;
    }

    print() {

        // 1. 渲染器字符串（通常是GPU型号）
        const renderer = this.gl.getParameter(this.renderInfo.UNMASKED_RENDERER_WEBGL);
        console.log('GPU渲染器:', renderer);

        // 2. 供应商字符串（GPU厂商）
        const vendor = this.gl.getParameter(this.renderInfo.UNMASKED_VENDOR_WEBGL);
        console.log('GPU供应商:', vendor);

        
    }
}