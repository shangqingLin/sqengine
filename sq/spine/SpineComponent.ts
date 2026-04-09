import Spine from "../assets/assets/Spine";
import { AssetManager } from "../assets/index";
import { editableProp, editorMenu, serializable, sqclass, SQFloat, type } from "../core/index";
import {RenderComponent} from "../framework/component/RenderComponent";
import { ComponentType } from "../native_binding/index";
import SkeletonSystem from "./SkeletonSystem";

/* editor:start */
@editorMenu("2D/SpineComponent")
/* editor:end */
@sqclass("sq.SpineComponent")
export default class SpineComponent extends RenderComponent {

    private _spine: Spine;
    private _speed: number = 1;
    private currentAnimation: number | string = -1;
    private _frame: number = 30;
    private loop: boolean;
    public static RENDER_CPU: number = 1;
    public static RENDER_GPU: number = 2;

    private _renderType: number = -1;// SpineComponent.RENDER_CPU;

    constructor() {
        super();
        if (!SkeletonSystem.enabled) {
            new SkeletonSystem();
        }
        this.nativeType = ComponentType.SpineComponent;
    }

    /*editor:start*/
    @editableProp(0)
    /*editor:end*/
    @type(Spine)
    @serializable
    set spine(asset: Spine) {
        if (this._spine === asset) return;
        this._spine = asset;
        this.nativeBeginOp(51);
        this.nativeWriteOpArg("i32", asset ? asset.getId() : -1);
        this.nativeEndOp();
        
        //默认使用CPU渲染
        if (this._renderType === -1) {
            this.renderType = SpineComponent.RENDER_CPU;
        }
    }

    get spine(): Spine {
        return this._spine;
    }

    set spineUrl(urlOrUuid: string | null) {
        this._spine = null;
        if (urlOrUuid) {
            AssetManager.getInstance().load(urlOrUuid, (erorr: Error, asset: Spine) => {
                if (erorr) return;
                this.spine = asset;
                this.onLoaded();
            });
        }
    }

    private onLoaded() {
        if (this.currentAnimation !== -1) {
            let hasAnim: boolean = false;
            if (typeof this.currentAnimation === "string") {
                let index = this.spine.getAnimationIndex(this.currentAnimation);
                hasAnim = index !== -1;
            } else {
                let name = this.spine.getAnimationName(this.currentAnimation);
                hasAnim = !!name;
            }
            if (hasAnim) {
                this.play(this.currentAnimation, this.loop);
            }
        }
    }

    /*editor:start*/
    @editableProp(1)
    /*editor:end*/
    @type(SQFloat)
    @serializable
    set playSpeed(speed: number) {
        if (speed && this._speed !== speed) {
            this._speed = speed;
            this.nativeBeginOp(52);
            this.nativeWriteOpArg("f32", speed);
            this.nativeEndOp();
        }
    }

    get playSpeed(): number {
        return this._speed;
    }

    set bakeAll(b: boolean) {

    }

    play(animIndexOrName: number | string, loop: boolean = false): void {
        this.loop = loop;
        if (!this._spine) {
            this.currentAnimation = animIndexOrName;
            return;
        }
        if (typeof animIndexOrName === "string") {
            animIndexOrName = this._spine.getAnimationIndex(animIndexOrName);
        }
        this.currentAnimation = animIndexOrName;
        this.nativeBeginOp(53);
        this.nativeWriteOpArg("i32", this.currentAnimation);
        this.nativeWriteOpArg("i8", loop ? 1 : 0);
        this.nativeEndOp();
    }

    stop(): void {
        this.nativeBeginOp(54);
        this.nativeEndOp();
    }

    puase(): void {
        this.nativeBeginOp(55);
        this.nativeEndOp();
    }

    resume(): void {
        this.nativeBeginOp(56);
        this.nativeEndOp();
    }

    @type(SQFloat)
    @serializable
    set frame(frame: number) {
        if (this._frame !== frame) {
            this._frame = frame;
            this.nativeBeginOp(57);
            this.nativeWriteOpArg("i32", frame);
            this.nativeEndOp();
        }
    }

    get frame(): number {
        return this._frame;
    }

    set renderType(type: number) {
        if (this._renderType === type) return;
        this._renderType = type;
        if (this._renderType === SpineComponent.RENDER_CPU) {
            // this.updateMaterial();
        } else if (this._renderType === SpineComponent.RENDER_GPU) {
            this.setShaderMaterialFromRes("build-in/material/ui-spine-gpu-material.mat");
        }
        this.nativeBeginOp(58);
        this.nativeWriteOpArg("i8", type);
        this.nativeEndOp();
    }

    get renderType(): number {
        return this._renderType;
    }
}