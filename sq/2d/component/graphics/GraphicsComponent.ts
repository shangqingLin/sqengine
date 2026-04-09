import { Color } from "../../../core/index";
import { ComponentType, dispatch } from "../../../native_binding/index";
import UIContentComponent from "../UIContentComponent";
import GraphicsShader from "./GraphicsShader";


/**
 * 填充绘制的样式
 */
export interface FillStyle {
    color?: Color;
}


interface StrokeAttributes {
    /**
     * 线条大小
     */
    width?: number;
}


/**
 * 线条的样式
 */
export interface StrokeStyle extends FillStyle, StrokeAttributes { }


export interface QueryShapeResult {

    // 如果点击是多边形的边，则记录边两个端点
    linePoint1Index: number;
    linePoint2Index: number;

    //在Path中的第几个
    gemoetryIndex: number;

    //记录是第几个path
    pathIndex: number;
};


/**
 * Graphics绘图会忽略Transform2DComponent中设置的锚点的，
 * 所以在指定坐标点的时候记得自己算上锚点的偏移
 */
export class GraphicsComponent extends UIContentComponent {

    constructor() {
        super(ComponentType.GraphicsComponent);
    }

    protected override onInitialize(): void {
        this.setShaderMaterial(GraphicsShader.getDefaultGraphicsMaterial());
    }

    public override setColor(color: Color): void {
        //绘图不能这样设置颜色，通过Style设置
    }

    public moveTo(x: number, y: number) {
        this.nativeBeginOp(50, true);
        this.nativeWriteOpArg("f32", x);
        this.nativeWriteOpArg("f32", y);
        this.nativeEndOp();
    }

    public lineTo(x: number, y: number) {
        this.nativeBeginOp(51, true);
        this.nativeWriteOpArg("f32", x);
        this.nativeWriteOpArg("f32", y);
        this.nativeEndOp();
    }

    /**
    * 更新使用linTo、MoveTo等函数绘制的图形中某一个点的坐标。
    * 这样不用清除整个图形就可以实现修改点的位置，提升绘制效率
    * @param pathIndex 
    * @param pointIndex 
    * @param x
    * @param y 
    */
    public updateLinePoint(pathIndex: number, pointIndex: number, x: number, y: number) {
        this.nativeBeginOp(52, true);
        this.nativeWriteOpArg("i32", pathIndex);
        this.nativeWriteOpArg("i32", pointIndex);
        this.nativeWriteOpArg("f32", x);
        this.nativeWriteOpArg("f32", y);
        this.nativeEndOp();
    }

    public fill(fillStyle?: FillStyle) {
        this.nativeBeginOp(53, true);
        let state = 0;
        let statePos = this.nativeWriteOpArg("i8", 0);
        if (fillStyle) {
            if (fillStyle.color) {
                state |= 1;
                this.nativeWriteOpArg("ui8", fillStyle.color.r);
                this.nativeWriteOpArg("ui8", fillStyle.color.g);
                this.nativeWriteOpArg("ui8", fillStyle.color.b);
                this.nativeWriteOpArg("ui8", fillStyle.color.a);
            }
        }
        let nativeObj = this.getNativeObject();
        nativeObj.rewriteArg(statePos, "i8", state);
        this.nativeEndOp();
    }

    public stroke(stye?: StrokeStyle) {
        this.nativeBeginOp(54, true);
        let state = 0;
        let statePos = this.nativeWriteOpArg("i8", 0);
        if (stye) {
            if (stye.color) {
                state |= 1;
                this.nativeWriteOpArg("ui8", stye.color.r);
                this.nativeWriteOpArg("ui8", stye.color.g);
                this.nativeWriteOpArg("ui8", stye.color.b);
                this.nativeWriteOpArg("ui8", stye.color.a);
            }

            if (stye.width) {
                state |= 2;
                this.nativeWriteOpArg("f32", stye.width);
            }
        }

        let nativeObj = this.getNativeObject();
        nativeObj.rewriteArg(statePos, "i8", state);
        this.nativeEndOp();
    }

    public drawCircle(centerX: number, centerY: number, r: number) {
        this.nativeBeginOp(55, true);
        this.nativeWriteOpArg("f32", centerX);
        this.nativeWriteOpArg("f32", centerY);
        this.nativeWriteOpArg("f32", r);
        this.nativeEndOp();
    }

    public drawEllipse(cx: number, cy: number, rx: number, ry: number) {
        this.nativeBeginOp(56, true);
        this.nativeWriteOpArg("f32", cx);
        this.nativeWriteOpArg("f32", cy);
        this.nativeWriteOpArg("f32", rx);
        this.nativeWriteOpArg("f32", ry);
        this.nativeEndOp();
    }

    public drawRect(x: number, y: number, w: number, h: number) {
        this.nativeBeginOp(57, true);
        this.nativeWriteOpArg("f32", x);
        this.nativeWriteOpArg("f32", y);
        this.nativeWriteOpArg("f32", w);
        this.nativeWriteOpArg("f32", h);
        this.nativeEndOp();
    }

    public drawRoundRect(x: number, y: number, w: number, h: number, cornerRadius: number) {

    }

    public clear() {
        this.nativeBeginOp(58);
        this.nativeEndOp();
    }

    public close() {
        this.nativeBeginOp(59, true);
        this.nativeEndOp();
    }

    /**
     * 通过指定本地坐标点，查询这个点位于哪个绘制的图形上
     * @param localX 
     * @param localX 
     */
    public queryGemoetry(localX: number, localY: number): QueryShapeResult {
        this.nativeBeginOpSysc(60);
        this.nativeWriteOpArg("f32", localX);
        this.nativeWriteOpArg("f32", localY);
        this.nativeEndOp();

        let nativeBuffer = dispatch.getNativeToJsByte();
        let result: QueryShapeResult = {
            gemoetryIndex: nativeBuffer.nativeByte.readInt32(),
            pathIndex: nativeBuffer.nativeByte.readInt32(),
            linePoint1Index: nativeBuffer.nativeByte.readInt32(),
            linePoint2Index: nativeBuffer.nativeByte.readInt32()
        }
        return result;
    }

    insertAfterLinePoint(pathIndex: number, insertAfterpointIndex: number, x: number, y: number) {
        this.nativeBeginOp(61, true);
        this.nativeWriteOpArg("i32", pathIndex);
        this.nativeWriteOpArg("i32", insertAfterpointIndex);
        this.nativeWriteOpArg("f32", x);
        this.nativeWriteOpArg("f32", y);
        this.nativeEndOp();
    }

    deleteLinPoint(pathIndex: number, pointIndex: number): void {
        this.nativeBeginOp(62, true);
        this.nativeWriteOpArg("i32", pathIndex);
        this.nativeWriteOpArg("i32", pointIndex);
        this.nativeEndOp();
    }

    drawCapsule(c1x: number, c1y: number, c2x: number, c2y: number, radius: number): void {
        this.nativeBeginOp(63, true);
        this.nativeWriteOpArg("f32", c1x);
        this.nativeWriteOpArg("f32", c1y);
        this.nativeWriteOpArg("f32", c2x);
        this.nativeWriteOpArg("f32", c2y);
        this.nativeWriteOpArg("f32", radius);
        this.nativeEndOp();
    }
}