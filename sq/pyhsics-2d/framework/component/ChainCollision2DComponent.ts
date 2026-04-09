import { ComponentType } from "../../../native_binding/index";
import Collision2DComponent from "./Collision2DComponent";
import { sqclass, editorMenu, editorFormType, EditorFormTypeEnum, serializable, type, editableProp, SQInteger, SQFloat } from "../../../core/index";

/* editor:start */
@editorMenu("physics 2D/ChainCollision2DComponent")
/* editor:end */
@sqclass("sq.ChainCollision2DComponent")
export default class ChainCollision2DComponent extends Collision2DComponent {

    private _points: Array<number>;
    private _oneSided: boolean;

    constructor() {
        super(ComponentType.ChainCollision2DComponent);
    }

    //线段也分为背面和正面的。Box2D中按照逆时针顺序指定线段端点的为正面
    //如果设置为false,则线段正面和反面都可以与其他多边形发生碰撞
    //如果设置为true，则线段只在正面与多边形发生碰撞。这样可以减少不必要的碰撞检测提升性能
    //默认为false
    /* editor:start */
    @editableProp(1)
    /* editor:end */
    @type(Boolean, true)
    @serializable
    set oneSided(b: boolean) {
        /* editor:start */
        this._oneSided = b;
        /* editor:end */

        if (this.physicsEnable()) {
            this.nativeBeginOp(52);
            this.nativeWriteOpArg("i8", b ? 1 : 0);
            this.nativeEndOp();
        }
    }

    get oneSided() {
        return this._oneSided;
    }

    /* editor:start */
    @editableProp(2)
    @editorFormType(EditorFormTypeEnum.point)
    /* editor:end */
    @type(SQFloat, true)
    @serializable
    /**
     * 传进来的顶点必须是逆时针
     */
    set point(points: Array<number>) {
        let count = points.length;

        //必须至少有2个点
        if (count < 4) {
            /* editor:start */
            throw new Error("必须至少有2个点");
            /* editor:end */
            return;
        }

        //游戏运行时为了节省内存，不保存这里的点
        //所以在游戏过程通过point获取的是为空的，要保存自己外部处理
        //这里保存完全是为了支持编辑器
        /* editor:start */
        this._points = points;
        /* editor:end */

        if (this.physicsEnable()) {
            this.nativeBeginOp(51);
            this.nativeWriteOpArg("i32", count >> 1);

            // for (let i = 0; i < count; i += 2) {
            //     this.nativeWriteOpArg("f32", points[i]);
            //     this.nativeWriteOpArg("f32", points[i + 1]);
            // }

            /**
             * 物理引擎要求必须是顺时针来指定顶点的,当传进来是逆时针，所以调换一下顺序
             */
            for (let i = count - 1; i >= 0; i -= 2) {
                this.nativeWriteOpArg("f32", points[i - 1]);
                this.nativeWriteOpArg("f32", points[i]);
            }

            this.nativeEndOp();
        }
    }

    get point(): Array<number> {
        return this._points;
    }

    set pointEnabledLink(points: Array<number>) {
        let count = points.length;

        //必须至少有3个点
        if (count < 9) {
            /* editor:start */
            throw new Error("必须至少有3个点");
            /* editor:end */
            return;
        }

        /* editor:start */
        this._points = points;
        /* editor:end */

        if (this.physicsEnable()) {
            this.nativeBeginOp(53);
            this.nativeWriteOpArg("i32", count / 3);

            for (let i = count - 1; i >= 0; i -= 3) {
                this.nativeWriteOpArg("f32", points[i - 1]);
                this.nativeWriteOpArg("f32", points[i]);
            }

            for (let i = count - 3; i >= 0; i -= 3) {
                this.nativeWriteOpArg("i32", points[i]);
            }
            this.nativeEndOp();
        }
    }

    public insertBefore(fromId: number, id: number, x: number, y: number): void {
        this.nativeBeginOp(54);
        this.nativeWriteOpArg("i32", fromId);
        this.nativeWriteOpArg("i32", id);
        this.nativeWriteOpArg("f32", x);
        this.nativeWriteOpArg("f32", y);
        this.nativeEndOp();
    }

    public insertAfter(fromId: number, id: number, x: number, y: number): void {
        this.nativeBeginOp(55);
        this.nativeWriteOpArg("i32", fromId);
        this.nativeWriteOpArg("i32", id);
        this.nativeWriteOpArg("f32", x);
        this.nativeWriteOpArg("f32", y);
        this.nativeEndOp();
    }

    public modify(id: number, x: number, y: number): void {
        this.nativeBeginOp(56);
        this.nativeWriteOpArg("i32", id);
        this.nativeWriteOpArg("f32", x);
        this.nativeWriteOpArg("f32", y);
        this.nativeEndOp();
    }
    public remove(id: number): void {
        this.nativeBeginOp(57);
        this.nativeWriteOpArg("i32", id);
        this.nativeEndOp();
    }

}