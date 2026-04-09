#include "SqRevoluteJoint.h"
#include "../../common/SqCore.h"
#include "../../common/math/SqVec2.h"
#include "../../common/math/SqTransform.h"
#include "../../common/math/SqMath.h"
#include "../sim/SqJointSim.h"
#include "../../common/math/SqMat22.h"
#include "../SqWorld.h"
#include <engine/scene/Node.h>

using namespace phxy;

SqRevoluteJoint::SqRevoluteJoint()
{
	type = SqJointType::sq_revoluteJoint;
}

void SqRevoluteJoint::setEnableSpring(bool enableSpring)
{
	if (enableSpring != this->enableSpring)
	{
		this->enableSpring = enableSpring;
		this->springImpulse = 0.0f;
	}
}

bool SqRevoluteJoint::isSpringEnabled()
{
	return enableSpring;
}

void SqRevoluteJoint::setSpringHertz(float hertz)
{
	this->hertz = hertz;
}

float SqRevoluteJoint::getSpringHertz()
{
	return hertz;
}

void SqRevoluteJoint::setSpringDampingRatio(float dampingRatio)
{
	this->dampingRatio = dampingRatio;
}

float SqRevoluteJoint::getSpringDampingRatio()
{
	return dampingRatio;
}

void SqRevoluteJoint::setTargetAngle(float angle)
{
	targetAngle = angle;
}

float SqRevoluteJoint::getTargetAngle()
{
	return targetAngle;
}

/**
 * 获取两个Body之间的夹角
 */
float SqRevoluteJoint::getAngle()
{
	SqJointSim *jointSim = getJointSim();
	SqBody *bodyA = world->getBody(jointSim->bodyAIndex);
	SqBody *bodyB = world->getBody(jointSim->bodyBIndex);
	SqTransform transformA = bodyA->getTransform();
	SqTransform transformB = bodyB->getTransform();
	SqRot qA = SqRot::Mul(transformA.q, jointSim->localFrameA.q);
	SqRot qB = SqRot::Mul(transformB.q, jointSim->localFrameB.q);
	float angle = SqRot::RelativeAngle(qA, qB);
	return angle;
}

void SqRevoluteJoint::setEnableLimit(bool enableLimit)
{
	if (enableLimit != this->enableLimit)
	{
		this->enableLimit = enableLimit;
		lowerImpulse = 0.0f;
		upperImpulse = 0.0f;
	}
}

bool SqRevoluteJoint::isLimitEnabled()
{
	return enableLimit;
}

float SqRevoluteJoint::getLowerLimit()
{
	return lowerAngle;
}

float SqRevoluteJoint::getUpperLimit()
{
	return upperAngle;
}

void SqRevoluteJoint::setLimits(float lower, float upper)
{
	// SQ_ASSERT(lower >= -0.99f * SQ_PI); //-180
	// SQ_ASSERT(upper <= 0.99f * SQ_PI);	// 180

	if (lower != lowerAngle || upper != upperAngle)
	{
		lowerAngle = min(lower, upper);
		upperAngle = max(lower, upper);
		lowerImpulse = 0.0f;
		upperImpulse = 0.0f;
	}
}

void SqRevoluteJoint::setEnableMotor(bool enableMotor)
{
	if (enableMotor != this->enableMotor)
	{
		this->enableMotor = enableMotor;
		motorImpulse = 0.0f;
	}
}

bool SqRevoluteJoint::isMotorEnabled()
{
	return enableMotor;
}

void SqRevoluteJoint::setMotorSpeed(float motorSpeed)
{
	this->motorSpeed = motorSpeed;
}

float SqRevoluteJoint::getMotorSpeed()
{
	return motorSpeed;
}

/**
 * 获取当前motor上应用的力距大小
 */
float SqRevoluteJoint::getMotorTorque()
{
	return motorImpulse / world->dt;
}

void SqRevoluteJoint::setMaxMotorTorque(float torque)
{
	maxMotorTorque = torque;
}

float SqRevoluteJoint::getMaxMotorTorque()
{
	return maxMotorTorque;
}

SqVec2 SqRevoluteJoint::getRevoluteJointForce()
{
	return SqVec2::MulSV(10.f / world->dt, linearImpulse);
}

float SqRevoluteJoint::getRevoluteJointTorque()
{
	return world->dt * (motorImpulse + lowerImpulse - upperImpulse);
}

