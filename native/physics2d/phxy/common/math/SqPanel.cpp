#include "SqPlane.h"
#include "SqVec2.h"
using namespace phxy;

float SqPlane::separation(const SqVec2 &point)
{
    return SqVec2::Dot(normal, point) - offset;
}