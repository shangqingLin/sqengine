#include "Mat4.h"
#include "Vec2.h"
#include "math.h"
#include "../../core/common/number_to_string.h"

Mat4::Mat4()
{
    setIdentity();
}

void Mat4::setIdentity()
{
    data[0] = 1;
    data[1] = 0;
    data[2] = 0;
    data[3] = 0;
    data[4] = 0;
    data[5] = 1;
    data[6] = 0;
    data[7] = 0;
    data[8] = 0;
    data[9] = 0;
    data[10] = 1;
    data[11] = 0;
    data[12] = 0;
    data[13] = 0;
    data[14] = 0;
    data[15] = 1;
}

void _getPerspectiveHalfSize(Vec2 &halfSize, float fov, float aspect, float znear, bool fovIsHorizontal)
{
    if (fovIsHorizontal)
    {
        halfSize.x = znear * tan(fov * Math::PI / 360);
        halfSize.y = halfSize.x / aspect;
    }
    else
    {
        halfSize.y = znear * tan(fov * Math::PI / 360);
        halfSize.x = halfSize.y * aspect;
    }
}

void Mat4::setPerspective(float fov, float aspect, float znear, float zfar, bool fovIsHorizontal)
{
    Vec2 _halfSize;
    _getPerspectiveHalfSize(_halfSize, fov, aspect, znear, fovIsHorizontal);
    setFrustum(-_halfSize.x, _halfSize.x, -_halfSize.y, _halfSize.y, znear, zfar);
}

void Mat4::setFrustum(float left, float right, float bottom, float top, float znear, float zfar)
{
    float temp1 = 2 * znear;
    float temp2 = right - left;
    float temp3 = top - bottom;
    float temp4 = zfar - znear;

    data[0] = temp1 / temp2;
    data[1] = 0;
    data[2] = 0;
    data[3] = 0;
    data[4] = 0;
    data[5] = temp1 / temp3;
    data[6] = 0;
    data[7] = 0;
    data[8] = (right + left) / temp2;
    data[9] = (top + bottom) / temp3;
    data[10] = (-zfar - znear) / temp4;
    data[11] = -1;
    data[12] = 0;
    data[13] = 0;
    data[14] = (-temp1 * zfar) / temp4;
    data[15] = 0;
}

void Mat4::setOrtho(float left, float right, float bottom, float top, float near, float far)
{
    data[0] = 2.f / (right - left);
    data[1] = 0;
    data[2] = 0;
    data[3] = 0;

    data[4] = 0;
    data[5] = 2.f / (top - bottom);
    data[6] = 0;
    data[7] = 0;

    data[8] = 0;
    data[9] = 0;
    data[10] = -2.f / (far - near);
    data[11] = 0;

    data[12] = -(right + left) / (right - left);
    data[13] = -(top + bottom) / (top - bottom);
    data[14] = -(far + near) / (far - near);
    data[15] = 1.f;
}

void Mat4::mul2(Mat4 &lhs, Mat4 &rhs)
{
    float *a = lhs.data;
    float *b = rhs.data;
    float *r = this->data;

    float a00 = a[0];
    float a01 = a[1];
    float a02 = a[2];
    float a03 = a[3];
    float a10 = a[4];
    float a11 = a[5];
    float a12 = a[6];
    float a13 = a[7];
    float a20 = a[8];
    float a21 = a[9];
    float a22 = a[10];
    float a23 = a[11];
    float a30 = a[12];
    float a31 = a[13];
    float a32 = a[14];
    float a33 = a[15];

    float b0, b1, b2, b3;

    b0 = b[0];
    b1 = b[1];
    b2 = b[2];
    b3 = b[3];
    r[0] = a00 * b0 + a10 * b1 + a20 * b2 + a30 * b3;
    r[1] = a01 * b0 + a11 * b1 + a21 * b2 + a31 * b3;
    r[2] = a02 * b0 + a12 * b1 + a22 * b2 + a32 * b3;
    r[3] = a03 * b0 + a13 * b1 + a23 * b2 + a33 * b3;

    b0 = b[4];
    b1 = b[5];
    b2 = b[6];
    b3 = b[7];
    r[4] = a00 * b0 + a10 * b1 + a20 * b2 + a30 * b3;
    r[5] = a01 * b0 + a11 * b1 + a21 * b2 + a31 * b3;
    r[6] = a02 * b0 + a12 * b1 + a22 * b2 + a32 * b3;
    r[7] = a03 * b0 + a13 * b1 + a23 * b2 + a33 * b3;

    b0 = b[8];
    b1 = b[9];
    b2 = b[10];
    b3 = b[11];
    r[8] = a00 * b0 + a10 * b1 + a20 * b2 + a30 * b3;
    r[9] = a01 * b0 + a11 * b1 + a21 * b2 + a31 * b3;
    r[10] = a02 * b0 + a12 * b1 + a22 * b2 + a32 * b3;
    r[11] = a03 * b0 + a13 * b1 + a23 * b2 + a33 * b3;

    b0 = b[12];
    b1 = b[13];
    b2 = b[14];
    b3 = b[15];
    r[12] = a00 * b0 + a10 * b1 + a20 * b2 + a30 * b3;
    r[13] = a01 * b0 + a11 * b1 + a21 * b2 + a31 * b3;
    r[14] = a02 * b0 + a12 * b1 + a22 * b2 + a32 * b3;
    r[15] = a03 * b0 + a13 * b1 + a23 * b2 + a33 * b3;
}

