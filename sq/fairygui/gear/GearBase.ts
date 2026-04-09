import { Controller } from "../Controller";
import { GObject } from "../GObject";
import { EaseType } from "../tween/EaseType";
import { GTweener } from "../tween/GTweener";

/**
 * 1、我们先在组件中定义控制器Controller
 * 2、然后通过将Controller设置到Gear，使得Controller完成对控件属性的控制
 * 
 * 这个就是属性控制的基类 * 
 */
export class GearBase {
    public static disableAllTweenEffect?: boolean;

    public _owner: GObject;
    protected _controller: Controller;
    protected _tweenConfig: GearTweenConfig;

    public dispose(): void {
        if (this._tweenConfig && this._tweenConfig._tweener) {
            this._tweenConfig._tweener.kill();
            this._tweenConfig._tweener = null;
        }
    }

    public get controller(): Controller {
        return this._controller;
    }

    public set controller(val: Controller) {
        if (val != this._controller) {
            this._controller = val;
            if (this._controller)
                this.init();
        }
    }

    public get tweenConfig(): GearTweenConfig {
        if (!this._tweenConfig)
            this._tweenConfig = new GearTweenConfig();
        return this._tweenConfig;
    }

    protected get allowTween(): boolean {
        // return this._tweenConfig && this._tweenConfig.tween && constructingDepth.n == 0 && !GearBase.disableAllTweenEffect;
        return false;
    }


    public updateFromRelations(dx: number, dy: number): void {

    }

    protected addStatus(pageId: string, ...param: any): void {

    }

    /**
     * 初始化一个属性
     */
    protected init(): void {

    }

    /**
     * 子类实现此方法。
     * 表示将此控制器应用到GObject上，相当于激活当前的属性，将属性值设置到GObject上
     */
    public apply(): void {
    }


    /**
     * 子类覆盖此方法
     * 当对应的GObject属性更改时，触发调用这个函数，重新计算控制器属性
     */
    public updateState(): void {
    }
}


export class GearTweenConfig {
    public tween: boolean;
    public easeType: number;
    public duration: number;
    public delay: number;

    public _displayLockToken: number;
    public _tweener: GTweener;

    constructor() {
        this.tween = true;
        this.easeType = EaseType.QuadOut;
        this.duration = 0.3;
        this.delay = 0;
    }
}