import Component from "../../framework/component/Component";
import { ComponentType, dispatch, JsToNativeObjectBridge } from "../../native_binding/index";
import { sqclass, serializable, Vec2, editableProp, editorMenu, type, Mat3, SQFloat, math } from "../../core/index";
import NodeEventType from "../../scene/NodeEventType";
import { GImage } from "../../fairygui";
import { bpFunction } from "../../blueprint";

/**
 * 2D节点变换矩阵运算
 * @constructor
 */


/**
 * 标记节点上的Transform组件改变的情况
 */
enum TransformBit {
    NONE = 0,
    POSITION = (1 << 0),
    ROTATION = (1 << 1),
    SCALE = (1 << 2),

    TRS = TransformBit.POSITION | TransformBit.ROTATION | TransformBit.SCALE
};

/* editor:start */
@editorMenu("2D/Transform2D")
/* editor:end */
@sqclass("sq.Transform2DComponent")
export default class Transform2DComponent extends Component {

    private _nativePointer: number;
    private _scale: Vec2;
    private _rotate: Vec2;
    private _position: Vec2;
    private _worldMat: Mat3;

    constructor() {
        super();
        this.nativeType = ComponentType.Transform2DComponent;
    }

    private checkNativeDirty() {
        return !!this.getNativeMemeroy(48, "i32");
    }

    private refreshNativeTransform() {
        if (this.checkNativeDirty()) {
            // console.info("refreshNativeTransform");
            this.nativeBeginOp(9);
            this.nativeEndOp();
            dispatch.flushJsToNative();
        }
    }

    private setDirty(bit: TransformBit) {
        this.nativeBeginOp(14, true);
        this.nativeWriteOpArg("i32", bit);
        this.nativeEndOp();
        this.setToNativeMemeroy(48, 1, "i32");
    }

    private checkNativePointer() {
        if (!this._nativePointer) {
            //保证addComponet的时候，nativePointer已经存在
            dispatch.flushJsToNative();

            //@ts-ignore
            let nativeObject: JsToNativeObjectBridge = this.node.nativeObject;
            this._nativePointer = window.Module.ccall("getNodeComponentPointer", "number", ["number", "number"], [ComponentType.Transform2DComponent, nativeObject.getId()]);
        }
    }

    set x(x: number) {
        let _this = this;

        // console.info("set x", this.node.id, x);

        if (!math.equlas(_this.x, x, 0.1)) { //因为是像素
            this.setToNativeMemeroy(0, x);
            this.setDirty(TransformBit.POSITION);
            // _this.setToNative(1, x, _this.y);
            // this.fire(NodeEventType.TRASNFORM_CHANGE);
        }
    }

    get x(): number {
        return this.getNativeMemeroy(0);
    }

    set y(y: number) {
        let _this = this;
        if (!math.equlas(_this.y, y, 0.1)) {
            // console.info("setY",y,this);
            this.setToNativeMemeroy(4, y);
            this.setDirty(TransformBit.POSITION);

            // 从C++端触发，不在这里触发 
            // this.fire(NodeEventType.TRASNFORM_CHANGE);
        }
    }

    get y(): number {
        return this.getNativeMemeroy(4);
    }

    setPosition(x: number, y: number): void {
        let _this = this;
        _this.x = x;
        _this.y = y;
    }

    @type(Vec2)
    @serializable
    /* editor:start */
    @editableProp(0)
    /* editor:end */
    set position(pos: Vec2) {
        let _this = this;
        _this.x = pos.x;
        _this.y = pos.y;
    }

    get position(): Readonly<Vec2> {
        let _this = this;
        if (!_this._position) {
            _this._position = new Vec2();
        }
        _this._position.set(this.x, this.y);
        return _this._position;
    }


    set scaleX(scaleX: number) {
        var _this = this;
        if (!math.equlas(_this.scaleX, scaleX, 0.001)) { //因为是倍数
            this.setToNativeMemeroy(8, scaleX);
            // _this.setToNative(2, scaleX, _this.scaleY);
            this.setDirty(TransformBit.SCALE);
        }
    }

    get scaleX() {
        return this.getNativeMemeroy(8);
    }

    set scaleY(scaleY) {
        var _this = this;
        if (!math.equlas(_this.scaleY, scaleY, 0.001)) {
            this.setToNativeMemeroy(12, scaleY);
            // _this.setToNative(2, _this.scaleX, scaleY);
            this.setDirty(TransformBit.SCALE);
        }
    }

    get scaleY() {
        return this.getNativeMemeroy(12);
    }

    @type(Vec2)
    @serializable
    /* editor:start */
    @editableProp(1)
    /* editor:end */
    set scale(scale: Vec2 | number) {
        var _this = this;

        if (typeof scale === "number") {
            _this.scaleX = scale;
            _this.scaleY = scale;
        } else {
            _this.scaleX = scale.x;
            _this.scaleY = scale.y;
        }
    }

    get scale(): Readonly<Vec2> {
        var _this = this;
        if (!_this._scale) {
            _this._scale = new Vec2();
        }
        _this._scale.set(_this.scaleX, _this.scaleY);
        return _this._scale;
    }


