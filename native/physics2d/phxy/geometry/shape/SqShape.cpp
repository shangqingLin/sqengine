#include "SqShape.h"
#include "../../common/SqConfig.h"
#include "SqCapsuleShape.h"
#include "../../collision/broadphase/SqDynamicTree.h"
#include "../../collision/broadphase/SqBroadPhase.h"
#include "../../dynamics/SqWorld.h"

#include <stdio.h>

using namespace phxy;

SqShape::SqShape() : next(nullptr), prev(nullptr)
{
}

SqShape::~SqShape()
{
	world = nullptr;
	bodyIndex = SQ_NULL_INDEX;
	proxyKey = SQ_NULL_INDEX;
	tree = nullptr;
	sensorIndex = SQ_NULL_INDEX;
}

void SqShape::setFilter(const SqShapeFilter &f)
{

	if (filter.categoryBits == f.categoryBits && filter.maskBits == f.maskBits)
		return;
	filter = f;

	// printf("SqShape::setFilter %llu %llu proxyKey %d \n", f.categoryBits, f.maskBits, proxyKey);

	if (proxyKey != SQ_NULL_INDEX)
	{
		int proxyId = SQ_PROXY_ID(proxyKey);
		tree->setCategoryBits(proxyId, f.categoryBits);
	}
}

void SqShape::setFriction(float v) { friction = v; };
void SqShape::setRestitution(float v) { restitution = v; };

void SqShape::setSensor(bool b)
{

	if (b)
	{
		if (sensorIndex != SQ_NULL_INDEX)
			return;
		sensorIndex = world->sensor.createSensor(this);
	}
	else
	{
		if (sensorIndex != SQ_NULL_INDEX)
		{
			world->sensor.removeSenesor(sensorIndex);
		}
	}
}

void SqShape::sensorAABB(bool b)
{
	if (sensorIndex != SQ_NULL_INDEX)
	{
		world->sensor.sensorAABB(sensorIndex, b);
	}
}

bool SqShape::isSensorAABB()
{
	return sensorIndex != SQ_NULL_INDEX && world->sensor.isSensorAABB(sensorIndex);
}

void SqShape::updateShapeAABBs(const SqTransform &transform, SqBodyType proxyType)
{
	aabb = computeShapeAABB(transform);
	updateFatAABB(proxyType);
}

void SqShape::updateFatAABB(SqBodyType proxyType)
{
	const float speculativeDistance = SqConfig::getInstance()->getSpeculativeDistance();
	const float aabbMargin = SqConfig::getInstance()->getAABBMargin();
	float margin = proxyType == sq_staticBody ? speculativeDistance : aabbMargin;
	fatAABB.lowerBound.x = aabb.lowerBound.x - margin;
	fatAABB.lowerBound.y = aabb.lowerBound.y - margin;
	fatAABB.upperBound.x = aabb.upperBound.x + margin;
	fatAABB.upperBound.y = aabb.upperBound.y + margin;
}

void SqShape::RayCastShape(SqCastOutput &output, const SqShapeCastInput &input, const SqTransform &transform) const
{
	if (input.proxy.count == 0)
	{
		return;
	}

	// proxy中如果存储的是世界坐标系下的点，则将点转换到当前Shape的局部坐标系下
	SqShapeCastInput localInput = input;
	for (int i = 0; i < localInput.proxy.count; ++i)
	{
		localInput.proxy.points[i] = transform.invTransformPoint(input.proxy.points[i]);
	}
	localInput.translation = SqRot::invRotateVector(transform.q, input.translation);

	RayCastShapeImpl(output, localInput, transform);

	output.point = SqTransform::transformPoint(transform, output.point);
	output.normal = SqRot::transformVector(transform.q, output.normal);
}

void SqShape::RayCast(SqCastOutput &output, const SqRayCastInput &input, const SqTransform &transform) const
{
	// 将点转换为Shape的本地坐标
	SqRayCastInput localInput = input;
	localInput.origin = transform.invTransformPoint(input.origin);
	localInput.translation = SqRot::invRotateVector(transform.q, input.translation);
	RayCastImpl(output, localInput, transform);

	// 将计算的结果转换会世界坐标系下
	if (output.hit)
	{
		output.point = transform.transformPoint(output.point);
		output.normal = SqRot::transformVector(transform.q, output.normal);
	}
}

void SqShape::CollideMover(SqPlaneResult &result, SqCapsuleShape *mover, SqTransform transform) const
{
	SqCapsuleShape localMover;
	localMover.center1 = transform.invTransformPoint(mover->center1);
	localMover.center2 = transform.invTransformPoint(mover->center2);
	localMover.radius = mover->radius;
	CollideMoverImpl(result, &localMover);
	if (result.hit == false)
	{
		return;
	}
	result.plane.normal = SqRot::transformVector(transform.q, result.plane.normal);
}
