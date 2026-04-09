import { Texture2d } from "script/engine/sq/assets";
import { Vec2 } from "../../math";

export default abstract class ShapePrimitive {
    public points: Array<Vec2> = [];
    public closed: boolean;

    /**
     * 将生成的顶点存储到一张纹理中
     */
    abstract buildFillForTexture(): Texture2d;


    /**
     * 取样图元的边
     * @param dist 
     * @param normal 是否为每个点生成法线
     * @param normalType 生成法线的类型
     *  1：法线指向图形外部
     *  2：法线指向图形内部
     */
    abstract sampleStrokePoints(dist: number, normal?: boolean, normalType?: number): Float32Array;

    getPointsNum(): number {
        return this.points.length;
    }
}