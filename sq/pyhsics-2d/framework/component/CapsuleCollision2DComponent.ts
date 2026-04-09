import { ComponentType } from "../../../native_binding/index";
import Collision2DComponent from "./Collision2DComponent";
import { sqclass, editorMenu, Vec2, serializable, SQFloat, type, SQJSON } from "../../../core/index";

/* editor:start */
@editorMenu("physics 2D/CapsuleCollision2DComponent")
/* editor:end */
@sqclass("sq.CapsuleCollision2DComponent")
export default class CapsuleCollision2DComponent extends Collision2DComponent {

    private _center1: Vec2 = new Vec2;
    private _center2: Vec2 = new Vec2;
    private _radius: number = 0;


    @serializable
    @type(SQFloat)
    private radiusSerialize: number;

    @serializable
    @type(SQJSON)
    private centerSerialize: any;

    constructor() {
        super(ComponentType.CapsuleCollision2DComponent);
    }

    protected override afterDeserialize() {
        super.afterDeserialize();
        if (this.radiusSerialize && this.centerSerialize) {
            this.create(this.centerSerialize.x, this.centerSerialize.y, this.centerSerialize.z, this.centerSerialize.w, this.radiusSerialize);
            delete this.radiusSerialize;
            delete this.centerSerialize;
        }
    }


    public create(p1x: number, p1y: number, p2x: number, p2y: number, radius: number) {
        this._center1.x = p1x;
        this._center1.y = p1y;
        this._center2.x = p2x;
        this._center2.y = p2y;
        this._radius = radius;
        this.nativeBeginOp(51);
        this.nativeWriteOpArg("f32", p1x);
        this.nativeWriteOpArg("f32", p1y);
        this.nativeWriteOpArg("f32", p2x);
        this.nativeWriteOpArg("f32", p2y);
        this.nativeWriteOpArg("f32", radius);
        this.nativeEndOp();
    }

    get center1(): Readonly<Vec2> {
        return this._center1;
    }

    get center2(): Readonly<Vec2> {
        return this._center2;
    }

    get radius(): number {
        return this._radius;
    }
}