#include "SqMouseJoint.h"
#include "../sim/SqJointSim.h"

#include "../SqWorld.h"

using namespace phxy;

SqMouseJoint::SqMouseJoint()
{
	type = SqJointType::sq_mouseJoint;
}

void SqMouseJoint::SetSpringHertz(float hertz)
{
	this->hertz = hertz;
}

float SqMouseJoint::GetSpringHertz()
{
	return hertz;
}

void SqMouseJoint::SetSpringDampingRatio(float dampingRatio)
{
	this->dampingRatio = dampingRatio;
}

float SqMouseJoint::GetSpringDampingRatio()
{
	return dampingRatio;
}

void SqMouseJoint::SetMaxForce(float maxForce)
{
	this->maxForce = maxForce;
}

float SqMouseJoint::GetMaxForce()
{
	return maxForce;
}

// SqVec2 b2GetMouseJointForce( b2World* world, b2JointSim* base )
// {
// 	SqVec2 force = SqVec2::MulSV( world->inv_h, base->mouseJoint.linearImpulse );
// 	return force;
// }

// float b2GetMouseJointTorque( b2World* world, b2JointSim* base )
// {
// 	return world->inv_h * base->mouseJoint.angularImpulse;
// }

void SqMouseJoint::solvePrepare(const SqStepContext &context)
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

	// Compute the initial center delta. Incremental position updates are relative to this.
	deltaCenter = SqVec2::Sub(bodySimB->center, bodySimA->center);

	this->linearSoftness.step(this->hertz, this->dampingRatio, context.h);

	float angularHertz = 0.5f;
	float angularDampingRatio = 0.1f;
	this->angularSoftness.step(angularHertz, angularDampingRatio, context.h);

	const SqVec2 &rA = frameA.p;
	const SqVec2 &rB = frameB.p;

	// 点对点约束K矩阵
	// K = [(1/m1 + 1/m2) * eye(2) - skew(r1) * invI1 * skew(r1) - skew(r2) * invI2 * skew(r2)]
	//   = [1/m1+1/m2     0    ] + invI1 * [r1.y*r1.y -r1.x*r1.y] + invI2 * [r1.y*r1.y -r1.x*r1.y]
	//     [    0     1/m1+1/m2]           [-r1.x*r1.y r1.x*r1.x]           [-r1.x*r1.y r1.x*r1.x]
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
		linearImpulse.zero();
		angularImpulse = 0.0f;
	}
}

void SqMouseJoint::warmStart(const SqStepContext &context)
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

	SqVec2 rA = SqRot::transformVector(stateA->deltaRotation, this->frameA.p);
	SqVec2 rB = SqRot::transformVector(stateB->deltaRotation, this->frameB.p);

	stateA->linearVelocity = SqVec2::MulSub(stateA->linearVelocity, mA, this->linearImpulse);
	stateA->angularVelocity -= iA * (SqVec2::Cross(rA, this->linearImpulse) + this->angularImpulse);
	stateB->linearVelocity = SqVec2::MulAdd(stateB->linearVelocity, mB, this->linearImpulse);
	stateB->angularVelocity += iB * (SqVec2::Cross(rB, this->linearImpulse) + this->angularImpulse);
}

void SqMouseJoint::solve(const SqStepContext &context, bool useBias)
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

	{
		// 将BodyB的角速度调整到和BodyA一样
		float massScale = this->angularSoftness.massScale;
		float impulseScale = this->angularSoftness.impulseScale;

		float Cdot = wB - wA;
		float impulse = -massScale * this->angularMass * Cdot - impulseScale * this->angularImpulse;
		this->angularImpulse += impulse;

		// printf("wB %f wA %f angularImpulse %f Cdot %f impulseScale %f \n", wB, wA, this->angularImpulse, Cdot, impulseScale);

		wA -= iA * impulse;
		wB += iB * impulse;
	}

	float maxImpulse = this->maxForce * context.h;

	{
		SqVec2 rA = SqRot::transformVector(stateA->deltaRotation, this->frameA.p);
		SqVec2 rB = SqRot::transformVector(stateB->deltaRotation, this->frameB.p);

		SqVec2 Cdot = SqVec2::Sub(SqVec2::Add(vB, SqVec2::CrossSV(wB, rB)), SqVec2::Add(vA, SqVec2::CrossSV(wA, rA)));

		SqVec2 dcA = stateA->deltaPosition;
		SqVec2 dcB = stateB->deltaPosition;
		SqVec2 C = SqVec2::Add(SqVec2::Add(SqVec2::Sub(dcB, dcA), SqVec2::Sub(rB, rA)), this->deltaCenter);
		SqVec2 bias = SqVec2::MulSV(this->linearSoftness.biasRate, C);

		float massScale = this->linearSoftness.massScale;
		float impulseScale = this->linearSoftness.impulseScale;

		SqVec2 b = SqMat22::MulMV(this->linearMass, SqVec2::Add(Cdot, bias));

		SqVec2 impulse;
		impulse.x = -massScale * b.x - impulseScale * this->linearImpulse.x;
		impulse.y = -massScale * b.y - impulseScale * this->linearImpulse.y;

		SqVec2 oldImpulse = this->linearImpulse;
		this->linearImpulse.x += impulse.x;
		this->linearImpulse.y += impulse.y;

		if (maxForce > 0.f)
		{
			float length = SqVec2::Length(this->linearImpulse);
			if (length > maxImpulse)
			{
				this->linearImpulse = SqVec2::MulSV(maxImpulse / length, this->linearImpulse);
			}
		}

		impulse.x = this->linearImpulse.x - oldImpulse.x;
		impulse.y = this->linearImpulse.y - oldImpulse.y;

		vA = SqVec2::MulSub(vA, mA, impulse);
		wA -= iA * SqVec2::Cross(rA, impulse);
		vB = SqVec2::MulAdd(vB, mB, impulse);
		wB += iB * SqVec2::Cross(rB, impulse);

		// printf(" mouse  vB %f %f vA %f %f impulse %f %f \n", vB.x, vB.y, vA.x, vA.y, impulse.x, impulse.y);
	}

	stateA->linearVelocity = vA;
	stateA->angularVelocity = wA;
	stateB->linearVelocity = vB;
	stateB->angularVelocity = wB;
}