void SqRevoluteJoint::setEnableSim(bool b)
{
	enable = b;
	if (b)
	{
		motorImpulse = 0.f;
		springImpulse = 0.f;
		lowerImpulse = 0.f;
		upperImpulse = 0.f;
		linearImpulse.x = 0.f;
		linearImpulse.y = 0.f;
	}
}

void SqRevoluteJoint::warmStart(const SqStepContext &context)
{

	SqJointSim *sim = getJointSim();
	SqBody *bodyA = world->getBody(sim->bodyAIndex);
	SqBody *bodyB = world->getBody(sim->bodyBIndex);
	SqBodySim *bodySimA = bodyA->getBodySim();
	SqBodySim *bodySimB = bodyB->getBodySim();

	SqBodySim dummyState;
	SqBodySim *stateA = bodyA->isAwake() ? bodySimA : &dummyState;
	SqBodySim *stateB = bodyB->isAwake() ? bodySimB : &dummyState;

	float mA = bodySimA->invMass;
	float mB = bodySimB->invMass;
	float iA = bodySimA->invInertia;
	float iB = bodySimB->invInertia;

	SqVec2 rA = SqRot::transformVector(stateA->deltaRotation, this->frameA.p);
	SqVec2 rB = SqRot::transformVector(stateB->deltaRotation, this->frameB.p);

	float axialImpulse = this->springImpulse + this->motorImpulse + this->lowerImpulse - this->upperImpulse;

	stateA->linearVelocity = SqVec2::MulSub(stateA->linearVelocity, mA, this->linearImpulse);
	stateA->angularVelocity -= iA * (SqVec2::Cross(rA, this->linearImpulse) + axialImpulse);

	stateB->linearVelocity = SqVec2::MulAdd(stateB->linearVelocity, mB, this->linearImpulse);
	stateB->angularVelocity += iB * (SqVec2::Cross(rB, this->linearImpulse) + axialImpulse);
}

void SqRevoluteJoint::solvePrepare(const SqStepContext &context)
{

	SqJointSim *sim = getJointSim();
	SqBody *bodyA = world->getBody(sim->bodyAIndex);
	SqBody *bodyB = world->getBody(sim->bodyBIndex);

	SQ_ASSERT(bodyA->isAwake() || bodyB->isAwake());

	SqBodySim *bodySimA = bodyA->getBodySim();
	SqBodySim *bodySimB = bodyB->getBodySim();

	// 计算锚点所在的世界坐标，但这里计算的bodySimA->transform中还没有包括当前帧的变动
	// sim->localFrameA.p为刚体上的本地锚点位置，sim->localFrameA.q为本地锚点的旋转
	// 先将质心移到锚点处，然后乘以世界坐标系中旋转
	this->frameA.q = SqRot::Mul(bodySimA->transform.q, sim->localFrameA.q);
	this->frameA.p = bodySimA->transform.transformVector(SqVec2::Sub(sim->localFrameA.p, bodySimA->localCenter));

	this->frameB.q = SqRot::Mul(bodySimB->transform.q, sim->localFrameB.q);
	this->frameB.p = bodySimB->transform.transformVector(SqVec2::Sub(sim->localFrameB.p, bodySimB->localCenter));

	// printf("localFrameA %f %f localFrameB %f %f \n" ,
	// 	sim->localFrameA.q.c,sim->localFrameA.q.s,
	// 	sim->localFrameB.q.c,sim->localFrameB.q.s
	// );

	// Compute the initial center delta. Incremental position updates are relative to this.
	this->deltaCenter = SqVec2::Sub(bodySimB->center, bodySimA->center);

	float k = bodySimA->invInertia + bodySimB->invInertia;
	this->axialMass = k > 0.0f ? 1.0f / k : 0.0f;

	this->springSoftness.step(this->hertz, this->dampingRatio, context.h);
	if (!world->isEnableWarmdStart())
	{
		this->linearImpulse.zero();
		this->springImpulse = 0.0f;
		this->motorImpulse = 0.0f;
		this->lowerImpulse = 0.0f;
		this->upperImpulse = 0.0f;
	}
}

