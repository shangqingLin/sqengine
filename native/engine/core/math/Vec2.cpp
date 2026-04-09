#include "Vec2.h"
#include <cmath>
#include "math.h"
#include <stdio.h>

Vec2::Vec2(float x, float y) : x(x), y(y) {}
Vec2::Vec2() : x(0), y(0) {}
Vec2::Vec2(float v) : x(v), y(v) {}
Vec2::Vec2(const Vec2 &v)
{
    // printf("++++copy\n");
    x = v.x;
    y = v.y;
}
bool Vec2::equals(const Vec2 &v)
{
    return this->x == v.x && this->y == v.y;
}

void Vec2::set(float x, float y)
{
    this->x = x;
    this->y = y;
};

void Vec2::add(const Vec2 &rhs)
{
    this->x += rhs.x;
    this->y += rhs.y;
}

void Vec2::add2(const Vec2 &lhs, const Vec2 &rhs)
{
    this->x = lhs.x + rhs.x;
    this->y = lhs.y + rhs.y;
}

void Vec2::sub(const Vec2 &rhs)
{
    this->x -= rhs.x;
    this->y -= rhs.y;
}

void Vec2::sub2(const Vec2 &lhs, const Vec2 &rhs)
{
    this->x = lhs.x - rhs.x;
    this->y = lhs.y - rhs.y;
}

void Vec2::scale(float scalar)
{
    this->x *= scalar;
    this->y *= scalar;
}

void Vec2::mul(const Vec2 &rhs)
{
    this->x *= rhs.x;
    this->y *= rhs.y;
}

void Vec2::mul(float s)
{
    x *= s;
    y *= s;
}

void Vec2::mul2(const Vec2 &lhs, const Vec2 &rhs)
{
    this->x = lhs.x * rhs.x;
    this->y = lhs.x * rhs.y;
}

float Vec2::dot(const Vec2 &rhs)
{
    return this->x * rhs.x + this->y * rhs.y;
}

float Vec2::cross(const Vec2 &rhs)
{
    return this->x * rhs.y - this->y * rhs.x;
}

void Vec2::copy(const Vec2 &vec)
{
    this->x = vec.x;
    this->y = vec.y;
}

float Vec2::lengthSqr() const
{
    return Vec2::lengthSqr(*this);
}

float Vec2::normalize()
{
    return Vec2::normalize(*this, *this);
}

float Vec2::normalize(const Vec2 &a, Vec2 &out)
{
    float x = a.x;
    float y = a.y;
    float len = sqrt(x * x + y * y);
    if (len > 0)
    {
        float invLen = 1 / len;
        out.x = x * invLen;
        out.y = y * invLen;
    }
    else
    {
        out.x = 0;
        out.y = 0;
    }
    return len;
}

float Vec2::len() const
{
    return sqrt(x * x + y * y);
}

float Vec2::lengthSqr(const Vec2 &v)
{
    return v.x * v.x + v.y * v.y;
}

void Vec2::skew()
{
    float x1 = x;
    x = -y;
    y = x1;
}

float Vec2::length(const Vec2 &v1, const Vec2 &v2)
{
    Vec2 temp;
    temp.set(v1.x - v2.x, v1.y - v2.y);
    return temp.len();
}

float Vec2::dot(const Vec2 &v1, const Vec2 &v2)
{
    Vec2 temp;
    temp.set(v1.x, v1.y);
    return temp.dot(v2);
}

float Vec2::cross(const Vec2 &v1, const Vec2 &v2)
{
    Vec2 temp;
    temp.set(v1.x, v1.y);
    return temp.cross(v2);
}

void Vec2::lerp(const Vec2 &v1, const Vec2 &v2, float alpha, Vec2 &out)
{
    out.x = Math::lerp(v1.x, v2.x, alpha);
    out.y = Math::lerp(v1.y, v2.y, alpha);
}

void Vec2::abs(Vec2 &v1)
{
    v1.x = std::fabs(v1.x);
    v1.y = std::fabs(v1.y);
}

void Vec2::max(Vec2 &v1, float v)
{
    v1.x = std::fmax(v1.x, v);
    v1.y = std::fmax(v1.y, v);
}

void Vec2::leftPerp(const Vec2 &v, Vec2 &out)
{
    float x = -v.y;
    out.y = v.x;
    out.x = x;
}

void Vec2::rightPerp(const Vec2 &v, Vec2 &out)
{
    float x = v.y;
    out.y = -v.x;
    out.x = x;
}

Vec2 Vec2::operator*(const Vec2 &v1) const
{
    return Vec2(x * v1.x, y * v1.y);
}

Vec2 Vec2::operator*(float v) const
{
    return Vec2(x * v, y * v);
}

Vec2 &Vec2::operator*=(float v)
{
    x *= v;
    y *= v;
    return *this;
}

Vec2 Vec2::operator+(const Vec2 &v1) const
{
    return Vec2(x + v1.x, y + v1.y);
}

Vec2 Vec2::operator+(float v) const
{
    return Vec2(x + v, y + v);
}

Vec2 &Vec2::operator+=(float v)
{
    x += v;
    y += v;
    return *this;
}

Vec2 &Vec2::operator+=(const Vec2 &v)
{
    x += v.x;
    y += v.y;
    return *this;
}

bool Vec2::operator==(const Vec2 &c) const
{
    return x == c.x && y == c.y;
}

bool Vec2::operator!=(const Vec2 &c) const
{
    return x != c.x || y != c.y;
}

Vec2 Vec2::operator-(const Vec2 &v1) const
{
    return Vec2(x - v1.x, y - v1.y);
}

Vec2 Vec2::operator/(float v) const
{
    return Vec2(x / v, y / v);
}
