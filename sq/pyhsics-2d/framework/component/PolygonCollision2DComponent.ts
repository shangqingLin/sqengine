import Collision2DComponent from "./Collision2DComponent";
import { ComponentType } from "../../../native_binding/index";
import { sqclass, editorMenu, editorFormType, EditorFormTypeEnum, serializable, type, editableProp, SQFloat } from "../../../core/index";

/* editor:start */
@editorMenu("physics 2D/PolygonCollision2DComponent")
/* editor:end */
@sqclass("sq.PolygonCollision2DComponent")
export default class PolygonCollision2DComponent extends Collision2DComponent {

    private _points: Array<number>;

    constructor() {
        super(ComponentType.PolygonCollision2DComponent);
    }


    /* editor:start */
    @editableProp(1)
    @editorFormType(EditorFormTypeEnum.point)
    /* editor:end */
    @type(SQFloat,true)
    @serializable
    set point(point: Array<number>) {
        //游戏运行时为了节省内存，不保存这里的点
        //所以在游戏过程通过point获取的是为空的，要保存自己外部处理
        //这里保存完全是为了支持编辑器
        /* editor:start */
        this._points = point;
        /* editor:end */

        if(this.physicsEnable()){
            let count = point.length;
            this.nativeBeginOp(51);
            this.nativeWriteOpArg("i32",count * 0.5);
            for(let i = 0 ; i < count ; i+=2){
                this.nativeWriteOpArg("f32",point[i]);
                this.nativeWriteOpArg("f32",point[i+1]);
            }
            this.nativeEndOp();
        }
    }

    get point(): Array<number> {
        return this._points;
    }
}