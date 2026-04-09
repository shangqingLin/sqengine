import { Texture2d } from "script/engine/sq/assets";
import ShapePrimitive from "./ShapePrimitive";
import { Vec2 } from "../../math";

export default class Polygon extends ShapePrimitive {
    override sampleStrokePoints(dist: number, normal?: boolean, normalType?: number): Float32Array {
        let pointCount: number = this.points.length;
        if (pointCount == 0) return new Float32Array(0);

        if (pointCount == 1) {
            let data = new Float32Array(2);
            data[0] = this.points[0].x;
            data[1] = this.points[1].y;
            return data;
        }

        dist = dist || 1;

        let num: number = 0;

        {
            for (let i = 0; i < pointCount - 1; ++i) {
                let begin = this.points[i];
                let end = this.points[i + 1];
                num += Vec2.temp1Vec2.sub2(end, begin).length() / dist;
                if (normal) num += 1;
            }

            if (this.closed && pointCount > 2) {
                let begin = this.points[pointCount - 1];
                let end = this.points[0];
                num += Vec2.temp1Vec2.sub2(end, begin).length() / dist;
                if (normal) num += 1;
            }
        }

        let data = new Float32Array(num * 2);
        {
            let index = 0;
            let numberCount: number = 0;
            let padding = normal ? 4 : 2;
            let create = (begin: Vec2, end: Vec2) => {
                let lineLength = Vec2.temp1Vec2.sub2(end, begin).length();

                let direction = Vec2.temp1Vec2.normalize();
                let normalVec: Vec2 = normal ? Vec2.leftPerp(direction, Vec2.temp2Vec2) : null;

                if (lineLength <= dist) {

                    data[index++] = begin.x;
                    data[index++] = begin.y;
                    if (normalVec) {
                        data[index++] = normalVec.x;
                        data[index++] = normalVec.y;
                    }

                    data[index++] = end.x;
                    data[index++] = end.y;

                    if (normalVec) {
                        data[index++] = normalVec.x;
                        data[index++] = normalVec.y;
                    }
                    return;
                }
                
                let process = 0;
                while (true) {

                    if (numberCount + padding >= data.length) {
                        let data1 = new Float32Array(numberCount + 200);
                        data1.set(data);
                        data = data1;
                    }

                    if (process >= lineLength) {
                        numberCount += padding;
                        data[index++] = end.x;
                        data[index++] = end.y;

                        if (normalVec) {
                            data[index++] = normalVec.x;
                            data[index++] = normalVec.y;
                        }
                        break;
                    }

                    numberCount += padding;
                    data[index++] = begin.x + direction.x * process;
                    data[index++] = begin.y + direction.y * process;
                    if (normalVec) {
                        data[index++] = normalVec.x;
                        data[index++] = normalVec.y;
                    }
                    process += dist;

                }
            }

            for (let i = 0; i < pointCount - 1; ++i) {
                let begin = this.points[i];
                let end = this.points[i + 1];
                create(begin, end);
            }

            if (this.closed && pointCount > 2) {
                let begin = this.points[pointCount - 1];
                let end = this.points[0];
                create(begin, end);
            }

            if (data.length > numberCount) {
                data = data.slice(0, numberCount);
            }

            // console.info(data);

            return data;
        }
    }


    override buildFillForTexture(): Texture2d {
        throw new Error("Method not implemented.");
    }
}