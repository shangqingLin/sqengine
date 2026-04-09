#pragma once
#include "collision.h"

namespace phxy
{
    /// These are collision planes that can be fed to b2SolvePlanes. Normally
    /// this is assembled by the user from plane results in b2PlaneResult
    struct SqCollisionPlane
    {
        /// The collision plane between the mover and some shape
        SqPlane plane;

        /// Setting this to FLT_MAX makes the plane as rigid as possible. Lower values can
        /// make the plane collision soft. Usually in meters.
        float pushLimit{0.f};

        /// The push on the mover determined by b2SolvePlanes. Usually in meters.
        float push{0.f};

        /// Indicates if b2ClipVector should clip against this plane. Should be false for soft collision.
        bool clipVelocity{false};
    };

    struct SqPlaneSolverResult
    {
        SqVec2 translation;
        int iterationCount{0};
    };

    SqPlaneSolverResult sqSolvePlanes(SqVec2 targetDelta, SqCollisionPlane *planes, int count);
    SqVec2 sqClipVector(SqVec2 vector, const SqCollisionPlane *planes, int count);
}