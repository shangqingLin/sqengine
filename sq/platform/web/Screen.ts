import { Mat3, Size } from "../../core/index";
import { ScreenEvent } from "../common";
import BaseScreen from "../common/BaseScreen";
import { getCanvas } from "./env";

enum WindowType {
    SubFrame,
    Fullscreen,
}

interface ScreenInitializeOption {
    canvas?: HTMLCanvasElement;
}

class ScreenAdapter extends BaseScreen {
    private canvas: HTMLCanvasElement;
    private winSize: Size;
    private _windowType: WindowType;
    public override initialize(options?: ScreenInitializeOption) {
        super.initialize();

        options = options || {};
        this.winSize = new Size();
        let c = getCanvas(options.canvas);
        this.canvas = c.canvas;
        // this._initContainer();
        //禁用右键
        c.canvas.addEventListener('contextmenu', function (event) {
            event.preventDefault();
        });

        const observer = new ResizeObserver(this.onCanvasWinResize.bind(this));
        observer.observe(this.canvas); // 观测Canvas在CSS宽度上的改变，不是渲染分辨率的改变


        document.addEventListener('visibilitychange', () => {
            if (document.hidden) {
                // console.log('页面进入后台');
                this.fire(ScreenEvent.HIDDEN);
            } else {
                // console.log('页面回到前台');
                // 恢复操作
                this.fire(ScreenEvent.VISIBLE);
            }
        });
    }


    private onCanvasWinResize() {
        let rect: DOMRect = this.canvas.getBoundingClientRect();
        this.fire("screen-resize", rect);
    }

    public get devicePixelRatio(): number {
        return window.devicePixelRatio || 1;
    }

    public set windowType(type: WindowType) {
        if (this._windowType !== type) {
            this._windowType = type;
            this._updateContainer();
        }
    }

    public get windowType(): WindowType {
        return this._windowType;
    }

    /**
     * 获取屏幕的真实的物理大小
     */
    public getScreenSize(useDevicePixelRatio: boolean = true): Readonly<Size> {
        let result = new Size();
        let p = this.devicePixelRatio;
        const w = document.documentElement.clientWidth;
        const h = document.documentElement.clientHeight;
        // let w = this.canvas.clientWidth;
        // let h = this.canvas.clientHeight
        result.set(useDevicePixelRatio ? w * p : w, useDevicePixelRatio ? h * p : h);
        return result;
    }


    /**
     * 画布的容器的大小。设置的是CSS像素单位
     * 设置为WindowType.SubFrame起作用
     * @param width 
     * @param height 
     */
    public setWindowSize(width: number, height: number): void {
        if (this.winSize.width !== width || this.winSize.height !== height) {
            this.winSize.width = width;
            this.winSize.height = height;
            this._updateContainer();
            this.fire("win-resize");
        }
    }

    /**
     * 设置真实的渲染分辨率。
     * @param width 
     * @param height 
     */
    public override setRenderSize(width: number, height: number): void {
        if (this.canvas.width !== width || this.canvas.height !== height) {
            this.canvas.width = width;
            this.canvas.height = height;
            super.setRenderSize(width, height);
            this._updateContainer();
            this.fire(ScreenEvent.RENDER_SIZE_CHANGE);
        }
    }

    public getRenderSize(result?: Size): Readonly<Size> {
        result = result || new Size();
        result.set(this.canvas.width, this.canvas.height);
        return result;
    }


    private _updateContainer() {

        let canvasStyle: CSSStyleDeclaration = this.canvas.style;
        let mat: Mat3 = new Mat3;
        let canvasCssWidth, canvasCssHeight;
        const browserWidth = document.documentElement.clientWidth;
        const browserheight = document.documentElement.clientHeight;

        if (this._windowType === WindowType.Fullscreen) {
            canvasCssWidth = this.canvas.width;
            canvasCssHeight = this.canvas.height;
            let sx = browserWidth / canvasCssWidth;
            let sy = browserheight / canvasCssHeight;
            mat.setScale(sx, sy);
        } else if (this._windowType === WindowType.SubFrame) {

            canvasCssWidth = this.winSize.width > browserWidth ? browserWidth : this.winSize.width;
            canvasCssHeight = this.winSize.height > browserheight ? browserheight : this.winSize.height;

            let scale = Math.min(canvasCssWidth / this.winSize.width, canvasCssHeight / this.winSize.height);
            canvasCssWidth = this.winSize.width * scale;
            canvasCssHeight = this.winSize.height * scale;

            //居中
            mat.setTranslate(
                (browserWidth - canvasCssWidth) * 0.5,
                (browserheight - canvasCssHeight) * 0.5
            );
        }


        canvasStyle.width = canvasCssWidth + "px";
        canvasStyle.height = canvasCssHeight + "px";
        //@ts-ignore
        canvasStyle.transformOrigin = canvasStyle.webkitTransformOrigin = canvasStyle.msTransformOrigin = canvasStyle.mozTransformOrigin = canvasStyle.oTransformOrigin = "0px 0px 0px";
        //@ts-ignore
        canvasStyle.transform = canvasStyle.webkitTransform = canvasStyle.msTransform = canvasStyle.mozTransform = canvasStyle.oTransform = "matrix(" + mat.toString() + ")";

    }
}

const screen = new ScreenAdapter();
export {
    screen,
    WindowType,
    ScreenInitializeOption
}