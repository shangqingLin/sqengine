#include "SqPrismaticJoint.h"
#include "../../common/math/SqMath.h"
#include "../SqWorld.h"
#include "../../common/math/SqMat22.h"
#include "../sim/SqJointSim.h"

using namespace phxy;

SqPrismaticJoint::SqPrismaticJoint()
{
    type = SqJointType::sq_prismaticJoint;
}

void SqPrismaticJoint::EnableSpring(bool enableSpring)
{
    if (enableSpring != this->enableSpring)
    {

        this->enableSpring = enableSpring;
        this->springImpulse = 0.0f;
    }
}

bool SqPrismaticJoint::IsSpringEnabled()
{
    return enableSpring;
}

void SqPrismaticJoint::SetSpringHertz(float hertz)
{
    this->hertz = hertz;
}

float SqPrismaticJoint::GetSpringHertz()
{
    return hertz;
}

void SqPrismaticJoint::SetSpringDampingRatio(float dampingRatio)
{
    this->dampingRatio = dampingRatio;
}

float SqPrismaticJoint::GetSpringDampingRatio()
{
    return dampingRatio;
}

void SqPrismaticJoint::SetTargetTranslation(float translation)
{
    this->targetTranslation = translation;
}

float SqPrismaticJoint::GetTargetTranslation()
{
    return targetTranslation;
}

void SqPrismaticJoint::EnableLimit(bool enableLimit)
{
    if (enableLimit != this->enableLimit)
    {
        this->enableLimit = enableLimit;
        this->lowerImpulse = 0.0f;
        this->upperImpulse = 0.0f;
    }
}

bool SqPrismaticJoint::IsLimitEnabled()
{
    return enableLimit;
}

float SqPrismaticJoint::GetLowerLimit()
{
    return lowerTranslation;
}

float SqPrismaticJoint::GetUpperLimit()
{
    return upperTranslation;
}

void SqPrismaticJoint::SetLimits(float lower, float upper)
{
    if (lower != this->lowerTranslation || upper != this->upperTranslation)
    {
        this->lowerTranslation = min(lower, upper);
        this->upperTranslation = max(lower, upper);
        this->lowerImpulse = 0.0f;
        this->upperImpulse = 0.0f;
    }
}

void SqPrismaticJoint::EnableMotor(bool enableMotor)
{
    if (enableMotor != this->enableMotor)
    {
        this->enableMotor = enableMotor;
        this->motorImpulse = 0.0f;
    }
}

bool SqPrismaticJoint::IsMotorEnabled()
{
    return enableMotor;
}

void SqPrismaticJoint::SetMotorSpeed(float motorSpeed)
{
    this->motorSpeed = motorSpeed;
}

float SqPrismaticJoint::GetMotorSpeed()
{
    return motorSpeed;
}

float SqPrismaticJoint::GetMotorForce()
{
    // b2World *world = b2GetWorld(jointId.world0);
    // b2JointSim *base = b2GetJointSimCheckType(jointId, b2_prismaticJoint);
    // return world->inv_h * base->prismaticJoint.motorImpulse;
    return 0.f;
}

void SqPrismaticJoint::SetEnableSyncAngle(bool enableSyncAngle)
{
    this->enableSyncAngle = enableSyncAngle;
}

void SqPrismaticJoint::SetMaxMotorForce(float force)
{
    this->maxMotorForce = force;
}

float SqPrismaticJoint::GetMaxMotorForce()
{
    return maxMotorForce;
}

float SqPrismaticJoint::GetTranslation()
{
    SqJointSim *jointSim = getJointSim();
    SqTransform transformA = world->getBody(jointSim->bodyAIndex)->getTransform();
    SqTransform transformB = world->getBody(jointSim->bodyBIndex)->getTransform();

    SqVec2 localAxisA = SqRot::transformVector(jointSim->localFrameA.q, SqVec2(1.0f, 0.0f));
    SqVec2 axisA = SqRot::transformVector(transformA.q, localAxisA);
    SqVec2 pA = transformA.transformPoint(jointSim->localFrameA.p);
    SqVec2 pB = transformB.transformPoint(jointSim->localFrameB.p);
    SqVec2 d = SqVec2::Sub(pB, pA);
    float translation = SqVec2::Dot(d, axisA);
    return translation;
}

