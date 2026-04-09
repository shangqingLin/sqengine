#include "SqVec2.h"
#include <math.h>
#include <float.h>
#include "SqMath.h"

using namespace phxy;

SqVec2::SqVec2() : x(0.f), y(0.f)
{
}

SqVec2::SqVec2(float x, float y) : x(x), y(y)
{
}

void SqVec2::zero()
{
	x = 0.f;
	y = 0.f;
}

SqVec2 SqVec2::Sub(const SqVec2 &a, const SqVec2 &b)
{
	return SqVec2{a.x - b.x, a.y - b.y};
}

SqVec2 SqVec2::Add(const SqVec2 &a, const SqVec2 &b)
{
	return SqVec2{a.x + b.x, a.y + b.y};
}

float SqVec2::LengthSquared(const SqVec2 &v)
{
	return v.x * v.x + v.y * v.y;
}

float SqVec2::Length(const SqVec2 &v)
{
	return sqrtf(v.x * v.x + v.y * v.y);
}

bool SqVec2::isValid(const SqVec2 &v)
{
	if (isnan(v.x) || isnan(v.y))
	{
		return false;
	}

	if (isinf(v.x) || isinf(v.y))
	{
		return false;
	}

	return true;
}

SqVec2 SqVec2::Normalize(const SqVec2 &v)
{
	float length = sqrtf(v.x * v.x + v.y * v.y);
	if (length < FLT_EPSILON)
	{
		return SqVec2();
	}

	float invLength = 1.0f / length;
	SqVec2 n = {invLength * v.x, invLength * v.y};
	return n;
}

/**
 * 判断向量a是否为单位向量
 */
bool SqVec2::IsNormalized(const SqVec2 a)
{
	float aa = SqVec2::Dot(a, a);
	return abs(1.0f - aa) < 100.0f * FLT_EPSILON;
}

SqVec2 SqVec2::MulSV(float s, const SqVec2 &v)
{
	return SqVec2{s * v.x, s * v.y};
}

SqVec2 SqVec2::Abs(const SqVec2 &a)
{
	SqVec2 b;
	b.x = abs(a.x);
	b.y = abs(a.y);
	return b;
}

SqVec2 SqVec2::MulAdd(const SqVec2 &a, float s, const SqVec2 &b)
{
	return SqVec2{a.x + s * b.x, a.y + s * b.y};
}

SqVec2 SqVec2::MulSub(const SqVec2 &a, float s, const SqVec2 &b)
{
	return SqVec2{a.x - s * b.x, a.y - s * b.y};
}

float SqVec2::Dot(const SqVec2 &a, const SqVec2 &b)
{
	return a.x * b.x + a.y * b.y;
}

float SqVec2::Distance(const SqVec2 &a, const SqVec2 &b)
{
	float dx = b.x - a.x;
	float dy = b.y - a.y;
	return sqrtf(dx * dx + dy * dy);
}

float SqVec2::DistanceSquared(const SqVec2 &a, const SqVec2 &b)
{
	SqVec2 c = {b.x - a.x, b.y - a.y};
	return c.x * c.x + c.y * c.y;
}

SqVec2 SqVec2::Min(const SqVec2 &a, const SqVec2 &b)
{
	SqVec2 c;
	c.x = min(a.x, b.x);
	c.y = min(a.y, b.y);
	return c;
}

SqVec2 SqVec2::Max(const SqVec2 &a, const SqVec2 &b)
{
	SqVec2 c;
	c.x = max(a.x, b.x);
	c.y = max(a.y, b.y);
	return c;
}

float SqVec2::Cross(const SqVec2 &a, const SqVec2 &b)
{
	return a.x * b.y - a.y * b.x;
}

SqVec2 SqVec2::CrossVS(const SqVec2 &v, float s)
{
	return SqVec2{s * v.y, -s * v.x};
}

SqVec2 SqVec2::CrossSV(float s, const SqVec2 &v)
{
	return SqVec2{-s * v.y, s * v.x};
}

SqVec2 SqVec2::Lerp(const SqVec2 &a, const SqVec2 &b, float t)
{
	return SqVec2{(1.0f - t) * a.x + t * b.x, (1.0f - t) * a.y + t * b.y};
}

float SqVec2::Normalize()
{
	float length = sqrtf(x * x + y * y);
	if (length < FLT_EPSILON)
	{
		return 0.f;
	}

	float invLength = 1.0f / length;
	x *= invLength;
	y *= invLength;

	return length;
}

SqVec2 SqVec2::GetLengthAndNormalize(float *length, const SqVec2 &v)
{
	*length = sqrtf(v.x * v.x + v.y * v.y);
	if (*length < FLT_EPSILON)
	{
		return SqVec2{0.0f, 0.0f};
	}

	float invLength = 1.0f / *length;
	SqVec2 n = {invLength * v.x, invLength * v.y};
	return n;
}

SqVec2 SqVec2::Skew(const SqVec2 &v)
{
	return SqVec2(-v.y, v.x);
}

SqVec2 SqVec2::LeftPerp(const SqVec2 &v)
{
	return SqVec2{-v.y, v.x};
}

SqVec2 SqVec2::RightPerp(const SqVec2 &v)
{
	return SqVec2{v.y, -v.x};
}

SqVec2 SqVec2::Neg(const SqVec2 &a)
{
	return SqVec2{-a.x, -a.y};
}