void Mat4::mul(Mat4 &m)
{
    mul2(*this, m);
}

void Mat4::setFromMat3(Mat3 &mat3)
{
    setIdentity();
    data[0] = mat3.data[0];
    data[1] = mat3.data[1];
    data[4] = mat3.data[2];
    data[5] = mat3.data[3];
    data[12] = mat3.data[4];
    data[13] = mat3.data[5];
}

void Mat4::transformPoint(Vec3& vec,Vec3& res){
    float x = vec.x;
    float y = vec.y;
    float z = vec.z;

    res.x = x * data[0] + y * data[4] + z * data[8] + data[12];
    res.y = x * data[1] + y * data[5] + z * data[9] + data[13];
    res.z = x * data[2] + y * data[6] + z * data[10] + data[14];
}

void Mat4::transformVector(Vec3& vec,Vec3& res){
    float x = vec.x;
    float y = vec.y;
    float z = vec.z;
    res.x = x * data[0] + y * data[4] + z * data[8];
    res.y = x * data[1] + y * data[5] + z * data[9];
    res.z = x * data[2] + y * data[6] + z * data[10];
}

void Mat4::copy(Mat4 &mat)
{
    data[0] = mat.data[0];
    data[1] = mat.data[1];
    data[2] = mat.data[2];
    data[3] = mat.data[3];
    data[4] = mat.data[4];
    data[5] = mat.data[5];
    data[6] = mat.data[6];
    data[7] = mat.data[7];
    data[8] = mat.data[8];
    data[9] = mat.data[9];
    data[10] = mat.data[10];
    data[11] = mat.data[11];
    data[12] = mat.data[12];
    data[13] = mat.data[13];
    data[14] = mat.data[14];
    data[15] = mat.data[15];
}

void Mat4::invert()
{

    //https://stackoverflow.com/questions/79040015/is-there-a-better-way-to-write-simd-code-to-invert-a-transformation-matrix
    
    float *m = data;

    float a00 = m[0];
    float a01 = m[1];
    float a02 = m[2];
    float a03 = m[3];
    float a10 = m[4];
    float a11 = m[5];
    float a12 = m[6];
    float a13 = m[7];
    float a20 = m[8];
    float a21 = m[9];
    float a22 = m[10];
    float a23 = m[11];
    float a30 = m[12];
    float a31 = m[13];
    float a32 = m[14];
    float a33 = m[15];

    float b00 = a00 * a11 - a01 * a10;
    float b01 = a00 * a12 - a02 * a10;
    float b02 = a00 * a13 - a03 * a10;
    float b03 = a01 * a12 - a02 * a11;
    float b04 = a01 * a13 - a03 * a11;
    float b05 = a02 * a13 - a03 * a12;
    float b06 = a20 * a31 - a21 * a30;
    float b07 = a20 * a32 - a22 * a30;
    float b08 = a20 * a33 - a23 * a30;
    float b09 = a21 * a32 - a22 * a31;
    float b10 = a21 * a33 - a23 * a31;
    float b11 = a22 * a33 - a23 * a32;

    float det = (b00 * b11 - b01 * b10 + b02 * b09 + b03 * b08 - b04 * b07 + b05 * b06);
    if (det == 0)
    {
        setIdentity();
    }
    else
    {
        float invDet = 1 / det;

        m[0] = (a11 * b11 - a12 * b10 + a13 * b09) * invDet;
        m[1] = (-a01 * b11 + a02 * b10 - a03 * b09) * invDet;
        m[2] = (a31 * b05 - a32 * b04 + a33 * b03) * invDet;
        m[3] = (-a21 * b05 + a22 * b04 - a23 * b03) * invDet;
        m[4] = (-a10 * b11 + a12 * b08 - a13 * b07) * invDet;
        m[5] = (a00 * b11 - a02 * b08 + a03 * b07) * invDet;
        m[6] = (-a30 * b05 + a32 * b02 - a33 * b01) * invDet;
        m[7] = (a20 * b05 - a22 * b02 + a23 * b01) * invDet;
        m[8] = (a10 * b10 - a11 * b08 + a13 * b06) * invDet;
        m[9] = (-a00 * b10 + a01 * b08 - a03 * b06) * invDet;
        m[10] = (a30 * b04 - a31 * b02 + a33 * b00) * invDet;
        m[11] = (-a20 * b04 + a21 * b02 - a23 * b00) * invDet;
        m[12] = (-a10 * b09 + a11 * b07 - a12 * b06) * invDet;
        m[13] = (a00 * b09 - a01 * b07 + a02 * b06) * invDet;
        m[14] = (-a30 * b03 + a31 * b01 - a32 * b00) * invDet;
        m[15] = (a20 * b03 - a21 * b01 + a22 * b00) * invDet;
    }
}

void Mat4::toString()
{
    printf(
        "(%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f) \n",
    data[0],
    data[1],
    data[2],
    data[3],
    data[4],
    data[5],
    data[6],
    data[7],
    data[8],
    data[9],
    data[10],
    data[11],
    data[12],
    data[13],
    data[14],
    data[15]
  );
}