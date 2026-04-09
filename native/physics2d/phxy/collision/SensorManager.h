#pragma once
#include "../common/collection/SqArray.h"
#include "../common/collection/SqBitSet.h"
#include "../geometry/shape/SqShape.h"

namespace phxy
{

    class SqWorld;

    enum SensorFlag
    {
        AABB_OVERLAP = 1,
        ONLY_AABB = 1 << 3,
        HAD_PROCESS = 1 << 4,
        REMOVE = 1 << 5,
        IN_HIT = 1 << 6
    };

    struct SqSensorShape
    {
        int sensorFlag{0};
        SqShape *shape{nullptr};
    };

    struct SqSensor
    {
        int sensorFlag{0};
        SqShape *shape{nullptr};

        //记录与之发生碰撞的所有Shape
        SqArray<SqSensorShape> overlaps;
    };

    class SensorManager
    {
    private:
        SqWorld *world;
        SqArray<SqSensor> sensors;
        SqBitSet activeSensors;
        int createSensor(SqShape *);
        void sensorAABB(int, bool);
        bool isSensorAABB(int);
        void removeSenesor(int);
        void callEndCallback(SqShape *A, SqShape *B);
        void processABSensor(SqShape *A, SqShape *B);
        void updateSensors();

    public:
        friend class SqShape;
        friend class SqBroadPhase;
        friend class SqBody;
        SensorManager(SqWorld *);
        void addOverlapSensor(int sensorIndex, SqShape *overladShape);
    };
}