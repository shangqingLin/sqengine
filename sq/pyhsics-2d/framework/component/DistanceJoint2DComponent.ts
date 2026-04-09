import { Joint2DComponent } from "./Joint2DComponent";
import { ComponentType } from "../../../native_binding/index";
import { serializable, SQBoolean, sqclass, SQFloat, type } from "../../../core";

@sqclass("sq.DistanceJoint2DComponent")
export default class DistanceJoint2DComponent extends Joint2DComponent {
    private _dampingRatio?: number;
    private _enableLimit?: boolean;
    private _enableMotor?: boolean;
    private _enableSpring?: boolean;
    private _hertz?: number;
    private _length?: number;
    private _maxLength?: number;
    private _maxMotorForce?: number;
    private _minLength?: number;
    private _motorSpeed?: number;

    constructor() {
        super();
        this.nativeType = ComponentType.DistanceJoint2DComponent;
    }

    @serializable
    @type(SQFloat)
    set length(v: number) {
        if (this._length !== v) {
            this._length = v;
            this.nativeBeginOp(51);
            this.nativeWriteOpArg("f32", v);
            this.nativeEndOp();
        }
    }

    get length() {
        return this._length;
    }

    @serializable
    @type(SQBoolean)
    set enableSpring(enable: boolean) {
        if (this._enableSpring !== enable) {
            this._enableSpring = enable;
            this.nativeBeginOp(52);
            this.nativeWriteOpArg("i8", enable ? 1 : 0);
            this.nativeEndOp();
        }
    }
    get enableSpring() {
        return this._enableSpring;
    }

    @serializable
    @type(SQFloat)
    set hertz(v: number) {
        if (this._hertz !== v) {
            this._hertz = v;
            this.nativeBeginOp(53);
            this.nativeWriteOpArg("f32", v);
            this.nativeEndOp();
        }
    }
    get hertz() {
        return this._hertz;
    }

    @serializable
    @type(SQFloat)
    set dampingRatio(v: number) {
        if (this._dampingRatio !== v) {
            this._dampingRatio = v;
            this.nativeBeginOp(54);
            this.nativeWriteOpArg("f32", v);
            this.nativeEndOp();
        }
    }

    get dampingRatio() {
        return this._dampingRatio;
    }

    @serializable
    @type(SQBoolean)
    set enableLimit(enable: boolean) {
        if (this._enableLimit !== enable) {
            this._enableLimit = enable;
            this.nativeBeginOp(55);
            this.nativeWriteOpArg("i8", enable ? 1 : 0);
            this.nativeEndOp();
        }
    }

    get enableLimit() {
        return this._enableLimit;
    }

    @serializable
    @type(SQFloat)
    set maxLength(v: number) {
        if (this._maxLength !== v) {
            this._maxLength = v;
            this.nativeBeginOp(56);
            this.nativeWriteOpArg("f32", v);
            this.nativeEndOp();
        }
    }
    get maxLength() {
        return this._maxLength;
    }

    @serializable
    @type(SQFloat)
    set maxMotorForce(v: number) {
        if (this._maxMotorForce !== v) {
            this._maxMotorForce = v;
            this.nativeBeginOp(57);
            this.nativeWriteOpArg("f32", v);
            this.nativeEndOp();
        }
    }

    get maxMotorForce() {
        return this._maxMotorForce;
    }

    @serializable
    @type(SQBoolean)
    set enableMotor(enable: boolean) {
        if (this._enableMotor !== enable) {
            this._enableMotor = enable;
            this.nativeBeginOp(58);
            this.nativeWriteOpArg("i8", enable ? 1 : 0);
            this.nativeEndOp();
        }
    }
    get enableMoto() {
        return this._enableMotor;
    }

    @serializable
    @type(SQFloat)
    set minLength(v: number) {
        if (this._minLength !== v) {
            this._minLength = v;
            this.nativeBeginOp(59);
            this.nativeWriteOpArg("f32", v);
            this.nativeEndOp();
        }
    }

    get minLength() {
        return this._minLength;
    }

    @serializable
    @type(SQFloat)
    set motorSpeed(v: number) {
        if (this._motorSpeed !== v) {
            this._motorSpeed = v;
            this.nativeBeginOp(60);
            this.nativeWriteOpArg("f32", v);
            this.nativeEndOp();
        }
    }
    get motorSpeed() {
        return this._motorSpeed;
    }
}