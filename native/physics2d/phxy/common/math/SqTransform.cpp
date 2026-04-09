#include "SqTransform.h"

using namespace phxy;

SqTransform::SqTransform()
{
	identity();
}

SqTransform::SqTransform(SqVec2 p, SqRot q) : p(p), q(q)
{
}

SqVec2 SqTransform::transformPoint(const SqVec2 &v) const
{
	float x = (q.c * v.x - q.s * v.y) + p.x;
	float y = (q.s * v.x + q.c * v.y) + p.y;

	return SqVec2{x, y};
}

SqVec2 SqTransform::transformVector(const SqVec2 &v) const
{
	return SqVec2{q.c * v.x - q.s * v.y, q.s * v.x + q.c * v.y};
}

void SqTransform::identity()
{
	p.zero();
	q.identity();
}

/**
 * 先求逆矩阵，然后乘以这个向量
 */
SqVec2 SqTransform::invTransformVector(const SqVec2 &v) const
{
	return SqVec2{q.c * v.x + q.s * v.y, -q.s * v.x + q.c * v.y};
}

/**
 * 先求逆矩阵，然后乘以这个点
 */
SqVec2 SqTransform::invTransformPoint(const SqVec2 &v) const
{
	float vx = v.x - p.x;
	float vy = v.y - p.y;
	return SqVec2{q.c * vx + q.s * vy, -q.s * vx + q.c * vy};
}

/**
 * 矩阵乘法
 */
SqTransform SqTransform::Mul(const SqTransform &A, const SqTransform &B)
{

	/**
	 * [ ac as 0] * [bc -bs 0] = [ac * bc + as * bs   		-ac *bs + as * bc 		0]
	 * [-as ac 0]   [bs  bc 0]   [-as * bc + ac * bs  		as * bs + ac * bc 		0]
	 * [ ax ay 1]   [bx  by 1]   [ac * bx + -as * by + ax  	as * bx + ac * by + ay 	1]
	 */
	SqTransform C;
	C.q = SqRot::Mul(A.q, B.q);

	// transformVector = q.c * v.x - q.s * v.y, q.s * v.x + q.c * v.y
	C.p = SqVec2::Add(SqRot::transformVector(A.q, B.p), A.p);
	return C;
}

/**
 * 先求A的逆矩阵，然后这个逆矩阵乘以B。相当于将B转换到A中的局部坐标系中
 *
 */
SqTransform SqTransform::InvMulTransforms(const SqTransform &A, const SqTransform &B)
{
	SqTransform C;
	C.q = SqRot::InvMulRot(A.q, B.q);
	C.p = ((SqTransform &)A).invTransformVector(SqVec2::Sub(B.p, A.p));
	return C;
}

SqVec2 SqTransform::transformPoint(const SqTransform &A, const SqVec2 &p)
{
	return ((SqTransform &)A).transformPoint(p);
}

SqVec2 SqTransform::transformVector(const SqTransform &A, const SqVec2 &v)
{
	return ((SqTransform &)A).transformVector(v);
}