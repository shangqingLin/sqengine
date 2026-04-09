#include "SqMotorJoint.h"
#include "../../common/math/SqMath.h"
#include "../SqWorld.h"
#include "../sim/SqJointSim.h"

using namespace phxy;

SqMotorJoint::SqMotorJoint()
{
	type = SqJointType::sq_motorJoint;
}

void SqMotorJoint::SetMaxForce(float maxForce)
{
	this->maxForce = max(0.0f, maxForce);
}

float SqMotorJoint::GetMaxForce()
{
	return maxForce;
}

void SqMotorJoint::SetMaxTorque(float maxTorque)
{
	this->maxTorque = max(0.0f, maxTorque);
}

float SqMotorJoint::GetMaxTorque()
{
	return maxTorque;
}

void SqMotorJoint::SetCorrectionFactor(float correctionFactor)
{
	this->correctionFactor = clamp(correctionFactor, 0.0f, 1.0f);
}

float SqMotorJoint::GetCorrectionFactor()
{
	return correctionFactor;
}

// SqVec2 b2GetMotorJointForce(b2World *world, b2JointSim *base)
// {
// 	SqVec2 force = SqVec2::MulSV(world->inv_h, base->motorJoint.linearImpulse);
// 	return force;
// }

// float b2GetMotorJointTorque(b2World *world, b2JointSim *base)
// {
// 	return world->inv_h * base->motorJoint.angularImpulse;
// }

// Point-to-point constraint
// C = p2 - p1
// Cdot = v2 - v1
//      = v2 + cross(w2, r2) - v1 - cross(w1, r1)
// J = [-I -r1_skew I r2_skew ]
// Identity used:
// w k % (rx i + ry j) = w * (-ry i + rx j)

// Angle constraint
// C = angle2 - angle1 - referenceAngle
// Cdot = w2 - w1
// J = [0 0 -1 0 0 1]
// K = invI1 + invI2

void SqMotorJoint::solvePrepare(const SqStepContext &context)
{

	SqJointSim *jointSim = getJointSim();
	SqBody *bodyA = world->getBody(jointSim->bodyAIndex);
	SqBody *bodyB = world->getBody(jointSim->bodyBIndex);

	SQ_ASSERT(bodyA->isAwake() || bodyB->isAwake());

	SqBodySim *bodySimA = bodyA->getBodySim();
	SqBodySim *bodySimB = bodyB->getBodySim();

	float mA = bodySimA->invMass;
	float iA = bodySimA->invInertia;
	float mB = bodySimB->invMass;
	float iB = bodySimB->invInertia;

	frameA.q = SqRot::Mul(bodySimA->transform.q, jointSim->localFrameA.q);
	frameA.p = SqTransform::transformVector(bodySimA->transform, SqVec2::Sub(jointSim->localFrameA.p, bodySimA->localCenter));
	frameB.q = SqRot::Mul(bodySimB->transform.q, jointSim->localFrameB.q);
	frameB.p = SqTransform::transformVector(bodySimB->transform, SqVec2::Sub(jointSim->localFrameB.p, bodySimB->localCenter));

	deltaCenter = SqVec2::Sub(bodySimB->center, bodySimA->center);

	const SqVec2 &rA = frameA.p;
	const SqVec2 &rB = frameB.p;

	SqMat22 K;
	K.cx.x = mA + mB + rA.y * rA.y * iA + rB.y * rB.y * iB;
	K.cx.y = -rA.y * rA.x * iA - rB.y * rB.x * iB;
	K.cy.x = K.cx.y;
	K.cy.y = mA + mB + rA.x * rA.x * iA + rB.x * rB.x * iB;
	this->linearMass = SqMat22::GetInverse22(K);

	float ka = iA + iB;
	this->angularMass = ka > 0.0f ? 1.0f / ka : 0.0f;

	if (!world->isEnableWarmdStart())
	{
		this->linearImpulse.zero();
		this->angularImpulse = 0.0f;
	}
}

