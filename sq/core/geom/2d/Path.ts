import { Texture2d } from "script/engine/sq/assets";
import { Vec2 } from "../../math";
import Capsule from "./Capsule";
import Circle from "./Circle";
import Ellipse from "./Ellipse";
import Polygon from "./Polygon";
import Rectangle from "./Rectangle";
import ShapePrimitive from "./ShapePrimitive";

export default class Path extends ShapePrimitive {
    private shapePrimitives: Array<ShapePrimitive> = [];
    private _currentPolygonShape: ShapePrimitive;
    private dirty: boolean;
    private addPoint(x: number, y: number) {
        this._currentPolygonShape.points.push(new Vec2(x, y));
    }

    public moveTo(x: number, y: number) {
        this.dirty = true;
        this._currentPolygonShape = new Polygon();
        this.shapePrimitives.push(this._currentPolygonShape);
        this.addPoint(x, y);
    }

    public lineTo(x: number, y: number) {
        this.addPoint(x, y);
    }

    public updateLinePoint(pointIndex: number, x: number, y: number) {
        let polygon: Polygon = this.shapePrimitives[0] as Polygon;
        polygon.points[pointIndex].x = x;
        polygon.points[pointIndex].y = y;
    }

    public insertAfterLinePoint(insertAfterpointIndex: number, x: number, y: number) {
        // let polygon:Polygon = shapePrimitives[0] as Polygon;
        // if (polygon -> points.capacity() < (insertAfterpointIndex < 0 ? 1 : insertAfterpointIndex + 1)) {
        //     polygon -> points.reserve(polygon -> points.capacity() + 10);
        // }
        // auto it = polygon -> points.begin() + insertAfterpointIndex + 1;
        // polygon -> points.insert(it, Vec2(x, y));
    }

    public deleteLinePoint(pointIndex: number) {
        let polygon: Polygon = this.shapePrimitives[0] as Polygon;
        // polygon.points.erase(polygon -> points.begin() + pointIndex);
    }

    private endShape(closed: boolean = false) {
        if (this._currentPolygonShape) {
            this._currentPolygonShape.closed = closed;
            this._currentPolygonShape = null;
        }
    }

    public close(): void {
        this.endShape(true);
    }

    public bezierCurveTo(ctX1: number, ctY1: number, ctX2: number, ctY2: number, endX: number, endY: number) {
        let last = this._currentPolygonShape.points[this._currentPolygonShape.points.length - 1];
        // casteljauBerzier2D(last.x, last.y, ctX1, ctY1, ctX2, ctY2, endX, endY, 0, _currentPolygonShape -> points);
    }

    public drawEllipse(cx: number, cy: number, rx: number, ry: number) {
        this.endShape();
        this.dirty = true;
        // this.shapePrimitives.push(new Ellipse(cx, cy, rx, ry));
    }

    public drawCircle(cx: number, cy: number, r: number) {
        this.endShape();
        this.dirty = true;
        // this.shapePrimitives.push(new Circle(cx, cy, r));
    }

    public drawRect(x: number, y: number, w: number, h: number) {
        this.endShape();
        this.dirty = true;
        // this.shapePrimitives.push(new Rectangle(x, y, w, h));
    }

    public drawCapsule(c1x: number, c1y: number, c2x: number, c2y: number, radius: number) {
        this.endShape();
        this.dirty = true;
        // this.shapePrimitives.push(new Capsule(c1x, c1y, c2x, c2y, radius));
    }

    //     public build(std:: vector<Vec2> & points, std:: vector < unsigned short > & triangles, float strokeWidth)
    // {
    //     for (int i = 0; i < shapePrimitives.size(); ++i)
    //     {
    //         if (action == GraphicsPathAction::FILL)
    //         {
    //             shapePrimitives[i] -> fill(points, triangles);
    //         }
    //         else
    //         {
    //             shapePrimitives[i] -> stroke(points, triangles, strokeWidth);
    //         }
    //     }
    // }

    // public fill()
    // {
    //     action = GraphicsPathAction:: FILL;
    // }

    // public stroke()
    // {
    //     action = GraphicsPathAction:: STROKE;
    // }

    public clear() {
        this.shapePrimitives.length = 0;
    }

    override sampleStrokePoints(dist: number, normal?: boolean, normalType?: number): Float32Array {
        let data = new Float32Array(2000);
        let currentNum: number = 0;
        for (let i = 0; i < this.shapePrimitives.length; ++i) {
            let points: Float32Array = this.shapePrimitives[i].sampleStrokePoints(dist, normal, normalType);
            let remainSize: number = data.length - currentNum;
            if (remainSize > points.length) {
                data.set(points, currentNum);
            } else {
                let data1 = new Float32Array(data.length * 2);
                data1.set(data);
                data1.set(points, currentNum);
            }
            currentNum += points.length;
        }
        if (currentNum < data.length) {
            data = data.slice(0, currentNum);
        }
        return data;
    }

    override buildFillForTexture(): Texture2d {
        throw new Error("Method not implemented.");
    }
}