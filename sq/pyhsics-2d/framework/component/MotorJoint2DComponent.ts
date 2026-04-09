import { serializable, sqclass, SQFloat, type } from "../../../core/index";
import { Joint2DComponent } from "./Joint2DComponent";
import { ComponentType } from "../../../native_binding/index";


@sqclass("sq.MotorJoint2DComponent")
export default class MotorJoint2DComponent extends Joint2DComponent {
    private _maxForce?: number;
    private _maxTorque?: number;
    private _correctionFactor?: number;

    constructor(){
        super();
        this.nativeType = ComponentType.MotorJoint2DComponent;
    }

    @type(SQFloat)
    @serializable
    set maxForce(v: number) {
        if (this._maxForce !== v) {
            this._maxForce = v;
            this.nativeBeginOp(51);
            this.nativeWriteOpArg("f32", v);
            this.nativeEndOp();
        }
    }
    get maxForce() {
        return this._maxForce;
    }

    @type(SQFloat)
    @serializable
    set maxTorque(v: number) {
        if (this._maxTorque !== v) {
            this._maxTorque = v;
            this.nativeBeginOp(52);
            this.nativeWriteOpArg("f32", v);
            this.nativeEndOp();
        }
    }
    get maxTorque() {
        return this._maxTorque;
    }

    @type(SQFloat)
    @serializable
    set correctionFactor(v: number) {
        if (this._correctionFactor !== v) {
            this._correctionFactor = v;
            this.nativeBeginOp(53);
            this.nativeWriteOpArg("f32", v);
            this.nativeEndOp();
        }
    }
    get correctionFactor() {
        return this._correctionFactor;
    }
}