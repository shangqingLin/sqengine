import type BaseGPUPerformance from "../webglbase/BaseGPUPerformance";
import type WebGL2Device from "./WebGL2Device";

/**
 * 用来支持查询GPU命令执行所花费的时间，这样可以用来得到：性能的耗费是来自于GPU端还是CPU端
 */
export default class WebGL2GPUPerformance implements BaseGPUPerformance {

    //@ts-ignore
    private ext: EXT_disjoint_timer_query_webgl2;
    private queryObj: WebGLQuery;
    private beginQueryObj: WebGLQuery;
    private endQueryObj: WebGLQuery;
    private gl: WebGL2RenderingContext;
    private querying: boolean = false;
    private queryingEnd: boolean = false;
    constructor(device: WebGL2Device) {
        //@ts-ignore
        this.ext = device.extensions.EXT_disjoint_timer_query_webgl2;
        this.gl = device.gl;
        console.info(this.ext);
    }

    delete(): void {
        if (this.queryObj) {
            this.gl.deleteQuery(this.queryObj);
            this.queryObj = null;
        }

        if (this.beginQueryObj) {
            this.gl.deleteQuery(this.beginQueryObj);
            this.gl.deleteQuery(this.endQueryObj);
            this.beginQueryObj = null;
            this.endQueryObj = null;
        }
        this.querying = false;
        this.queryingEnd = false;
    }

    public beginQuery(timestamp: boolean = false) {

        //查看上一帧的查询是否结束，拿到上一帧的结果
        //然后开始当前帧
        if (this.querying) {
            this.pollQuery();
        };

        //如果上一帧的查询还没有返回结果，则丢弃当前帧的查询
        if (this.querying) return;


        this.querying = true;

        if (timestamp) {
            /**
             * 
             * 记录GPU在某一刻的“时间戳”,即将当前GPU的时间记录下来，类似于：
             * let begin = Date.now();
             * 将当前GPU时间记录在一个变量，随后再拿这个变量出来使用。
             * 所以我们需要 两个 timestamp，才能算 duration。
             * 
             * 相对于计算的流程交给我们处理，beginQuery由扩展底层自动计算。
             * 
             */

            if (!this.beginQueryObj) {
                this.beginQueryObj = this.gl.createQuery();
                this.endQueryObj = this.gl.createQuery();
            }
            this.ext.queryCounterEXT(this.beginQueryObj, this.ext.TIMESTAMP_EXT);
        } else {
            if (!this.queryObj) {
                this.queryObj = this.gl.createQuery();
            }
            //这种方式是查询：GPU 执行 begin 和 end 之间花了多久
            this.gl.beginQuery(this.ext.TIME_ELAPSED_EXT, this.queryObj);
        }
    }

    public endQuery() {

        if (!this.queryingEnd) {
            this.queryingEnd = true;
            if (this.endQueryObj) {
                this.ext.queryCounterEXT(this.endQueryObj, this.ext.TIMESTAMP_EXT);
            } else {
                this.gl.endQuery(this.ext.TIME_ELAPSED_EXT);
            }
        }
        this.pollQuery();
    }

    /**
     * 这个查询是异步的，不能是调用完endQuery就能立刻拿到结果的。
     */
    private pollQuery() {
        if (!this.querying || !this.queryingEnd) return;
        if (this.endQueryObj) {
            this.pollQueryTimestamp();
        } else {
            this.pollQuery1();
        }
    }

    private pollQuery1() {
        //检查计算的结果是否回来了没
        const available = this.gl.getQueryParameter(this.queryObj, this.gl.QUERY_RESULT_AVAILABLE);
        if (available) {
            this.queryingEnd = false;
            this.querying = false;
            const disjoint = this.gl.getParameter(this.ext.GPU_DISJOINT_EXT);
            let textContent: string;
            if (!disjoint) {
                const timeNs = this.gl.getQueryParameter(this.queryObj, this.gl.QUERY_RESULT);
                let gpuTime = timeNs / 1000000; // 转换为毫秒
                textContent = `GPU绘制耗时: ${gpuTime.toFixed(3)} ms`;
            } else {
                textContent = '计时失效 (GPU Disjoint)，丢弃本次测量';
            }
            console.info(textContent);
        }
    }

    private pollQueryTimestamp() {
        const available0 = this.gl.getQueryParameter(this.beginQueryObj, this.gl.QUERY_RESULT_AVAILABLE);
        const available1 = this.gl.getQueryParameter(this.endQueryObj, this.gl.QUERY_RESULT_AVAILABLE);
        if (available0 && available1) {
            this.queryingEnd = false;
            this.querying = false;
            const disjoint = this.gl.getParameter(this.ext.GPU_DISJOINT_EXT);
            if (!disjoint) {
                const t0 = this.gl.getQueryParameter(this.beginQueryObj, this.gl.QUERY_RESULT);
                const t1 = this.gl.getQueryParameter(this.endQueryObj, this.gl.QUERY_RESULT);
                console.log('GPU duration (ms):', (t1 - t0) / 1e6);
            } else {
                console.log('计时失效 (GPU Disjoint)，丢弃本次测量');
            }
        }
    }
}