    /**
     * 旋转：
     * 负数表示顺时针旋转
     * 正数表示逆时针旋转
     */
    @type(Vec2)
    @serializable
    set rotate(rotate: number | Vec2) {
        if (rotate instanceof Vec2) {
            this.rotateX = rotate.x;
            this.rotateY = rotate.y;
        } else {
            this.rotateX = rotate;
            this.rotateY = rotate;
        }
    }

    /**
     * 旋转累加
     */
    set rotateBy(r: number) {
        this.setDirty(TransformBit.ROTATION);
        this.nativeBeginOp(11, true);
        this.nativeWriteOpArg("f32", r);
        this.nativeEndOp();
    }

    set rotateX(rotateX) {
        var _this = this;
        if (!math.equlas(_this.rotateX, rotateX, 0.01)) { //因为是角度
            this.setToNativeMemeroy(16, rotateX);
            // _this.setToNative(3, rotateX, _this.rotateY);
            this.setDirty(TransformBit.ROTATION);
        }
    }

    get rotateX() {
        return this.getNativeMemeroy(16);
    }

    set rotateY(rotateY) {
        var _this = this;
        if (!math.equlas(_this.rotateY, rotateY, 0.01)) {
            this.setToNativeMemeroy(20, rotateY);
            this.setDirty(TransformBit.ROTATION);
            // _this.setToNative(3, _this.rotateX, rotateY);
        }
    }

    get rotateY() {
        return this.getNativeMemeroy(20);
    }

    getWorldPosition(out?: Vec2): Vec2 {
        let t: Readonly<Mat3> = this.getWorldTransform();
        out = out || new Vec2();
        t.getTranslation(out);
        return out;
    }

    @bpFunction(null, {
        type: Mat3
    })
    getWorldTransform(): Readonly<Mat3> {
        this.refreshNativeTransform();

        if (!this._worldMat) {
            this._worldMat = new Mat3();
        }
        let data: Array<number> = this._worldMat.data;
        data[0] = this.getNativeMemeroy(24);
        data[1] = this.getNativeMemeroy(28);
        data[2] = this.getNativeMemeroy(32);
        data[3] = this.getNativeMemeroy(36);
        data[4] = this.getNativeMemeroy(40);
        data[5] = this.getNativeMemeroy(44);
        return this._worldMat;
    }

    @bpFunction([
        {
            type: Mat3,
            name: "mat"
        }
    ])
    setWorldTransform(mat: Mat3) {
        this.setDirty(TransformBit.TRS);
        let data: Array<number> = mat.data;
        this.nativeBeginOp(10);
        this.nativeWriteOpArg("f32", data[0]);
        this.nativeWriteOpArg("f32", data[1]);
        this.nativeWriteOpArg("f32", data[2]);
        this.nativeWriteOpArg("f32", data[3]);
        this.nativeWriteOpArg("f32", data[4]);
        this.nativeWriteOpArg("f32", data[5]);
        this.nativeEndOp();
    }

    @bpFunction([
        {
            type: SQFloat,
            name: "wx"
        },
        {
            type: SQFloat,
            name: "wy"
        }
    ])
    setWorldPosition(wx: number, wy: number) {
        this.setToNative(4, wx, wy);
    }

    /**
     * 在现在的位置上移动指定的距离
     * @param dx
     * @param dy
     */
    setWorldTranslate(dx: number, dy: number) {
        this.setToNative(5, dx, dy, true);
    }


    worldPositionToLocal(wx: number, wy: number, localPoint: Vec2): Vec2 {

        let worldMat: Readonly<Mat3> = this.getWorldTransform();
        Mat3.TEMP.copy(worldMat);
        Mat3.TEMP.invert();

        localPoint.x = wx;
        localPoint.y = wy;
        Mat3.TEMP.transformPoint(localPoint, localPoint);
        return localPoint;
    }

    localToWorldPosition(x: number, y: number, worldPosition: Vec2): Vec2 {
        let mat: Readonly<Mat3> = this.getWorldTransform();
        worldPosition.x = x;
        worldPosition.y = y;
        mat.transformPoint(worldPosition, worldPosition);
        return worldPosition;
    }

    setLocalTransform(mat: Mat3) {
        this.setDirty(TransformBit.TRS);
        let data: Array<number> = mat.data;
        this.nativeBeginOp(12);
        this.nativeWriteOpArg("f32", data[0]);
        this.nativeWriteOpArg("f32", data[1]);
        this.nativeWriteOpArg("f32", data[2]);
        this.nativeWriteOpArg("f32", data[3]);
        this.nativeWriteOpArg("f32", data[4]);
        this.nativeWriteOpArg("f32", data[5]);
        this.nativeEndOp();
    }

    private setToNativeMemeroy(offset: number, value: number, type: string = "float") {
        this.checkNativePointer();
        window.Module.setValue(this._nativePointer + offset, value, type);
    }

    private getNativeMemeroy(offset: number, type: string = "float") {
        this.checkNativePointer();
        return window.Module.getValue(this._nativePointer + offset, type);
    }

    private setToNative(op: number, x: number, y: number, ignoreRepeat: boolean = false) {
        this.nativeBeginOp(op, ignoreRepeat);
        this.nativeWriteOpArg("f32", x);
        this.nativeWriteOpArg("f32", y);
        this.nativeEndOp();
    }
}