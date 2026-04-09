#include "SqTimeOfImpactSolver.h"
#include "../SqWorld.h"
#include "../../common/math/SqVec2.h"
#include "../../common/math/SqMath.h"
#include "../../collision/broadphase/SqBroadPhaseFilter.h"
#include "../../geometry/shape/SqChainEdgeShape.h"
#include "../../common/SqConfig.h"
#include "../../collision/narrowphase/collide-timeofimpact.h"

using namespace phxy;

#define SQ_MAX_CONTINUOUS_SENSOR_HITS 8

struct SqContinuousContext
{
    SqWorld *world{nullptr};
    SqBodySim *bodySim{nullptr};
    SqShape *shape{nullptr};
    SqVec2 centroid1, centroid2;
    SqSweep sweep;
    float fraction{1.0f};
    // b2SensorHit sensorHits[B2_MAX_CONTINUOUS_SENSOR_HITS];
    // float sensorFractions[B2_MAX_CONTINUOUS_SENSOR_HITS];
    int sensorCount{0};
};

static bool sqContinuousQueryCallback(int proxyId, void *userData, void *context)
{
    SqShape *proxyShape = (SqShape *)userData;
    SqContinuousContext *continuousContext = (SqContinuousContext *)context;
    SqShape *fastShape = continuousContext->shape;
    SqBodySim *fastBodySim = continuousContext->bodySim;
    SqWorld *world = continuousContext->world;

    if (!SqBroadShapeFilterCollide(world, fastShape, proxyShape))
    {
        return true;
    }

    bool isSensor = false;
    // bool isSensor = shapeB->sensorIndex != B2_NULL_INDEX;
    // if (isSensor && (fastBodySim->flags & b2_enableSensorHits) == 0)
    // {
    //     return true;
    // }

    SqBody *body = world->getBody(proxyShape->getBody());

    SQ_ASSERT(body->getType() == sq_staticBody || (fastBodySim->flags & SqBodyFlags::sq_isBullet));
    if (body->isBullet())
    {
        return true;
    }

    SqBodySim *proxyBodySim = body->getBodySim();

    // 对于线段，我们只需要简单计算即可，不需要掉用timeOfImpact计算
    if (proxyShape->type == sq_chainSegmentShape && isSensor == false)
    {
        SqChainEdgeShape *chainShape = (SqChainEdgeShape *)proxyShape;
        SqVec2 p1 = SqTransform::transformPoint(proxyBodySim->transform, chainShape->point1);
        SqVec2 p2 = SqTransform::transformPoint(proxyBodySim->transform, chainShape->point2);
        SqVec2 e = SqVec2::Sub(p2, p1);
        float length;
        e = SqVec2::GetLengthAndNormalize(&length, e);
        if (length > SqConfig::getInstance()->getLinearSlop())
        {
            const SqVec2 &c1 = continuousContext->centroid1;
            float offset1 = SqVec2::Cross(SqVec2::Sub(c1, p1), e);
            const SqVec2 &c2 = continuousContext->centroid2;
            float offset2 = SqVec2::Cross(SqVec2::Sub(c2, p1), e);

            // todo this should use the min extent of the fast shape, not the body
            const float allowedFraction = 0.25f;
            if (offset1 < 0.0f || offset1 - offset2 < allowedFraction * fastBodySim->minExtent)
            {
                // Minimal clipping
                return true;
            }
        }
    }

    SqTOIInput input;
    input.proxyA = sqMakeProxy(proxyShape);
    input.proxyB = sqMakeProxy(fastShape);
    input.sweepA = sqMakeSweep(proxyBodySim);
    input.sweepB = continuousContext->sweep;
    input.maxFraction = continuousContext->fraction;

    SqTOIOutput output = sqTimeOfImpact(&input);
    if (isSensor)
    {
        // Only accept a sensor hit that is sooner than the current solid hit.
        if (output.fraction <= continuousContext->fraction && continuousContext->sensorCount < SQ_MAX_CONTINUOUS_SENSOR_HITS)
        {
            // int index = continuousContext->sensorCount;
            // SqTransform hitTransform = sqGetSweepTransform(&continuousContext->sweep, output.fraction);
            // b2SensorHit sensorHit = {
            //     .sensorId = shape->id,
            //     .visitorId = fastShape->id,
            //     .visitorTransform = hitTransform,
            // };
            // continuousContext->sensorHits[index] = sensorHit;
            // continuousContext->sensorFractions[index] = output.fraction;
            // continuousContext->sensorCount += 1;
        }
    }
    else
    {
        float hitFraction = continuousContext->fraction;
        bool didHit = false;
        if (0.0f < output.fraction && output.fraction < continuousContext->fraction)
        {
            hitFraction = output.fraction;
            didHit = true;
        }
        else if (0.0f == output.fraction)
        {
            // fallback to TOI of a small circle around the fast shape centroid
            SqVec2 centroid = fastShape->getCentroid();
            SqShapeExtent extent = fastShape->computeShapeExtent(centroid);
            float radius = 0.25f * extent.minExtent;
            input.proxyB = sqMakeProxy(&centroid, 1, radius);
            output = sqTimeOfImpact(&input);
            if (0.0f < output.fraction && output.fraction < continuousContext->fraction)
            {
                hitFraction = output.fraction;
                didHit = true;
            }
        }

        // if (didHit && (shape->enablePreSolveEvents || fastShape->enablePreSolveEvents) && world->preSolveFcn != NULL)
        // {
        //     b2ShapeId shapeIdA = {shape->id + 1, world->worldId, shape->generation};
        //     b2ShapeId shapeIdB = {fastShape->id + 1, world->worldId, fastShape->generation};
        //     didHit = world->preSolveFcn(shapeIdA, shapeIdB, output.point, output.normal, world->preSolveContext);
        // }

        if (didHit)
        {
            continuousContext->fraction = hitFraction;
        }
    }

    // Continue query
    return true;
}

