#include "collide-timeofimpact.h"
#include "../../common/math/SqMath.h"
#include "../../common/SqConfig.h"
#include "../../common/SqCore.h"
#include "gjk-distance.h"
#include "../../dynamics/sim/SqBodySim.h"

using namespace phxy;

typedef enum sqSeparationType
{
    sq_pointsType,
    sq_faceAType,
    sq_faceBType
} sqSeparationType;

typedef struct SqSeparationFunction
{
    const SqShapeProxy *proxyA;
    const SqShapeProxy *proxyB;
    SqSweep sweepA, sweepB;
    SqVec2 localPoint;
    SqVec2 axis;
    sqSeparationType type;
} SqSeparationFunction;

SqSweep phxy::sqMakeSweep(const SqBodySim *bodySim)
{
    SqSweep s;
    s.c1 = bodySim->prevSim.center;
    s.c2 = bodySim->center;
    s.q1 = bodySim->prevSim.rotation;
    s.q2 = bodySim->transform.q;
    s.localCenter = bodySim->localCenter;
    return s;
}

SqTransform phxy::sqGetSweepTransform(const SqSweep *sweep, float time)
{
    // https://fgiesen.wordpress.com/2012/08/15/linear-interpolation-past-present-and-future/
    SqTransform xf;
    xf.p = SqVec2::Add(SqVec2::MulSV(1.0f - time, sweep->c1), SqVec2::MulSV(time, sweep->c2));

    SqRot q = {
        (1.0f - time) * sweep->q1.c + time * sweep->q2.c,
        (1.0f - time) * sweep->q1.s + time * sweep->q2.s,
    };

    xf.q = SqRot::Normalize(q);

    // Shift to origin
    xf.p = SqVec2::Sub(xf.p, SqTransform::transformVector(xf, sweep->localCenter));
    return xf;
}

static SqSeparationFunction sqMakeSeparationFunction(const SqSimplexCache *cache, const SqShapeProxy *proxyA,
                                                     const SqSweep *sweepA, const SqShapeProxy *proxyB, const SqSweep *sweepB,
                                                     float t1)
{
    SqSeparationFunction f;

    f.proxyA = proxyA;
    f.proxyB = proxyB;
    int count = cache->count;
    SQ_ASSERT(0 < count && count < 3);

    f.sweepA = *sweepA;
    f.sweepB = *sweepB;

    SqTransform xfA = sqGetSweepTransform(sweepA, t1);
    SqTransform xfB = sqGetSweepTransform(sweepB, t1);

    if (count == 1)
    {
        f.type = sq_pointsType;
        SqVec2 localPointA = proxyA->points[cache->indexA[0]];
        SqVec2 localPointB = proxyB->points[cache->indexB[0]];
        SqVec2 pointA = xfA.transformPoint(localPointA);
        SqVec2 pointB = xfB.transformPoint(xfB, localPointB);
        f.axis = SqVec2::Normalize(SqVec2::Sub(pointB, pointA));
        f.localPoint.zero();
        return f;
    }

    if (cache->indexA[0] == cache->indexA[1])
    {
        // Two points on B and one on A.
        f.type = sq_faceBType;
        SqVec2 localPointB1 = proxyB->points[cache->indexB[0]];
        SqVec2 localPointB2 = proxyB->points[cache->indexB[1]];

        f.axis = SqVec2::CrossVS(SqVec2::Sub(localPointB2, localPointB1), 1.0f);
        f.axis = SqVec2::Normalize(f.axis);
        SqVec2 normal = xfB.transformVector(f.axis);

        f.localPoint = {0.5f * (localPointB1.x + localPointB2.x), 0.5f * (localPointB1.y + localPointB2.y)};
        SqVec2 pointB = xfB.transformPoint(f.localPoint);

        SqVec2 localPointA = proxyA->points[cache->indexA[0]];
        SqVec2 pointA = xfA.transformPoint(xfA, localPointA);

        float s = SqVec2::Dot(SqVec2::Sub(pointA, pointB), normal);
        if (s < 0.0f)
        {
            f.axis = SqVec2::Neg(f.axis);
        }
        return f;
    }

    // Two points on A and one or two points on B.
    f.type = sq_faceAType;
    SqVec2 localPointA1 = proxyA->points[cache->indexA[0]];
    SqVec2 localPointA2 = proxyA->points[cache->indexA[1]];

    f.axis = SqVec2::CrossVS(SqVec2::Sub(localPointA2, localPointA1), 1.0f);
    f.axis = SqVec2::Normalize(f.axis);
    SqVec2 normal = xfA.transformVector(f.axis);

    f.localPoint = {0.5f * (localPointA1.x + localPointA2.x), 0.5f * (localPointA1.y + localPointA2.y)};
    SqVec2 pointA = xfA.transformPoint(f.localPoint);

    SqVec2 localPointB = proxyB->points[cache->indexB[0]];
    SqVec2 pointB = xfB.transformPoint(localPointB);

    float s = SqVec2::Dot(SqVec2::Sub(pointB, pointA), normal);
    if (s < 0.0f)
    {
        f.axis = SqVec2::Neg(f.axis);
    }
    return f;
}

