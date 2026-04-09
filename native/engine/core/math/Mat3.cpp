#include "Mat3.h"
#include "math.h"
#include <stdio.h>
#include "../simd/simd.h"

/**
 *
 * OpenGL的GLSL中，矩阵是遵循列主序（Column-Major）规则
 * 即矩阵与向量相乘时，向量作为列向量，矩阵的每一行与向量相乘
 * [
 *  cos  sin 0
 *  -sin cos 0
 *  tx   ty  1
 * ] = [
 *    a[0] a[1] 0
 *    a[2] a[3] 0
 *    a[4] a[5] 1
 * ]
 *
 */
Mat3::Mat3()
{
    setIdentity();
}

void Mat3::setTRS(const Vec2 &t, const Vec2 &r, const Vec2 &s)
{
    float srx, crx, sry, cry, rad;
    if (r.x == 0.f)
    {
        crx = 1.f;
        srx = 0.f;
    }
    else
    {
        rad = r.x * Math::DEG_TO_RAD;
        srx = sin(rad);
        crx = cos(rad);
    }

    if (r.y == 0.f)
    {
        sry = 0.f;
        cry = 1.f;
    }
    else if (r.y == r.x)
    {
        sry = srx;
        cry = crx;
    }
    else
    {
        rad = r.y * Math::DEG_TO_RAD;
        sry = sin(rad);
        cry = cos(rad);
    }

    /**
     *
     * cyx sry
     * -srx cry
     *
     */
    data[0] = crx * s.x;
    data[1] = sry * s.x;

    data[2] = -srx * s.y;
    data[3] = cry * s.y;

    data[4] = t.x;
    data[5] = t.y;
}

// static void mul2Simd(Mat3 &lmat, const Mat3 &rmat, Mat3 &resultMat)
// {
//     float *data = resultMat.data;
//     SIMD_128f column = simd_set_ps(lmat.data[0], lmat.data[2], lmat.data[1], lmat.data[3]);
//     SIMD_128f row_1 = simd_set_ps(rmat.data[0], rmat.data[1], rmat.data[0], rmat.data[1]);
//     SIMD_128f row_2 = simd_set_ps(rmat.data[2], rmat.data[3], rmat.data[2], rmat.data[3]);
//     SIMD_128f row_3 = simd_set_ps(rmat.data[4], rmat.data[5], rmat.data[4], rmat.data[5]);

//     // 求第一行

//     SIMD_128f simd_result_1 = simd_mul_ps(column, row_1);

//     // 求第二行
//     SIMD_128f simd_result_2 = simd_mul_ps(column, row_2);

//     // 求第三行
//     SIMD_128f simd_result_3 = simd_mul_ps(column, row_3);

//     float result[4];
//     simd_store_ps(result, simd_result_1);

//     float result2[4];
//     simd_store_ps(result2, simd_result_2);

//     SIMD_128f add1 = simd_set_ps(result[0], result[2], result2[0], result2[2]);
//     SIMD_128f add2 = simd_set_ps(result[1], result[3], result2[1], result2[3]);
//     SIMD_128f addResult = simd_add_ps(add1, add2);
//     simd_store_ps(data, addResult);
//     simd_store_ps(result, simd_result_3);
//     data[4] = result[0] + result[1] + lmat.data[4];
//     data[5] = result[2] + result[3] + lmat.data[5];
// }

static void mul2Normal(Mat3 &lmat, const Mat3 &rmat, Mat3 &result)
{
    float a0, a1, a2, a3, a4, a5, b0, b1;

    float *a = lmat.data;
    const float *b = rmat.data;
    float *r = result.data;

    a0 = a[0];
    a1 = a[1];

    a2 = a[2];
    a3 = a[3];

    a4 = a[4];
    a5 = a[5];

    // 第一行第一列
    b0 = b[0];
    b1 = b[1];
    r[0] = a0 * b0 + a2 * b1;

    // 第一行第二列
    r[1] = a1 * b0 + a3 * b1;

    // 第二行第一列
    b0 = b[2];
    b1 = b[3];
    r[2] = a0 * b0 + a2 * b1;

    // 第二行第二列
    r[3] = a1 * b0 + a3 * b1;

    // 第三行第一列
    b0 = b[4];
    b1 = b[5];
    r[4] = a0 * b0 + a2 * b1 + a4;

    // 第三行第二列
    r[5] = a1 * b0 + a3 * b1 + a5;
}

