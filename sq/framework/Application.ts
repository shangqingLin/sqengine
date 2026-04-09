/* debug:start */
import GameStat from "../profiler/GameStat";
/* debug:end */

import { Timer, EventDispatch } from "../core/index";
import inputManager from "../input/InputManager";
import { Pacer, Platform, ScreenInitializeOption, getCanvas, screen } from "../platform/index"
import { dispatch as NativeDispatch } from "../native_binding/index";
import type System from "./System";
import DeviceManager from "../gfx/DeviceManager";
import type Scene from "../scene/Scene";
import { AssetManager } from "../assets/index";
import NodeNativeToJsBridge from "../scene/NodeNatvieToJsBridge";
import type { RenderPipline } from "../rendering";
import buildinResManager from "../assets/assetmanager/BuildinResManager";
import { GRoot } from "../fairygui";
import NodeManager from "../scene/NodeManager";

export interface ApplicationInitializeOption {
    canvas?: HTMLCanvasElement;
    resBasePath?: string;
}

export class Application extends EventDispatch {
    static ins: Application;
    public readonly timer: Timer;
    public readonly postTimer: Timer
    private systems: Array<System> = [];
    private pacer: Pacer;
    private native: Module.Application;
    private scene: Scene;
    private frameStartTime: number = 0;
    constructor() {
        super();
        Application.ins = this;
        this.timer = new Timer();
        this.postTimer = new Timer();
    }

    async initialize(option: ApplicationInitializeOption) {
        return Promise.resolve()
            .then(() => {
                return new Promise((resolve, reject) => {
                    if (window.Module.calledRun) {
                        resolve(null);
                    } else {
                        window.__ATINIT__.push(() => {
                            resolve(null);
                        })
                    }
                });
            }).then(() => {
                if (option.resBasePath) {
                    return AssetManager.getInstance().initialize(option.resBasePath);
                }
            }).then(() => {
                this.native = new window.Module.Application();
                screen.initialize({
                    canvas: option.canvas
                } as ScreenInitializeOption);
                let env = getCanvas();
                DeviceManager.ins.initialize(env.canvas);
                this.native.initialize();
                NativeDispatch.initialize();
                inputManager.initialize();
                NodeNativeToJsBridge.initialize();
                this.pacer = new Pacer();
                this.pacer.onTick = this._update.bind(this);
            })
            .then(() => {
                Platform.initialize();
            }).then(() => {
                buildinResManager.initialize();
                GRoot.initialize();
            })
            /*debug:start */
            .catch((error: Error) => {
                console.error(error);
                throw error;
            });
        /*debug:end */
    }

    setRenderPipeline(renderPipeline: RenderPipline) {

        //@ts-ignore
        this.native.setRenderPipeline(renderPipeline.native);
    }

    /**
     * 开启游戏循环
     */
    start() {
        this.pacer.start();
    }

    /**
     * 停止游戏循环
     */
    stop() {
        this.pacer.stop();
    }

    /**
     * @param system {System}
     */
    registerSystem(system: System) {

        //@ts-ignore
        if (system.native) {
            //表示为Native的System，给Native调用即可
            //@ts-ignore
            this.native.registerSystem(system.native);
        } else {
            system.init();
            this.systems.push(system);
        }
    }

    setRunScene(scene: Scene) {
        if (this.scene === scene) return;
        if (this.scene) {
            this.scene._active(false);
        }
        this.scene = scene;
        scene._active(true);
    }

    getRunScene(): Scene {
        return this.scene;
    }

    setFrameRate(frame: number) {
        this.pacer.setFrameRate(frame);
    }

    private _update(dt: number) {
        console.info("------------------------update");

        this.frameStartTime = Date.now();

        NativeDispatch.frameBegin(dt);

        //@ts-ignore
        this.timer.runCallLater();

        //处理用户输入
        inputManager.update();

        //处理业务
        //@ts-ignore
        this.timer.update(dt);

        for (let i = 0, n = this.systems.length; i < n; ++i) {
            this.systems[i].update(dt);
        }

        NativeDispatch.dispatchJsToNative();

        this.native.update();

        // let end = Date.now();
        // console.info("update:", end - start);
        // start = Date.now();

        NativeDispatch.dispatchNativeToJs();

        NativeDispatch.frameEnd();

        this.native.postUpdate();

        //@ts-ignore
        this.postTimer.update(dt);

        /* debug:start */
        GameStat.loop(dt);
        /* debug:end */

        NodeManager.destroyNode();
        // end = Date.now();
        // console.info("end frame :", end - start);
    }

    getFromFrameStartTime(): number {
        return Date.now() - this.frameStartTime;
    }
}
