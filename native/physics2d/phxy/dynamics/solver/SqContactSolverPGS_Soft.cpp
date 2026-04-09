#include "SqContactSolverPGS_Soft.h"
#include "../../common/constants-define.h"
#include "../SqBody.h"
#include "../../common/math/SqMath.h"
#include "../SqWorld.h"

#include <engine/scene/Node.h>
#include <engine/core/simd/simd.h>

using namespace phxy;

SqContactSolverPGS_Soft::SqContactSolverPGS_Soft(SqWorld *world) : SqContactSolver(world)
{
}

/**
 * 在法线方向上应用速度，防止穿透
 */
void SqContactSolverPGS_Soft::solveNonPenetrationConstraint(SqStepContext &context, SqContactSim *contactSim, SqManifoldPoint &manifold, bool useBias)
{
    float pointMassScale = 1.f, pointImpulseScale = 1.f, biasRate = 0;

    SqBodySim *bodySimA = contactSim->bodyAIndex != SQ_NULL_INDEX ? context.world->getBody(contactSim->bodyAIndex)->getBodySim() : nullptr;
    SqBodySim *bodySimB = contactSim->bodyBIndex != SQ_NULL_INDEX ? context.world->getBody(contactSim->bodyBIndex)->getBodySim() : nullptr;

    // 下面这段逻辑相当于将位于局部的坐标点转换到世界坐标系下，然后再计算这两个点的向量
    SqVec2 dp = SqVec2::Sub(bodySimB ? bodySimB->deltaPosition : SqVec2(), bodySimA ? bodySimA->deltaPosition : SqVec2());
    SqVec2 rsA = manifold.anchorA, rsB = manifold.anchorB;

    if (bodySimA)
    {
        rsA = SqRot::transformVector(bodySimA->deltaRotation, rsA);
    }

    if (bodySimB)
    {
        rsB = SqRot::transformVector(bodySimB->deltaRotation, rsB);
    }

    // 计算这两个碰撞点的向量
    dp = SqVec2::Add(dp, SqVec2::Sub(rsB, rsA));

    // 计算碰撞点向量在法线的方向的投影长度，即这两点穿透的长度
    // 相当于锚点A和B都投影到法线上的长度，然后减去这两个长度
    float dd = manifold.separation - SqVec2::Dot(SqVec2::Sub(manifold.anchorB, manifold.anchorA), contactSim->manifold.normal);

    // d为穿透深度。如果d > 0则表示没有发生穿透
    float d = SqVec2::Dot(contactSim->manifold.normal, dp) + dd;

    if (useBias)
    {
        const SqSoftConstraint &softContraint = bodySimA && bodySimB ? context.world->contactSoftness : context.world->staticSoftness;
        biasRate = softContraint.massScale * softContraint.biasRate;
        pointMassScale = d > 0.f ? 1.0 : softContraint.massScale;
        pointImpulseScale = d > 0.f ? 0.f : softContraint.impulseScale;
    }
    else
    {
        pointMassScale = 1.f;
        pointImpulseScale = 0.f;
        biasRate = 0.f;
    }

    // printf("s %f biasRate %f \n",d,biasRate);

    // 计算弹簧质量体的当前的位置，代入到求速度的公式中
    // biasRate 已经是与公式计算过，只需要乘以位移就可以了
    float bias = d > 0.f ? d * context.inv_h : max(biasRate * d, -context.world->contactSpeed);

    // bias = manifold.separation * context.inv_h;

    // 冲量公式中的分母、法线方向的相对速度大小
    float vv = 0.f;
    float kNormal = calculateK(manifold, contactSim->manifold.normal, bodySimA, bodySimB, vv);

    // float impulse = -kNormal * (vv);

    // float negImpulse = kNormal * ( pointMassScale * vn + bias) + pointImpulseScale * cp->normalImpulse1;
    float impulse = -kNormal * (pointMassScale * vv + bias) - pointImpulseScale * manifold.normalImpulse;

    // printf("+++++ bias %f \n",bias);

    float newImpulse = max(manifold.normalImpulse + impulse, 0.f);

    // float impulse1 = impulse;

    // 当前真正应用的冲量
    impulse = newImpulse - manifold.normalImpulse;

    // printf(" %d massImpulse %f impulse %f normalImpulse %f newImpulse %f \n", j, impulse1, impulse, manifold.normalImpulse, newImpulse);

    // 保存当前的冲量，给下一次求解该碰撞点使用
    manifold.normalImpulse = newImpulse;

    // 实现累积冲量的计算
    manifold.totalNormalImpulse += newImpulse;

    SqVec2 normalImpulse = SqVec2::MulSV(impulse, contactSim->manifold.normal);

    if (bodySimA)
    {
        bodySimA->linearVelocity = SqVec2::Sub(bodySimA->linearVelocity, SqVec2::MulSV(bodySimA->invMass, normalImpulse));
        bodySimA->angularVelocity -= bodySimA->invInertia * SqVec2::Cross(manifold.anchorA, normalImpulse);
        SQ_ASSERT(SqVec2::isValid(bodySimA->linearVelocity));
        SQ_ASSERT(isValidFloat(bodySimA->angularVelocity));
    }

    if (bodySimB)
    {
        // printf("before i %d v:(%f %f) a:%f\n", j, bodySimB->linearVelocity.x, bodySimB->linearVelocity.y, bodySimB->angularVelocity);

        bodySimB->linearVelocity = SqVec2::Add(bodySimB->linearVelocity, SqVec2::MulSV(bodySimB->invMass, normalImpulse));
        bodySimB->angularVelocity += bodySimB->invInertia * SqVec2::Cross(manifold.anchorB, normalImpulse);

        // printf("solve contact after body %d v:(%f %f) a:%f\n", bodySimB->bodyIndex, bodySimB->linearVelocity.x, bodySimB->linearVelocity.y, bodySimB->angularVelocity);

        SQ_ASSERT(SqVec2::isValid(bodySimB->linearVelocity));
        SQ_ASSERT(isValidFloat(bodySimB->angularVelocity));
    }
}

