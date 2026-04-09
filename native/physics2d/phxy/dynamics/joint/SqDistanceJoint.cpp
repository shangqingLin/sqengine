#include "SqDistanceJoint.h"
#include "../../common/SqConfig.h"
#include "../../common/math/SqMath.h"
#include "../SqWorld.h"
#include "../sim/SqJointSim.h"
#include "../../../../engine/scene/Node.h"

using namespace phxy;

SqDistanceJoint::SqDistanceJoint()
{
    type = SqJointType::sq_distanceJoint;
}

void SqDistanceJoint::SetLength(float length)
{
    this->length = clamp(length, SqConfig::getInstance()->getLinearSlop(), SqConfig::getInstance()->getHugValue());
    impulse = 0.0f;
    lowerImpulse = 0.0f;
    upperImpulse = 0.0f;
}

float SqDistanceJoint::GetLength()
{
    return length;
}

void SqDistanceJoint::EnableLimit(bool enableLimit)
{
    this->enableLimit = enableLimit;
}

bool SqDistanceJoint::IsLimitEnabled()
{
    return enableLimit;
}

void SqDistanceJoint::SetLengthRange(float minLength, float maxLength)
{

    float linearSlop = SqConfig::getInstance()->getLinearSlop();
    float hug = SqConfig::getInstance()->getHugValue();

    minLength = clamp(minLength, linearSlop, hug);
    maxLength = clamp(maxLength, linearSlop, hug);
    this->minLength = min(minLength, maxLength);
    this->maxLength = max(minLength, maxLength);
    this->impulse = 0.0f;
    this->lowerImpulse = 0.0f;
    this->upperImpulse = 0.0f;
}

float SqDistanceJoint::GetMinLength()
{
    return minLength;
}

float SqDistanceJoint::GetMaxLength()
{
    return maxLength;
}

float SqDistanceJoint::GetCurrentLength()
{

    SQ_ASSERT(world->lock == false);
    if (world->lock)
    {
        return 0.0f;
    }

    SqJointSim *sim = getJointSim();
    SqTransform transformA = world->getBody(sim->bodyAIndex)->getTransform();
    SqTransform transformB = world->getBody(sim->bodyBIndex)->getTransform();

    SqVec2 pA = SqTransform::transformPoint(transformA, sim->localFrameA.p);
    SqVec2 pB = SqTransform::transformPoint(transformB, sim->localFrameB.p);
    SqVec2 d = SqVec2::Sub(pB, pA);
    float length = SqVec2::Length(d);
    return length;
}

void SqDistanceJoint::EnableSpring(bool enableSpring)
{
    this->enableSpring = enableSpring;
}

bool SqDistanceJoint::IsSpringEnabled()
{
    return enableSpring;
}

void SqDistanceJoint::SetSpringHertz(float hertz)
{
    this->hertz = hertz;
}

void SqDistanceJoint::SetSpringDampingRatio(float dampingRatio)
{
    this->dampingRatio = dampingRatio;
}

float SqDistanceJoint::GetSpringHertz()
{
    return hertz;
}

float SqDistanceJoint::GetSpringDampingRatio()
{
    return dampingRatio;
}

void SqDistanceJoint::EnableMotor(bool enableMotor)
{
    if (enableMotor != this->enableMotor)
    {
        this->enableMotor = enableMotor;
        this->motorImpulse = 0.0f;
    }
}

bool SqDistanceJoint::IsMotorEnabled()
{
    return enableMotor;
}

void SqDistanceJoint::SetMotorSpeed(float motorSpeed)
{
    this->motorSpeed = motorSpeed;
}

float SqDistanceJoint::GetMotorSpeed()
{
    return motorSpeed;
}

float SqDistanceJoint::GetMotorForce()
{
    // b2World *world = b2GetWorld(jointId.world0);
    // b2JointSim *base = b2GetJointSimCheckType(jointId, b2_distanceJoint);
    // return world->inv_h * base->distanceJoint.motorImpulse;
    return 0.f;
}

void SqDistanceJoint::SetMaxMotorForce(float force)
{
    maxMotorForce = force;
}

float SqDistanceJoint::GetMaxMotorForce()
{
    return maxMotorForce;
}

void SqDistanceJoint::setEnableSim(bool b)
{
    enable = b;
    if (b)
    {
        lowerImpulse = 0.f;
        upperImpulse = 0.f;
        impulse = 0.f;
        motorImpulse = 0.f;
    }
}