void Mat3::mul2(Mat3 &lmat, const Mat3 &rmat)
{

    /**
     *
     *  lmat * rmat = [
     *     l.data[0]* r.data[0] + l.data[2]* r.data[1],              l.data[1]* r.data[0] + l.data[3]* r.data[1]
     *     l.data[0]* r.data[2] + l.data[2]* r.data[3],              l.data[1]* r.data[2] + l.data[3]* r.data[3]
     *     l.data[0]* r.data[4] + l.data[2]* r.data[5] + l.data[4],  l.data[1]* r.data[4] + l.data[3]* r.data[5] + l.data[5]
     * ]
     */

    // if (USE_SIMD)
    // {
    //     mul2Simd(lmat, rmat, *this);
    // }
    // else
    // {
        mul2Normal(lmat, rmat, *this);
    // }
}

void Mat3::mul(const Mat3 &rhs)
{
    mul2(*this, rhs);
}

void Mat3::setIdentity()
{
    data[0] = 1;
    data[1] = 0;

    data[2] = 0;
    data[3] = 1;

    data[4] = 0;
    data[5] = 0;
}

void Mat3::setTranslate(float tx, float ty)
{
    data[0] = data[3] = 1;
    data[1] = data[2] = 0;
    data[4] = tx;
    data[5] = ty;
}

void Mat3::translate(float x, float y)
{
    data[4] = data[0] * x +
              data[2] * y +
              data[4];

    data[5] =
        data[1] * x +
        data[3] * y +
        data[5];
}

void Mat3::setScale(float x, float y)
{
    data[0] = x;
    data[3] = y;
}

void Mat3::scale(float x, float y)
{

    data[0] *= x;
    data[1] *= x;

    data[2] *= y;
    data[3] *= y;
}

void Mat3::setRotate(float angle)
{
    float cos = Math::cos(angle);
    float sin = Math::sin(angle);
    data[0] = cos;
    data[1] = sin;
    data[2] = -sin;
    data[3] = cos;
    data[4] = 0.f;
    data[5] = 0.f;
}

void Mat3::rotate(float angle)
{
    float cos = Math::cos(angle);
    float sin = Math::sin(angle);
    float a0 = data[0];
    float a1 = data[1];
    float a2 = data[2];
    float a3 = data[3];
    float a4 = data[4];
    float a5 = data[5];

    // [
    //     a0, a1, 0,
    //     a2, a3, 0,
    //     a4, a5, 1,
    // ]

    // [
    //     cos, sin, 0,
    //     -sin, cos, 0,
    //      0, 0,      1 ,
    // ]

    // if (USE_SIMD)
    // {
    //     SIMD_128f vv1 = simd_set_ps(a0, a1, a0, a1);
    //     SIMD_128f vv2 = simd_set_ps(a2, a3, a2, a3);
    //     SIMD_128f vv3 = simd_set_ps(cos, cos, -sin, -sin);
    //     SIMD_128f vv4 = simd_set_ps(sin, sin, cos, cos);
    //     SIMD_128f r1 = simd_mul_ps(vv1, vv3);
    //     SIMD_128f r2 = simd_mul_ps(vv2, vv4);
    //     SIMD_128f r3 = simd_add_ps(r1, r2);
    //     simd_store_ps(data, r3);
    // }
    // else
    // {
        data[0] = a0 * cos + a2 * sin;
        data[1] = a1 * cos + a3 * sin;
        data[2] = a0 * -sin + a2 * cos;
        data[3] = a1 * -sin + a3 * cos;
    // }

    data[4] = a4;
    data[5] = a5;
}

