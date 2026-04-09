#pragma once
#include <math.h>

#define SQ_PI 3.14159265359f

namespace phxy
{
    /**
     * 判断一个浮点数是否为合法的值
     * 即不是NaN，也不是inf
     */
    bool isValidFloat(float a);

    template <typename T>
    T min(T a, T b)
    {
        return a < b ? a : b;
    }

    template <typename T>
    T max(T a, T b)
    {
        return a > b ? a : b;
    }

    template <typename T>
    T abs(T a)
    {
        return a < 0 ? -a : a;
    }

    template <typename T>
    T clamp(T a, T lower, T upper)
    {
        return a < lower ? lower : (a > upper ? upper : a);
    }

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
    uint32_t sqCTZ32(uint32_t block);
    uint32_t sqCLZ32(uint32_t value);
    uint32_t sqCTZ64(uint64_t block);

    float SqAtan2(float y, float x);

    float sqUnwindAngle(float radians);

    bool isPowerOf2(int x);
    int boundingPowerOf2(int x);
    int roundUpPowerOf2(int x);
    float sqSpringDamper(float hertz, float dampingRatio, float position, float velocity, float timeStep);
    float sqInvSqrt(float x);

    template <typename T>
    inline void sqSwap(T &a, T &b)
    {
        T tmp = a;
        a = b;
        b = tmp;
    }
}