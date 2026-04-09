
#pragma once
#include "../../common/math/SqTransform.h"
#include "../../common/constants-define.h"
#include "../../common/math/SqPlane.h"

namespace phxy
{

    class SqShape;
    struct SqShapeProxy
    {
        /// The point cloud
        SqVec2 points[SQ_MAX_POLYGON_VERTICES];

        /// The number of points. Must be greater than 0.
        int count{0};

        /// The external radius of the point cloud. May be zero.
        float radius{0.f};
    };

    SqShapeProxy sqMakeProxy(const SqVec2 *points, int count, float radius);
    SqShapeProxy sqMakeProxy(const SqShape *shape);

    /// Input for b2ShapeDistance
    struct SqDistanceInput
    {
        /// The proxy for shape A
        SqShapeProxy proxyA;

        /// The proxy for shape B
        SqShapeProxy proxyB;

        /// The world transform for shape A
        SqTransform transformA;

        /// The world transform for shape B
        SqTransform transformB;
        
        bool useRadii = false;

        /**
         * 有些地方已经将proxyA和proxyB转换到同一个坐标系下了
         * 则在GJK算法中就不需要再次转换，节省一点性能，所以如果不需要再次转换将transformLocaBVertex设置为false
         */
        bool transformLocaBVertex = true;
    };

    /// Output for b2ShapeDistance
    struct SqDistanceOutput
    {
        SqVec2 pointA;       ///< Closest point on shapeA。即ProxyA
        SqVec2 pointB;       ///< Closest point on shapeB。即ProxyB。 看你调用sqShapeDistance时传进去决定哪个是A，哪个是B了
        SqVec2 normal;       ///< Normal vector that points from A to B. Invalid if distance is zero.
        float distance{0.f}; ///< The final distance, zero if overlapped
        int iterations{0};   ///< Number of GJK iterations used
        int simplexCount{0}; ///< The number of simplexes stored in the simplex array
    };

    /// Result of computing the distance between two line segments
    struct SqSegmentDistanceResult
    {
        /// The closest point on the first segment
        SqVec2 closest1;

        /// The closest point on the second segment
        SqVec2 closest2;

        /// The barycentric coordinate on the first segment
        float fraction1{0.f};

        /// The barycentric coordinate on the second segment
        float fraction2{0.f};

        /// The squared distance between the closest points
        float distanceSquared{0.f};
    };

    /// These are the collision planes returned from b2World_CollideMover
    struct SqPlaneResult
    {
        /// The collision plane between the mover and a convex shape
        SqPlane plane;

        // The collision point on the shape.
        SqVec2 point;

        /// Did the collision register a hit? If not this plane should be ignored.
        bool hit{false};
    };

    /**
     * 专门用于射线检测。用于定义射线的输入参数
     */
    struct SqRayCastInput
    {
        // 射线的起点
        SqVec2 origin;

        // 射线的方向（包括长度）
        SqVec2 translation;

        float maxFraction{1.f};
    };

    bool sqIsValidRay(const SqRayCastInput *input);

    // 专门用于射线检查的Callbak，检查到射线碰撞到东西就回到这个函数
    typedef float SqTreeRayCastCallbackFcn(const SqRayCastInput *input, int proxyId, void *userData, void *context);

    /**
     * 射线检测是无体积的，是射线与多边形的交互。
     * 而这里你可以定义一个形状，然后这个形状按照translation定义的方向和距离在这条路径上与proxy形状发生碰撞的东西
     */
    struct SqShapeCastInput
    {
        // 定义一个形状
        SqShapeProxy proxy;

        // 定义检测的路径。（包括方向和长度）
        SqVec2 translation;

        float maxFraction{1.0f};

        bool canEncroach{false};
    };

    typedef float SqTreeShapeCastCallbackFcn(const SqShapeCastInput *input, int proxyId, void *userData, void *context);

    struct SqCastOutput
    {
        /// The surface normal at the hit point
        SqVec2 normal;

        /// The surface hit point
        SqVec2 point;

        /// The fraction of the input translation at collision
        float fraction{1.0f};

        /// The number of iterations used
        int iterations{0};

        /// Did the cast hit?
        bool hit{false};
    };

    /**
     * 外部可以自定义一个函数，用于在BroadPhase碰撞阶段过滤自己不要的碰撞对
     */
    typedef bool SqBroadPhaseCustomFilterFcn(SqShape *shapeA, SqShape *shapeB, void *context);
    typedef bool SqPlaneResultFcn(SqShape *, const SqPlaneResult *plane, void *context);
    typedef float SqCastResultFcn(SqShape *, SqVec2 point, SqVec2 normal, float fraction, void *context);
    typedef bool SqOverlapResultFcn(SqShape *shape, void *context);

}
