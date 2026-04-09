#include "SqAABB.h"
#include "SqMath.h"
#include "../SqCore.h"

using namespace phxy;

SqAABB SqAABB::Union(const SqAABB &a, const SqAABB &b)
{
	SqAABB c;
	c.lowerBound.x = min(a.lowerBound.x, b.lowerBound.x);
	c.lowerBound.y = min(a.lowerBound.y, b.lowerBound.y);
	c.upperBound.x = max(a.upperBound.x, b.upperBound.x);
	c.upperBound.y = max(a.upperBound.y, b.upperBound.y);
	return c;
}

SqVec2 SqAABB::Center(const SqAABB &a)
{
	SqVec2 b = {0.5f * (a.lowerBound.x + a.upperBound.x), 0.5f * (a.lowerBound.y + a.upperBound.y)};
	return b;
}

SqVec2 SqAABB::Extents(const SqAABB &a)
{
	SqVec2 b = {0.5f * (a.upperBound.x - a.lowerBound.x), 0.5f * (a.upperBound.y - a.lowerBound.y)};
	return b;
}

float SqAABB::Perimeter(const SqAABB &a)
{
	float wx = a.upperBound.x - a.lowerBound.x;
	float wy = a.upperBound.y - a.lowerBound.y;
	return 2.0f * (wx + wy);
}

bool SqAABB::isValid(const SqAABB &a)
{
	SqVec2 d = SqVec2::Sub(a.upperBound, a.lowerBound);
	bool valid = d.x >= 0.0f && d.y >= 0.0f;
	valid = valid && SqVec2::isValid(a.lowerBound) && SqVec2::isValid(a.upperBound);
	return valid;
}

bool SqAABB::enlargeAABB(SqAABB *a, const SqAABB &b)
{
	bool changed = false;
	if (b.lowerBound.x < a->lowerBound.x)
	{
		a->lowerBound.x = b.lowerBound.x;
		changed = true;
	}

	if (b.lowerBound.y < a->lowerBound.y)
	{
		a->lowerBound.y = b.lowerBound.y;
		changed = true;
	}

	if (a->upperBound.x < b.upperBound.x)
	{
		a->upperBound.x = b.upperBound.x;
		changed = true;
	}

	if (a->upperBound.y < b.upperBound.y)
	{
		a->upperBound.y = b.upperBound.y;
		changed = true;
	}

	return changed;
}

bool SqAABB::Contains(const SqAABB &a, const SqAABB &b)
{
	bool s = true;
	s = s && a.lowerBound.x <= b.lowerBound.x;
	s = s && a.lowerBound.y <= b.lowerBound.y;
	s = s && b.upperBound.x <= a.upperBound.x;
	s = s && b.upperBound.y <= a.upperBound.y;
	return s;
}

bool SqAABB::Overlaps(const SqAABB &a, const SqAABB &b)
{
	return !(b.lowerBound.x > a.upperBound.x || b.lowerBound.y > a.upperBound.y || a.lowerBound.x > b.upperBound.x ||
			 a.lowerBound.y > b.upperBound.y);
}

SqAABB SqAABB::MakeAABB(const SqVec2 *points, int count, float radius)
{
	SQ_ASSERT(count > 0);
	SqAABB a = {points[0], points[0]};
	for (int i = 1; i < count; ++i)
	{
		a.lowerBound = SqVec2::Min(a.lowerBound, points[i]);
		a.upperBound = SqVec2::Max(a.upperBound, points[i]);
	}

	SqVec2 r = {radius, radius};
	a.lowerBound = SqVec2::Sub(a.lowerBound, r);
	a.upperBound = SqVec2::Add(a.upperBound, r);

	return a;
}