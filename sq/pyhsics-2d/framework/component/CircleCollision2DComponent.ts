import { ComponentType } from "../../../native_binding/index";
import Collision2DComponent from "./Collision2DComponent";
import { sqclass, editorMenu, editableProp, serializable, SQFloat, type } from "../../../core/index";

/* editor:start */
@editorMenu("physics 2D/CircleCollision2DComponent")
/* editor:end */
@sqclass("sq.CircleCollision2DComponent")
export default class CircleCollision2DComponent extends Collision2DComponent {

    private _radius: number;

    constructor() {
        super(ComponentType.CircleCollision2DComponent);
    }

    /* editor:start */
    @editableProp(1)
    /* editor:end */
    @type(SQFloat)
    @serializable
    set radius(r: number) {
        /* editor:start */
        this._radius = r;
        /* editor:end */
        if (this.physicsEnable()) {
            this.nativeBeginOp(51);
            this.nativeWriteOpArg("f32",r);
            this.nativeEndOp();
        }
    }

    get radius() {
        return this._radius;
    }


}