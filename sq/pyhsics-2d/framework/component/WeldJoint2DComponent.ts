import { serializable, sqclass, SQFloat, type } from "../../../core/index";
import { Joint2DComponent } from "./Joint2DComponent";
import { ComponentType } from "../../../native_binding/index";


@sqclass("sq.WeldJoint2DComponent")
export default class WeldJoint2DComponent extends Joint2DComponent {
    private _linearHertz?: number;
    private _angularHertz?: number;
    private _linearDampingRatio?: number;
    private _angularDampingRatio?: number;


    constructor(){
        super();
        this.nativeType = ComponentType.WeldJoint2DComponent;
    }

    @type(SQFloat)
    @serializable
    set linearHertz(v: number) {
        if (this._linearHertz !== v) {
            this._linearHertz = v;
            this.nativeBeginOp(51);
            this.nativeWriteOpArg("f32", v);
            this.nativeEndOp();
        }
    }

    get linearHertz() {
        return this._linearHertz;
    }

    @type(SQFloat)
    @serializable
    set angularHertz(v: number) {
        if (this._angularHertz !== v) {
            this._angularHertz = v;
            this.nativeBeginOp(52);
            this.nativeWriteOpArg("f32", v);
            this.nativeEndOp();
        }
    }

    get angularHertz() {
        return this._angularHertz;
    }

    @type(SQFloat)
    @serializable
    set linearDampingRatio(v: number) {
        if (this._linearDampingRatio !== v) {
            this._linearDampingRatio = v;
            this.nativeBeginOp(53);
            this.nativeWriteOpArg("f32", v);
            this.nativeEndOp();
        }
    }

    get linearDampingRatio() {
        return this._linearDampingRatio;
    }

    @type(SQFloat)
    @serializable
    set angularDampingRatio(v: number) {
        if (this._angularDampingRatio !== v) {
            this._angularDampingRatio = v;
            this.nativeBeginOp(54);
            this.nativeWriteOpArg("f32", v);
            this.nativeEndOp();
        }
    }

    get angularDampingRatio() {
        return this._angularDampingRatio;
    }
}