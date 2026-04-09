import math from "./math";
import Vec2 from "./Vec2";

/**
 *
 * 3X3矩阵
 * 这里使用3行2列矩阵来模拟3X3矩阵，对于图形学来说，不需要使用真正的3X3矩阵,因为最后一列总是认为是(0,0,1)
 *
 * 矩阵排列方式
 * [
 *   1, 0, 0,
 *   0, 1, 0,
 *   0, 0, 1
 * ]
 * @param v0 0
 * @param v1 1
 * @param v2 2
 * @param v3 3
 * @param v4 4
 * @param v5 5
 * @constructor
 */
export default class Mat3 {

    private _data: Array<number>;

    public static readonly TEMP: Mat3 = new Mat3();

    constructor() {
        this._data = new Array(6);
        this.setIdentity();
    }

    get data() {
        return this._data;
    }

    /**
     *
     * @param t 位移
     * @param r 旋转角度，单位为角度
     * @param s 缩放
     */
    setTRS(t: Vec2, r: Vec2, s: Vec2) {

        var srx, crx, sry, cry, rad;
        if (r.x === 0) {
            srx = 0;
            crx = 1;
        } else {
            rad = r.x * math.DEG_TO_RAD;
            srx = Math.sin(rad);
            crx = Math.cos(rad);
        }

        if (r.y === 0) {
            sry = 0;
            cry = 1;
        } else if (r.y === r.x) {
            sry = srx;
            cry = crx;
        } else {
            rad = r.y * math.DEG_TO_RAD;
            sry = Math.sin(rad);
            cry = Math.cos(rad);
        }

        this.data[0] = cry * s.x;
        this.data[2] = -sry * s.x;

        this.data[1] = srx * s.y;
        this.data[3] = crx * s.y;

        this.data[4] = t.x;
        this.data[5] = t.y;
    }

    mul2(lmat: Mat3, rmat: Mat3) {
        var a0, a1, a2, a3, a4, a5, b0, b1,
            a = lmat.data,
            b = rmat.data,
            r = this.data;


        a0 = a[0];
        a1 = a[1];

        a2 = a[2];
        a3 = a[3];

        a4 = a[4];
        a5 = a[5];

        //第一行第一列
        b0 = b[0];
        b1 = b[1];
        r[0] = a0 * b0 + a2 * b1;

        //第一行第二列
        r[1] = a1 * b0 + a3 * b1;

        //第二行第一列
        b0 = b[2];
        b1 = b[3];
        r[2] = a0 * b0 + a2 * b1;

        //第二行第二列
        r[3] = a1 * b0 + a3 * b1;


        //第三行第一列
        b0 = b[4];
        b1 = b[5];
        r[4] = a0 * b0 + a2 * b1 + a4;

        //第三行第二列
        r[5] = a1 * b0 + a3 * b1 + a5;

        return this;
    }

    mul(rhs: Mat3) {
        return this.mul2(this, rhs);
    }


    setIdentity() {
        var m = this.data;
        m[0] = 1;
        m[1] = 0;

        m[2] = 0;
        m[3] = 1;

        m[4] = 0;
        m[5] = 0;

        return this;
    }

    setTranslate(tx: number, ty: number) {
        let data = this.data;
        data[0] = data[3] = 1;
        data[1] = data[2] = 0;
        data[4] = tx;
        data[5] = ty;
    }

    translate(x: number, y: number) {
        var data = this.data;
        data[4] = data[0] * x +
            data[2] * y +
            data[4];

        data[5] =
            data[1] * x +
            data[3] * y +
            data[5];
        return this;
    }

    getScale(out: Vec2): Vec2 {
        var data = this.data;
        out.x = Math.sqrt(data[0] * data[0] + data[2] * data[2]);
        out.y = Math.sqrt(data[1] * data[1] + data[3] * data[3]);
        return out;
    }

    getTranslation(out: Vec2): Vec2 {
        out.x = this.data[4];
        out.y = this.data[5];
        return out;
    }


