
import { systemInfo } from "../platform";
import { API } from "./base/define";
import Device from "./base/Device";
import WebGLDevice from "./webgl/WebGLDevice";
import WebGL2Device from "./webgl2/WebGL2Device";

export default class DeviceManager {
    static ins: DeviceManager;

    public device: Device;

    constructor() {
        DeviceManager.ins = this;
    }

    initialize(canvas: HTMLCanvasElement): void {
        if (systemInfo.supportWebGL2) {
            this.device = new WebGL2Device();
        } else {
            this.device = new WebGLDevice();
        }
        this.device.initialize(canvas);
    }

    apiType(): number {
        let deviceApi: API = this.device.getAPI();
        switch (deviceApi) {
            case API.WEBGL:
            case API.GLES2:
                return 1;
            case API.WEBGL2:
            case API.GLES3:
                return 2;
        }
        return 0;
    }
}

new DeviceManager();

