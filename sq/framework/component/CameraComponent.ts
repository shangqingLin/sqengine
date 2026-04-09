import Component from "./Component";
import { ComponentType, dispatch, WasmByte } from "../../native_binding/index";
import { ClearFlags } from "../../gfx/index";
import { Color, editorMenu, Rect, serializable, sqclass, Vec2, type, SQFloat, editableProp } from "../../core/index";
import { RenderTexutre } from "../../assets/index";
import { Transform2DComponent } from "../../2d";
import Mat4 from "../../core/math/Mat4";

export enum CameraType {
    ORTHO,
    PERSPECTIVE
}

/* editor:start */
@editorMenu("2D/CameraComponent")
/* editor:end */
@sqclass("sq.CameraComponent")
export class CameraComponent extends Component {
    private _fov: number;
    private _near: number;
    private _far: number;
    private _cameraType: CameraType;
    private _aspect: number;
    private _viewport: Rect;
    private _clearFlag = ClearFlags.NONE;
    private _clearColor: Color;
    private _clearDepth = 1.0;
    private _clearStencil = 0;
    private _orthoHeight: number;
    private _renderTexture: RenderTexutre;
    private _priority: number;
    private _viewMat: Mat4;
    constructor() {
        super();
        this.nativeType = ComponentType.CameraComponent;
    }

    @type(SQFloat)
    @serializable
    /*editor:start*/
    @editableProp(0)
    /*editor:end*/
    set fov(fov: number) {
        if (this._fov !== fov) {
            this._fov = fov;
            this.nativeBeginOp(1);
            this.nativeWriteOpArg("f32", fov);
            this.nativeEndOp();
        }
    }

    get fov(): number {
        return this._fov;
    }

    @type(SQFloat)
    @serializable
    /*editor:start*/
    @editableProp(1)
    /*editor:end*/
    set near(near: number) {
        if (this._near !== near) {
            this._near = near;
            this.nativeBeginOp(2);
            this.nativeWriteOpArg("f32", near);
            this.nativeEndOp();
        }
    }
    get near(): number {
        return this._near === undefined ? 1 : this._near;
    }

    @type(SQFloat)
    @serializable
    /*editor:start*/
    @editableProp(2)
    /*editor:end*/
    set far(far: number) {
        if (this._far !== far) {
            this._far = far;
            this.nativeBeginOp(3);
            this.nativeWriteOpArg("f32", far);
            this.nativeEndOp();
        }
    }

    get far(): number {
        return this._far === undefined ? 1000 : this._far;
    }

    @type(SQFloat)
    @serializable
    set aspect(aspect: number) {
        if (this._aspect !== aspect) {
            this._aspect = aspect;
            this.nativeBeginOp(4);
            this.nativeWriteOpArg("f32", aspect);
            this.nativeEndOp();
        }
    }

    get aspect(): number {
        return this._aspect === undefined ? 16 / 9 : this._aspect;
    }

    @type(CameraType)
    @serializable
    /*editor:start*/
    @editableProp(0)
    /*editor:end*/
    set cameraType(type: CameraType) {
        if (this._cameraType !== type) {
            this._cameraType = type;
            this.nativeBeginOp(5);
            this.nativeWriteOpArg("i8", type);
            this.nativeEndOp();
        }
    }

    get cameraType(): CameraType {
        return this._cameraType;
    }

    @type(Rect)
    @serializable
    /*editor:start*/
    @editableProp(5)
    /*editor:end*/
    set viewport(rect: Rect) {
        let self = this;
        if (!self._viewport) {
            self._viewport = new Rect();
        }

        if (
            self._viewport.x === rect.x
            && self._viewport.y === rect.y
            && self._viewport.width === rect.width
            && self._viewport.height === rect.height) {
            return;
        }

        self._viewport.x = rect.x;
        self._viewport.y = rect.y;
        self._viewport.width = rect.width;
        self._viewport.height = rect.height;

        self.nativeBeginOp(6);
        self.nativeWriteOpArg("f32", self._viewport.x);
        self.nativeWriteOpArg("f32", self._viewport.y);
        self.nativeWriteOpArg("f32", self._viewport.width);
        self.nativeWriteOpArg("f32", self._viewport.height);
        self.nativeEndOp();
    }


    /**
     * 快捷设置 平行投影 投影平面的方法
     * @param width 
     * @param height 
     */
    setOrthoScreenSize(width: number, height: number) {
        let halfW = width * 0.5;
        let halfH = height * 0.5;
        this.viewport = new Rect(0, 0, width, height);
        this.aspect = halfW / halfH;
        this.orthoHeight = halfH;
    }

    get viewport(): Rect {
        return this._viewport;
    }