/**
 * 计算滑动摩擦
 */
void SqContactSolverPGS_Soft::solveFrictionConstraint(SqStepContext &context, SqContactSim *contactSim, SqManifoldPoint &manifold, bool useBias)
{

    // 摩擦力为0不会在切线方向添加任何的速度
    if (contactSim->friction == 0)
        return;

    SqBodySim *bodySimA = contactSim->bodyAIndex != SQ_NULL_INDEX ? context.world->getBody(contactSim->bodyAIndex)->getBodySim() : nullptr;
    SqBodySim *bodySimB = contactSim->bodyBIndex != SQ_NULL_INDEX ? context.world->getBody(contactSim->bodyBIndex)->getBodySim() : nullptr;

    SqVec2 tangent = SqVec2::RightPerp(contactSim->manifold.normal);
    float vv = 0.f;
    float tangentK = calculateK(manifold, tangent, bodySimA, bodySimB, vv);

    // 外部可以添加一点速度来控制滑动摩擦的快慢，不用去调整摩擦力那么麻烦
    vv -= contactSim->tangentSpeed;

    float impulse = -tangentK * vv;
    float maxFriction = contactSim->friction * manifold.normalImpulse;
    impulse = clamp(impulse, -maxFriction, maxFriction);
    float newImpulse = clamp(manifold.tangentImpulse + impulse, -maxFriction, maxFriction);
    impulse = newImpulse - manifold.tangentImpulse;
    manifold.tangentImpulse = newImpulse;

    SqVec2 tangentImpulse = SqVec2::MulSV(impulse, tangent);

    if (bodySimA)
    {
        bodySimA->linearVelocity = SqVec2::Sub(bodySimA->linearVelocity, SqVec2::MulSV(bodySimA->invMass, tangentImpulse));
        bodySimA->angularVelocity -= bodySimA->invInertia * SqVec2::Cross(manifold.anchorA, tangentImpulse);
        SQ_ASSERT(SqVec2::isValid(bodySimA->linearVelocity));
        SQ_ASSERT(isValidFloat(bodySimA->angularVelocity));
    }

    if (bodySimB)
    {
        // printf("before i %d v:(%f %f) a:%f\n", j, bodySimB->linearVelocity.x, bodySimB->linearVelocity.y, bodySimB->angularVelocity);

        // if (useBias)
        //     printf("before B v:(%f %f) a:%f VLength %f \n", bodySimB->linearVelocity.x, bodySimB->linearVelocity.y, bodySimB->angularVelocity,
        //            SqVec2::Length(bodySimB->linearVelocity));

        bodySimB->linearVelocity = SqVec2::Add(bodySimB->linearVelocity, SqVec2::MulSV(bodySimB->invMass, tangentImpulse));
        bodySimB->angularVelocity += bodySimB->invInertia * SqVec2::Cross(manifold.anchorB, tangentImpulse);
        SQ_ASSERT(SqVec2::isValid(bodySimB->linearVelocity));
        SQ_ASSERT(isValidFloat(bodySimB->angularVelocity));
        
        // printf("%f \n", bodySimB->invInertia);

        // if (useBias)
        //     printf(" after B v:(%f %f) a:%f cVLength %f vLength %f \n", bodySimB->linearVelocity.x, bodySimB->linearVelocity.y, bodySimB->angularVelocity,
        //            SqVec2::Length(bodySimB->linearVelocity),
        //            SqVec2::Length(SqVec2::MulSV(bodySimB->invMass, tangentImpulse)));
    }
}