SqTimeOfImpactSolver::SqTimeOfImpactSolver(SqWorld *world)
    : world(world)
{
}

bool SqTimeOfImpactSolver::checkBodyNeedContinuous(SqBodySim *)
{
    return false;
}

void SqTimeOfImpactSolver::solveContinuous(SqBodySim *sim)
{

    SqSweep sweep = sqMakeSweep(sim);

    // 这里求上一帧的Transform
    SqTransform xf1;
    xf1.q = sim->prevSim.rotation;
    xf1.p = SqVec2::Sub(sim->prevSim.center, SqRot::transformVector(sim->prevSim.rotation, sim->localCenter));

    SqBody *body = world->getBody(sim->bodyIndex);
    SqShape *shape = body->headShape;
    SqContinuousContext context;
    context.world = world;
    context.bodySim = sim;
    context.sweep = sweep;
    context.fraction = 1.0f;

    while (shape)
    {

        SqVec2 localCentroid = shape->getCentroid();
        context.shape = shape;
        context.centroid1 = xf1.transformPoint(localCentroid);
        context.centroid2 = sim->transform.transformPoint(localCentroid);

        // No continuous collision for sensors (but still need the updated bounds)
        // if ( fastShape->sensorIndex != B2_NULL_INDEX )
        // {
        // 	continue;
        // }

        SqAABB sweptBox = SqAABB::Union(shape->computeShapeAABB(xf1), shape->computeShapeAABB(sim->transform));

        SqTransform xf2 = sim->transform;
        // printf("xf1 %f %f %f %f xf2 %f %f %f %f \n", xf1.p.x, xf1.p.y, xf1.q.c, xf1.q.s, xf2.p.x, xf2.p.y, xf2.q.c, xf2.q.s);
        // printf("box aabb %f %f %f %f \n", sweptBox.lowerBound.x, sweptBox.lowerBound.y, sweptBox.upperBound.x, sweptBox.upperBound.y);

        // 如果Body不是Bullet，则外部的意图肯定将Body当做普通的Body使用，如果不小心发生速度过快肯定是不合理的原因造成的
        // 所以对于不是Bullet Body,我只需要处理哪些需要碰撞的地方就是了，对于动态的Body穿过就穿过
        // 如果你的意图是很快的Body，那么就其设置为bullet
        world->broadPhase.getTree(SqBodyType::sq_staticBody).Query(sweptBox, SQ_DEFAULT_MASK_BITS, sqContinuousQueryCallback, &context);

        if (body->isBullet())
        {
            world->broadPhase.getTree(SqBodyType::sq_kinematicBody).Query(sweptBox, SQ_DEFAULT_MASK_BITS, sqContinuousQueryCallback, &context);
            world->broadPhase.getTree(SqBodyType::sq_dynamicBody).Query(sweptBox, SQ_DEFAULT_MASK_BITS, sqContinuousQueryCallback, &context);
        }
        shape = shape->next;
    }

    // const float speculativeDistance = B2_SPECULATIVE_DISTANCE;
    // const float aabbMargin = B2_AABB_MARGIN;

    // printf("fraction %f \n", context.fraction);

    if (context.fraction < 1.0f)
    {
        // Handle time of impact event
        SqRot q = SqRot::NLerp(sweep.q1, sweep.q2, context.fraction);
        SqVec2 c = SqVec2::Lerp(sweep.c1, sweep.c2, context.fraction);
        SqVec2 origin = SqVec2::Sub(c, SqRot::transformVector(q, sweep.localCenter));

        // Advance body
        SqTransform transform = {origin, q};
        sim->transform = transform;
        sim->center = c;
        sim->prevSim.rotation = q;
        sim->prevSim.center = c;

        sim->flags |= SqBodyFlags::sq_enlargeBounds;

        // Update body move event
        // b2BodyMoveEvent *event = b2BodyMoveEventArray_Get(&world->bodyMoveEvents, bodySimIndex);
        // event->transform = transform;

        // Prepare AABBs for broad-phase.
        // Even though a body is fast, it may not move much. So the AABB may not need enlargement.

        // shapeId = fastBody->headShapeId;
        // while (shapeId != B2_NULL_INDEX)
        // {
        //     b2Shape *shape = b2ShapeArray_Get(&world->shapes, shapeId);

        //     // Must recompute aabb at the interpolated transform
        //     b2AABB aabb = b2ComputeShapeAABB(shape, transform);
        //     aabb.lowerBound.x -= speculativeDistance;
        //     aabb.lowerBound.y -= speculativeDistance;
        //     aabb.upperBound.x += speculativeDistance;
        //     aabb.upperBound.y += speculativeDistance;
        //     shape->aabb = aabb;

        //     if (b2AABB_Contains(shape->fatAABB, aabb) == false)
        //     {
        //         b2AABB fatAABB;
        //         fatAABB.lowerBound.x = aabb.lowerBound.x - aabbMargin;
        //         fatAABB.lowerBound.y = aabb.lowerBound.y - aabbMargin;
        //         fatAABB.upperBound.x = aabb.upperBound.x + aabbMargin;
        //         fatAABB.upperBound.y = aabb.upperBound.y + aabbMargin;
        //         shape->fatAABB = fatAABB;

        //         shape->enlargedAABB = true;
        //         fastBodySim->flags |= b2_enlargeBounds;
        //     }

        //     shapeId = shape->nextShapeId;
        // }
    }
    else
    {
        // No time of impact event

        // Advance body
        sim->prevSim.rotation = sim->transform.q;
        sim->prevSim.center = sim->center;

        // Prepare AABBs for broad-phase
        // shapeId = fastBody->headShapeId;
        // while (shapeId != B2_NULL_INDEX)
        // {
        //     b2Shape *shape = b2ShapeArray_Get(&world->shapes, shapeId);

        //     // shape->aabb is still valid from above

        //     if (b2AABB_Contains(shape->fatAABB, shape->aabb) == false)
        //     {
        //         b2AABB fatAABB;
        //         fatAABB.lowerBound.x = shape->aabb.lowerBound.x - aabbMargin;
        //         fatAABB.lowerBound.y = shape->aabb.lowerBound.y - aabbMargin;
        //         fatAABB.upperBound.x = shape->aabb.upperBound.x + aabbMargin;
        //         fatAABB.upperBound.y = shape->aabb.upperBound.y + aabbMargin;
        //         shape->fatAABB = fatAABB;

        //         shape->enlargedAABB = true;
        //         fastBodySim->flags |= b2_enlargeBounds;
        //     }

        //     shapeId = shape->nextShapeId;
        // }
    }

    // Push sensor hits on the the task context for serial processing.
    // for (int i = 0; i < context.sensorCount; ++i)
    // {
    //     if (context.sensorFractions[i] < context.fraction)
    //     {
    //         b2SensorHitArray_Push(&taskContext->sensorHits, context.sensorHits[i]);
    //     }
    // }
}

void SqTimeOfImpactSolver::solve(SqStepContext &context)
{
    if (!world->enableContinuous)
    {
        return;
    }

    SqSolverSet *awakeSet = world->getSloverSet(sq_awakeSet);
    int awakeBodyCount = awakeSet->bodySims.getCount();
    for (int i = 0; i < awakeBodyCount; ++i)
    {
        SqBodySim *sim = awakeSet->bodySims.get(i);

        if (sim->flags & sq_needTOI)
        {
            sim->flags &= ~sq_needTOI;
            // printf("TOI\n");
            solveContinuous(sim);
        }
    }
}