    getRotation(out: Vec2, rad: boolean = true): Vec2 {

        // Step 1: 推算缩放因子
        this.getScale(out);

        // Step 2: 去除缩放影响
        let crx = this.data[0] / out.x;
        let sry = this.data[1] / out.x;
        let srx = -this.data[2] / out.y;
        let cry = this.data[3] / out.y;

        // Step 3: 提取旋转角度
        out.x = Math.atan2(srx, crx);  // 提取 Rotate X
        out.y = Math.atan2(sry, cry);  // 提取 Rotate Y
        if (!rad) {
            out.x = out.x * math.RAD_TO_DEG;
            out.y = out.y * math.RAD_TO_DEG;
        }
        return out;
    }


    setScale(x: number, y: number) {
        this.data[0] = x;
        this.data[3] = y;
    }

    scale(x: number, y: number) {
        this.data[0] *= x;
        this.data[3] *= y;
        this.data[2] *= x;
        this.data[1] *= y;
        this.data[4] *= x;
        this.data[5] *= y;
    }

    setRotate(angle: number) {
        var cos = Math.cos(angle);
        var sin = Math.sin(angle);
        var data = this.data;
        var a0 = data[0];
        var a1 = data[1];
        var a2 = data[2];
        var a3 = data[3];
        var a4 = data[4];
        var a5 = data[5];

        // [
        //     a0, a1, 0,
        //     a2, a3, 0,
        //     a4, a5, 1,
        // ]

        // [
        //     cos, sin, 0,
        //     -sin, cos, 0,
        //      0, 0,      1 ,     
        // ]

        this.data[0] = a0 * cos + a2 * sin;
        this.data[1] = a1 * cos + a3 * sin;
        this.data[2] = a0 * -sin + a2 * cos;
        this.data[3] = a1 * -sin + a3 * cos;
        this.data[4] = a4;
        this.data[5] = a5;
    }

    /**
     * 将一个点与这个矩阵进行相乘,也就是对一个点进行位移，缩放和旋转，得到新的点
     * @param vec {Vec2} 2维空间的点
     * @param res {Vec2}
     * @returns {*}
     */
    transformPoint(vec: Vec2, res?: Vec2) {
        var x, y, m = this.data;

        res = (res === undefined) ? new Vec2() : res;

        x =
            vec.x * m[0] +
            vec.y * m[2] +
            m[4]; //x轴的平移

        y =
            vec.x * m[1] +
            vec.y * m[3] +
            m[5]; //y轴的平移

        return res.set(x, y);
    }

    /**
     *
     * @param vec {Vec2}
     * @param res {Vec2}
     */
    transformVector(vec: Vec2, res: Vec2) {
        var x, y, m = this.data;

        res = (res === undefined) ? new Vec2() : res;

        x = vec.x * m[0] + vec.y * m[2];
        y = vec.x * m[1] + vec.y * m[3];

        return res.set(x, y);
    }

    /**
     * 将当前的矩阵设置为逆矩阵
     * @returns {*}
     */
    invert() {

        var me = this.data,
            te = this.data,

            n11 = me[0],
            n21 = me[1],

            n12 = me[2],
            n22 = me[3],

            n13 = me[4],
            n23 = me[5];

        var det = n11 * n22 - n21 * n12;
        if (det === 0) {
            return this.setIdentity();
        }

        var detInv = 1 / det;

        te[0] = n22 * detInv;
        te[1] = -n21 * detInv;

        te[2] = -n12 * detInv;
        te[3] = n11 * detInv;

        te[4] = (n23 * n12 - n22 * n13) * detInv;
        te[5] = (n21 * n13 - n23 * n11) * detInv;
        return this;
    }

    copy(rhs: Readonly<Mat3>) {
        var src = rhs.data;
        var dst = this.data;

        dst[0] = src[0];
        dst[1] = src[1];
        dst[2] = src[2];
        dst[3] = src[3];
        dst[4] = src[4];
        dst[5] = src[5];
        return this;
    }

    clone() {
        return new Mat3().copy(this);
    }

    toString() {
        var t = [];
        for (var i = 0; i < 6; i++) {
            t.push(this.data[i]);
        }
        return t.join(",");
    }
}

