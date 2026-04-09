
export default {


    /**
     * 角度转弧度
     */
    DEG_TO_RAD: Math.PI / 180,

    /**
     * 弧度转角度
     */
    RAD_TO_DEG: 180 / Math.PI,

    /**
     * 角度转弧度
     */
    degreesToRadians(angle: number) {
        return angle * this.RAD_TO_DEG;
    },

    clamp(value: number, min: number, max: number) {
        if (value >= max) return max;
        if (value <= min) return min;
        return value;
    },


    /**
     * 将值限制在0~1之间
     * @param value 
     */
    clamp01(value: number): number {
        return this.clamp(value, 0, 1);
    },

    lerp(a: number, b: number, alpha: number) {
        return a + (b - a) * this.clamp(alpha, 0, 1);
    },

    lerpAngle(a: number, b: number, alpha: number) {
        if (b - a > 180) {
            b -= 360;
        }
        if (b - a < -180) {
            b += 360;
        }
        return this.lerp(a, b, this.clamp(alpha, 0, 1));
    },


    powerOfTwo(x: number) {
        return ((x !== 0) && !(x & (x - 1)));
    },

    nextPowerOfTwo(val: number) {
        val--;
        val |= (val >> 1);
        val |= (val >> 2);
        val |= (val >> 4);
        val |= (val >> 8);
        val |= (val >> 16);
        val++;
        return val;
    },

    random(min: number, max: number) {
        var diff = max - min;
        return Math.random() * diff + min;
    },


    smoothstep(min: number, max: number, x: number) {
        if (x <= min) return 0;
        if (x >= max) return 1;

        x = (x - min) / (max - min);

        return x * x * (3 - 2 * x);
    },

    smootherstep(min: number, max: number, x: number) {
        if (x <= min) return 0;
        if (x >= max) return 1;

        x = (x - min) / (max - min);

        return x * x * x * (x * (x * 6 - 15) + 10);
    },

    fsel(a: number, b: number, c: number) {
        return a >= 0 ? b : c;
    },


    /**
     * 分段插值函数，返回[1,-1]范围的值
     * t <= 0.5 返回[0,1]
     * t>=0.5 返回 (0,-1]
     * @param t 
     * @returns 
     */
    smootherUpDown(t: number) {
        if (t <= 0.5)
            return 2 * t;
        else
            return 2 - 2 * t;
    },

    /**
     * 返回向量在一个圆周内的角度，与x轴的夹角
     * 以逆时针方向为准，返回0～360范围的角度。如果向量与x轴重叠即(1,0)返回0度，（-1，0）返回180度
     */
    getAngle(x: number, y: number) {
        let r: number = Math.atan2(y, x);
        r *= this.RAD_TO_DEG;
        return r < 0 ? 360 + r : r;
    },

    /**
     * 第二象限（90° ~ 180°） → 转换到 0° ~ 90°
     * 第一象限（0° ~ 90°） → 转换到 270° ~ 360
     * 第三象限（180° ~ 270°） → 转换到 90° ~ 180°
     * 第四象限（270° ~ 360°） → 转换到 180° ~ 270°
     * 
     * @param x 
     * @param y 
     * @returns 
     */
    convertRotateAngle(x: number, y: number): number {
        let angle = Math.atan2(y, x) * this.RAD_TO_DEG;
        angle = (angle + 360) % 360; // 转换到 [0, 360] 范围

        // 使第二象限为 0°，逆时针旋转
        // angle = (angle - 90 + 360) % 360;
        angle = (angle + 270) % 360;
        return angle;
    },

    getReflectedAngleX(x: number, y: number): number {
        // 计算反射向量 (-x, y)
        return this.convertRotateAngle(-x, y);
    },

    equlas(a: number, b: number, epsilon: number = 0.1): boolean {
        return Math.abs(a - b) < epsilon;
    },

    /**
     *  返回t / length的浮点余数。
     * @param t 
     * @param length 
     * @returns 
     */
    repeat(t: number, length: number): number {
        return t - Math.floor(t / length) * length;
    }
} as {
    DEG_TO_RAD: number;
    RAD_TO_DEG: number;
    degreesToRadians(angle: number): number;
    clamp(value: number, min: number, max: number): number;
    clamp01(value: number): number;
    lerp(a: number, b: number, alpha: number): number;
    getAngle(x: number, y: number): number;
    smootherUpDown(t: number): number;
    convertRotateAngle(x: number, y: number): number;
    getReflectedAngleX(x: number, y: number): number;
    equlas(a: number, b: number, epsilon?: number): boolean;
    repeat(t: number, length: number): number;
    random(min: number, max: number): number;
}