// SqVec2 b2GetDistanceJointForce(b2World *world, b2JointSim *base)
// {
//     b2DistanceJoint *joint = &base->distanceJoint;

//     SqTransform transformA = b2GetBodyTransform(world, base->bodyIdA);
//     SqTransform transformB = b2GetBodyTransform(world, base->bodyIdB);

//     SqVec2 pA = b2TransformPoint(transformA, base->localFrameA.p);
//     SqVec2 pB = b2TransformPoint(transformB, base->localFrameB.p);
//     SqVec2 d = SqVec2::Sub(pB, pA);
//     SqVec2 axis = SqVec2::Normalize(d);
//     float force = (joint->impulse + joint->lowerImpulse - joint->upperImpulse + joint->motorImpulse) * world->inv_h;
//     return SqVec2::MulSV(force, axis);
// }

// 1-D constrained system
// m (v2 - v1) = lambda
// v2 + (beta/h) * x1 + gamma * lambda = 0, gamma has units of inverse mass.
// x2 = x1 + h * v2

// 1-D mass-damper-spring system
// m (v2 - v1) + h * d * v2 + h * k *

// C = norm(p2 - p1) - L
// u = (p2 - p1) / norm(p2 - p1)
// Cdot = dot(u, v2 + cross(w2, r2) - v1 - cross(w1, r1))
// J = [-u -cross(r1, u) u cross(r2, u)]
// K = J * invM * JT
//   = invMass1 + invI1 * cross(r1, u)^2 + invMass2 + invI2 * cross(r2, u)^2

void SqDistanceJoint::solvePrepare(const SqStepContext &context)
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

    // initial anchors in world space
    anchorA = SqTransform::transformVector(bodySimA->transform, SqVec2::Sub(jointSim->localFrameA.p, bodySimA->localCenter));
    anchorB = SqTransform::transformVector(bodySimB->transform, SqVec2::Sub(jointSim->localFrameB.p, bodySimB->localCenter));
    deltaCenter = SqVec2::Sub(bodySimB->center, bodySimA->center);

    // if (((Node *)bodyA->getUserData())->nativeId == 32 || ((Node *)bodyB->getUserData())->nativeId == 32)
    // {
    //     printf("BodyA %d BodyB %d localFrameA %f %f localFrameB %f %f \n",
    //            ((Node *)bodyA->getUserData())->nativeId,
    //            ((Node *)bodyB->getUserData())->nativeId,
    //            jointSim->localFrameA.p.x, jointSim->localFrameA.p.y,
    //            jointSim->localFrameB.p.x, jointSim->localFrameB.p.y);

    //     printf("anchorA %f %f anchorB %f %f \n",
    //            anchorA.x, anchorA.y,
    //            anchorB.x, anchorB.y);
    // }

    const SqVec2 &rA = anchorA;
    const SqVec2 &rB = anchorB;
    SqVec2 separation = SqVec2::Add(SqVec2::Sub(rB, rA), this->deltaCenter);
    SqVec2 axis = SqVec2::Normalize(separation);

    // compute effective mass
    float crA = SqVec2::Cross(rA, axis);
    float crB = SqVec2::Cross(rB, axis);
    float k = mA + mB + iA * crA * crA + iB * crB * crB;
    this->axialMass = k > 0.0f ? 1.0f / k : 0.0f;

    this->distanceSoftness.step(this->hertz, this->dampingRatio, context.h);

    if (!world->isEnableWarmdStart())
    {
        impulse = 0.0f;
        lowerImpulse = 0.0f;
        upperImpulse = 0.0f;
        motorImpulse = 0.0f;
    }
}

