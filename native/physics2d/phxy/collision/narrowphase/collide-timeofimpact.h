#pragma once
#include "../../common/math/SqVec2.h"
#include "collision.h"

namespace phxy
{
    struct SqBodySim;
    typedef struct SqSweep
    {
        SqVec2 localCenter; ///< Local center of mass position
        SqVec2 c1;          ///< Starting center of mass world position
        SqVec2 c2;          ///< Ending center of mass world position
        SqRot q1;           ///< Starting world rotation
        SqRot q2;           ///< Ending world rotation
    } SqSweep;

    /// Time of impact input
    typedef struct SqTOIInput
    {
        SqShapeProxy proxyA; ///< The proxy for shape A
        SqShapeProxy proxyB; ///< The proxy for shape B
        SqSweep sweepA;      ///< The movement of shape A
        SqSweep sweepB;      ///< The movement of shape B
        float maxFraction{1.0f};   ///< Defines the sweep interval [0, maxFraction]
    } SqTOIInput;

    /// Describes the TOI output
    typedef enum SqTOIState
    {
        sq_toiStateUnknown,
        sq_toiStateFailed,
        sq_toiStateOverlapped,
        sq_toiStateHit,
        sq_toiStateSeparated
    } SqTOIState;

    /// Time of impact output
    typedef struct SqTOIOutput
    {
        /// The type of result
        SqTOIState state;

        /// The hit point
        SqVec2 point;

        /// The hit normal
        SqVec2 normal;

        /// The sweep time of the collision
        float fraction{1.0f};
    } SqTOIOutput;

    SqSweep sqMakeSweep(const SqBodySim *bodySim);
    SqTransform sqGetSweepTransform(const SqSweep *sweep, float time);
    SqTOIOutput sqTimeOfImpact(const SqTOIInput *input);
}