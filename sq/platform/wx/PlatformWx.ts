import { Application } from "../../framework/Application";
import { Platform } from "../common";


class PlatformWx implements Platform {
    initialize(): void {
        this.performanceProcess();
    }

    private performanceProcess() {


        // 只有IOS支持高性能模式
        //@ts-ignore
        console.info("处于高性能模式：", GameGlobal.isIOSHighPerformanceMode);
        //@ts-ignore
        console.info("处于高性能模式+：", GameGlobal.isIOSHighPerformanceModePlus);

        const deviceInfo = wx.getDeviceInfo();
        let benchmarkLevel = deviceInfo.benchmarkLevel;
        if (deviceInfo.platform === "devtools") {
            //开发工具中benchmarkLevel返回 1
            benchmarkLevel = 50;
        }
        this.processBenchmarkLevel(benchmarkLevel);
    }

    private processBenchmarkLevel(benchmarkLevel: number) {
        //https://developers.weixin.qq.com/minigame/dev/guide/performance/perf-benchmarkLevel.html

        console.log("benchmarkLevel", benchmarkLevel);
        // if (benchmarkLevel >= 36) {
        //     this.performance_high(benchmarkLevel);
        // } else if (benchmarkLevel >= 30) {
        //     this.performanceProcess_middle(benchmarkLevel);
        // } else {
        //     this.performanceProcess_low(benchmarkLevel);
        // }

         if (benchmarkLevel >= 30) {
            this.performance_high(benchmarkLevel);
        } else {
            this.performanceProcess_low(benchmarkLevel);
        }
    }

    /**
     * 
     * 高端机器
     * 
     */
    private performance_high(benchmarkLevel: number): void {
        Application.ins.setFrameRate(60);
    }

    /**
     * 中端机器
     */
    private performanceProcess_middle(benchmarkLevel: number): void {
        let fps = 45 + (50 - 45) * (benchmarkLevel / 35);
        Application.ins.setFrameRate(fps);
    }

    /**
     * 低端机器
     */
    private performanceProcess_low(benchmarkLevel: number): void {
        benchmarkLevel = benchmarkLevel < 0 ? 0 : benchmarkLevel;
        let fps = 35 + (40 - 35) * (benchmarkLevel / 29);
        Application.ins.setFrameRate(fps);
    }
}

export default new PlatformWx;