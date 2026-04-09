#include "math.h"
#include <stdlib.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

/**
 * 角度转弧度
 */
float Math::DEG_TO_RAD = Math::PI / 180;

/**
 * 弧度转角度
 */
float Math::RAD_TO_DEG = 180 / Math::PI;

float Math::getAngle(float x, float y)
{
	/**
	 * atan2返回与x轴的夹角弧度
	 *            90
	 * 逆时针      |  逆时针
	 * [90,180]   |  [0,90]
	 * -------------------------
	 * [-90,-180] |  [0,-90]
	 * 顺时针      |  顺时针
	 *            -90
	 */
	float r = ::atan2(y, x);
	r *= Math::RAD_TO_DEG;
	return r < 0 ? 360 + r : r;
}

bool Math::equlas(float a, float b, float epsilon)
{
	return fabs(a - b) < epsilon;
}

float Math::angleToRadian(float angle)
{
	return angle * Math::DEG_TO_RAD;
}

float Math::radianToAngle(float radian)
{
	return radian * Math::RAD_TO_DEG;
}

// 生成 [0.0, 1.0) 的随机浮点数
double random_float_0_1()
{
	return (double)rand() / (RAND_MAX + 1.0);
}

float Math::randomFloat(float min, float max)
{
#ifdef __EMSCRIPTEN__
	return min + emscripten_random() * max;
#else
	return min + (max - min) * random_float_0_1();
#endif
}

float Math::lerp(float a, float b, float alpha)
{
	return a + (b - a) * alpha;
}

double Math::fmod(double p_x, double p_y)
{
	return ::fmod(p_x, p_y);
}
float Math::fmod(float p_x, float p_y)
{
	return ::fmodf(p_x, p_y);
}

float Math::ceil(float v)
{
	return ::ceil(v);
}

float Math::abs(float v)
{
	return ::abs(v);
}

int Math::abs(int v)
{
	return ::abs(v);
}

float Math::floor(float v)
{
	return ::floor(v);
}

float Math::asin(float angle)
{
	return ::asin(angle);
}

float Math::cos(float angle)
{
	return ::cos(angle);
}

float Math::sin(float angle)
{
	return ::sin(angle);
}

float Math::atan2(float x, float y)
{
	return ::atan2(y, x);
}

#if defined(_MSC_VER) && !defined(__clang__)
#include <intrin.h>

// https://en.wikipedia.org/wiki/Find_first_set

uint32_t Math::CTZ32(uint32_t block)
{
	unsigned long index;
	_BitScanForward(&index, block);
	return index;
}

// This function doesn't need to be fast, so using the Ivy Bridge fallback.
uint32_t Math::CLZ32(uint32_t value)
{
#if 1
	// Use BSR (Bit Scan Reverse) which is available on Ivy Bridge
	unsigned long index;
	if (_BitScanReverse(&index, value))
	{
		// BSR gives the index of the most significant 1-bit
		// We need to invert this to get the number of leading zeros
		return 31 - index;
	}
	else
	{
		// If x is 0, BSR sets the zero flag and doesn't modify index
		// LZCNT should return 32 for an input of 0
		return 32;
	}

#else
	return __lzcnt(value);

#endif
}

uint32_t Math::CTZ64(uint64_t block)
{
	unsigned long index;

#ifdef _WIN64
	_BitScanForward64(&index, block);
#else
	// 32-bit fall back
	if ((uint32_t)block != 0)
	{
		_BitScanForward(&index, (uint32_t)block);
	}
	else
	{
		_BitScanForward(&index, (uint32_t)(block >> 32));
		index += 32;
	}
#endif

	return index;
}

#else

uint32_t Math::CTZ32(uint32_t block)
{
	return __builtin_ctz(block);
}

uint32_t Math::CLZ32(uint32_t value)
{
	return __builtin_clz(value);
}

uint32_t Math::CTZ64(uint64_t block)
{
	return __builtin_ctzll(block);
}

#endif