void SqRevoluteJoint::test(SqBodySim *stateA, SqBodySim *stateB, float wA, float wB, const SqStepContext &context, float C)
{
	SqRot qA = SqRot::Mul(stateA->deltaRotation, this->frameA.q);
	SqRot qB = SqRot::Mul(stateB->deltaRotation, this->frameB.q);
	SqRot angleA = SqRot(context.h * wA);
	SqRot angleB = SqRot(context.h * wB);
	angleA = SqRot::Mul(qA, angleA);
	angleB = SqRot::Mul(qB, angleB);

	SqRot relQ = SqRot::InvMulRot(angleA, angleB);
	float angle1 = SqRot::GetAngle(relQ);
	printf(" C %f wA %f wB %f jointAngle  %f qA %f qB %f \n",
		   C * 180 / SQ_PI,
		   wA, wB,
		   angle1 * 180 / SQ_PI,
		   SqRot::GetAngle(angleA) * 180 / SQ_PI,
		   SqRot::GetAngle(angleB) * 180 / SQ_PI);
}

void SqRevoluteJoint::solve(const SqStepContext &context, bool useBias)
{

	// printf("solveSoftness \n");

	SqJointSim *jointSim = getJointSim();

	// dummy state for static bodies
	SqBodySim dummyState;

	SqBody *bodyA = world->getBody(jointSim->bodyAIndex);
	SqBody *bodyB = world->getBody(jointSim->bodyBIndex);
	SqBodySim *bodySimA = bodyA->getBodySim();
	SqBodySim *bodySimB = bodyB->getBodySim();
	float mA = bodySimA->invMass;
	float iA = bodySimA->invInertia;
	float mB = bodySimB->invMass;
	float iB = bodySimB->invInertia;

	SqBodySim *stateA = bodyA->isAwake() ? bodyA->getBodySim() : &dummyState;
	SqBodySim *stateB = bodyB->isAwake() ? bodyB->getBodySim() : &dummyState;

	SqVec2 vA = stateA->linearVelocity;
	float wA = stateA->angularVelocity;
	SqVec2 vB = stateB->linearVelocity;
	float wB = stateB->angularVelocity;

	// Node *nodeA = (Node *)(bodyA->getUserData());
	// Node *nodeB = (Node *)(bodyB->getUserData());

	float jointAngle = 0.f;
	if (enableSpring || enableLimit)
	{
		// 计算当前两个刚体的角度差
		SqRot qA = SqRot::Mul(stateA->deltaRotation, this->frameA.q);
		SqRot qB = SqRot::Mul(stateB->deltaRotation, this->frameB.q);
		SqRot relQ = SqRot::InvMulRot(qA, qB);
		jointAngle = SqRot::GetAngle(relQ);

		// if (nodeA->nativeId == 18 && nodeB->nativeId == 17)
		// {
		// 	printf("begin： wA %f  wB %f jointAngle %f qA %f qB %f lowerAngle %f upperAngle %f\n",
		// 		   wA, wB,
		// 		   jointAngle * 180 / SQ_PI,
		// 		   SqRot::GetAngle(qA) * 180 / SQ_PI,
		// 		   SqRot::GetAngle(qB) * 180 / SQ_PI,
		// 		   this->lowerAngle * 180 / SQ_PI,
		// 		   this->upperAngle * 180 / SQ_PI);
		// }
	}

	bool fixedRotation = (iA + iB == 0.0f);

	// Solve spring.
	if (enableSpring && fixedRotation == false)
	{

		float jointAngleDelta = sqUnwindAngle(jointAngle - this->targetAngle);
		float C = jointAngleDelta;
		float bias = this->springSoftness.biasRate * C;
		float massScale = this->springSoftness.massScale;
		float impulseScale = this->springSoftness.impulseScale;
		float Cdot = wB - wA;

		// printf("Cdot %f bias %f jointAngleDelta %f biasRate %f \n",Cdot,bias,jointAngleDelta,this->springSoftness.biasRate);

		// impulse直接就是为角冲量了
		float impulse = -massScale * this->axialMass * (Cdot + bias) - impulseScale * this->springImpulse;
		this->springImpulse += impulse;

		wA -= iA * impulse;
		wB += iB * impulse;

		// printf("++enableSpring %f %f wA %f wB %f impulse %f \n", jointAngle, this->targetAngle, wA, wB, impulse);
	}

	if (enableMotor && fixedRotation == false)
	{
		float Cdot = wB - wA - motorSpeed;
		float impulse = -axialMass * Cdot;
		float oldImpulse = motorImpulse;
		float maxImpulse = context.h * maxMotorTorque;
		motorImpulse = clamp(motorImpulse + impulse, -maxImpulse, maxImpulse);
		impulse = motorImpulse - oldImpulse;

		// impulse直接就是为角冲量了
		wA -= iA * impulse;
		wB += iB * impulse;
	}

	// if (nodeA->nativeId == 18 && nodeB->nativeId == 17)
	// {
	// 	printf("jointAngle %f lowerAngle %f upperAngle %f \n",
	// 		   jointAngle * 180 / SQ_PI,
	// 		   this->lowerAngle * 180 / SQ_PI,
	// 		   this->upperAngle * 180 / SQ_PI);
	// }

	if (enableLimit && fixedRotation == false)
	{

		// Lower limit
		{
			// 求到最小角度相差多少
			float C = jointAngle - this->lowerAngle;

			// printf("joint Angle %f \n",jointAngle * 180 / SQ_PI);

			float bias = 0.0f;
			float massScale = 1.0f;
			float impulseScale = 0.0f;
			if (C > 0.0f)
			{
				// 表示当前角度大于最小角度,直接往最小角度调节就行
				bias = C * context.inv_h;
			}
			else if (useBias)
			{
				bias = jointSim->constraintSoftness.biasRate * C;
				massScale = jointSim->constraintSoftness.massScale;
				impulseScale = jointSim->constraintSoftness.impulseScale;
				// bias = C * context.inv_h;
			}

			float Cdot = wB - wA;
			float oldImpulse = lowerImpulse;
			float impulse = -massScale * axialMass * (Cdot + bias) - impulseScale * oldImpulse;
			// float impulse = -axialMass * (Cdot + bias);

			// printf("impulse %f \n", impulse);

			// printf("bias %f C %f biasRate %f massScale %f axialMass %f Cdot %f oldImpulse %f impulseScale %f \n", bias, C,
			// 	   jointSim->constraintSoftness.biasRate, massScale, this->axialMass, Cdot, oldImpulse, impulseScale);

			lowerImpulse = max(oldImpulse + impulse, 0.0f);
			impulse = lowerImpulse - oldImpulse;

			// impulse直接就是为角冲量了
			wA -= iA * impulse;
			wB += iB * impulse;

			// if (nodeA->nativeId == 18 && nodeB->nativeId == 17)
			// {
			// 	// printf("iA %f iB %f impulse %f Cdot %f bias %f joint->lowerImpulse %f oldImpulse %f joint->axialMass %f \n",
			// 	// 	   iA, iB, impulse, Cdot, bias, this->lowerImpulse, oldImpulse, this->axialMass);
			// 	test(stateA, stateB, wA, wB, context, C);
			// }
		}

		// Upper limit
		// Note: signs are flipped to keep C positive when the constraint is satisfied.
		// This also keeps the impulse positive when the limit is active.

		{
			float C = this->upperAngle - jointAngle;
			float bias = 0.0f;
			float massScale = 1.0f;
			float impulseScale = 0.0f;
			if (C > 0.0f)
			{
				// speculation
				bias = C * context.inv_h;
			}
			else if (useBias)
			{
				bias = jointSim->constraintSoftness.biasRate * C;
				massScale = jointSim->constraintSoftness.massScale;
				impulseScale = jointSim->constraintSoftness.impulseScale;
				// bias = C * context.inv_h;
			}

			// sign flipped on Cdot
			float Cdot = wA - wB;
			float oldImpulse = this->upperImpulse;
			float impulse = -massScale * this->axialMass * (Cdot + bias) - impulseScale * oldImpulse;
			// float impulse = -this->axialMass * (Cdot + bias);

			this->upperImpulse = max(oldImpulse + impulse, 0.0f);
			impulse = this->upperImpulse - oldImpulse;

			// printf("====== %f %f %f %f %f %f\n",impulse,Cdot,bias,joint->upperImpulse, oldImpulse,joint->axialMass);

			// impulse直接就是为角冲量了
			// sign flipped on applied impulse
			wA += iA * impulse;
			wB -= iB * impulse;

			// if (nodeA->nativeId == 18 && nodeB->nativeId == 17)
			// {
			// 	// printf("iA %f iB %f impulse %f Cdot %f bias %f joint->lowerImpulse %f oldImpulse %f joint->axialMass %f \n",
			// 	// 	   iA, iB, impulse, Cdot, bias, this->lowerImpulse, oldImpulse, this->axialMass);
			// 	test(stateA, stateB, wA, wB, context, C);
			// }
		}
	}

	// printf(" before bodyAId %d vA %f %f wA %f  bodyBId %d vB %f %f wB %f \n", bodySimA->bodyIndex, vA.x, vA.y, wA, bodySimB->bodyIndex, vB.x, vB.y, wB);

	// 点对点约束，将两个刚体约束在一个固定点上
	{
		// J = [-I -r1_skew I r2_skew]
		// r_skew = [-ry; rx]
		// K = [ mA+r1y^2*iA+mB+r2y^2*iB,  -r1y*iA*r1x-r2y*iB*r2x]
		//     [  -r1y*iA*r1x-r2y*iB*r2x, mA+r1x^2*iA+mB+r2x^2*iB]

		// current anchors
		SqVec2 rA = SqRot::transformVector(stateA->deltaRotation, this->frameA.p);
		SqVec2 rB = SqRot::transformVector(stateB->deltaRotation, this->frameB.p);

		// printf("rA %f %f rB %f %f \n",rA.x,rA.y,rB.x,rB.y);

		// Ax = b ，Cdot就是公式中的b
		SqVec2 bpV = SqVec2::Add(vB, SqVec2::CrossSV(wB, rB));
		SqVec2 apV = SqVec2::Add(vA, SqVec2::CrossSV(wA, rA));
		SqVec2 Cdot = SqVec2::Sub(bpV, apV);

		SqVec2 bias;
		float massScale = 1.0f;
		float impulseScale = 0.0f;
		if (useBias)
		{
			const SqVec2 &dcA = stateA->deltaPosition;
			const SqVec2 &dcB = stateB->deltaPosition;
			SqVec2 separation = SqVec2::Add(SqVec2::Add(SqVec2::Sub(dcB, dcA), SqVec2::Sub(rB, rA)), this->deltaCenter);
			bias = SqVec2::MulSV(jointSim->constraintSoftness.biasRate, separation);
			massScale = jointSim->constraintSoftness.massScale;
			impulseScale = jointSim->constraintSoftness.impulseScale;
		}

		SqMat22 K;
		K.cx.x = mA + mB + rA.y * rA.y * iA + rB.y * rB.y * iB;
		K.cy.x = -rA.y * rA.x * iA - rB.y * rB.x * iB;
		K.cx.y = K.cy.x;
		K.cy.y = mA + mB + rA.x * rA.x * iA + rB.x * rB.x * iB;

		// 求解方程x = k^{-1} * b
		SqVec2 b = SqMat22::Solve22(K, SqVec2::Add(Cdot, bias));

		SqVec2 impulse;
		impulse.x = -massScale * b.x - impulseScale * this->linearImpulse.x;
		impulse.y = -massScale * b.y - impulseScale * this->linearImpulse.y;
		this->linearImpulse.x += impulse.x;
		this->linearImpulse.y += impulse.y;

		// printf("%f %f %f \n",impulse,massScale,impulseScale);

		// 这里计算的impulse为线冲量
		vA = SqVec2::MulSub(vA, mA, impulse);
		wA -= iA * SqVec2::Cross(rA, impulse);
		vB = SqVec2::MulAdd(vB, mB, impulse);
		wB += iB * SqVec2::Cross(rB, impulse);
	}

	stateA->linearVelocity = vA;
	stateA->angularVelocity = wA;
	stateB->linearVelocity = vB;
	stateB->angularVelocity = wB;

	// if (nodeA->nativeId == 18 && nodeB->nativeId == 17)
	// {
	// 	test(stateA, stateB, wA, wB, context, 0);
	// }

	// if(enableSpring)
	// printf(" after bodyAId %d vA %f %f wA %f  bodyBId %d vB %f %f wB %f \n", bodySimA->bodyIndex, vA.x, vA.y, wA, bodySimB->bodyIndex, vB.x, vB.y, wB);

	SQ_ASSERT(SqVec2::isValid(stateA->linearVelocity));
	SQ_ASSERT(SqVec2::isValid(stateB->linearVelocity));
}