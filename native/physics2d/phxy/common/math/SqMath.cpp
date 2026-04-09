#include "SqMath.h"
#include <math.h>
#include <stdbool.h>
#include <stdint.h>

using namespace phxy;

bool phxy::isValidFloat(float a)
{
	if (isnan(a))
	{
		return false;
	}

	if (isinf(a))
	{
		return false;
	}

	return true;
}

#if defined(_MSC_VER) && !defined(__clang__)
#include <intrin.h>

// https://en.wikipedia.org/wiki/Find_first_set

uint32_t phxy::sqCTZ32(uint32_t block)
{
	unsigned long index;
	_BitScanForward(&index, block);
	return index;
}

// This function doesn't need to be fast, so using the Ivy Bridge fallback.
uint32_t phxy::sqCLZ32(uint32_t value)
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

uint32_t phxy::sqCTZ64(uint64_t block)
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

uint32_t phxy::sqCTZ32(uint32_t block)
{
	return __builtin_ctz(block);
}

uint32_t phxy::sqCLZ32(uint32_t value)
{
	return __builtin_clz(value);
}

uint32_t phxy::sqCTZ64(uint64_t block)
{
	return __builtin_ctzll(block);
}

#endif

bool phxy::isPowerOf2(int x)
{
	return (x & (x - 1)) == 0;
}

int phxy::boundingPowerOf2(int x)
{
	if (x <= 1)
	{
		return 1;
	}

	return 32 - (int)sqCLZ32((uint32_t)x - 1);
}

int phxy::roundUpPowerOf2(int x)
{
	if (x <= 1)
	{
		return 1;
	}

	return 1 << (32 - (int)sqCLZ32((uint32_t)x - 1));
}

// https://stackoverflow.com/questions/46210708/atan2-approximation-with-11bits-in-mantissa-on-x86with-sse2-and-armwith-vfpv4
float phxy::SqAtan2(float y, float x)
{
	// Added check for (0,0) to match atan2f and avoid NaN
	if (x == 0.0f && y == 0.0f)
	{
		return 0.0f;
	}

	float ax = abs(x);
	float ay = abs(y);
	float mx = max(ay, ax);
	float mn = min(ay, ax);
	float a = mn / mx;

	// Minimax polynomial approximation to atan(a) on [0,1]
	float s = a * a;
	float c = s * a;
	float q = s * s;
	float r = 0.024840285f * q + 0.18681418f;
	float t = -0.094097948f * q - 0.33213072f;
	r = r * s + t;
	r = r * c + a;

	// Map to full circle
	if (ay > ax)
	{
		r = 1.57079637f - r;
	}

	if (x < 0)
	{
		r = 3.14159274f - r;
	}

	if (y < 0)
	{
		r = -r;
	}

	return r;
}

/**
 * 将任意角度（弧度制）转换到 [-π, π] 的范围内，确保角度不会超出这个区间
 */
float phxy::sqUnwindAngle(float radians)
{
	return remainderf(radians, 2.0f * SQ_PI);
}

float phxy::sqSpringDamper(float hertz, float dampingRatio, float position, float velocity, float timeStep)
{
	float omega = 2.0f * SQ_PI * hertz;
	float omegaH = omega * timeStep;
	return (velocity - omega * omegaH * position) / (1.0f + 2.0f * dampingRatio * omegaH + omegaH * omegaH);
}


/**
 * 
 * 快速求 倒数平方根 的算法。即将x开方，然后求倒数: 1/开方(x)
 * 被称为：
 * "Fast Inverse Square Root"
 */
float phxy::sqInvSqrt(float x)
{
	union
	{
		float x;
		int i;
	} convert;

	convert.x = x;
	float xhalf = 0.5f * x;
	convert.i = 0x5f3759df - (convert.i >> 1);
	x = convert.x;
	x = x * (1.5f - xhalf * x * x);
	return x;
}
