import { Application } from "../framework/Application";
import System from "../framework/System";
import { dispatch, JsToNativeObjectBridge, NativeObjectType, ObjectType } from "../native_binding";
import NativeToJsDispatchTweenBridge from "./NativeToJsDispatchTweenBridge";
import { Tween } from "./Tween";

export default class TweenSystem extends System {
    private native: Module.TweenSystem;
    readonly nativeDispatch: JsToNativeObjectBridge;
    private static _inst: TweenSystem;
    private tweenPool: Array<Tween> = [];
    private tweenMap: { [key: number]: Tween } = Object.create(null);
    constructor() {
        super();
        this.native = new window.Module.TweenSystem();
        this.nativeDispatch = dispatch.createJsToNativeObject(ObjectType.TweenSystem);
        dispatch.registerNativeToJsObject(NativeObjectType.tween, new NativeToJsDispatchTweenBridge);
        Application.ins.registerSystem(this);
    }

    static get instance(): TweenSystem {
        if (!this._inst) {
            this._inst = new TweenSystem;
        }
        return this._inst;
    }

    getOrCreateTween(): Tween {
        let tween = this.tweenPool.pop();
        if (!tween) {
            tween = new Tween();
        }
        this.tweenMap[tween.getId()] = tween;
        return tween;
    }

    getTween(id: number) {
        return this.tweenMap[id];
    }

    recovery(tween:Tween) {
        //@ts-ignore
        tween.recovery();
        this.tweenMap[tween.getId()] = null;
        this.tweenPool.push(tween);
    }
}