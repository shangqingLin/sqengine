import TweenEase from "./TweenEase";
import type Node from "../scene/Node";
import { IDGen } from "../core/index";
import TweenSystem from "./TweenSystem";
export interface TweenOption {

    /**
     * 持续时长，单位为毫秒
     */
    duration: number;

    /**
     * 缓动函数，可以使用已有的，也可以传入自定义的函数。
     * 默认是 linear
     */
    easing?: TweenEase | ((k: number) => number);

    /**
     * 
     * @param start 
     * @param end 
     * @param current 
     * @param ratio 
     * @returns 
     */
    progress?: (start: number, end: number, current: number, ratio: number) => number;

    /**
     * 开始播放的时候触发
     * @param target 
     * @returns 
     */
    onStart?: (target?: Node) => void;

    /**
     * 动画帧循环中触发
     * @param target 
     * @param ratio 
     * @returns 
     */
    onUpdate?: (target?: Node, ratio?: number) => void;

    /**
     * 缓动播放完毕时调用
     * @param target 
     * @returns 
     */
    onComplete?: (target?: Node) => void;
}

export type NodePropsKeyPartial = {
    x?: number;
    y?: number;
    rotation?: number;
    scaleX?: number;
    scaleY?: number;
    rotationX?: number;
    rotationY?: number;
};


let idGen = new IDGen;
export class Tween {
    private nativeId: number = idGen.get();
    private target: Node;
    private onUpdate?: (target?: Node, ratio?: number) => void;
    private onComplete?: (target?: Node) => void;
    private onStart?: (target?: Node) => void;

    getId(): number {
        return this.nativeId;
    }

    private callUpdate(ratio: number) {
        if (this.onUpdate) this.onUpdate(this.target, ratio);
    }

    private callComplete() {
        if (this.onComplete) this.onComplete(this.target);
        TweenSystem.instance.recovery(this);
    }

    private callStart() {
        if (this.onStart) this.onStart(this.target);
    }

    private recovery()
    {
        this.target = null;
        if(this.onUpdate) this.onUpdate = null;
        if(this.onComplete) this.onComplete = null;
        if(this.onStart) this.onStart = null;
    }

    static to(target: Node, props: NodePropsKeyPartial, opt: TweenOption) {
        let tween = TweenSystem.instance.getOrCreateTween();
        tween.onComplete = opt.onComplete;
        tween.onStart = opt.onStart;
        tween.onUpdate = opt.onUpdate;
        let nativeObj = TweenSystem.instance.nativeDispatch;
        nativeObj.beginOp(1);
        nativeObj.writeOpArg("i32", tween.nativeId);
        let propertyState = 0;
        let propertyValue = [];
        if (props.x !== undefined) {
            propertyState |= 1 << 1;
            propertyValue.push(props.x);
        }

        if (props.y !== undefined) {
            propertyState |= 1 << 2;
            propertyValue.push(props.y);
        }

        if (props.scaleX !== undefined) {
            propertyState |= 1 << 3;
            propertyValue.push(props.scaleX);
        }

        if (props.scaleY !== undefined) {
            propertyState |= 1 << 4;
            propertyValue.push(props.scaleY);
        }

        if (props.rotationX !== undefined) {
            propertyState |= 1 << 5;
            propertyValue.push(props.rotationX);
        }

        if (props.rotationY !== undefined) {
            propertyState |= 1 << 6;
            propertyValue.push(props.rotationY);
        }

        if (props.rotation !== undefined) {
            propertyState |= 1 << 7;
            propertyValue.push(props.rotation);
        }

        nativeObj.writeOpArg("ui32", propertyState);
        for (let i = 0; i < propertyValue.length; ++i) {
            nativeObj.writeOpArg("f32", propertyValue[i]);
        }

        nativeObj.writeOpArg("i32", target.id);
        nativeObj.writeOpArg("i8", opt.easing !== undefined ? opt.easing : TweenEase.linear);
        nativeObj.writeOpArg("f32", opt.duration);

        let eventState = 0;

        //onComplete 
        eventState |= 1 << 1;

        if (opt.onStart) {
            eventState |= 1 << 2;
        }

        if (opt.onUpdate) {
            eventState |= 1 << 3;
        }
        nativeObj.writeOpArg("ui8", eventState);
        nativeObj.endOp();
    }
}