float SqPrismaticJoint::GetSpeed()
{
    SqJointSim *jointSim = getJointSim();
    SqBody *bodyA = world->getBody(jointSim->bodyAIndex);
    SqBody *bodyB = world->getBody(jointSim->bodyBIndex);
    SqBodySim *bodySimA = bodyA->getBodySim();
    SqBodySim *bodySimB = bodyB->getBodySim();

    SqTransform transformA = bodySimA->transform;
    SqTransform transformB = bodySimB->transform;

    SqVec2 localAxisA = SqRot::transformVector(jointSim->localFrameA.q, SqVec2(1.0f, 0.0f));
    SqVec2 axisA = SqRot::transformVector(transformA.q, localAxisA);
    SqVec2 cA = bodySimA->center;
    SqVec2 cB = bodySimB->center;
    SqVec2 rA = SqRot::transformVector(transformA.q, SqVec2::Sub(jointSim->localFrameA.p, bodySimA->localCenter));
    SqVec2 rB = SqRot::transformVector(transformB.q, SqVec2::Sub(jointSim->localFrameB.p, bodySimB->localCenter));

    SqVec2 d = SqVec2::Add(SqVec2::Sub(cB, cA), SqVec2::Sub(rB, rA));

    SqVec2 vA = bodyA->isAwake() ? bodySimA->linearVelocity : SqVec2();
    SqVec2 vB = bodyB->isAwake() ? bodySimB->linearVelocity : SqVec2();
    float wA = bodyA->isAwake() ? bodySimA->angularVelocity : 0.0f;
    float wB = bodyB->isAwake() ? bodySimB->angularVelocity : 0.0f;

    SqVec2 vRel = SqVec2::Sub(SqVec2::Add(vB, SqVec2::CrossSV(wB, rB)), SqVec2::Add(vA, SqVec2::CrossSV(wA, rA)));
    float speed = SqVec2::Dot(d, SqVec2::CrossSV(wA, axisA)) + SqVec2::Dot(axisA, vRel);
    return speed;
}

// SqVec2 b2GetPrismaticJointForce(b2World *world, b2JointSim *base)
// {
//     int idA = base->bodyIdA;
//     SqTransform transformA = b2GetBodyTransform(world, idA);

//     b2PrismaticJoint *joint = &base->prismaticJoint;

//     SqVec2 localAxisA = b2RotateVector(base->localFrameA.q, (SqVec2){1.0f, 0.0f});
//     SqVec2 axisA = b2RotateVector(transformA.q, localAxisA);
//     SqVec2 perpA = SqVec2::LeftPerp(axisA);

//     float inv_h = world->inv_h;
//     float perpForce = inv_h * joint->impulse.x;
//     float axialForce = inv_h * (joint->motorImpulse + joint->lowerImpulse - joint->upperImpulse);

//     SqVec2 force = SqVec2::Add(SqVec2::MulSV(perpForce, perpA), SqVec2::MulSV(axialForce, axisA));
//     return force;
// }

// float b2GetPrismaticJointTorque(b2World *world, b2JointSim *base)
// {
//     return world->inv_h * base->prismaticJoint.impulse.y;
// }

// Linear constraint (point-to-line)
// d = p2 - p1 = x2 + r2 - x1 - r1
// C = dot(perp, d)
// Cdot = dot(d, cross(w1, perp)) + dot(perp, v2 + cross(w2, r2) - v1 - cross(w1, r1))
//      = -dot(perp, v1) - dot(cross(d + r1, perp), w1) + dot(perp, v2) + dot(cross(r2, perp), v2)
// J = [-perp, -cross(d + r1, perp), perp, cross(r2,perp)]
//
// Angular constraint
// C = a2 - a1 + a_initial
// Cdot = w2 - w1
// J = [0 0 -1 0 0 1]
//
// K = J * invM * JT
//
// J = [-a -s1 a s2]
//     [0  -1  0  1]
// a = perp
// s1 = cross(d + r1, a) = cross(p2 - x1, a)
// s2 = cross(r2, a) = cross(p2 - x2, a)

