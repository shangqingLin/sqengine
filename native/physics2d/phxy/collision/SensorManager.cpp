#include "SensorManager.h"
#include "../dynamics/SqWorld.h"
#include "./narrowphase/gjk-distance.h"

using namespace phxy;

SensorManager::SensorManager(SqWorld *world) : world(world)
{
}

int SensorManager::createSensor(SqShape *shape)
{

    sensors.Add();
    int sensorIndex = sensors.getCount() - 1;
    SqSensor *sensor = sensors.get(sensorIndex);
    activeSensors.addValue(sensorIndex);

    // printf(" createSensor %d %d \n", sensorIndex, sensors.getCount());

    sensor->sensorFlag = 0;
    sensor->shape = shape;

    // 添加好之后让其立刻执行碰撞检测
    if (shape->proxyKey != SQ_NULL_INDEX)
        world->broadPhase.proxyMove(shape->proxyKey);

    return sensorIndex;
}

void SensorManager::sensorAABB(int sensorIndex, bool b)
{
    SqSensor *sensor = sensors.get(sensorIndex);
    if (b)
    {
        sensor->sensorFlag |= SensorFlag::ONLY_AABB;
    }
    else
    {
        sensor->sensorFlag &= ~SensorFlag::ONLY_AABB;
    }
}

bool SensorManager::isSensorAABB(int sensorIndex)
{
    SqSensor *sensor = sensors.get(sensorIndex);
    return sensor->sensorFlag & SensorFlag::ONLY_AABB;
}

void SensorManager::removeSenesor(int sensorIndex)
{

    SQ_ASSERT(!world->lock);

    SqSensor *sensor = sensors.get(sensorIndex);

    // 不立刻删除，只是标记为删除，在update中再删除，避免发生错误
    sensor->sensorFlag |= SensorFlag::REMOVE;
}

/**
 * 添加一个与之发生碰撞的Shape。一个Sensor Shape可以和多个Sensor Shape发生碰撞的，是一对多关系
 * 在broadphase阶段检测BVH动态树被添加
 */
void SensorManager::addOverlapSensor(int sensorIndex, SqShape *overladShape)
{

    SqSensor *sensor = sensors.get(sensorIndex);

    // printf("addOverlapSensor %d %d %p \n", sensorIndex, sensor->overlaps.getCount(), overladShape);

    SqSensorShape *shapeSensor = nullptr;
    for (int i = 0; i < sensor->overlaps.getCount(); ++i)
    {
        if (sensor->overlaps.get(i)->shape == overladShape)
        {
            // printf("===========find \n");
            shapeSensor = sensor->overlaps.get(i);
            break;
        }
    }

    if (shapeSensor == nullptr)
    {
        // printf("AABB check first \n");
        shapeSensor = sensor->overlaps.Add();
        shapeSensor->sensorFlag = 0;
    }

    shapeSensor->shape = overladShape;
    shapeSensor->sensorFlag |= SensorFlag::AABB_OVERLAP;
}

void SensorManager::callEndCallback(SqShape *A, SqShape *B)
{
    SqBody *bodyA = world->getBody(A->getBody());
    SqBody *bodyB = world->getBody(B->getBody());
    if (bodyA->isEnableSensorBeginEvent() || bodyB->isEnableSensorBeginEvent())
    {
        SqSensorEndEvent *event = world->sensorEndEvents.Add();
        event->shapeA = A;
        event->shapeB = B;
    }
}

void SensorManager::processABSensor(SqShape *A, SqShape *B)
{
    SqSensor *bSensor = sensors.get(B->sensorIndex);

    for (int i = 0; i < bSensor->overlaps.getCount(); ++i)
    {
        if (bSensor->overlaps.get(i)->shape == A)
        {
            bSensor->overlaps.get(i)->sensorFlag |= SensorFlag::HAD_PROCESS;
            break;
        }
    }
}

/**
 * 这个方法必须在SqBroadPhase::updatePairs前调用
 * 因为这里会使用到moveArray数据，确保moveArray清理之前调用这个
 */