void SqDistanceJoint::warmStart(const SqStepContext &context)
{

    SqJointSim *jointSim = getJointSim();
    SqBody *bodyA = world->getBody(jointSim->bodyAIndex);
    SqBody *bodyB = world->getBody(jointSim->bodyBIndex);

    SqBodySim *bodySimA = bodyA->getBodySim();
    SqBodySim *bodySimB = bodyB->getBodySim();

    SqBodySim dummyState;

    SqBodySim *stateA = bodyA->isAwake() ? bodySimA : &dummyState;
    SqBodySim *stateB = bodyB->isAwake() ? bodySimB : &dummyState;

    float mA = bodySimA->invMass;
    float iA = bodySimA->invInertia;
    float mB = bodySimB->invMass;
    float iB = bodySimB->invInertia;

    SqVec2 rA = SqRot::transformVector(stateA->deltaRotation, anchorA);
    SqVec2 rB = SqRot::transformVector(stateB->deltaRotation, anchorB);

    SqVec2 ds = SqVec2::Add(SqVec2::Sub(stateB->deltaPosition, stateA->deltaPosition), SqVec2::Sub(rB, rA));
    SqVec2 separation = SqVec2::Add(deltaCenter, ds);
    SqVec2 axis = SqVec2::Normalize(separation);

    float axialImpulse = this->impulse + this->lowerImpulse - this->upperImpulse + this->motorImpulse;
    SqVec2 P = SqVec2::MulSV(axialImpulse, axis);

    stateA->linearVelocity = SqVec2::MulSub(stateA->linearVelocity, mA, P);
    stateA->angularVelocity -= iA * SqVec2::Cross(rA, P);
    stateB->linearVelocity = SqVec2::MulAdd(stateB->linearVelocity, mB, P);
    stateB->angularVelocity += iB * SqVec2::Cross(rB, P);
}

