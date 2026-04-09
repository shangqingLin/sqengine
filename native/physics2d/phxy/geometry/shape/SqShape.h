#pragma once
#include "SqShapeDefine.h"
#include "../../common/math/SqAABB.h"
#include "../../common/math/SqTransform.h"
#include "../../dynamics/SqBodyDefine.h"
#include "../../collision/narrowphase/collision.h"

namespace phxy
{

    class SqBody;
    class SqDebugDraw;
    class SqWorld;
    class SqCapsuleShape;
    class SqDynamicTree;

    struct SqMassData;

    /**
     * 所有Shape的基类
     */
    class SqShape
    {

    private:
        // 在Body中，多个Shape用链表方式存储
        SqShape *next;
        SqShape *prev;

        // 在SqWorld中存储的Index
        int shapeIndex = SQ_NULL_INDEX;

    protected:
        SqWorld *world = nullptr;

        // 不要保存Body指针，因为Body是存储在可变数组中，数组大小更改后地址会改变了
        int bodyIndex = SQ_NULL_INDEX;

        SqAABB aabb;

        /**
         * Shape在动态树中、执行实际的碰撞检测时使用的AABB
         * 这个fatAABB会比实际的aabb大一些。这样会避免很多数值问题。
         */
        SqAABB fatAABB;

        /**
         * Shape在SqDynamicTree中的key
         * 在将Shape添加到SqDynamicTree的时候设置
         */
        int proxyKey = SQ_NULL_INDEX;
        SqDynamicTree *tree = nullptr;

        int sensorIndex = SQ_NULL_INDEX;

        float friction = 0.f;
        float restitution = 0.f;
        float density = 1.0;

        /**
         * 外部可以添加一点速度来控制滑动摩擦的快慢，不用去调整摩擦力那么麻烦
         */
        float tangentSpeed = 0.f;
        uint32_t customColor = 0;

        SqShapeFilter filter;

        virtual void RayCastShapeImpl(SqCastOutput &output, const SqShapeCastInput &input, const SqTransform &transform) const = 0;
        virtual void RayCastImpl(SqCastOutput &output, const SqRayCastInput &input, const SqTransform &transform) const = 0;
        virtual void CollideMoverImpl(SqPlaneResult &result, SqCapsuleShape *mover) const = 0;

    public:
        friend class SqBody;
        friend class SqWorld;
        friend class SqBroadPhase;
        friend class SqTimeOfImpactSolver;
        friend class SqContactManager;
        friend class SqWorldDraw;
        friend class SensorManager;

        SqShapeType type;
        SqShape();
        virtual ~SqShape();
        inline const SqShapeFilter &getFilter() { return filter; };
        virtual void setFilter(const SqShapeFilter &f);
        inline float getFriction() const { return friction; };
        virtual void setFriction(float v);
        inline float getRestitution() const { return restitution; };
        virtual void setRestitution(float v);
        inline void setDensity(float v) { density = v; };
        inline float getDensity() const { return density; };
        inline float getTangentSpeed() const { return tangentSpeed; };
        inline virtual void setTangentSpeed(float f) { tangentSpeed = f; };
        inline int getBody() const { return bodyIndex; };
        inline void setCustomColor(uint32_t color) { customColor = color; };
        inline uint32_t getCustomColor() { return customColor; };
        inline const SqAABB &getAABB() const { return aabb; };
        inline const SqAABB &getFatAABB() const { return fatAABB; };
        void setSensor(bool);
        void sensorAABB(bool);
        bool isSensorAABB();
        inline bool isSensor() const { return sensorIndex != SQ_NULL_INDEX; };
        inline int getSensorIndex() const { return sensorIndex; };
        void updateShapeAABBs(const SqTransform &transform, SqBodyType proxyType);
        virtual SqAABB computeShapeAABB(const SqTransform &transform) const = 0;
        virtual void updateFatAABB(SqBodyType proxyType);
        virtual SqMassData computeShapeMass() = 0;
        virtual SqVec2 getCentroid() = 0;
        virtual void scale(float sx,float sy) = 0;

        /**
         * 判断一个点是否在Shape内，指定的是Shape的本地坐标
         */
        virtual bool PointIn(const SqVec2 &point) const { return false; };
        void RayCastShape(SqCastOutput &output, const SqShapeCastInput &input, const SqTransform &transform) const;
        void RayCast(SqCastOutput &output, const SqRayCastInput &input, const SqTransform &transform) const;
        void CollideMover(SqPlaneResult &, SqCapsuleShape *mover, SqTransform transform) const;
        /**
         * 用于计算Shape上距离localCenter最远和最近的点的直线长度
         */
        virtual SqShapeExtent computeShapeExtent(const SqVec2 &localCenter) = 0;

        /**
         * 求一个点到Shape的距离，并返回距离最短处的法线
         */
        virtual void computeDistance(const SqVec2 &point, const SqTransform &xf, float &outDistance, SqVec2 &outNormal) const = 0;
    };

}
