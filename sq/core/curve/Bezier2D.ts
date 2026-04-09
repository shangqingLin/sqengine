

/** 
*  三阶贝塞尔曲线公式:
*    B(t) = (1-t)³·P0 + 3(1-t)²t·P1 + 3(1-t)t²·P2 + t³·P3
*/
export default class Bezier2D {


    public static cubicBezier(t: number, p0: SimplePoint, p1: SimplePoint, p2: SimplePoint, p3: SimplePoint): SimplePoint {
        let one_minus_t: number = 1.0 - t;
        let one_minus_t2: number = one_minus_t * one_minus_t;
        let one_minus_t3: number = one_minus_t2 * one_minus_t;
        let t2: number = t * t;
        let t3: number = t2 * t;

        let a: number = 3.0 * t * one_minus_t2;
        let b: number = 3.0 * t2 * one_minus_t;
        return {
            x: p0.x * one_minus_t3 +
                p1.x * a +
                p2.x * b +
                p3.x * t3,
            y: p0.y * one_minus_t3 +
                p1.y * a +
                p2.y * b +
                p3.y * t3
        }
    }

    public static quadraticBezier(t: number, p0: SimplePoint, p1: SimplePoint, p2: SimplePoint): SimplePoint {
        let one_minus_t: number = 1.0 - t;
        let tMinus2: number = one_minus_t * one_minus_t;
        let t2: number = t * t;
        let a: number = 2 * t * one_minus_t;
        return {
            x: tMinus2 * p0.x + a * p1.x + t2 * p2.x,
            y: tMinus2 * p0.y + a * p1.y + t2 * p2.y
        }
    }

    /**
     * 求二阶贝塞尔曲线上的控制点
     * @param p0 起点
     * @param p1 过曲线上的一个点
     * @param p3 终点
     * @returns 
     */
    public static quadraticBezierControlPoint(p0: SimplePoint, p1: SimplePoint, p3: SimplePoint): SimplePoint {
        //x轴作为t,同时也表示曲线上的x坐标，表示t进度必须到达这一点上
        let t: number = p1.x;
        let one_minus_t: number = 1.0 - t;

        let tMinus2: number = one_minus_t * one_minus_t;
        let p0x = tMinus2 * p0.x;
        let p0y = tMinus2 * p0.y;

        let t2 = t * t;
        let p3x = p3.x * t2;
        let p3y = p3.y * t2;

        let x = p1.x - p0x - p3x;
        let y = p1.y - p0y - p3y;

        let a = 2 * t * one_minus_t;
        return {
            x: x / a,
            y: y / a
        }
    }


    /**
     * 
     * 求三阶贝塞尔曲线上的两个控制点
     * @param P0 起点
     * @param Q1 第一个过曲线上的点
     * @param Q2 第二个过曲线上的点
     * @param P3 终点
     * @returns 
     */
    public static cubicBezierControlPoints(P0: SimplePoint, Q1: SimplePoint, Q2: SimplePoint, P3: SimplePoint): { c1: SimplePoint, c2: SimplePoint } {

        //x轴作为t,同时也表示曲线上的x坐标，表示t进度必须到达这一点上
        let t1: number = Q1.x;
        let t2: number = Q2.x;

        // 辅助函数：矩阵求逆 * 向量
        let solve2x2 = (a: number, b: number, c: number, d: number, e: number, f: number) => {
            const det = a * d - b * c;
            if (det === 0) throw new Error("不可逆矩阵，选择的 t1 和 t2 可能不合适");
            const x = (e * d - b * f) / det;
            const y = (a * f - e * c) / det;
            return [x, y];
        }

        // 构造方程系数
        const a1 = 3 * (1 - t1) * (1 - t1) * t1;
        const b1 = 3 * (1 - t1) * t1 * t1;
        const a2 = 3 * (1 - t2) * (1 - t2) * t2;
        const b2 = 3 * (1 - t2) * t2 * t2;

        // 构造右侧向量
        const c1x = Q1.x - (1 - t1) ** 3 * P0.x - t1 ** 3 * P3.x;
        const c1y = Q1.y - (1 - t1) ** 3 * P0.y - t1 ** 3 * P3.y;
        const c2x = Q2.x - (1 - t2) ** 3 * P0.x - t2 ** 3 * P3.x;
        const c2y = Q2.y - (1 - t2) ** 3 * P0.y - t2 ** 3 * P3.y;

        // 求解 P1 和 P2 的 x 分量
        const [P1x, P2x] = solve2x2(a1, b1, a2, b2, c1x, c2x);
        // 求解 P1 和 P2 的 y 分量
        const [P1y, P2y] = solve2x2(a1, b1, a2, b2, c1y, c2y);

        return {
            c1: { x: P1x, y: P1y },
            c2: { x: P2x, y: P2y }
        };
    }

    static test(P0: SimplePoint, Q1: SimplePoint, Q2: SimplePoint, P3: SimplePoint) {
        let c = Bezier2D.cubicBezierControlPoints(P0, Q1, Q2, P3);
        // console.info(Bezier2D.cubicBezier(Q1.x, P0, c.c1, c.c2, P3));
        // console.info(Bezier2D.cubicBezier(Q2.x, P0, c.c1, c.c2, P3));
        // console.info(Bezier2D.cubicBezier(P0.x, P0, c.c1, c.c2, P3));
        // console.info(Bezier2D.cubicBezier(P3.x, P0, c.c1, c.c2, P3));
        // console.info(Bezier2D.cubicBezier(0.7, P0, c.c1, c.c2, P3));

        let t = 0;
        for (let i = 0; i < 60; ++i) {
            console.info(t, Bezier2D.cubicBezier(t, P0, c.c1, c.c2, P3));
            t += 1 / 60;
        }
    }
}