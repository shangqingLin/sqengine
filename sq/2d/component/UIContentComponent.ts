import { Color, Size, SQFloat, Vec2 } from "../../core/index";
import { sqclass, type, serializable, editableProp } from "../../core/index";
import { ComponentType, dispatch } from "../../native_binding/index";
import { Application } from "../../framework/Application";
import NodeEventType from "../../scene/NodeEventType";
import { RenderComponent } from "../../framework/component/RenderComponent";
import { StencilStage } from "../../rendering";

/**
 * 子类的op值需要从50开始，50以下由UIContentComponent占用
 */
@sqclass("sq.UIContentComponent")
export default class UIContentComponent extends RenderComponent {
    protected _contentSize: Size;
    private _anchor: Vec2;
    private _pivot: Vec2;
    private _uiNativePropsPointer: number;
    private dirtySet: boolean = false;
    private _color: Color;
    private _stencilStage: StencilStage;

    constructor(nativeType?: number) {
        super(nativeType || ComponentType.UIContentComponent);
    }

    //目前这种方式很有问题，特别是同一个节点中多个可渲染的组件，不小心会多个组件间找到相同的UIContentComponent实例
    private checkNativePointer() {
        if (!this._uiNativePropsPointer) {
            //保证addComponet的时候，nativePointer已经存在
            dispatch.flushJsToNative();

            //@ts-ignore
            let nativeObject: JsToNativeObjectBridge = this.node.nativeObject;
            this._uiNativePropsPointer = window.Module.ccall("getNodeComponentPointer", "number", ["number", "number"], [this.nativeType, nativeObject.getId()]);
        }
    }

    private setDirty() {
        if (!this.dirtySet) {
            this.dirtySet = true;
            this.nativeBeginOp(24);
            this.nativeEndOp();
            Application.ins.postTimer.frameOnce(1, this, this.clearFlag);
        }
    }

    protected override onRemove(): void {
        Application.ins.timer.clearAll(this);
    }

    private clearFlag() {
        this.dirtySet = false;
    }

    set anchorX(anchorX) {
        var _this = this;
        if (!_this._anchor) {
            _this._anchor = new Vec2(0.5, 0.5);
        }

        if (_this._anchor.x !== anchorX) {
            _this._anchor.x = anchorX;
            // this.setToNative(21, this._anchor.x, this._anchor.y);
            _this.setToNativeMemeroy(0, anchorX);
            // if (!_this._uiNativePropsPointer) {
            //     _this.setToNative(1, _this._anchor.x, _this._anchor.y);
            // }
            this.node.fire(NodeEventType.ANCHOR_CHANGE);
        }
    }


    get anchorX() {
        return this.getNativeMemeroy(0);
    }

    set anchorY(anchorY) {
        var _this = this;
        if (!_this._anchor) {
            _this._anchor = new Vec2(0.5, 0.5);
        }
        if (_this._anchor.y !== anchorY) {
            _this._anchor.y = anchorY;
            // _this.setToNative(21, _this._anchor.x, _this._anchor.y);
            _this.setToNativeMemeroy(4, anchorY);
            // if (!_this._uiNativePropsPointer) {
            //     _this.setToNative(1, _this._anchor.x, _this._anchor.y);
            // }
            this.node.fire(NodeEventType.ANCHOR_CHANGE);
        }
    }

    get anchorY() {
        return this.getNativeMemeroy(4);
    }

    @type(Vec2)
    @serializable
    /* editor:start */
    @editableProp(3)
    /* editor:end */
    set anchor(anchor: Vec2) {
        this.anchorX = anchor.x;
        this.anchorY = anchor.y;
    }

    get anchor(): Readonly<Vec2> {
        var _this = this;
        if (!_this._anchor) {
            _this._anchor = new Vec2();
        }
        _this._anchor.set(_this.anchorX, _this.anchorY);
        return _this._anchor;
    }

    setAnchorPoint(x: number, y: number) {
        this.anchorX = x;
        this.anchorY = y;
    }

    set pivotX(pivotX) {
        var _this = this;
        if (!_this._pivot) {
            _this._pivot = new Vec2();
        }

        if (_this._pivot.x !== pivotX) {
            _this._pivot.x = pivotX;
            // _this.setToNative(22, _this._pivot.x, _this._pivot.y);
            _this.setToNativeMemeroy(8, pivotX);
            // if (!_this._uiNativePropsPointer) {
            //     _this.setToNative(2, _this._pivot.x, _this._pivot.y);
            // }
        }
    }

    get pivotX() {
        return this.getNativeMemeroy(8);
    }