/**
 * 应用滚动阻力
 */
void SqContactSolverPGS_Soft::solveRollingResistance(SqStepContext &context, SqContactSim *contactSim, SqManifoldPoint &manifold, bool useBias)
{
}

void SqContactSolverPGS_Soft::solve(SqStepContext &context, bool useBias)
{

    // printf("================solve %d \n", context.contactCount);

    for (int i = 0; i < context.contactCount; ++i)
    {
        SqContactSim *contactSim = context.contacts[i];

        // printf("solveContacts contactId %d  point count %d \n", contactSim->contactId, contactSim->manifold.pointCount);

        solveNonPenetrationConstraint(context, contactSim, contactSim->manifold.points[0], useBias);
        if (contactSim->manifold.pointCount > 1)
            solveNonPenetrationConstraint(context, contactSim, contactSim->manifold.points[1], useBias);

        solveFrictionConstraint(context, contactSim, contactSim->manifold.points[0], useBias);
        if (contactSim->manifold.pointCount > 1)
            solveFrictionConstraint(context, contactSim, contactSim->manifold.points[1], useBias);
    }
}



void SqContactSolverPGS_Soft::solveSIMD(SqStepContext &context)
{


    SqContactSim dummyContactSim;
    SqBodySim dummyBodySim;

    //每4个为一组求解
    for (int i = 0; i < context.contactCount;)
    {
        SqContactSim *contactSim1 = context.contacts[i];
        // ++i;
        // SqContactSim *contactSim2 = i < context.contactCount ? context.contacts[i] : nullptr;
        // ++i;
        // SqContactSim *contactSim3 = i < context.contactCount ? context.contacts[i] : nullptr;
        // ++i;
        // SqContactSim *contactSim4 = i < context.contactCount ? context.contacts[i] : nullptr;
        // ++i;


       
        // SqVec2 dp = SqVec2::Sub(bodySimB ? bodySimB->deltaPosition : SqVec2(), bodySimA ? bodySimA->deltaPosition : SqVec2());
        // SqVec2 rsA = manifold.anchorA, rsB = manifold.anchorB;
        // if (bodySimA)
        // {
        //     rsA = SqRot::transformVector(bodySimA->deltaRotation, rsA);
        // }

        // if (bodySimB)
        // {
        //     rsB = SqRot::transformVector(bodySimB->deltaRotation, rsB);
        // }
        // // 计算这两个碰撞点的向量
        // dp = SqVec2::Add(dp, SqVec2::Sub(rsB, rsA));



    }
}