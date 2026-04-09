import { Joint2DComponent } from "./Joint2DComponent";
import { ComponentType } from "../../../native_binding/index";
import { serializable, SQBoolean, sqclass, SQFloat, type } from "../../../core";

@sqclass("sq.RevoluteJoint2DComponent")
export default class RevoluteJoint2DComponent extends Joint2DComponent {

    private _targetAngle?: number;
    private _enableSpring?: boolean;
    private _hertz?: number;
    private _dampingRatio?: number;
    private _enableLimit?: boolean;
    private _lowerAngle?: number;
    private _upperAngle?: number;
    private _enableMotor?: boolean;
    private _maxMotorTorque?: number;
    private _motorSpeed?: number;
    constructor() {
        super();
        this.nativeType = ComponentType.RevoluteJoint2DComponent;
    }

    @type(SQBoolean)
    @serializable
    set enableSpring(enable: boolean) {
        if (this._enableSpring !== enable) {
            this._enableSpring = enable;
            this.nativeBeginOp(51);
            this.nativeWriteOpArg("i8", enable ? 1 : 0);
            this.nativeEndOp();
        }
    }
    get enableSpring() {
        return this._enableSpring;
    }

    @type(SQFloat)
    @serializable
    set targetAngle(angle: number) {
        if (this._targetAngle !== angle) {
            this._targetAngle = angle;
            this.nativeBeginOp(52);
            this.nativeWriteOpArg("f32", angle);
            this.nativeEndOp();
        }
    }

    get targetAngle() {
        return this._targetAngle;
    }

    @type(SQFloat)
    @serializable
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

    @type(SQFloat)
    @serializable
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

    @type(SQBoolean)
    @serializable
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

    @type(SQFloat)
    @serializable
    set lowerAngle(angle: number) {
        if (this._lowerAngle !== angle) {
            this._lowerAngle = angle;
            this.nativeBeginOp(56);
            this.nativeWriteOpArg("f32", angle);
            this.nativeEndOp();
        }
    }
    get lowerAngle() {
        return this._lowerAngle;
    }

    @type(SQFloat)
    @serializable
    set upperAngle(angle: number) {
        if (this._upperAngle !== angle) {
            this._upperAngle = angle;
            this.nativeBeginOp(57);
            this.nativeWriteOpArg("f32", angle);
            this.nativeEndOp();
        }
    }

    get upperAngle() {
        return this._upperAngle;
    }

    @type(SQBoolean)
    @serializable
    set enableMotor(enable: boolean) {
        if (this._enableMotor !== enable) {
            this._enableMotor = enable;
            this.nativeBeginOp(58);
            this.nativeWriteOpArg("i8", enable ? 1 : 0);
            this.nativeEndOp();
        }
    }
    get enableMotor() {
        return this._enableMotor;
    }

    @type(SQFloat)
    @serializable
    set maxMotorTorque(v: number) {
        if (this._maxMotorTorque !== v) {
            this._maxMotorTorque = v;
            this.nativeBeginOp(59);
            this.nativeWriteOpArg("f32", v);
            this.nativeEndOp();
        }
    }

    get maxMotorTorque() {
        return this._maxMotorTorque;
    }

    @type(SQFloat)
    @serializable
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