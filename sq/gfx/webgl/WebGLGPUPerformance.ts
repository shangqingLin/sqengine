import type WebGLDevice from "./WebGLDevice";
import BaseGPUPerformance from "../webglbase/BaseGPUPerformance";

export default class WebGLGPUPerformance implements BaseGPUPerformance {

    //@ts-ignore
    private ext: EXT_disjoint_timer_query_webgl;
    private queryObj: WebGLQuery;
    private gl: WebGLRenderingContext;
    private querying: boolean = false;
    private queryingEnd: boolean = false;
    constructor(device: WebGLDevice) {
        //@ts-ignore
        this.ext = device.extensions.EXT_disjoint_timer_query_webgl;
        this.gl = device.gl;
        console.info(this.ext);
    }

    delete(): void {
        if (this.queryObj) {
            this.ext.deleteQueryEXT(this.queryObj);
            this.queryObj = null;
            this.querying = false;
            this.queryingEnd = false;

        }
    }

    public beginQuery() {

        if (this.querying) {
            this.pollQuery();
        };

        if (this.querying) return;

        this.querying = true;

        if (!this.queryObj) {
            this.queryObj = this.ext.createQueryEXT();
        }
        this.ext.beginQueryEXT(this.ext.TIME_ELAPSED_EXT, this.queryObj);

    }

    public endQuery() {
        if (!this.queryingEnd) {
            this.queryingEnd = true;
            this.ext.endQueryEXT(this.ext.TIME_ELAPSED_EXT);
        }
        this.pollQuery();
    }

    /**
    * 这个查询是异步的，不能是调用完endQuery就能立刻拿到结果的。
    */
    private pollQuery() {
        if (!this.querying || !this.queryingEnd) return;

        // 检查结果是否可用
        const available = this.ext.getQueryObjectEXT(this.queryObj, this.ext.QUERY_RESULT_AVAILABLE_EXT);

        // 检查是否有disjoint事件发生（计时失效）
        const disjoint = this.gl.getParameter(this.ext.GPU_DISJOINT_EXT);

        if (available) {
            let textContent;
            if (!disjoint) {
                // 获取查询结果（纳秒）
                const timeElapsed = this.ext.getQueryObjectEXT(this.queryObj, this.ext.QUERY_RESULT_EXT);
                let gpuTime = timeElapsed / 1000000; // 转换为毫秒
                textContent = `GPU绘制耗时: ${gpuTime.toFixed(3)} ms`;
            } else {
                textContent = '计时失效 (GPU Disjoint)，丢弃本次测量';
            }
            console.info(textContent);
        }
    }
}