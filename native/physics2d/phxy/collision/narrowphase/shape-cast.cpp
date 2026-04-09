#include "shape-cast.h"
#include "gjk-distance.h"
#include "../../common/math/SqMath.h"
#include "../../common/math/SqVec2.h"

using namespace phxy;

void phxy::sqShapeCast(SqCastOutput &output, const SqShapeCastPairInput *input)
{
    // Compute tolerance
    float linearSlop = SQ_LINEAR_SLOP;
    float totalRadius = input->proxyA.radius + input->proxyB.radius;
    float target = max(linearSlop, totalRadius - linearSlop);
    float tolerance = 0.25f * linearSlop;

    SQ_ASSERT(target > tolerance);

    // Prepare input for distance query
    SqSimplexCache cache = {0};

    float fraction = 0.0f;

    SqDistanceInput distanceInput;
    distanceInput.proxyA = input->proxyA;
    distanceInput.proxyB = input->proxyB;
    distanceInput.transformA = input->transformA;
    distanceInput.transformB = input->transformB;
    distanceInput.useRadii = false;

    SqVec2 delta2 = input->translationB;
    int iteration = 0;
    const int maxIterations = 20;

    for (; iteration < maxIterations; ++iteration)
    {
        output.iterations += 1;

        SqDistanceOutput distanceOutput = sqShapeDistance(&distanceInput, &cache, NULL, 0);

        if (distanceOutput.distance < target + tolerance)
        {
            if (iteration == 0)
            {
                if (input->canEncroach && distanceOutput.distance > 2.0f * linearSlop)
                {
                    target = distanceOutput.distance - linearSlop;
                }
                else
                {
                    // Initial overlap
                    output.hit = true;

                    // Compute a common point
                    SqVec2 c1 = SqVec2::MulAdd(distanceOutput.pointA, input->proxyA.radius, distanceOutput.normal);
                    SqVec2 c2 = SqVec2::MulAdd(distanceOutput.pointB, -input->proxyB.radius, distanceOutput.normal);
                    output.point = SqVec2::Lerp(c1, c2, 0.5f);
                    return;
                }
            }
            else
            {
                // Regular hit
                SQ_ASSERT(distanceOutput.distance > 0.0f && SqVec2::IsNormalized(distanceOutput.normal));
                output.fraction = fraction;
                output.point = SqVec2::MulAdd(distanceOutput.pointA, input->proxyA.radius, distanceOutput.normal);
                output.normal = distanceOutput.normal;
                output.hit = true;
                return;
            }
        }

        SQ_ASSERT(distanceOutput.distance > 0.0f);
        SQ_ASSERT(SqVec2::IsNormalized(distanceOutput.normal));

        // Check if shapes are approaching each other
        float denominator = SqVec2::Dot(delta2, distanceOutput.normal);
        if (denominator >= 0.0f)
        {
            // Miss
            return;
        }

        // Advance sweep
        fraction += (target - distanceOutput.distance) / denominator;
        if (fraction >= input->maxFraction)
        {
            // Miss
            return;
        }

        distanceInput.transformB.p = SqVec2::MulAdd(input->transformB.p, fraction, delta2);
    }
}