#pragma once

#include <cmath>
#include <stdint.h>

#define equlas_epsilon 0.00001 // 1e-6

namespace Math
{

    constexpr float PI = 3.14159265359f;

    /**
     * 角度转弧度
     */
    extern float DEG_TO_RAD;

    /**
     * 弧度转角度
     */
    extern float RAD_TO_DEG;

    /**
     * 返回向量在一个圆周内的角度，与x轴的夹角
     * 以逆时针方向为准，返回0～360范围的角度。如果向量与x轴重叠即(1,0)返回0度，（-1，0）返回180度
     */
    float getAngle(float x, float y);
    bool equlas(float a, float b, float epsilon = equlas_epsilon);

    /**
     * 角度转弧度
     */
    float angleToRadian(float angle);

    /**
     * 弧度转角度
     */
    float radianToAngle(float radian);

    /**
     * 随机一个范围内的浮点数
     */
    float randomFloat(float min, float max);

    /**
     * 线性插值
     */
    float lerp(float a, float b, float alpha);

    /**
     * 返回数字的符号
     *   如果输入 x > 0，返回 1.0
     *   如果输入 x = 0，返回 0.
     *   如果输入 x < 0，返回 -1.0
     *
     */
    template <typename T>
    T sign(T v)
    {
        return v == T(0) ? 0 : v < T(0) ? T(-1)
                                        : T(1);
    }

    // 计算余数
    double fmod(double p_x, double p_y);
    float fmod(float p_x, float p_y);
    float ceil(float);

    float abs(float v);
    int abs(int v);

    float floor(float v);

    template <typename T>
    T max(T v1, T v2)
    {
        return v1 > v2 ? v1 : v2;
    }

    template <typename T>
    T min(T v1, T v2)
    {
        return v1 > v2 ? v2 : v1;
    }

    template <typename T>
    T clamp(T value, T min, T max)
    {
        if (value >= max)
            return max;
        if (value <= min)
            return min;
        return value;
    }

    float asin(float);
    float cos(float angle);
    float sin(float angle);
    float atan2(float x, float y);

    /**
     * 下面的三个方法计算一个 64或32 位的整数从右往左第一个 1 位的位置
     * 简单来说，它告诉你一个数字的二进制表示中，从右边开始有多少个零，直到遇到第一个 1。
     *
     * 例如：
     * 1、0000000000000000000000000000000000000000000000000000000010000000
     * 这个数的右边有 6 个零，直到第一个 1 出现。所以，尾随零的个数是 6。
     *
     * 2、比如 block = 0x10（二进制 00000000000000000000000000010000），这个数从右开始有 4 个零，第一个 1 位在第 4 位，所以返回的值是 4。
     */
    uint32_t CTZ32(uint32_t block);
    uint32_t CLZ32(uint32_t value);
    uint32_t CTZ64(uint64_t block);

}