static float sqFindMinSeparation(const SqSeparationFunction *f, int *indexA, int *indexB, float t)
{
    SqTransform xfA = sqGetSweepTransform(&f->sweepA, t);
    SqTransform xfB = sqGetSweepTransform(&f->sweepB, t);

    switch (f->type)
    {
    case sqSeparationType::sq_pointsType:
    {
        SqVec2 axisA = xfA.invTransformVector(f->axis);
        SqVec2 axisB = xfB.invTransformVector(SqVec2::Neg(f->axis));

        *indexA = sqFindSupport(f->proxyA, axisA);
        *indexB = sqFindSupport(f->proxyB, axisB);

        SqVec2 localPointA = f->proxyA->points[*indexA];
        SqVec2 localPointB = f->proxyB->points[*indexB];

        SqVec2 pointA = xfA.transformPoint(localPointA);
        SqVec2 pointB = xfB.transformPoint(localPointB);

        float separation = SqVec2::Dot(SqVec2::Sub(pointB, pointA), f->axis);
        return separation;
    }

    case sqSeparationType::sq_faceAType:
    {
        SqVec2 normal = xfA.transformVector(f->axis);
        SqVec2 pointA = xfA.transformPoint(f->localPoint);

        SqVec2 axisB = xfB.invTransformVector(SqVec2::Neg(normal));

        *indexA = -1;
        *indexB = sqFindSupport(f->proxyB, axisB);

        SqVec2 localPointB = f->proxyB->points[*indexB];
        SqVec2 pointB = xfB.transformPoint(xfB, localPointB);

        float separation = SqVec2::Dot(SqVec2::Sub(pointB, pointA), normal);
        return separation;
    }

    case sqSeparationType::sq_faceBType:
    {
        SqVec2 normal = xfB.transformVector(f->axis);
        SqVec2 pointB = xfB.transformPoint(f->localPoint);

        SqVec2 axisA = xfA.invTransformVector(SqVec2::Neg(normal));

        *indexB = -1;
        *indexA = sqFindSupport(f->proxyA, axisA);

        SqVec2 localPointA = f->proxyA->points[*indexA];
        SqVec2 pointA = xfA.transformPoint(localPointA);

        float separation = SqVec2::Dot(SqVec2::Sub(pointA, pointB), normal);
        return separation;
    }

    default:
        SQ_ASSERT(false);
        *indexA = -1;
        *indexB = -1;
    }
    return 0.0f;
}

static float sqEvaluateSeparation(const SqSeparationFunction *f, int indexA, int indexB, float t)
{
    SqTransform xfA = sqGetSweepTransform(&f->sweepA, t);
    SqTransform xfB = sqGetSweepTransform(&f->sweepB, t);

    switch (f->type)
    {
    case sqSeparationType::sq_pointsType:
    {
        SqVec2 localPointA = f->proxyA->points[indexA];
        SqVec2 localPointB = f->proxyB->points[indexB];

        SqVec2 pointA = xfA.transformPoint(localPointA);
        SqVec2 pointB = xfB.transformPoint(xfB, localPointB);

        float separation = SqVec2::Dot(SqVec2::Sub(pointB, pointA), f->axis);
        return separation;
    }

    case sqSeparationType::sq_faceAType:
    {
        SqVec2 normal = xfA.transformVector(f->axis);
        SqVec2 pointA = xfA.transformPoint(f->localPoint);

        SqVec2 localPointB = f->proxyB->points[indexB];
        SqVec2 pointB = xfB.transformPoint(localPointB);

        float separation = SqVec2::Dot(SqVec2::Sub(pointB, pointA), normal);
        return separation;
    }

    case sqSeparationType::sq_faceBType:
    {
        SqVec2 normal = xfB.transformVector(f->axis);
        SqVec2 pointB = xfB.transformPoint(f->localPoint);

        SqVec2 localPointA = f->proxyA->points[indexA];
        SqVec2 pointA = xfA.transformPoint(localPointA);

        float separation = SqVec2::Dot(SqVec2::Sub(pointA, pointB), normal);
        return separation;
    }

    default:
        SQ_ASSERT(false);
        return 0.0f;
    }
}

