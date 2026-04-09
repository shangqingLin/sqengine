#include "SqWeldJoint.h"
#include "../SqWorld.h"
#include "../../common/math/SqMat22.h"
#include "../sim/SqJointSim.h"

using namespace phxy;

SqWeldJoint::SqWeldJoint()
{
    type = SqJointType::sq_weldJoint;
}

void SqWeldJoint::SetLinearHertz(float hertz)
{
    linearHertz = hertz;
}

float SqWeldJoint::GetLinearHertz()
{
    return linearHertz;
}

void SqWeldJoint::SetLinearDampingRatio(float dampingRatio)
{
    linearDampingRatio = dampingRatio;
}

float SqWeldJoint::GetLinearDampingRatio()
{
    return linearDampingRatio;
}

void SqWeldJoint::SetAngularHertz(float hertz)
{
    angularHertz = hertz;
}

float SqWeldJoint::GetAngularHertz()
{
    return angularHertz;
}

void SqWeldJoint::SetAngularDampingRatio(float dampingRatio)
{
    angularDampingRatio = dampingRatio;
}

float SqWeldJoint::GetAngularDampingRatio()
{
    return angularDampingRatio;
}

// SqVec2 b2GetWeldJointForce( b2World* world, b2JointSim* base )
// {
// 	SqVec2 force = SqVec2::MulSV( world->inv_h, base->weldJoint.linearImpulse );
// 	return force;
// }

// float b2GetWeldJointTorque( b2World* world, b2JointSim* base )
// {
// 	return world->inv_h * base->weldJoint.angularImpulse;
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

void SqWeldJoint::solvePrepare(const SqStepContext &context)
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

    // Compute joint anchor frames with world space rotation, relative to center of mass
    frameA.q = SqRot::Mul(bodySimA->transform.q, jointSim->localFrameA.q);
    frameA.p = SqTransform::transformVector(bodySimA->transform, SqVec2::Sub(jointSim->localFrameA.p, bodySimA->localCenter));
    frameB.q = SqRot::Mul(bodySimB->transform.q, jointSim->localFrameB.q);
    frameB.p = SqTransform::transformVector(bodySimB->transform, SqVec2::Sub(jointSim->localFrameB.p, bodySimB->localCenter));

    // Compute the initial center delta. Incremental position updates are relative to this.
    this->deltaCenter = SqVec2::Sub(bodySimB->center, bodySimA->center);

    float ka = iA + iB;
    this->axialMass = ka > 0.0f ? 1.0f / ka : 0.0f;

    if (this->linearHertz == 0.0f)
    {
        this->linearSoftness = jointSim->constraintSoftness;
    }
    else
    {
        this->linearSoftness.step(this->linearHertz, this->linearDampingRatio, context.h);
    }

    if (this->angularHertz == 0.0f)
    {
        this->angularSoftness = jointSim->constraintSoftness;
    }
    else
    {
        this->angularSoftness.step(this->angularHertz, this->angularDampingRatio, context.h);
    }

    if (!world->isEnableWarmdStart())
    {
        this->linearImpulse.zero();
        this->angularImpulse = 0.0f;
    }
}

void SqWeldJoint::warmStart(const SqStepContext &context)
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

    // dummy state for static bodies
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


