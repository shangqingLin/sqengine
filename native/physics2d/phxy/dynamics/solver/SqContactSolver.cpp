#include "SqContactSolver.h"
#include "../../common/constants-define.h"
#include "../../common/math/SqMath.h"
#include "../SqWorld.h"

#include <stdio.h>

using namespace phxy;

SqContactSolver::SqContactSolver(SqWorld *world) : world(world)
{
}

void SqContactSolver::prepare(SqStepContext &context)
{
    for (int i = 0; i < context.contactCount; ++i)
    {
        SqContactSim *contactSim = context.contacts[i];
        if (contactSim->manifold.pointCount > 0)
        {
            SqContact *contact = context.world->narrowPhase.getContact(contactSim->contactId);

            // printf(" contact prepare %d %p %p %d %d %d \n", contactSim->contactId,contact->shapeA,contact->shapeB,contact->shapeA->getBody(), contact->shapeB->getBody(), world->bodyArray.getCount());

            SqBody *bodyA = context.world->getBody(contact->shapeA->getBody());
            SqBody *bodyB = context.world->getBody(contact->shapeB->getBody());
        }
    }
};

void SqContactSolver::warmStart(SqStepContext &context)
{

    /**
     * 所谓的warmStart就是拿前一帧得到的冲量重新计算一下
     * 因为可能外部重新设置非常大的速度、或后续的阶段破坏了、或精度引起的，
     * 反正是过了求解SolveContact之后的其他地方修改了刚体速度，造成容易穿透
     */

    // printf("====warm start \n");

    for (int i = 0; i < context.contactCount; ++i)
    {

        SqContactSim *contactSim = context.contacts[i];
        SqVec2 tangent = SqVec2::RightPerp(contactSim->manifold.normal);
        for (int j = 0; j < contactSim->manifold.pointCount; ++j)
        {
            SqManifoldPoint &manifold = contactSim->manifold.points[j];

            // 一般新生成的点就是为0，不需要应用
            if (manifold.normalImpulse == 0.f && manifold.tangentImpulse == 0.f)
                continue;

            // printf("====warm start \n");
            SqBodySim *bodySimA = contactSim->bodyAIndex != SQ_NULL_INDEX ? context.world->getBody(contactSim->bodyAIndex)->getBodySim() : nullptr;
            SqBodySim *bodySimB = contactSim->bodyBIndex != SQ_NULL_INDEX ? context.world->getBody(contactSim->bodyBIndex)->getBodySim() : nullptr;

            SqVec2 normalImpulse = SqVec2::MulSV(manifold.normalImpulse, contactSim->manifold.normal);
            SqVec2 tangentImpulse = SqVec2::MulSV(manifold.tangentImpulse, tangent);
            SqVec2 impulse = SqVec2::Add(normalImpulse, tangentImpulse);
            if (bodySimA)
            {
                bodySimA->linearVelocity = SqVec2::Sub(bodySimA->linearVelocity, SqVec2::MulSV(bodySimA->invMass, impulse));
                bodySimA->angularVelocity -= bodySimA->invInertia * SqVec2::Cross(manifold.anchorA, impulse);
            }

            if (bodySimB)
            {
                // printf("before i %d v:(%f %f) a:%f\n", j, bodySimB->linearVelocity.x, bodySimB->linearVelocity.y, bodySimB->angularVelocity);

                bodySimB->linearVelocity = SqVec2::Add(bodySimB->linearVelocity, SqVec2::MulSV(bodySimB->invMass, impulse));
                bodySimB->angularVelocity += bodySimB->invInertia * SqVec2::Cross(manifold.anchorB, impulse);

                // printf("after i %d v:(%f %f) a:%f\n", j, bodySimB->linearVelocity.x, bodySimB->linearVelocity.y, bodySimB->angularVelocity);
            }
        }
    }
}

float SqContactSolver::calculateK(SqManifoldPoint &manifoldPoint, const SqVec2 &direction, SqBodySim *bodySimA, SqBodySim *bodySimB, float &relVelocity)
{

    SqVec2 vA, vB;
    float wA = 0.0f, mA = 0.f, iA = 0.f;
    float wB = 0.0f, mB = 0.f, iB = 0.f;

    wA = 0.0f;
    mA = 0.0f;
    iA = 0.0f;
    vA.zero();

    // 如果body为静态或sq_kinematicBody,则contactSim->bodySim为空，表示不需要模拟
    if (bodySimA)
    {
        mA = bodySimA->invMass;
        iA = bodySimA->invInertia;

        vA = bodySimA->linearVelocity;
        wA = bodySimA->angularVelocity;
    }

    wB = 0.0f;
    mB = 0.0f;
    iB = 0.0f;
    vB.zero();
    if (bodySimB)
    {
        mB = bodySimB->invMass;
        iB = bodySimB->invInertia;
        vB = bodySimB->linearVelocity;
        wB = bodySimB->angularVelocity;
    }

    // 计算direction方向上的冲量
    float rnA = SqVec2::Cross(manifoldPoint.anchorA, direction);
    float rnB = SqVec2::Cross(manifoldPoint.anchorB, direction);

    // 冲量公式中的分母
    float kNormal = mA + mB + iA * rnA * rnA + iB * rnB * rnB;
    kNormal = kNormal > 0.0f ? 1.0f / kNormal : 0.0f;

    /**
     * 计算碰撞响应需要相对速度，下面就是计算相对速度
     * 物体上一点的速度公式：v = v_ + ω x r (v_为当前速度，ω为角速度，r为碰撞点到质心的方向)
     * 并且 r = (-manifold.anchor.y,manifold.anchor.x)
     */
    SqVec2 vA_ = SqVec2::Add(vA, SqVec2::CrossSV(wA, manifoldPoint.anchorA));
    SqVec2 vB_ = SqVec2::Add(vB, SqVec2::CrossSV(wB, manifoldPoint.anchorB));
    SqVec2 rel = SqVec2::Sub(vB_, vA_);

    // direction方向的相对速度大小
    relVelocity = SqVec2::Dot(rel, direction);

    return kNormal;
}
