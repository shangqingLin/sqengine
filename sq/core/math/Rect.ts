import { sqclass, type, serializable, SQFloat } from "../data/index";

@sqclass("sq.Rect")
export default class Rect {

    @type(SQFloat)
    @serializable
    public x: number = 0;

    @type(SQFloat)
    @serializable
    public y: number = 0;

    @type(SQFloat)
    @serializable
    public width: number = 0;

    @type(SQFloat)
    @serializable
    public height: number = 0;

    constructor(x?: number, y?: number, width?: number, height?: number) {
        this.set(x, y, width, height);
    }

    set(x: number, y: number, width: number, height: number) {
        this.x = x || 0;
        this.y = y || 0;
        this.width = width || 0;
        this.height = height || 0;
    }

    copyFrom(rect: Rect): void {
        this.x = rect.x;
        this.y = rect.y;
        this.width = rect.width;
        this.height = rect.height;
    }

    isZero(): boolean {
        return this.x == 0 && this.y === 0 && this.width === 0 && this.height === 0;
    }

    get xMin(): number {
        return this.x;
    }

    set xMin(value) {
        this.width += this.x - value;
        this.x = value;
    }


    get yMin(): number {
        return this.y;
    }

    set yMin(value) {
        this.height += this.y - value;
        this.y = value;
    }

    get xMax(): number {
        return this.x + this.width;
    }

    set xMax(value) {
        this.width = value - this.x;
    }

    get yMax(): number {
        return this.y + this.height;
    }

    set yMax(value) {
        this.height = value - this.y;
    }
}