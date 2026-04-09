import { sqclass, Vec2, type, serializable, SQFloat } from "../../../core/index";
import { Joint2DComponent } from "./Joint2DComponent";
import { ComponentType } from "../../../native_binding/index";


@sqclass("sq.MouseJoint2DComponent")
export default class MouseJoint2DComponent extends Joint2DComponent {
    private _target?: Vec2;
    private _hertz?: number;
    private _dampingRatio?: number;
    private _maxForce?: number;

    constructor() {
        super();
        this.nativeType = ComponentType.MouseJoint2DComponent;
    }


    @type(Vec2)
    @serializable
    set target(vec: Vec2) {
        this.nativeBeginOp(51);
        this.nativeWriteOpArg("f32", vec.x);
        this.nativeWriteOpArg("f32", vec.y);
        this.nativeEndOp();

        /* editor:start */
        if (!this._target) {
            this._target = new Vec2;
        }
        this._target.copy(vec);
        /* editor:end */
    }

    get target() {
        return this._target;
    }

    @type(SQFloat)
    @serializable
    set hertz(v: number) {
        if (this._hertz !== v) {
            this._hertz = v;
            this.nativeBeginOp(52);
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
            this.nativeBeginOp(53);
            this.nativeWriteOpArg("f32", v);
            this.nativeEndOp();
        }
    }

    get dampingRatio() {
        return this._dampingRatio;
    }

    @type(SQFloat)
    @serializable
    set maxForce(v: number) {
        if (this._maxForce !== v) {
            this._maxForce = v;
            this.nativeBeginOp(54);
            this.nativeWriteOpArg("f32", v);
            this.nativeEndOp();
        }
    }
    get maxForce() {
        return this._maxForce;
    }
}