void SqMotorJoint::warmStart(const SqStepContext &context)
{

	SqJointSim *jointSim = getJointSim();
	SqBody *bodyA = world->getBody(jointSim->bodyAIndex);
	SqBody *bodyB = world->getBody(jointSim->bodyBIndex);

	SQ_ASSERT(bodyA->isAwake() || bodyB->isAwake());

	SqBodySim *bodySimA = bodyA->getBodySim();
	SqBodySim *bodySimB = bodyB->getBodySim();

	float mA = bodySimA->invMass;
	float iA = bodySimA->invInertia;
	float mB = bodySimB->invMass;
	float iB = bodySimB->invInertia;

	SqBodySim dummyState;

	SqBodySim *stateA = bodyA->isAwake() ? bodySimA : &dummyState;
	SqBodySim *stateB = bodyB->isAwake() ? bodySimB : &dummyState;

	SqVec2 rA = SqRot::transformVector(stateA->deltaRotation, frameA.p);
	SqVec2 rB = SqRot::transformVector(stateB->deltaRotation, frameB.p);

	stateA->linearVelocity = SqVec2::MulSub(stateA->linearVelocity, mA, this->linearImpulse);
	stateA->angularVelocity -= iA * (SqVec2::Cross(rA, this->linearImpulse) + this->angularImpulse);
	stateB->linearVelocity = SqVec2::MulAdd(stateB->linearVelocity, mB, this->linearImpulse);
	stateB->angularVelocity += iB * (SqVec2::Cross(rB, this->linearImpulse) + this->angularImpulse);
}

void SqMotorJoint::solve(const SqStepContext &context, bool useBias)
{
	SqJointSim *jointSim = getJointSim();
	SqBody *bodyA = world->getBody(jointSim->bodyAIndex);
	SqBody *bodyB = world->getBody(jointSim->bodyBIndex);

	SqBodySim *bodySimA = bodyA->getBodySim();
	SqBodySim *bodySimB = bodyB->getBodySim();

	float mA = bodySimA->invMass;
	float iA = bodySimA->invInertia;
	float mB = bodySimB->invMass;
	float iB = bodySimB->invInertia;

	SqBodySim dummyState;

	SqBodySim *stateA = bodyA->isAwake() ? bodySimA : &dummyState;
	SqBodySim *stateB = bodyB->isAwake() ? bodySimB : &dummyState;

	SqVec2 vA = stateA->linearVelocity;
	float wA = stateA->angularVelocity;
	SqVec2 vB = stateB->linearVelocity;
	float wB = stateB->angularVelocity;

	// angular constraint
	{
		SqRot qA = SqRot::Mul(stateA->deltaRotation, frameA.q);
		SqRot qB = SqRot::Mul(stateB->deltaRotation, frameB.q);
		SqRot relQ = SqRot::InvMulRot(qA, qB);

		float jointAngle = SqRot::GetAngle(relQ);
		float angularBias = context.inv_h * this->correctionFactor * jointAngle;

		float Cdot = wB - wA;
		float impulse = -this->angularMass * (Cdot + angularBias);

		float oldImpulse = this->angularImpulse;
		float maxImpulse = context.h * this->maxTorque;
		this->angularImpulse = clamp(this->angularImpulse + impulse, -maxImpulse, maxImpulse);
		impulse = this->angularImpulse - oldImpulse;

		wA -= iA * impulse;
		wB += iB * impulse;
	}

	// linear constraint
	{

		/**
		 * 下面三句语句的解析请查看 SqWheelJoint的说明
		 */
		SqVec2 rA = SqRot::transformVector(stateA->deltaRotation, frameA.p);
		SqVec2 rB = SqRot::transformVector(stateB->deltaRotation, frameB.p);
		SqVec2 ds = SqVec2::Add(SqVec2::Sub(stateB->deltaPosition, stateA->deltaPosition), SqVec2::Sub(rB, rA));
		
		SqVec2 linearSeparation = SqVec2::Add(this->deltaCenter, ds);
		SqVec2 linearBias = SqVec2::MulSV(context.inv_h * this->correctionFactor, linearSeparation);

		SqVec2 Cdot = SqVec2::Sub(SqVec2::Add(vB, SqVec2::CrossSV(wB, rB)), SqVec2::Add(vA, SqVec2::CrossSV(wA, rA)));
		SqVec2 b = SqMat22::MulMV(this->linearMass, SqVec2::Add(Cdot, linearBias));
		SqVec2 impulse = {-b.x, -b.y};

		SqVec2 oldImpulse = this->linearImpulse;
		float maxImpulse = context.h * this->maxForce;
		this->linearImpulse = SqVec2::Add(this->linearImpulse, impulse);

		if (SqVec2::LengthSquared(this->linearImpulse) > maxImpulse * maxImpulse)
		{
			this->linearImpulse = SqVec2::Normalize(this->linearImpulse);
			this->linearImpulse.x *= maxImpulse;
			this->linearImpulse.y *= maxImpulse;
		}

		impulse = SqVec2::Sub(this->linearImpulse, oldImpulse);

		vA = SqVec2::MulSub(vA, mA, impulse);
		wA -= iA * SqVec2::Cross(rA, impulse);
		vB = SqVec2::MulAdd(vB, mB, impulse);
		wB += iB * SqVec2::Cross(rB, impulse);
	}

	stateA->linearVelocity = vA;
	stateA->angularVelocity = wA;
	stateB->linearVelocity = vB;
	stateB->angularVelocity = wB;
}
