#include "SqContactSolverPGS_Baumgarte.h"
#include "../../common/constants-define.h"
#include "../SqBody.h"
#include "../../common/math/SqMath.h"
#include "../SqWorld.h"

#include <stdio.h>

using namespace phxy;

#define s2_baumgarte 0.2f
#define s2_linearSlop 0.005f
#define s2_maxBaumgarteVelocity 4.0f

 SqContactSolverPGS_Baumgarte::SqContactSolverPGS_Baumgarte(SqWorld* world) : SqContactSolver(world)
 {

 }

void SqContactSolverPGS_Baumgarte::solve(SqStepContext &context, bool useBias)
{

    for (int i = 0; i < context.contactCount; ++i)
    {
        
        SqContactSim *contactSim = context.contacts[i];
        for (int j = 0; j < contactSim->manifold.pointCount; ++j)
        {

            SqManifoldPoint &manifold = contactSim->manifold.points[j];
            SqBodySim *bodySimA = contactSim->bodyAIndex != SQ_NULL_INDEX ? context.world->getBody(contactSim->bodyAIndex)->getBodySim() : nullptr;
            SqBodySim *bodySimB = contactSim->bodyBIndex != SQ_NULL_INDEX ? context.world->getBody(contactSim->bodyBIndex)->getBodySim() : nullptr;

            float bias = 0.0f;
            if (manifold.separation > 0.0f)
            {
                bias = manifold.separation * context.inv_dt;
            }
            else
            {
                bias = max(s2_baumgarte * context.inv_dt * min(0.0f, manifold.separation + s2_linearSlop), -s2_maxBaumgarteVelocity);
            }

            float vv = 0.f;
            float kNormal = calculateK(manifold, contactSim->manifold.normal, bodySimA, bodySimB, vv);
            float impulse = -kNormal * (vv + bias);

            float newImpulse = max(manifold.normalImpulse + impulse, 0.0f);
            impulse = newImpulse - manifold.normalImpulse;
            manifold.normalImpulse = newImpulse;

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
                bodySimB->linearVelocity = SqVec2::Add(bodySimB->linearVelocity, SqVec2::MulSV(bodySimB->invMass, normalImpulse));
                bodySimB->angularVelocity += bodySimB->invInertia * SqVec2::Cross(manifold.anchorB, normalImpulse);
                SQ_ASSERT(SqVec2::isValid(bodySimB->linearVelocity));
                SQ_ASSERT(isValidFloat(bodySimB->angularVelocity));
                //printf("i %d v:(%f %f) a:%f\n", j, bodySimB->linearVelocity.x, bodySimB->linearVelocity.y, bodySimB->angularVelocity);
            }
        }
    }
}