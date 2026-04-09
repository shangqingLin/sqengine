
/**
 * 二维向量
 * */

import { sqclass, type, serializable, SQFloat } from "../data/index";
import { CommonUtils } from "../utils";

@sqclass("sq.Vec2")
export default class Vec2 {


    public static temp1Vec2 = new Vec2();
    public static temp2Vec2 = new Vec2();

    @type(SQFloat)
    @serializable
    public x: number;

    @type(SQFloat)
    @serializable
    public y: number;

    constructor(x?: number, y?: number) {
        this.x = x || 0;
        this.y = y || 0;
    }

    /**
     * 向量加法
     * @param rhs
     */
    add(rhs: Vec2) {
        this.x += rhs.x;
        this.y += rhs.y;
        return this;
    }

    add2(lhs: Vec2, rhs: Vec2) {
        this.x = lhs.x + rhs.x;
        this.y = lhs.y + rhs.y;
        return this;
    }

    sub(rhs: Vec2) {
        this.x -= rhs.x;
        this.y -= rhs.y;
        return this;
    }


    sub2(lhs: Vec2, rhs: Vec2) {
        this.x = lhs.x - rhs.x;
        this.y = lhs.y - rhs.y;
        return this;
    }

    scale(scalar: number) {
        this.x *= scalar;
        this.y *= scalar;

        return this;
    }

    mul(rhs: Vec2) {
        this.x *= rhs.x;
        this.y *= rhs.y;
        return this;
    }

    mul2(lhs: Vec2, rhs: Vec2) {
        this.x = lhs.x * rhs.x;
        this.y = lhs.x * rhs.y;
        return this;
    }

    dot(rhs: Vec2) {
        return this.x * rhs.x + this.y * rhs.y;
    }

    cross(rhs: Vec2) {
        return this.x * rhs.y - this.y * rhs.x;
    }

    crossSV(s: number) {
        let x = this.x;
        this.x = this.y * -s;
        this.y = x * s;
        return this;
    }

    abs() {
        this.x = Math.abs(this.x);
        this.y = Math.abs(this.y);
        return this;
    }

    equals(rhs: Vec2): boolean {
        return this.x === rhs.x && this.y === rhs.y;
    }

    length(): number {
        return Math.sqrt(this.x * this.x + this.y * this.y);
    }

    lengthSq() {
        return this.x * this.x + this.y * this.y;
    }

    lerp(lhs: Vec2, rhs: Vec2, alpha: number) {
        this.x = lhs.x + alpha * (rhs.x - lhs.x);
        this.y = lhs.y + alpha * (rhs.y - lhs.y);
        return this;
    }


    normalize() {
        var lengthSq = this.lengthSq();
        if (lengthSq > 0) {
            var invLength = 1 / Math.sqrt(lengthSq);
            this.x *= invLength;
            this.y *= invLength;
        }
        return this;
    }


    set(x: number, y: number) {
        this.x = x;
        this.y = y;
        return this;
    }

    copy(vec: Vec2) {
        this.x = vec.x;
        this.y = vec.y;
        return this;
    }

    toString() {
        return "(" + this.x + "," + this.y + ")";
    }

    static cross(rv: Vec2, lv: Vec2): number {
        return rv.x * lv.y - lv.x * rv.y;
    }

    static dot(rv: Vec2, lv: Vec2): number {
        return rv.x * lv.x + rv.y * rv.y;
    }

    static min(vec1: Vec2, vec2: Vec2, result: Vec2): Vec2 {
        result.x = Math.min(vec1.x, vec2.x);
        result.y = Math.min(vec2.y, vec2.y);
        return result;
    }

    static max(vec1: Vec2, vec2: Vec2, result: Vec2): Vec2 {
        result.x = Math.max(vec1.x, vec2.x);
        result.y = Math.max(vec1.y, vec2.y);
        return result;
    }

    static isValid(a: Vec2): boolean {
        return CommonUtils.isDefine(a.x) && CommonUtils.isDefine(a.y);
    }

    static mulAdd(a: Vec2, s: number, b: Vec2, result?: Vec2): Vec2 {
        result = result || new Vec2();
        result.x = a.x + s * b.x;
        result.y = a.y + s * b.y
        return result;
    }

    static distanceSquared(a: Vec2, b: Vec2): number {
        let x: number = b.x - a.x;
        let y: number = b.y - a.y;
        return x * x + y * y;
    }

    static leftPerp(v: Vec2, out: Vec2) : Vec2 {
        let x = -v.y;
        out.y = v.x;
        out.x = x;
        return out;
    }

    static rightPerp(v: Vec2, out: Vec2): Vec2 {
        let x = v.y;
        out.y = -v.x;
        out.x = x;
        return out;
    }
}