// CCD via the local separating axis method. This seeks progression
// by computing the largest time at which separation is maintained.
SqTOIOutput phxy::sqTimeOfImpact(const SqTOIInput *input)
{

#if B2_SNOOP_TOI_COUNTERS
    uint64_t ticks = b2GetTicks();
    ++b2_toiCalls;
#endif

    float linearSlop = SqConfig::getInstance()->getLinearSlop();

    SqTOIOutput output;
    output.state = sq_toiStateUnknown;
    output.fraction = input->maxFraction;

    SqSweep sweepA = input->sweepA;
    SqSweep sweepB = input->sweepB;
    SQ_ASSERT(SqRot::isNormalized(sweepA.q1) && SqRot::isNormalized(sweepA.q2));
    SQ_ASSERT(SqRot::isNormalized(sweepB.q1) && SqRot::isNormalized(sweepB.q2));

    // todo_erin
    // c1 can be at the origin yet the points are far away
    // SqVec2 origin = b2Add(sweepA.c1, input->proxyA.points[0]);

    const SqShapeProxy *proxyA = &input->proxyA;
    const SqShapeProxy *proxyB = &input->proxyB;

    float tMax = input->maxFraction;

    float totalRadius = proxyA->radius + proxyB->radius;
    float target = max(linearSlop, totalRadius - linearSlop);
    float tolerance = 0.25f * linearSlop;
    SQ_ASSERT(target > tolerance);

    float t1 = 0.0f;
    const int k_maxIterations = 20;
    int distanceIterations = 0;

    // Prepare input for distance query.
    SqSimplexCache cache = {0};
    SqDistanceInput distanceInput;
    distanceInput.proxyA = input->proxyA;
    distanceInput.proxyB = input->proxyB;
    distanceInput.useRadii = false;

    // The outer loop progressively attempts to compute new separating axes.
    // This loop terminates when an axis is repeated (no progress is made).
    for (;;)
    {
        SqTransform xfA = sqGetSweepTransform(&sweepA, t1);
        SqTransform xfB = sqGetSweepTransform(&sweepB, t1);

        // Get the distance between shapes. We can also use the results
        // to get a separating axis.
        distanceInput.transformA = xfA;
        distanceInput.transformB = xfB;
        SqDistanceOutput distanceOutput = sqShapeDistance(&distanceInput, &cache, NULL, 0);

        // Progressive time of impact. This handles slender geometry well but introduces
        // significant time loss.
        // if (distanceIterations == 0)
        //{
        //	if ( distanceOutput.distance > totalRadius + B2_SPECULATIVE_DISTANCE )
        //	{
        //		target = totalRadius + B2_SPECULATIVE_DISTANCE - tolerance;
        //	}
        //	else
        //	{
        //		target = distanceOutput.distance - 1.5f * tolerance;
        //		target = b2MaxFloat( target, 2.0f * tolerance );
        //	}
        //}

        distanceIterations += 1;
#if B2_SNOOP_TOI_COUNTERS
        b2_toiDistanceIterations += 1;
#endif

        // If the shapes are overlapped, we give up on continuous collision.
        if (distanceOutput.distance <= 0.0f)
        {
            // Failure!
            output.state = SqTOIState::sq_toiStateOverlapped;
#if B2_SNOOP_TOI_COUNTERS
            b2_toiOverlappedCount += 1;
#endif
            output.fraction = 0.0f;
            break;
        }

        if (distanceOutput.distance <= target + tolerance)
        {
            // Victory!
            output.state = SqTOIState::sq_toiStateHit;
#if B2_SNOOP_TOI_COUNTERS
            b2_toiHitCount += 1;
#endif
            // Averaged hit point
            SqVec2 pA = SqVec2::MulAdd(distanceOutput.pointA, proxyA->radius, distanceOutput.normal);
            SqVec2 pB = SqVec2::MulAdd(distanceOutput.pointB, -proxyB->radius, distanceOutput.normal);
            output.point = SqVec2::Lerp(pA, pB, 0.5f);
            output.normal = distanceOutput.normal;
            output.fraction = t1;
            break;
        }

        // Initialize the separating axis.
        SqSeparationFunction fcn = sqMakeSeparationFunction(&cache, proxyA, &sweepA, proxyB, &sweepB, t1);
#if 0
		// Dump the curve seen by the root finder
		{
			const int N = 100;
			float dx = 1.0f / N;
			float xs[N + 1];
			float fs[N + 1];

			float x = 0.0f;

			for (int i = 0; i <= N; ++i)
			{
				sweepA.GetTransform(&xfA, x);
				sweepB.GetTransform(&xfB, x);
				float f = fcn.Evaluate(xfA, xfB) - target;

				printf("%g %g\n", x, f);

				xs[i] = x;
				fs[i] = f;

				x += dx;
			}
		}
#endif

        // Compute the TOI on the separating axis. We do this by successively
        // resolving the deepest point. This loop is bounded by the number of vertices.
        bool done = false;
        float t2 = tMax;
        int pushBackIterations = 0;
        for (;;)
        {
            // Find the deepest point at t2. Store the witness point indices.
            int indexA, indexB;
            float s2 = sqFindMinSeparation(&fcn, &indexA, &indexB, t2);

            // Is the final configuration separated?
            if (s2 > target + tolerance)
            {
                // Victory!
                output.state = sq_toiStateSeparated;
#if B2_SNOOP_TOI_COUNTERS
                b2_toiSeparatedCount += 1;
#endif
                output.fraction = tMax;
                done = true;
                break;
            }

            // Has the separation reached tolerance?
            if (s2 > target - tolerance)
            {
                // Advance the sweeps
                t1 = t2;
                break;
            }

            // Compute the initial separation of the witness points.
            float s1 = sqEvaluateSeparation(&fcn, indexA, indexB, t1);

            // Check for initial overlap. This might happen if the root finder
            // runs out of iterations.
            if (s1 < target - tolerance)
            {
                output.state = SqTOIState::sq_toiStateFailed;
#if B2_SNOOP_TOI_COUNTERS
                b2_toiFailedCount += 1;
#endif
                output.fraction = t1;
                done = true;
                break;
            }

            // Check for touching
            if (s1 <= target + tolerance)
            {
                // Victory! t1 should hold the TOI (could be 0.0).
                output.state = SqTOIState::sq_toiStateHit;
#if B2_SNOOP_TOI_COUNTERS
                b2_toiHitCount += 1;
#endif
                // Averaged hit point
                SqVec2 pA = SqVec2::MulAdd(distanceOutput.pointA, proxyA->radius, distanceOutput.normal);
                SqVec2 pB = SqVec2::MulAdd(distanceOutput.pointB, -proxyB->radius, distanceOutput.normal);
                output.point = SqVec2::Lerp(pA, pB, 0.5f);
                output.normal = distanceOutput.normal;
                output.fraction = t1;
                done = true;
                break;
            }

            // Compute 1D root of: f(x) - target = 0
            int rootIterationCount = 0;
            float a1 = t1, a2 = t2;
            for (;;)
            {
                // Use a mix of the secant rule and bisection.
                float t;
                if (rootIterationCount & 1)
                {
                    // Secant rule to improve convergence.
                    t = a1 + (target - s1) * (a2 - a1) / (s2 - s1);
                }
                else
                {
                    // Bisection to guarantee progress.
                    t = 0.5f * (a1 + a2);
                }

                rootIterationCount += 1;

#if B2_SNOOP_TOI_COUNTERS
                ++b2_toiRootIterations;
#endif

                float s = sqEvaluateSeparation(&fcn, indexA, indexB, t);

                if (abs(s - target) < tolerance)
                {
                    // t2 holds a tentative value for t1
                    t2 = t;
                    break;
                }

                // Ensure we continue to bracket the root.
                if (s > target)
                {
                    a1 = t;
                    s1 = s;
                }
                else
                {
                    a2 = t;
                    s2 = s;
                }

                if (rootIterationCount == 50)
                {
                    break;
                }
            }

#if B2_SNOOP_TOI_COUNTERS
            b2_toiMaxRootIterations = b2MaxInt(b2_toiMaxRootIterations, rootIterationCount);
#endif

            pushBackIterations += 1;

            if (pushBackIterations == SQ_MAX_POLYGON_VERTICES)
            {
                break;
            }
        }

        if (done)
        {
            break;
        }

        if (distanceIterations == k_maxIterations)
        {
            // Root finder got stuck. Semi-victory.
            output.state = SqTOIState::sq_toiStateFailed;
#if B2_SNOOP_TOI_COUNTERS
            b2_toiFailedCount += 1;
#endif
            // Averaged hit point
            SqVec2 pA = SqVec2::MulAdd(distanceOutput.pointA, proxyA->radius, distanceOutput.normal);
            SqVec2 pB = SqVec2::MulAdd(distanceOutput.pointB, -proxyB->radius, distanceOutput.normal);
            output.point = SqVec2::Lerp(pA, pB, 0.5f);
            output.normal = distanceOutput.normal;
            output.fraction = t1;
            break;
        }
    }

#if B2_SNOOP_TOI_COUNTERS
    b2_toiMaxDistanceIterations = b2MaxInt(b2_toiMaxDistanceIterations, distanceIterations);

    float time = b2GetMilliseconds(ticks);
    b2_toiMaxTime = b2MaxFloat(b2_toiMaxTime, time);
    b2_toiTime += time;
#endif

    return output;
}
