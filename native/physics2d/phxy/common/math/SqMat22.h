#pragma once
#include "SqVec2.h"

namespace phxy
{
    class SqMat22
    {
    public:
        /// columns
        SqVec2 cx, cy;
        static SqVec2 MulMV(const SqMat22 &A, const SqVec2 &v);
        static SqMat22 GetInverse22(const SqMat22 &A);
        static SqVec2 Solve22(const SqMat22 &A, const SqVec2 &b);
    };
}