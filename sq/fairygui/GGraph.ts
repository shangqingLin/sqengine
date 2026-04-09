import { FillStyle, GraphicsComponent, StrokeStyle } from "../2d";
import { Color, math, Vec2 } from "../core";
import { ObjectPropID } from "./common/FieldTypes";
import { GObject } from "./GObject";

export class GGraph extends GObject {
    public _content: GraphicsComponent;

    private _type: number = 0;
    private _lineSize: number = 0;
    private _lineColor: Color;
    private _fillColor: Color;
    private _cornerRadius?: Array<number>;
    private _sides?: number;
    private _startAngle?: number;
    private _polygonPoints?: Array<number>;
    private _distances?: Array<number>;
    private _hasContent: boolean;

    public constructor() {
        super();
        this._node.name = "GGraph";
        this._lineSize = 1;
        this._lineColor = new Color();
        this._fillColor = new Color(255, 255, 255, 255);
    }

    protected override onCreateUIContent(): void {
        this._uiTrans = this._content = this._node.addComponent(GraphicsComponent);
    }

    public drawRect(lineSize: number, lineColor: Color, fillColor: Color, corner?: Array<number>): void {
        this._type = 1;
        this._lineSize = lineSize;
        this._lineColor.setFormColor(lineColor);
        this._fillColor.setFormColor(fillColor);
        this._cornerRadius = corner;
        this.updateGraph();
    }

    public drawEllipse(lineSize: number, lineColor: Color, fillColor: Color): void {
        this._type = 2;
        this._lineSize = lineSize;
        this._lineColor.setFormColor(lineColor);
        this._fillColor.setFormColor(fillColor);
        this.updateGraph();
    }

    public drawRegularPolygon(lineSize: number, lineColor: Color, fillColor: Color, sides: number, startAngle?: number, distances?: number[]): void {
        this._type = 4;
        this._lineSize = lineSize;
        this._lineColor.setFormColor(lineColor);
        this._fillColor.setFormColor(fillColor);
        this._sides = sides;
        this._startAngle = startAngle || 0;
        this._distances = distances;
        this.updateGraph();
    }

    public drawPolygon(lineSize: number, lineColor: Color, fillColor: Color, points: Array<number>): void {
        this._type = 3;
        this._lineSize = lineSize;
        this._lineColor.setFormColor(lineColor);
        this._fillColor.setFormColor(fillColor);
        this._polygonPoints = points;
        this.updateGraph();
    }

    public get distances(): number[] {
        return this._distances;
    }

    public set distances(value: number[]) {
        this._distances = value;
        if (this._type == 3)
            this.updateGraph();
    }

    public clearGraphics(): void {
        this._type = 0;
        if (this._hasContent) {
            this._content.clear();
            this._hasContent = false;
        }
    }

    public get type(): number {
        return this._type;
    }

    public get color(): Color {
        return this._fillColor;
    }

    public set color(value: Color) {
        this._fillColor.setFormColor(value);
        if (this._type !== 0)
            this.updateGraph();
    }

    private updateGraph(): void {
        let ctx = this._content;
        if (this._hasContent) {
            this._hasContent = false;
            ctx.clear();
        }

        var w: number = this._width;
        var h: number = this._height;
        if (w === 0 || h === 0)
            return;

        var px: number = -this.pivotX * this._width;
        var py: number = this.pivotY * this._height;

        let ls = this._lineSize / 2;


        if (this._type === 1) {
            if (this._cornerRadius) {
                ctx.drawRoundRect(px + ls, -h + py + ls, w - this._lineSize, h - this._lineSize, this._cornerRadius[0]);
            }
            else
                ctx.drawRect(px + ls, -h + py + ls, w - this._lineSize, h - this._lineSize);
        }
        else if (this._type === 2) {
            ctx.drawEllipse(w / 2 + px, -h / 2 + py, w / 2 - ls, h / 2 - ls);
        }
        else if (this._type === 3) {
            this.drawPath(ctx, this._polygonPoints, px, py);
        }
        else if (this._type === 4) {
            if (!this._polygonPoints)
                this._polygonPoints = [];
            var radius: number = Math.min(w, h) / 2 - ls;
            this._polygonPoints.length = 0;
            var angle: number = math.degreesToRadians(this._startAngle);
            var deltaAngle: number = 2 * Math.PI / this._sides;
            var dist: number;
            for (var i: number = 0; i < this._sides; i++) {
                if (this._distances) {
                    dist = this._distances[i];
                    if (isNaN(dist))
                        dist = 1;
                }
                else
                    dist = 1;

                var xv: number = radius + radius * dist * Math.cos(angle);
                var yv: number = radius + radius * dist * Math.sin(angle);
                this._polygonPoints.push(xv, yv);

                angle += deltaAngle;
            }

            this.drawPath(ctx, this._polygonPoints, px, py);
        }

        if (this._fillColor.a !== 0) {
            let style: FillStyle = {
                width: this._lineSize,
                color: this._fillColor
            } as FillStyle;
            ctx.fill(style);
        }

        
        if (ls !== 0) {
            let style: StrokeStyle = {
                width: this._lineSize,
                color: this._lineColor
            } as StrokeStyle;
            ctx.stroke(style);
        }


        this._hasContent = true;
    }

    private drawPath(ctx: GraphicsComponent, points: number[], px: number, py: number): void {
        var cnt: number = points.length;
        ctx.moveTo(points[0] + px, -points[1] + py);
        for (var i: number = 2; i < cnt; i += 2)
            ctx.lineTo(points[i] + px, -points[i + 1] + py);
        ctx.lineTo(points[0] + px, -points[1] + py);
    }

    protected override handleSizeChanged(): void {
        super.handleSizeChanged();

        if (this._type != 0)
            this.updateGraph();
    }

    protected override handleAnchorChanged(): void {
        super.handleAnchorChanged();

        if (this._type != 0)
            this.updateGraph();
    }

    public override getProp(index: number): any {
        if (index === ObjectPropID.Color)
            return this.color;
        else
            return super.getProp(index);
    }

    public override setProp(index: number, value: any): void {
        if (index === ObjectPropID.Color)
            this.color = value;
        else
            super.setProp(index, value);
    }

    protected _hitTest(pt: Vec2): GObject {
        if (pt.x >= 0 && pt.y >= 0 && pt.x < this._width && pt.y < this._height) {
            if (this._type == 3) {
                let points = this._polygonPoints;
                let len: number = points.length / 2;
                let i: number;
                let j: number = len - 1;
                let oddNodes: boolean = false;
                let w: number = this._width;
                let h: number = this._height;

                for (i = 0; i < len; ++i) {
                    let ix: number = points[i * 2];
                    let iy: number = points[i * 2 + 1];
                    let jx: number = points[j * 2];
                    let jy: number = points[j * 2 + 1];
                    if ((iy < pt.y && jy >= pt.y || jy < pt.y && iy >= pt.y) && (ix <= pt.x || jx <= pt.x)) {
                        if (ix + (pt.y - iy) / (jy - iy) * (jx - ix) < pt.x)
                            oddNodes = !oddNodes;
                    }

                    j = i;
                }

                return oddNodes ? this : null;
            }
            else
                return this;
        }
        else
            return null;
    }
}