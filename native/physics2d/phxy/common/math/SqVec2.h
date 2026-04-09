#pragma once

namespace phxy
{

    class SqVec2
    {
    public:
        float x, y;
        SqVec2();
        SqVec2(float, float);
        void zero();
        void Set(float x, float y) { this->x = x, this->y = y; };
        float Normalize();
        static float Dot(const SqVec2 &a, const SqVec2 &b);
        static SqVec2 Sub(const SqVec2 &a, const SqVec2 &b);
        static SqVec2 Add(const SqVec2 &a, const SqVec2 &b);
        static float LengthSquared(const SqVec2 &v);
        static float Length(const SqVec2 &v);
        static float DistanceSquared(const SqVec2 &a, const SqVec2 &b);
        static float Distance(const SqVec2 &a, const SqVec2 &b);
        static bool isValid(const SqVec2 &v);
        static SqVec2 Normalize(const SqVec2 &v);

        /**
         * 返回v的长度和归一化后的向量
         */
        static SqVec2 GetLengthAndNormalize(float *length, const SqVec2 &v);
        static SqVec2 Skew(const SqVec2 &);
        static SqVec2 Min(const SqVec2 &a, const SqVec2 &b);
        static SqVec2 Max(const SqVec2 &a, const SqVec2 &b);
        static float Cross(const SqVec2 &a, const SqVec2 &b);

        /**
         * 求垂直于V的向量，并且对此向量缩放s
         */
        static SqVec2 CrossSV(float s, const SqVec2 &v);
        static SqVec2 CrossVS(const SqVec2 &v, float s);

        /**
         * 求垂直于向量v左边的向量
         * 也就是向量v逆时针旋转90度的向量
         */
        static SqVec2 LeftPerp(const SqVec2 &v);

        /**
         * 求垂直于向量v右边的向量
         */
        static SqVec2 RightPerp(const SqVec2 &v);

        static SqVec2 Abs(const SqVec2 &a);
        static SqVec2 Neg(const SqVec2 &a);

        static SqVec2 MulSV(float s, const SqVec2 &v);

        /**
         * a + s * b
         */
        static SqVec2 MulAdd(const SqVec2 &a, float s, const SqVec2 &b);

        /**
         * a - s * b
         */
        static SqVec2 MulSub(const SqVec2 &a, float s, const SqVec2 &b);

        static SqVec2 Lerp(const SqVec2 &a, const SqVec2 &b, float t);

        static bool IsNormalized(const SqVec2 a);
    };

    inline SqVec2 operator+(const SqVec2 &a, const SqVec2 &b)
    {
        return SqVec2(a.x + b.x, a.y + b.y);
    }

    inline SqVec2 operator-(const SqVec2 &a, const SqVec2 &b)
    {
        return SqVec2(a.x - b.x, a.y - b.y);
    }

    /// Unary add one vector to another
    inline void operator+=(SqVec2 &a, SqVec2 b)
    {
        a.x += b.x;
        a.y += b.y;
    }

    /// Unary subtract one vector from another
    inline void operator-=(SqVec2 &a, SqVec2 b)
    {
        a.x -= b.x;
        a.y -= b.y;
    }

    /// Unary multiply a vector by a scalar
    inline void operator*=(SqVec2 &a, float b)
    {
        a.x *= b;
        a.y *= b;
    }

    /// Unary negate a vector
    inline SqVec2 operator-(SqVec2 a)
    {
        return {-a.x, -a.y};
    }

    /// Binary scalar and vector multiplication
    inline SqVec2 operator*(float a, SqVec2 b)
    {
        return {a * b.x, a * b.y};
    }

    /// Binary scalar and vector multiplication
    inline SqVec2 operator*(SqVec2 a, float b)
    {
        return {a.x * b, a.y * b};
    }

    /// Binary vector equality
    inline bool operator==(SqVec2 a, SqVec2 b)
    {
        return a.x == b.x && a.y == b.y;
    }

    /// Binary vector inequality
    inline bool operator!=(SqVec2 a, SqVec2 b)
    {
        return a.x != b.x || a.y != b.y;
    }

    /// Binary scalar and vector multiplication
    inline SqVec2 operator/(SqVec2 a, float b)
    {
        return {a.x * b, a.y * b};
    }

}