void SqDistanceJoint::solve(const SqStepContext &context, bool useBias)
{
    SqJointSim *jointSim = getJointSim();
    SqBody *bodyA = world->getBody(jointSim->bodyAIndex);
    SqBody *bodyB = world->getBody(jointSim->bodyBIndex);

    SqBodySim *bodySimA = bodyA->getBodySim();
    SqBodySim *bodySimB = bodyB->getBodySim();

    SqBodySim dummyState;

    SqBodySim *stateA = bodyA->isAwake() ? bodySimA : &dummyState;
    SqBodySim *stateB = bodyB->isAwake() ? bodySimB : &dummyState;

    float mA = bodySimA->invMass;
    float iA = bodySimA->invInertia;
    float mB = bodySimB->invMass;
    float iB = bodySimB->invInertia;

    SqVec2 vA = stateA->linearVelocity;
    float wA = stateA->angularVelocity;
    SqVec2 vB = stateB->linearVelocity;
    float wB = stateB->angularVelocity;

    /**
     * 下面三句语句的解析请查看 SqWheelJoint的说明
     */
    SqVec2 rA = SqRot::transformVector(stateA->deltaRotation, anchorA);
    SqVec2 rB = SqRot::transformVector(stateB->deltaRotation, anchorB);
    SqVec2 ds = SqVec2::Add(SqVec2::Sub(stateB->deltaPosition, stateA->deltaPosition), SqVec2::Sub(rB, rA));

    SqVec2 separation = SqVec2::Add(this->deltaCenter, ds);

    float length = SqVec2::Length(separation);
    SqVec2 axis = SqVec2::Normalize(separation);

    // printf("++++++++++++++++fufufufufff %d %f %f \n",this->enableSpring,this->minLength,this->maxLength);
    // joint is soft if
    // - spring is enabled
    // - and (joint limit is disabled or limits are not equal)
    if (this->enableSpring && (this->minLength <= this->maxLength || this->enableLimit == false))
    {
        // spring
        if (this->hertz > 0.0f)
        {
            // Cdot = dot(u, v + cross(w, r))
            SqVec2 vr = SqVec2::Add(SqVec2::Sub(vB, vA), SqVec2::Sub(SqVec2::CrossSV(wB, rB), SqVec2::CrossSV(wA, rA)));
            float Cdot = SqVec2::Dot(axis, vr);
            float C = length - this->length;
            float bias = this->distanceSoftness.biasRate * C;

            float m = this->distanceSoftness.massScale * this->axialMass;
            float impulse = -m * (Cdot + bias) - this->distanceSoftness.impulseScale * this->impulse;
            this->impulse += impulse;

            SqVec2 P = SqVec2::MulSV(impulse, axis);
            vA = SqVec2::MulSub(vA, mA, P);
            wA -= iA * SqVec2::Cross(rA, P);
            vB = SqVec2::MulAdd(vB, mB, P);
            wB += iB * SqVec2::Cross(rB, P);
        }
        if (this->enableLimit)
        {
            // lower limit
            {
                SqVec2 vr = SqVec2::Add(SqVec2::Sub(vB, vA), SqVec2::Sub(SqVec2::CrossSV(wB, rB), SqVec2::CrossSV(wA, rA)));
                float Cdot = SqVec2::Dot(axis, vr);

                float C = length - this->minLength;
                float bias = 0.0f;
                float massCoeff = 1.0f;
                float impulseCoeff = 0.0f;
                if (C > 0.0f)
                {
                    // speculative
                    bias = C * context.inv_h;
                }
                else if (useBias)
                {
                    bias = jointSim->constraintSoftness.biasRate * C;
                    massCoeff = jointSim->constraintSoftness.massScale;
                    impulseCoeff = jointSim->constraintSoftness.impulseScale;
                }

                float impulse = -massCoeff * this->axialMass * (Cdot + bias) - impulseCoeff * this->lowerImpulse;
                float newImpulse = max(0.0f, this->lowerImpulse + impulse);
                impulse = newImpulse - this->lowerImpulse;
                this->lowerImpulse = newImpulse;

                SqVec2 P = SqVec2::MulSV(impulse, axis);
                vA = SqVec2::MulSub(vA, mA, P);
                wA -= iA * SqVec2::Cross(rA, P);
                vB = SqVec2::MulAdd(vB, mB, P);
                wB += iB * SqVec2::Cross(rB, P);
            }

            // upper
            {
                SqVec2 vr = SqVec2::Add(SqVec2::Sub(vA, vB), SqVec2::Sub(SqVec2::CrossSV(wA, rA), SqVec2::CrossSV(wB, rB)));
                float Cdot = SqVec2::Dot(axis, vr);

                float C = this->maxLength - length;

                float bias = 0.0f;
                float massScale = 1.0f;
                float impulseScale = 0.0f;
                if (C > 0.0f)
                {
                    // speculative
                    bias = C * context.inv_h;
                }
                else if (useBias)
                {
                    bias = jointSim->constraintSoftness.biasRate * C;
                    massScale = jointSim->constraintSoftness.massScale;
                    impulseScale = jointSim->constraintSoftness.impulseScale;
                }

                float impulse = -massScale * this->axialMass * (Cdot + bias) - impulseScale * this->upperImpulse;
                float newImpulse = max(0.0f, this->upperImpulse + impulse);
                impulse = newImpulse - this->upperImpulse;
                this->upperImpulse = newImpulse;

                SqVec2 P = SqVec2::MulSV(-impulse, axis);
                vA = SqVec2::MulSub(vA, mA, P);
                wA -= iA * SqVec2::Cross(rA, P);
                vB = SqVec2::MulAdd(vB, mB, P);
                wB += iB * SqVec2::Cross(rB, P);
            }
        }

        if (this->enableMotor)
        {
            SqVec2 vr = SqVec2::Add(SqVec2::Sub(vB, vA), SqVec2::Sub(SqVec2::CrossSV(wB, rB), SqVec2::CrossSV(wA, rA)));
            float Cdot = SqVec2::Dot(axis, vr);
            float impulse = this->axialMass * (this->motorSpeed - Cdot);
            float oldImpulse = this->motorImpulse;
            float maxImpulse = context.h * this->maxMotorForce;
            this->motorImpulse = clamp(this->motorImpulse + impulse, -maxImpulse, maxImpulse);
            impulse = this->motorImpulse - oldImpulse;

            SqVec2 P = SqVec2::MulSV(impulse, axis);
            vA = SqVec2::MulSub(vA, mA, P);
            wA -= iA * SqVec2::Cross(rA, P);
            vB = SqVec2::MulAdd(vB, mB, P);
            wB += iB * SqVec2::Cross(rB, P);
        }
    }
    else
    {
        // rigid constraint
        SqVec2 vr = SqVec2::Add(SqVec2::Sub(vB, vA), SqVec2::Sub(SqVec2::CrossSV(wB, rB), SqVec2::CrossSV(wA, rA)));
        float Cdot = SqVec2::Dot(axis, vr);

        float C = length - this->length;

        float bias = 0.0f;
        float massScale = 1.0f;
        float impulseScale = 0.0f;
        if (useBias)
        {
            bias = jointSim->constraintSoftness.biasRate * C;
            massScale = jointSim->constraintSoftness.massScale;
            impulseScale = jointSim->constraintSoftness.impulseScale;
        }

        float impulse = -massScale * this->axialMass * (Cdot + bias) - impulseScale * this->impulse;
        this->impulse += impulse;

        SqVec2 P = SqVec2::MulSV(impulse, axis);
        vA = SqVec2::MulSub(vA, mA, P);
        wA -= iA * SqVec2::Cross(rA, P);
        vB = SqVec2::MulAdd(vB, mB, P);
        wB += iB * SqVec2::Cross(rB, P);
    }

    stateA->linearVelocity = vA;
    stateA->angularVelocity = wA;
    stateB->linearVelocity = vB;
    stateB->angularVelocity = wB;
}