void Mat3::transformPoint(const Vec2 &vec, Vec2 &res) const
{
    float x, y;
    const float *m = data;
    x =
        vec.x * m[0] +
        vec.y * m[2] +
        m[4]; // x轴的平移

    y =
        vec.x * m[1] +
        vec.y * m[3] +
        m[5]; // y轴的平移

    res.set(x, y);
}

void Mat3::transformVector(const Vec2 &vec, Vec2 &res) const
{
    float x, y;
    const float *m = data;

    x = vec.x * m[0] + vec.y * m[2];
    y = vec.x * m[1] + vec.y * m[3];

    res.set(x, y);
}

void Mat3::invert()
{
    float *me = data;
    float *te = data;

    float n11 = me[0],
          n21 = me[1],

          n12 = me[2],
          n22 = me[3],

          n13 = me[4],
          n23 = me[5];

    float det = n11 * n22 - n21 * n12;
    if (det == 0)
    {
        return setIdentity();
    }

    // if (USE_SIMD)
    // {
    //     SIMD_128f vv1 = simd_set_ps(n22, -n21, -n12, n11);
    //     SIMD_128f vv2 = simd_set1_ps(det);
    //     SIMD_128f r = simd_div_ps(vv1, vv2);
    //     simd_store_ps(te, r);

    //     float detInv = 1 / det;
    //     te[4] = (n23 * n12 - n22 * n13) * detInv;
    //     te[5] = (n21 * n13 - n23 * n11) * detInv;
    // }
    // else
    // {
    float detInv = 1 / det;
    te[0] = n22 * detInv;
    te[1] = -n21 * detInv;

    te[2] = -n12 * detInv;
    te[3] = n11 * detInv;

    te[4] = (n23 * n12 - n22 * n13) * detInv;
    te[5] = (n21 * n13 - n23 * n11) * detInv;
    // }
}

void Mat3::getTranslation(Vec2 &out) const
{
    out.x = data[4];
    out.y = data[5];
}

void Mat3::getTranslation(float &x, float &y) const
{
    x = data[4];
    y = data[5];
}

void Mat3::getScale(Vec2 &out) const
{
    out.x = sqrt(data[0] * data[0] + data[2] * data[2]);
    out.y = sqrt(data[1] * data[1] + data[3] * data[3]);
}

void Mat3::getScale(float &x, float &y) const
{
    // 缩放就是两个轴向量的长度
    x = sqrt(data[0] * data[0] + data[2] * data[2]);
    y = sqrt(data[1] * data[1] + data[3] * data[3]);
}

void Mat3::getRotation(float &x, float &y, bool rad) const
{

    // Step 1: 推算缩放因子
    float scaleX = sqrt(data[0] * data[0] + data[2] * data[2]);
    float scaleY = sqrt(data[1] * data[1] + data[3] * data[3]);

    // Step 2: 去除缩放影响
    float crx = data[0] / scaleX;
    float sry = data[1] / scaleX;
    float srx = -data[2] / scaleY;
    float cry = data[3] / scaleY;

    // Step 3: 提取旋转角度
    x = atan2(srx, crx); // 提取 Rotate X
    y = atan2(sry, cry); // 提取 Rotate Y
    if (!rad)
    {
        x = x * Math::RAD_TO_DEG;
        y = y * Math::RAD_TO_DEG;
    }
}

void Mat3::copy(const Mat3 &m)
{
    data[0] = m.data[0];
    data[1] = m.data[1];
    data[2] = m.data[2];
    data[3] = m.data[3];
    data[4] = m.data[4];
    data[5] = m.data[5];
}

bool Mat3::operator!=(const Mat3& m) const
{
    return data[0] != m.data[0] || data[1] != m.data[1] || data[2] != m.data[2]
        || data[3] != m.data[3] || data[4] != m.data[4]  || data[5] != m.data[5];
}

void Mat3::print() const
{
    printf(
        "mat3:(%f %f %f %f %f %f) \n",
        data[0],
        data[1],
        data[2],
        data[3],
        data[4],
        data[5]);
}