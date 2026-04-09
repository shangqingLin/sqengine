#include "mover.h"
#include "../../common/math/SqMath.h"
#include <stdio.h>

using namespace phxy;

SqPlaneSolverResult phxy::sqSolvePlanes(SqVec2 targetDelta, SqCollisionPlane *planes, int count)
{

    for (int i = 0; i < count; ++i)
    {
        planes[i].push = 0.0f;
    }

    SqVec2 delta = targetDelta;
    float tolerance = SQ_LINEAR_SLOP;

    // printf("====================targetDelta %f %f \n", targetDelta.x, targetDelta.y);

    int iteration;
    for (iteration = 0; iteration < 20; ++iteration)
    {
        float totalPush = 0.0f;
        for (int planeIndex = 0; planeIndex < count; ++planeIndex)
        {
            SqCollisionPlane *plane = planes + planeIndex;

            // 计算按照delta这个位移与碰撞面产生的穿透深度
            //  separation < 0 表示发生穿透
            //  separation >= 0 表示刚好接触或还没有碰撞到
            float separation = plane->plane.separation(delta) + SQ_LINEAR_SLOP;
            // if (separation > 0.0f)
            //{
            //	continue;
            // }

            // 所以，如果发生了穿透，则表示separation<0，所以push > 0
            float push = -separation;

            // Clamp accumulated push
            float accumulatedPush = plane->push;
            plane->push = clamp(plane->push + push, 0.0f, plane->pushLimit);
            push = plane->push - accumulatedPush;

            //printf(" normal %f %f separation %f offset %f \n", plane->plane.normal.x, plane->plane.normal.y, separation,plane->plane.offset);

            // 计算按照这个法线挪动之后的delta，然后拿新的delta再计算下一个Panel
            delta = SqVec2::MulAdd(delta, push, plane->plane.normal);

            // Track maximum push for convergence
            totalPush += abs(push);
        }

        if (totalPush < tolerance)
        {
            break;
        }
    }

    SqPlaneSolverResult result = {
        delta,
        iteration,
    };
    return result;
}

SqVec2 phxy::sqClipVector(SqVec2 vector, const SqCollisionPlane *planes, int count)
{
    SqVec2 v = vector;

    for (int planeIndex = 0; planeIndex < count; ++planeIndex)
    {
        const SqCollisionPlane *plane = planes + planeIndex;
        if (plane->push == 0.0f || plane->clipVelocity == false)
        {
            continue;
        }

        v = SqVec2::MulSub(v, min(0.0f, SqVec2::Dot(v, plane->plane.normal)), plane->plane.normal);
    }

    return v;
}