// Motor/Limit linear constraint
// C = dot(ax1, d)
// Cdot = -dot(ax1, v1) - dot(cross(d + r1, ax1), w1) + dot(ax1, v2) + dot(cross(r2, ax1), v2)
// J = [-ax1 -cross(d+r1,ax1) ax1 cross(r2,ax1)]

// Predictive limit is applied even when the limit is not active.
// Prevents a constraint speed that can lead to a constraint error in one time step.
// Want C2 = C1 + h * Cdot >= 0
// Or:
// Cdot + C1/h >= 0
// I do not apply a negative constraint error because that is handled in position correction.
// So:
// Cdot + max(C1, 0)/h >= 0

// Block Solver
// We develop a block solver that includes the angular and linear constraints. This makes the limit stiffer.
//
// The Jacobian has 2 rows:
// J = [-uT -s1 uT s2] // linear
//     [0   -1   0  1] // angular
//
// u = perp
// s1 = cross(d + r1, u), s2 = cross(r2, u)
// a1 = cross(d + r1, v), a2 = cross(r2, v)

void SqPrismaticJoint::solvePrepare(const SqStepContext &context)
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

    // Compute joint anchor frames with world space rotation, relative to center of mass
    frameA.q = SqRot::Mul(bodySimA->transform.q, jointSim->localFrameA.q);
    frameA.p = SqTransform::transformVector(bodySimA->transform, SqVec2::Sub(jointSim->localFrameA.p, bodySimA->localCenter));
    frameB.q = SqRot::Mul(bodySimB->transform.q, jointSim->localFrameB.q);
    frameB.p = SqTransform::transformVector(bodySimB->transform, SqVec2::Sub(jointSim->localFrameB.p, bodySimB->localCenter));

    // Compute the initial center delta. Incremental position updates are relative to this.
    deltaCenter = SqVec2::Sub(bodySimB->center, bodySimA->center);

    const SqVec2 &rA = frameA.p;
    const SqVec2 &rB = frameB.p;

    SqVec2 axisA = SqRot::transformVector(frameA.q, SqVec2(1.0f, 0.0f));

    SqVec2 d = SqVec2::Add(deltaCenter, SqVec2::Sub(rB, rA));
    float a1 = SqVec2::Cross(SqVec2::Add(d, rA), axisA);
    float a2 = SqVec2::Cross(rB, axisA);

    // effective masses
    float k = mA + mB + iA * a1 * a1 + iB * a2 * a2;
    this->axialMass = k > 0.0f ? 1.0f / k : 0.0f;

    this->springSoftness.step(this->hertz, this->dampingRatio, context.h);

    if (!world->isEnableWarmdStart())
    {
        this->impulse.zero();
        this->springImpulse = 0.0f;
        this->motorImpulse = 0.0f;
        this->lowerImpulse = 0.0f;
        this->upperImpulse = 0.0f;
    }
}

void SqPrismaticJoint::warmStart(const SqStepContext &context)
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

    SqVec2 d = SqVec2::Add(SqVec2::Add(SqVec2::Sub(stateB->deltaPosition, stateA->deltaPosition), this->deltaCenter), SqVec2::Sub(rB, rA));

    SqVec2 axisA = SqRot::transformVector(this->frameA.q, SqVec2(1.0f, 0.0f));
    axisA = SqRot::transformVector(stateA->deltaRotation, axisA);

    // impulse is applied at anchor point on body B
    float a1 = SqVec2::Cross(SqVec2::Add(d, rA), axisA);
    float a2 = SqVec2::Cross(rB, axisA);
    float axialImpulse = this->springImpulse + this->motorImpulse + this->lowerImpulse - this->upperImpulse;

    // perpendicular constraint
    SqVec2 perpA = SqVec2::LeftPerp(axisA);
    float s1 = SqVec2::Cross(SqVec2::Add(d, rA), perpA);
    float s2 = SqVec2::Cross(rB, perpA);
    float perpImpulse = this->impulse.x;
    float angleImpulse = this->impulse.y;

    SqVec2 P = SqVec2::Add(SqVec2::MulSV(axialImpulse, axisA), SqVec2::MulSV(perpImpulse, perpA));
    float LA = axialImpulse * a1 + perpImpulse * s1 + angleImpulse;
    float LB = axialImpulse * a2 + perpImpulse * s2 + angleImpulse;

    stateA->linearVelocity = SqVec2::MulSub(stateA->linearVelocity, mA, P);
    stateA->angularVelocity -= iA * LA;
    stateB->linearVelocity = SqVec2::MulAdd(stateB->linearVelocity, mB, P);
    stateB->angularVelocity += iB * LB;
}

