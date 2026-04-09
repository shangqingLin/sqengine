import { Texture2d } from "script/engine/sq/assets";
import ShapePrimitive from "./ShapePrimitive";


export default class Circle extends ShapePrimitive
{
    override sampleStrokePoints(dist: number, normal?: boolean, normalType?: number): Float32Array {
        throw new Error("Method not implemented.");
    }
    override buildFillForTexture(): Texture2d {
        throw new Error("Method not implemented.");
    }
    
}