void SensorManager::updateSensors()
{

    // printf("updateSensors %d \n", sensors.getCount());

    auto iterate = [this](int sensorIndex)
    {
        SqSensor *sensor = sensors.get(sensorIndex);

        // Shape proxyKey == SQ_NULL_INDEX 表示 被Disable了，那么一切都失效了。
        if (sensor->sensorFlag & SensorFlag::REMOVE || sensor->shape->proxyKey == SQ_NULL_INDEX)
        {
            for (int i = 0; i < sensor->overlaps.getCount(); ++i)
            {
                SqSensorShape *shapeSensor = sensor->overlaps.get(i);
                if (shapeSensor->sensorFlag & SensorFlag::IN_HIT)
                {
                    callEndCallback(sensor->shape, shapeSensor->shape);
                }
            }
            sensor->overlaps.clear();
            
            if (sensor->sensorFlag & SensorFlag::REMOVE)
                activeSensors.removeValue(sensorIndex);

            return;
        }

        int c = 0;
        while (c < sensor->overlaps.getCount())
        {
            SqSensorShape *shapeSensor = sensor->overlaps.get(c);

            // Shape被Disable了
            if (shapeSensor->shape->proxyKey == SQ_NULL_INDEX)
            {
                ++c;
                continue;
            }

            /**
             * 比如A和B Shape都是Sensor，如果A和B发生了碰撞
             * 则A中记录了B，B中也记录了A。
             * 在处理A Sensor的时候会带着一起处理B的；所以再在处理B Sensor的时候我们就不需要再处理A了
             */
            if (shapeSensor->sensorFlag & SensorFlag::HAD_PROCESS)
            {
                ++c;
                shapeSensor->sensorFlag &= ~SensorFlag::HAD_PROCESS;
                continue;
            }

            
            if (shapeSensor->shape->isSensor()) // 另外一个Shape也是Sensor，那么标记一下已经处理了
            {
                processABSensor(sensor->shape, shapeSensor->shape);
            }
            //--------------------上段逻辑结束----------------------

            // 只要有其中一个Shape发生位置变换就需要检查是否还处于碰撞状态
            if (world->broadPhase.proxyHasMove(sensor->shape->proxyKey) || world->broadPhase.proxyHasMove(shapeSensor->shape->proxyKey))
            {

                SqBody *bodyA = world->getBody(sensor->shape->getBody());
                SqBody *bodyB = world->getBody(shapeSensor->shape->getBody());

                // printf("check shape %d %d \n", shapeSensor->shape->proxyKey, shapeSensor->sensorFlag & SensorFlag::AABB_OVERLAP);

                // 还在AABB中
                if (shapeSensor->sensorFlag & SensorFlag::AABB_OVERLAP)
                {

                    shapeSensor->sensorFlag &= ~SensorFlag::AABB_OVERLAP;
                    ++c;

                    bool hit = false;

                    // 有些需求只要AABB盒子发生碰撞就可以了，没必须再准确地计算,提升性能
                    if (sensor->sensorFlag & SensorFlag::ONLY_AABB)
                    {
                        hit = true;
                    }
                    else
                    {
                        // 精确地计算是否发生了碰撞
                        SqDistanceInput input;
                        input.proxyA = sqMakeProxy(sensor->shape);
                        input.proxyB = sqMakeProxy(shapeSensor->shape);
                        input.transformA = bodyA->getTransform();
                        input.transformB = bodyB->getTransform();
                        input.useRadii = true;
                        SqSimplexCache cache = {0};
                        SqDistanceOutput output = sqShapeDistance(&input, &cache, NULL, 0);
                        hit = output.distance < 10.0f * FLT_EPSILON;
                    }

                    if (hit)
                    {

                        if (!(shapeSensor->sensorFlag & SensorFlag::IN_HIT))
                        {
                            shapeSensor->sensorFlag |= SensorFlag::IN_HIT;

                            if (bodyA->isEnableSensorBeginEvent() || bodyB->isEnableSensorBeginEvent())
                            {
                                SqSensorBeginEvent *event = world->sensorBeginEvents.Add();
                                event->shapeA = sensor->shape;
                                event->shapeB = shapeSensor->shape;
                            }
                        }
                    }
                    else
                    {

                        if (shapeSensor->sensorFlag & SensorFlag::IN_HIT)
                        {
                            shapeSensor->sensorFlag &= ~SensorFlag::IN_HIT;
                            callEndCallback(sensor->shape, shapeSensor->shape);
                            // 这里不能移除，因为AABB还在重叠状态，完全脱离AABB重叠的状态时才删除
                            //  sensor->overlaps.removeSwap(c);
                        }
                    }
                }
                else
                {
                    // AABB都不重叠，直接删除
                    if (shapeSensor->sensorFlag & SensorFlag::IN_HIT)
                    {
                        callEndCallback(sensor->shape, shapeSensor->shape);
                    }
                    // printf("=====remove b \n");
                    sensor->overlaps.removeSwap(c);
                }
            }
            else
            {
                ++c;
            }
        }
    };
    activeSensors.iterate<int>(iterate);
}
