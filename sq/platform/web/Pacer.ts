import { ScreenEvent } from "../common";
import { screen } from "./Screen";

export default class Pacer {

    private _rAF: typeof requestAnimationFrame;
    private _cAF: typeof cancelAnimationFrame;
    private _onTick: ((dt: number) => void) | null = null;
    private _isPlaying: boolean = false;
    private _stHandle = 0;
    private _enabledFrameLimit: boolean = false;
    private _frameLimitTime: number = 60;
    private _onHadHidden: boolean = false;
    constructor() {
        this._rAF = window.requestAnimationFrame
            || window.webkitRequestAnimationFrame
            || window.mozRequestAnimationFrame
            || window.oRequestAnimationFrame
            || window.msRequestAnimationFrame;
        this._cAF = window.cancelAnimationFrame
            || window.cancelRequestAnimationFrame
            || window.msCancelRequestAnimationFrame
            || window.mozCancelRequestAnimationFrame
            || window.oCancelRequestAnimationFrame
            || window.webkitCancelRequestAnimationFrame
            || window.msCancelAnimationFrame
            || window.mozCancelAnimationFrame
            || window.webkitCancelAnimationFrame
            || window.ocancelAnimationFrame;

        this._rAF = this._rAF.bind(window);
        this._cAF = this._cAF.bind(window);
        screen.on(ScreenEvent.HIDDEN, this, this.onScreenHide);
    }

    private onScreenHide() {
        this._onHadHidden = true;
    }



    set onTick(val: ((dt:number) => void) | null) {
        this._onTick = val;
    }

    /**
     * 限制游戏运行帧率。数值为1~60范围
     * @param frame 
     */
    setFrameRate(frame: number): void {
        if (frame < 60) {
            this._enabledFrameLimit = true;
            this._frameLimitTime = 1000 / frame;
        } else {
            this._enabledFrameLimit = false;
        }
    }

    start(): void {
        if (this._isPlaying) return;

        let self = this;
        let lastTimestamp: number = performance.now();
        const updateCallback = (timestamp: number): void => {
            let deltaTime = timestamp - lastTimestamp;

            //首帧的时候会出现小于0的情况，lastTimestamp < timestamp
            //这是因为performance.now()和requestAnimation计算时间机制不一样
            if(deltaTime < 0){
                deltaTime = 16;
            }
            if (this._onHadHidden) {
                this._onHadHidden = false;
                deltaTime = Math.min(deltaTime, 16);
            }

            if (self._enabledFrameLimit) {
                if (deltaTime >= self._frameLimitTime) {
                    lastTimestamp = timestamp - (deltaTime % self._frameLimitTime);
                    if (this._onTick) {
                        this._onTick(deltaTime);
                    }
                }
            } else {
                lastTimestamp = timestamp;
                if (this._onTick) {
                    this._onTick(deltaTime);
                }
            }
            this._stHandle = this._rAF(updateCallback);

        };
        this._stHandle = this._rAF(updateCallback);
        this._isPlaying = true;
    }

    stop(): void {
        if (!this._isPlaying) return;
        this._cAF(this._stHandle);
        this._stHandle = 0;
        this._isPlaying = false;
    }
}