    get clearFlag(): ClearFlags {
        return this._clearFlag;
    }

    set clearFlag(flag: ClearFlags) {
        if (this._clearFlag !== flag) {
            this._clearFlag = flag;
            this.nativeBeginOp(7);
            this.nativeWriteOpArg("i32", flag);
            this.nativeEndOp();
        }
    }


    set clearColor(val: Color) {

        if (!this._clearColor) {
            this._clearColor = new Color();
        }

        if (
            this._clearColor.r === val.r
            && this._clearColor.g === val.g
            && this._clearColor.b === val.b
            && this._clearColor.a === val.a) {
            return;
        }

        this._clearColor.r = val.r;
        this._clearColor.g = val.g;
        this._clearColor.b = val.b;
        this._clearColor.a = val.a;

        this.nativeBeginOp(8);
        this.nativeWriteOpArg("f32", this._clearColor.r);
        this.nativeWriteOpArg("f32", this._clearColor.g);
        this.nativeWriteOpArg("f32", this._clearColor.b);
        this.nativeWriteOpArg("f32", this._clearColor.a);
        this.nativeEndOp();

    }

    get clearColor(): Color {
        return this._clearColor;
    }


    get clearDepth(): number {
        return this._clearDepth;
    }
    set clearDepth(depth: number) {
        if (this._clearDepth !== depth) {
            this._clearDepth = depth;
            this.nativeBeginOp(9);
            this.nativeWriteOpArg("f32", depth);
            this.nativeEndOp();
        }
    }


    get clearStencil(): number {
        return this._clearStencil;
    }
    set clearStencil(stencil: number) {
        if (this._clearStencil !== stencil) {
            this._clearStencil = stencil;
            this.nativeBeginOp(10);
            this.nativeWriteOpArg("i32", stencil);
            this.nativeEndOp();
        }
    }

    @type(SQFloat)
    @serializable
    /*editor:start*/
    @editableProp(6)
    /*editor:end*/
    set orthoHeight(h: number) {
        if (this._orthoHeight !== h) {
            this._orthoHeight = h;
            this.nativeBeginOp(11);
            this.nativeWriteOpArg("f32", h);
            this.nativeEndOp();
        }
    }
    get orthoHeight(): number {
        return this._orthoHeight === undefined ? 10 : this._orthoHeight;
    }


    /**
     * 将屏幕空间下的坐标转换到照相下
     * sx和sy是以左下角为原点，Y轴向上为正，X轴向右为正的像素坐标系
     * @param sx 
     * @param sy 
     * @param out 
     */
    screenToWorld(sx: number, sy: number, out: Vec2) {
        this.nativeBeginOpSysc(13);
        this.nativeWriteOpArg("f32", sx);
        this.nativeWriteOpArg("f32", sy);
        this.nativeEndOp();
        let nativeBuffer = dispatch.getNativeToJsByte();
        out.x = nativeBuffer.nativeByte.readFloat32();
        out.y = nativeBuffer.nativeByte.readFloat32();
    }

    /**
     * 如果设置了renderTexture，则该Camera及其相关节点的内容都渲染到这个纹理上
     * 实现渲染到纹理的离屏渲染功能
     */
    set renderTexture(texture: RenderTexutre) {
        if (this._renderTexture !== texture) {
            this._renderTexture = texture;
            this.nativeBeginOp(14);
            this.nativeWriteOpArg("i32", texture.getId())
            this.nativeEndOp();
        }
    }

    get renderTexture(): RenderTexutre {
        return this._renderTexture;
    }

    setEnable(enable: boolean) {
        this.nativeBeginOp(15);
        this.nativeWriteOpArg("i8", enable ? 1 : 0);
        this.nativeEndOp();
    }

    set priority(priority: number) {
        if (this._priority === priority)
            return;
        this._priority = priority;
        this.nativeBeginOp(16);
        this.nativeWriteOpArg("i32", priority);
        this.nativeEndOp();
    }

    get priority(): number {
        return this._priority || 0;
    }

    getViewProjMat(): Readonly<Mat4> {
        let transform = this.node.getComponent(Transform2DComponent);
        transform.getWorldTransform(); //刷新一下位置

        this.nativeBeginOpSysc(17);
        this.nativeEndOp();

        if (!this._viewMat) {
            this._viewMat = new Mat4();
        }
        let byte: WasmByte = dispatch.getNativeToJsByte().beginReadSyncData();
        for (let i = 0; i < 16; ++i) {
            this._viewMat.data[i] = byte.readFloat32();
        }
        dispatch.getNativeToJsByte().endReadSyncData();
        return this._viewMat;
    }
}
