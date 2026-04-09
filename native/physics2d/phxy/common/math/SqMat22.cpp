#include "SqMat22.h"
using namespace phxy;

/**
 *
 *  求 Ax=b
 *   x = A^{-1} * b
 *
 * 即对A矩阵求逆乘以b，得出x的值
 */
SqVec2 SqMat22::Solve22(const SqMat22 &A, const SqVec2 &b)
{
    float a11 = A.cx.x, a12 = A.cy.x, a21 = A.cx.y, a22 = A.cy.y;
    float det = a11 * a22 - a12 * a21;
    if (det != 0.0f)
    {
        det = 1.0f / det;
    }
    SqVec2 x = {det * (a22 * b.x - a12 * b.y), det * (a11 * b.y - a21 * b.x)};
    return x;
}

SqVec2 SqMat22::MulMV(const SqMat22& A, const SqVec2& v)
{
    SqVec2 u = {
        A.cx.x * v.x + A.cy.x * v.y,
        A.cx.y * v.x + A.cy.y * v.y,
    };
    return u;
}


/**
 * 二维逆矩阵
 */
SqMat22 SqMat22::GetInverse22(const SqMat22& A)
{
    float a = A.cx.x, b = A.cy.x, c = A.cx.y, d = A.cy.y;
    float det = a * d - b * c;
    if (det != 0.0f)
    {
        det = 1.0f / det;
    }

    SqMat22 B = {
        {det * d, -det * c},
        {-det * b, det * a},
    };
    return B;
}