void SqPrismaticJoint::solve(const SqStepContext &context, bool useBias)
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

    SqRot qA = SqRot::Mul(stateA->deltaRotation, this->frameA.q);
    SqRot qB = SqRot::Mul(stateB->deltaRotation, this->frameB.q);
    SqRot relQ = SqRot::InvMulRot(qA, qB);

    /**
     * 下面三句语句的解析请查看 SqWheelJoint的说明。目的是计算d
     */
    SqVec2 rA = SqRot::transformVector(stateA->deltaRotation, this->frameA.p);
    SqVec2 rB = SqRot::transformVector(stateB->deltaRotation, this->frameB.p);
    SqVec2 d = SqVec2::Add(SqVec2::Add(SqVec2::Sub(stateB->deltaPosition, stateA->deltaPosition), this->deltaCenter), SqVec2::Sub(rB, rA));

    // axisA为BodyB沿着运动的轴，即拿BodyA的localFrameA的旋转对x正向轴旋转后为运动轴
    SqVec2 axisA = SqRot::transformVector(this->frameA.q, SqVec2(1.0f, 0.0f));
    axisA = SqRot::transformVector(stateA->deltaRotation, axisA);

    // 两个锚点在运动轴上的投影长度，即两点在运动轴上的距离
    float translation = SqVec2::Dot(axisA, d);

    // These scalars are for torques generated by axial forces
    float a1 = SqVec2::Cross(SqVec2::Add(d, rA), axisA);
    float a2 = SqVec2::Cross(rB, axisA);

    // spring constraint
    if (this->enableSpring)
    {
        // This is a real spring and should be applied even during relax
        float C = translation - this->targetTranslation;
        float bias = this->springSoftness.biasRate * C;
        float massScale = this->springSoftness.massScale;
        float impulseScale = this->springSoftness.impulseScale;

        float Cdot = SqVec2::Dot(axisA, SqVec2::Sub(vB, vA)) + a2 * wB - a1 * wA;
        float deltaImpulse = -massScale * this->axialMass * (Cdot + bias) - impulseScale * this->springImpulse;
        this->springImpulse += deltaImpulse;

        SqVec2 P = SqVec2::MulSV(deltaImpulse, axisA);
        float LA = deltaImpulse * a1;
        float LB = deltaImpulse * a2;

        vA = SqVec2::MulSub(vA, mA, P);
        wA -= iA * LA;
        vB = SqVec2::MulAdd(vB, mB, P);
        wB += iB * LB;
    }

    // Solve motor constraint
    if (this->enableMotor)
    {
        float Cdot = SqVec2::Dot(axisA, SqVec2::Sub(vB, vA)) + a2 * wB - a1 * wA;
        float impulse = this->axialMass * (this->motorSpeed - Cdot);
        float oldImpulse = this->motorImpulse;
        float maxImpulse = context.h * this->maxMotorForce;
        this->motorImpulse = clamp(this->motorImpulse + impulse, -maxImpulse, maxImpulse);
        impulse = this->motorImpulse - oldImpulse;

        SqVec2 P = SqVec2::MulSV(impulse, axisA);
        float LA = impulse * a1;
        float LB = impulse * a2;

        vA = SqVec2::MulSub(vA, mA, P);
        wA -= iA * LA;
        vB = SqVec2::MulAdd(vB, mB, P);
        wB += iB * LB;
    }

    if (this->enableLimit)
    {
        // Lower limit
        {
            float C = translation - this->lowerTranslation;
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
            }

            float oldImpulse = this->lowerImpulse;
            float Cdot = SqVec2::Dot(axisA, SqVec2::Sub(vB, vA)) + a2 * wB - a1 * wA;
            float impulse = -this->axialMass * massScale * (Cdot + bias) - impulseScale * oldImpulse;
            this->lowerImpulse = max(oldImpulse + impulse, 0.0f);
            impulse = this->lowerImpulse - oldImpulse;

            SqVec2 P = SqVec2::MulSV(impulse, axisA);
            float LA = impulse * a1;
            float LB = impulse * a2;

            vA = SqVec2::MulSub(vA, mA, P);
            wA -= iA * LA;
            vB = SqVec2::MulAdd(vB, mB, P);
            wB += iB * LB;
        }

        // Upper limit
        // Note: signs are flipped to keep C positive when the constraint is satisfied.
        // This also keeps the impulse positive when the limit is active.
        {
            // sign flipped
            float C = this->upperTranslation - translation;
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
            }

            float oldImpulse = this->upperImpulse;
            // sign flipped
            float Cdot = SqVec2::Dot(axisA, SqVec2::Sub(vA, vB)) + a1 * wA - a2 * wB;
            float impulse = -this->axialMass * massScale * (Cdot + bias) - impulseScale * oldImpulse;
            this->upperImpulse = max(oldImpulse + impulse, 0.0f);
            impulse = this->upperImpulse - oldImpulse;

            SqVec2 P = SqVec2::MulSV(impulse, axisA);
            float LA = impulse * a1;
            float LB = impulse * a2;

            // sign flipped
            vA = SqVec2::MulAdd(vA, mA, P);
            wA += iA * LA;
            vB = SqVec2::MulSub(vB, mB, P);
            wB -= iB * LB;
        }
    }

    // 线性约束和角度约束组合
    {
        SqVec2 perpA = SqVec2::LeftPerp(axisA);

        // 下面来计算b
        //  b = t x (vB + wB * rB - vA - wA * rA)
        float s1 = SqVec2::Cross(SqVec2::Add(d, rA), perpA);
        float s2 = SqVec2::Cross(rB, perpA);

        SqVec2 Cdot;

        /**
         * 线性相对速度
         *
         * 先求在perpA方向上的线速度
         * SqVec2::Dot(perpA, SqVec2::Sub(vB, vA)) 相当于
         * vB = SqVec2::Dot(perpA, vB)
         * vA = SqVec2::Dot(perpA, vB)
         * vB-vA;
         */
        Cdot.x = SqVec2::Dot(perpA, SqVec2::Sub(vB, vA)) + s2 * wB - s1 * wA;

        // 角相对速度
        Cdot.y = wB - wA;

        SqVec2 bias;
        float massScale = 1.0f;
        float impulseScale = 0.0f;
        if (useBias)
        {
            // 要算上偏移，因为这两个锚点在物理模拟中肯定偏离了角度和位置
            // 所以需要将其拉回到运动轴上和固定和运动轴一样的角度
            SqVec2 C;
            C.x = SqVec2::Dot(perpA, d);
            C.y = 0; // this->enableSyncAngle ? SqRot::GetAngle(relQ) : 0.0f;

            bias = SqVec2::MulSV(jointSim->constraintSoftness.biasRate, C);
            massScale = jointSim->constraintSoftness.massScale;
            impulseScale = jointSim->constraintSoftness.impulseScale;
        }

        float k11 = mA + mB + iA * s1 * s1 + iB * s2 * s2;
        float k12 = iA * s1 + iB * s2;
        float k22 = iA + iB;
        if (k22 == 0.0f)
        {
            // For bodies with fixed rotation.
            k22 = 1.0f;
        }

        SqMat22 K = {{k11, k12}, {k12, k22}};

        SqVec2 b = SqMat22::Solve22(K, SqVec2::Add(Cdot, bias));
        SqVec2 impulse;
        impulse.x = -massScale * b.x - impulseScale * this->impulse.x;
        impulse.y = -massScale * b.y - impulseScale * this->impulse.y;

        this->impulse.x += impulse.x;
        this->impulse.y += impulse.y;

        SqVec2 P = SqVec2::MulSV(impulse.x, perpA);
        float LA = impulse.x * s1 + impulse.y;
        float LB = impulse.x * s2 + impulse.y;

        vA = SqVec2::MulSub(vA, mA, P);
        wA -= iA * LA;
        vB = SqVec2::MulAdd(vB, mB, P);
        wB += iB * LB;
    }

    stateA->linearVelocity = vA;
    stateA->angularVelocity = wA;
    stateB->linearVelocity = vB;
    stateB->angularVelocity = wB;
}