void SqWeldJoint::solve(const SqStepContext &, bool useBias)
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

    SqVec2 vA = stateA->linearVelocity;
    float wA = stateA->angularVelocity;
    SqVec2 vB = stateB->linearVelocity;
    float wB = stateB->angularVelocity;
    
    // angular constraint
    {
        SqRot qA = SqRot::Mul(stateA->deltaRotation, this->frameA.q);
        SqRot qB = SqRot::Mul(stateB->deltaRotation, this->frameB.q);
        SqRot relQ = SqRot::InvMulRot(qA, qB);
        float jointAngle = SqRot::GetAngle(relQ);

        float bias = 0.0f;
        float massScale = 1.0f;
        float impulseScale = 0.0f;
        if (useBias || this->angularHertz > 0.0f)
        {
            float C = jointAngle;
            bias = this->angularSoftness.biasRate * C;
            massScale = this->angularSoftness.massScale;
            impulseScale = this->angularSoftness.impulseScale;
        }

        float Cdot = wB - wA;
        float impulse = -massScale * this->axialMass * (Cdot + bias) - impulseScale * this->angularImpulse;
        this->angularImpulse += impulse;

        wA -= iA * impulse;
        wB += iB * impulse;
    }

    // linear constraint
    {
        SqVec2 rA = SqRot::transformVector(stateA->deltaRotation, this->frameA.p);
        SqVec2 rB = SqRot::transformVector(stateB->deltaRotation, this->frameB.p);

        SqVec2 bias;
        float massScale = 1.0f;
        float impulseScale = 0.0f;
        if (useBias || this->linearHertz > 0.0f)
        {
            SqVec2 dcA = stateA->deltaPosition;
            SqVec2 dcB = stateB->deltaPosition;
            SqVec2 C = SqVec2::Add(SqVec2::Add(SqVec2::Sub(dcB, dcA), SqVec2::Sub(rB, rA)), this->deltaCenter);

            bias = SqVec2::MulSV(this->linearSoftness.biasRate, C);
            massScale = this->linearSoftness.massScale;
            impulseScale = this->linearSoftness.impulseScale;
        }

        SqVec2 Cdot = SqVec2::Sub(SqVec2::Add(vB, SqVec2::CrossSV(wB, rB)), SqVec2::Add(vA, SqVec2::CrossSV(wA, rA)));

        SqMat22 K;
        K.cx.x = mA + mB + rA.y * rA.y * iA + rB.y * rB.y * iB;
        K.cy.x = -rA.y * rA.x * iA - rB.y * rB.x * iB;
        K.cx.y = K.cy.x;
        K.cy.y = mA + mB + rA.x * rA.x * iA + rB.x * rB.x * iB;
        SqVec2 b = SqMat22::Solve22(K, SqVec2::Add(Cdot, bias));

        SqVec2 impulse = {
            -massScale * b.x - impulseScale * this->linearImpulse.x,
            -massScale * b.y - impulseScale * this->linearImpulse.y,
        };

        this->linearImpulse = SqVec2::Add(this->linearImpulse, impulse);

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

#if 0
void b2DumpWeldJoint()
{
	int32 indexA = m_bodyA->m_islandIndex;
	int32 indexB = m_bodyB->m_islandIndex;

	b2Dump("  b2WeldJointDef jd;\n");
	b2Dump("  jd.bodyA = sims[%d];\n", indexA);
	b2Dump("  jd.bodyB = sims[%d];\n", indexB);
	b2Dump("  jd.collideConnected = bool(%d);\n", m_collideConnected);
	b2Dump("  jd.localAnchorA.Set(%.9g, %.9g);\n", m_localAnchorA.x, m_localAnchorA.y);
	b2Dump("  jd.localAnchorB.Set(%.9g, %.9g);\n", m_localAnchorB.x, m_localAnchorB.y);
	b2Dump("  jd.referenceAngle = %.9g;\n", m_referenceAngle);
	b2Dump("  jd.stiffness = %.9g;\n", m_stiffness);
	b2Dump("  jd.damping = %.9g;\n", m_damping);
	b2Dump("  joints[%d] = m_world->CreateJoint(&jd);\n", m_index);
}
#endif

// void b2DrawWeldJoint(b2DebugDraw *draw, b2JointSim *base, b2Transform transformA, b2Transform transformB, float drawSize)
// {
//     B2_ASSERT(base->type == b2_weldJoint);

//     b2Transform frameA = b2MulTransforms(transformA, base->localFrameA);
//     b2Transform frameB = b2MulTransforms(transformB, base->localFrameB);

//     b2Polygon box = b2MakeBox(0.25f * drawSize, 0.125f * drawSize);

//     SqVec2 points[4];

//     for (int i = 0; i < 4; ++i)
//     {
//         points[i] = b2TransformPoint(frameA, box.vertices[i]);
//     }
//     draw->DrawPolygonFcn(points, 4, b2_colorDarkOrange, draw->context);

//     for (int i = 0; i < 4; ++i)
//     {
//         points[i] = b2TransformPoint(frameB, box.vertices[i]);
//     }

//     draw->DrawPolygonFcn(points, 4, b2_colorDarkCyan, draw->context);
// }
