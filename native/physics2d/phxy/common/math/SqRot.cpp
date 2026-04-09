#include "SqRot.h"
#include "SqMath.h"

using namespace phxy;

SqRot::SqRot()
{
	identity();
}

SqRot::SqRot(float c, float s) : c(c), s(s)
{
}

SqRot::SqRot(float radius)
{
	setRadius(radius);
}

void SqRot::identity()
{
	c = 1.0f;
	s = 0.0f;
}

void SqRot::setRadius(float angle)
{
	s = sinf(angle);
	c = cosf(angle);
}

SqRot SqRot::Normalize(const SqRot &q)
{
	float mag = sqrtf(q.s * q.s + q.c * q.c);
	float invMag = mag > 0.0 ? 1.0f / mag : 0.0f;
	SqRot qn = {q.c * invMag, q.s * invMag};
	return qn;
}

bool SqRot::isValid(const SqRot &q)
{
	if (isnan(q.s) || isnan(q.c))
	{
		return false;
	}

	if (isinf(q.s) || isinf(q.c))
	{
		return false;
	}
	return SqRot::isNormalized(q);
}

bool SqRot::isNormalized(const SqRot &q)
{
	float qq = q.s * q.s + q.c * q.c;
	return 1.0f - 0.0006f < qq && qq < 1.0f + 0.0006f;
}

/**
 * 矩阵乘法。相当于两个角度相加，然后构建SqRot
 */
SqRot SqRot::Mul(const SqRot &q, const SqRot &r)
{
	// 对于旋转的元素对角线相等的，求一个就行
	// [qc -qs] * [rc -rs] = [qc*rc-qs*rs -qc*rs-qs*rc]
	// [qs  qc]   [rs  rc]   [qs*rc+qc*rs -qs*rs+qc*rc]
	// s(q + r) = qs * rc + qc * rs
	// c(q + r) = qc * rc - qs * rs
	SqRot qr;
	qr.s = q.s * r.c + q.c * r.s;
	qr.c = q.c * r.c - q.s * r.s;
	return qr;
}

/**
 * 相当于两个角度相减得到差角，构建SqRot
 */
SqRot SqRot::InvMulRot(const SqRot &a, const SqRot &b)
{
	// [ ac as] * [bc -bs] = [ac*bc+as*bs -ac*bs+as*bc]
	// [-as ac]   [bs  bc]   [-as*bc+ac*bs as*bs+ac*bc]
	// s(a - b) = ac * bs - as * bc  = sin(a - b)
	// c(a - b) = ac * bc + as * bs  = cost(a- b)
	SqRot r;
	r.s = a.c * b.s - a.s * b.c;
	r.c = a.c * b.c + a.s * b.s;
	return r;
}

/**
 * 先求逆，然后将变换向量
 */
SqVec2 SqRot::invRotateVector(const SqRot &q, const SqVec2 &v)
{
	return SqVec2{q.c * v.x + q.s * v.y, -q.s * v.x + q.c * v.y};
}

/**
 *
 * 对角速度进行积分，也就是将deltaAngle累积到q1上
 * deltaAngle是一个角位移，即deltaAngle = dt * ω
 */
SqRot SqRot::IntegrateRotation(const SqRot &q1, float deltaAngle)
{

	/**
	 *
	 * 公式推导：
	 *
	 * 按照正常的逻辑理解，角度叠加应该是这样的：
	 *  cos(θ + deltaAngle) = cosθ·cosΔθ - sinθ·sinΔθ
	 *	sin(θ + deltaAngle) = sinθ·cosΔθ + cosθ·sinΔθ
	 * 即在原来的角度基础上加上deltaAngle，然后再重新求cos和sin
	 *
	 * 当这里为了减少sin和cos的运算，提升性能，
	 * 这里把当 deltaAngle当时是一个很小的变化，所以cos deltaAngle 和 sin deltaAngle可以近似：
	 *	cos deltaAngle ≈ 1（因为 cos(0) = 1，且小角度的余弦接近1）
	 *	sin deltaAngle ≈ Δθ（因为 sin(x) ≈ x 当 x → 0）
	 *
	 * 即:
	 *  dc/dt = 1;
	 *  ds/dt = deltaAngle;
	 *
	 * 代入回原来的公式中：
	 * cos(θ + deltaAngle) ≈ cosθ · 1 - sinθ · deltaAngle = cosθ - deltaAngle · sinθ
	 * sin(θ + deltaAngle) ≈ sinθ · 1 + cosθ · deltaAngle = sinθ + deltaAngle · cosθ
	 *
	 * 这就是欧拉积分
	 */
	SqRot q2 = {q1.c - deltaAngle * q1.s, q1.s + deltaAngle * q1.c};

	// 由于欧拉积分会引入误差，导致 (c2, s2) 不再是单位向量，因此需重新归一化
	// 这个只是一个角度变化，不是真正的向量，在矩阵也就是一个缩放值，因此不需要长度
	float mag = sqrtf(q2.s * q2.s + q2.c * q2.c);
	float invMag = mag > 0.0 ? 1.0f / mag : 0.0f;
	q2.c *= invMag;
	q2.s *= invMag;
	return q2;
}

SqVec2 SqRot::transformVector(const SqRot &q, const SqVec2 &v)
{
	return SqVec2{
		q.c * v.x - q.s * v.y, 
		q.s * v.x + q.c * v.y
	};
}

/// Normalized linear interpolation
/// https://fgiesen.wordpress.com/2012/08/15/linear-interpolation-past-present-and-future/
///	https://web.archive.org/web/20170825184056/http://number-none.com/product/Understanding%20Slerp,%20Then%20Not%20Using%20It/
SqRot SqRot::NLerp(const SqRot &q1, const SqRot &q2, float t)
{
	float omt = 1.0f - t;
	SqRot q = {
		omt * q1.c + t * q2.c,
		omt * q1.s + t * q2.s,
	};

	float mag = sqrtf(q.s * q.s + q.c * q.c);
	float invMag = mag > 0.0 ? 1.0f / mag : 0.0f;
	SqRot qn = {q.c * invMag, q.s * invMag};
	return qn;
}

float SqRot::RelativeAngle(const SqRot &a, const SqRot &b)
{
	// sin(b - a) = bs * ac - bc * as
	// cos(b - a) = bc * ac + bs * as
	float s = a.c * b.s - a.s * b.c;
	float c = a.c * b.c + a.s * b.s;
	return SqAtan2(s, c);
}

float SqRot::GetAngle(const SqRot &q)
{
	return SqAtan2(q.s, q.c);
}