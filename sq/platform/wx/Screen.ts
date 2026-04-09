import { EventDispatch, Size } from "../../core/index";
import { ScreenEvent } from "../common";
import BaseScreen from "../common/BaseScreen";
import { getCanvas } from "./env";

enum WindowType {
    SubFrame,
    Fullscreen,
}

interface ScreenInitializeOption {
    canvasContainer?: HTMLDivElement;
}

class ScreenAdapter extends BaseScreen {
    private canvas: HTMLCanvasElement;
    private windowInfo: wx.WindowInfo;
    public override initialize(options?: ScreenInitializeOption) {
        super.initialize();
        this.canvas = getCanvas().canvas;
        this.windowInfo = wx.getWindowInfo();
    }

    public get devicePixelRatio(): number {
        return this.windowInfo.pixelRatio || 1;
    }

    public set windowType(type: WindowType) {
        //微信上一直都是全屏
    }

    public get windowType(): WindowType {
        return WindowType.Fullscreen;
    }

    /**
     * 获取屏幕的真实的物理大小
     */
    public getScreenSize(): Size {
        let result = new Size();
        let p = this.devicePixelRatio;
        result.set(
            this.windowInfo.windowWidth * p,
            this.windowInfo.windowHeight * p
        );
        return result;
    }

    public setWindowSize(width: number, height: number): void {
        //微信上一直都是全屏
    }

    /**
     * 设置真实的渲染分辨率。
     * @param width 
     * @param height 
     */
    public override setRenderSize(width: number, height: number): void {
        if (this.canvas.width !== width || this.canvas.height !== height) {
            super.setRenderSize(width, height);
            this.canvas.width = width;
            this.canvas.height = height;
            this.fire(ScreenEvent.RENDER_SIZE_CHANGE);
        }
    }

    public getRenderSize(result?: Size): Size {
        result = result || new Size();
        result.set(this.canvas.width, this.canvas.height);
        return result;
    }
}

const screen = new ScreenAdapter();
export {
    screen,
    WindowType,
    ScreenInitializeOption
}