    set pivotY(pivotY) {
        var _this = this;
        if (!_this._pivot) {
            _this._pivot = new Vec2();
        }

        if (_this._pivot.y !== pivotY) {
            _this._pivot.y = pivotY;
            // _this.setToNative(22, _this._pivot.x, _this._pivot.y);
            _this.setToNativeMemeroy(12, pivotY);
            // if (!_this._uiNativePropsPointer) {
            //     _this.setToNative(2, _this._pivot.x, _this._pivot.y);
            // }
        }
    }

    get pivotY(): number {
        return this.getNativeMemeroy(12);
    }

    set pivot(pivot: Vec2) {
        this.pivotX = pivot.x;
        this.pivotY = pivot.y;
    }

    get pivot(): Readonly<Vec2> {
        var _this = this;
        if (!_this._pivot) {
            _this._pivot = new Vec2();
        }
        return _this._pivot;
    }

    @type(SQFloat)
    @serializable
    set width(w: number) {
        var _this = this;
        if (!_this._contentSize) {
            _this._contentSize = new Size();
        }
        if (_this._contentSize.width !== w) {
            _this._contentSize.width = w;
            // _this.setToNative(23, _this._contentSize.width, _this._contentSize.height);
            _this.setToNativeMemeroy(16, w);
            // if (!_this._uiNativePropsPointer) {
            //     _this.setToNative(3, _this._contentSize.width, _this._contentSize.height);
            // }
            Application.ins.timer.callLater(this, this.onSizeChange);
        }
    }

    get width(): number {
        // return this._contentSize ? this._contentSize.width : 0;
        return this.getNativeMemeroy(16);
    }

    @type(SQFloat)
    @serializable
    set height(h: number) {
        var _this = this;
        if (!_this._contentSize) {
            _this._contentSize = new Size();
        }
        if (_this._contentSize.height !== h) {
            _this._contentSize.height = h;
            // _this.setToNative(23, _this._contentSize.width, _this._contentSize.height);
            _this.setToNativeMemeroy(20, h);
            // if (!_this._uiNativePropsPointer) {
            //     _this.setToNative(3, _this._contentSize.width, _this._contentSize.height);
            // }
            Application.ins.timer.callLater(this, this.onSizeChange);
        }
    }
    get height(): number {
        return this.getNativeMemeroy(20);
    }

    getAnchorXInPix(): number {
        return this.anchorX * this.width;
    }

    getAnchorYInPix(): number {
        return this.anchorY * this.height;
    }

    private onSizeChange() {
        this.node.fire(NodeEventType.RESIZE);
    }



    @type(Size)
    @serializable
    /* editor:start */
    @editableProp(4)
    /* editor:end */
    set contentSize(value: Size) {
        if (!this._contentSize) {
            this._contentSize = new Size();
        }
        this.width = value.width;
        this.height = value.height;
    }

    get contentSize(): Readonly<Size> {
        var _this = this;
        if (!_this._contentSize) {
            _this._contentSize = new Size();
        }
        _this._contentSize.set(_this.width, _this.height);
        return _this._contentSize;
    }

    setContentSize(width: number, height: number): void {
        this.width = width;
        this.height = height;
    }

    private setToNativeMemeroy(offset: number, value: number) {
        this.checkNativePointer();
        this.setDirty();
        window.Module.setValue(this._uiNativePropsPointer + offset, value, "float");
    }

    private getNativeMemeroy(offset: number) {
        this.checkNativePointer();
        return window.Module.getValue(this._uiNativePropsPointer + offset, "float");
    }

    private setToNative(op: number, x: number, y: number, ignoreRepeat: boolean = false) {
        this.nativeBeginOp(op, ignoreRepeat);
        this.nativeWriteOpArg("f32", x);
        this.nativeWriteOpArg("f32", y);
        this.nativeEndOp();
    }

    /**
  * 设置颜色。这个颜色和当前节点的纹理颜色相乘
  * 设置的为[0,1]区间的浮点值
  * @param color 
  */
    public setColor(color: Color | string) {
        this.nativeBeginOp(30);
        let color1: Color = color as Color;
        if (typeof color === "string") {
            color1 = Color.strHexToColor(color);
            color1.r /= 255;
            color1.g /= 255;
            color1.b /= 255;
            color1.a /= 255;
        }
        this._color = color1;
        this.nativeWriteOpArg("f32", color1.r);
        this.nativeWriteOpArg("f32", color1.g);
        this.nativeWriteOpArg("f32", color1.b);
        this.nativeWriteOpArg("f32", color1.a);
        this.nativeEndOp();
    }

    getColor(): Color {
        return this._color;
    }

    get stencilStage(): StencilStage {
        return this._stencilStage;
    }

    set stencilStage(val: StencilStage) {
        if (this._stencilStage !== val) {
            this._stencilStage = val;
            this.nativeBeginOp(25);
            this.nativeWriteOpArg("i8", val);
            this.nativeEndOp();
        }
    }
}