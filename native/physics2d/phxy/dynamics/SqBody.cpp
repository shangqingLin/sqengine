#include "SqBody.h"
#include "SqWorld.h"
#include "../geometry/shape/SqChainShape.h"
#include <stdio.h>
#include "../common/math/SqMath.h"
#include "../common/SqConfig.h"
#include "../geometry/shape/SqShape.h"
#include "../collision/narrowphase/SqContact.h"
#include "./joint/SqJoint.h"
#include <engine/scene/Node.h>

// #include <emscripten.h>
// void print_js_stack() {
//     EM_ASM({
//         console.log(new Error().stack);
//     });
// }

using namespace phxy;

SqBody::SqBody()
{
	reset();
}

void SqBody::reset()
{
	headShape = nullptr;
	world = nullptr;
	userData = nullptr;
	headContactKey = SQ_NULL_INDEX;
	contactCount = 0;

	islandId = SQ_NULL_INDEX;
	islandPrev = SQ_NULL_INDEX;
	islandNext = SQ_NULL_INDEX;
	isMarkedForIslandSplit = false;

	sleepThreshold = 0.05f * SqConfig::getInstance()->getLengthUnitsPerMeter();

	setIndex = SQ_NULL_INDEX;
	simIndex = SQ_NULL_INDEX;
	bodyIndex = SQ_NULL_INDEX;

	type = sq_staticBody;

	mass = 0.f;
	inertia = 0.f;

	headJointKey = SQ_NULL_INDEX;
	jointCount = 0;

	sleepTime = 0.f;
	bodyFlag = 0;
	bodyFlag |= SqBodyFlags::sq_enableParticlePressure;
	bodyFlag |= SqBodyFlags::sq_enableParticleDamping;
}

void SqBody::addShape(SqShape *shape, bool updateBodyMass)
{
	if (headShape)
	{
		// 新添加的Shape插入到表头
		shape->next = headShape;
		headShape->prev = shape;

		headShape = shape;
	}
	else
	{
		headShape = shape;
	}
	shape->bodyIndex = bodyIndex;
	if (shape->type == sq_chainShape)
	{
		SqChainShape *chainShape = (SqChainShape *)shape;
		for (int i = 0; i < chainShape->segments.getCount(); ++i)
		{
			SqShape *segment = chainShape->segments.get(i);
			segment->bodyIndex = bodyIndex;
		}
	}

	if (setIndex != SqSetType::sq_disabledSet)
	{
		createShapeProxy(shape, false);
	}

	if (updateBodyMass)
	{
		updateBodyMassData();
	}
}

void SqBody::removeShape(SqShape *shape)
{

	SQ_ASSERT(shape->getBody() == bodyIndex);

	// 从SqBody SqShape 链表上移除当前的Shape

	if (shape == headShape)
	{
		headShape = shape->next;
		shape->next->prev = nullptr;
	}
	else
	{
		if (shape->next != nullptr)
		{
			shape->next->prev = shape->prev;
		}
		shape->prev->next = shape->next;
	}

	removeShapeProxy(shape);

	if (shape->isSensor())
	{
		world->sensor.removeSenesor(shape->getSensorIndex());
	}
	else
	{

		// 删除与当前Shape相关的Contact
		int contactKey = headContactKey;
		while (contactKey != SQ_NULL_INDEX)
		{
			int contactId = contactKey >> 1;
			int edgeIndex = contactKey & 1;

			SqContact *contact = world->narrowPhase.getContact(contactId);
			contactKey = contact->edges[edgeIndex].nextKey;
			if (contact->shapeA == shape || contact->shapeB == shape)
			{
				removeContact(contact, false);
			}
		}
	}
}

void SqBody::createShapeProxy(SqShape *shape, bool forcePairCreation)
{
	if (shape->type == sq_chainShape)
	{
		SqChainShape *chainShape = (SqChainShape *)shape;
		const SqTransform &transform = getTransform();
		for (int i = 0; i < chainShape->segments.getCount(); ++i)
		{
			SqChainEdgeShape *segment = chainShape->segments.get(i);
			segment->updateShapeAABBs(transform, type);

			// printf("SqBody::createShapeProxy %llu %llu  \n", segment->filter.categoryBits, segment->filter.maskBits);

			// printf("createShapeProxy chain  %f %f %f %f \n", segment->fatAABB.lowerBound.x, segment->fatAABB.lowerBound.y, segment->fatAABB.upperBound.x, segment->fatAABB.upperBound.y);
			world->broadPhase.createShapeProxy(type, segment, segment->fatAABB, segment->filter.categoryBits, forcePairCreation);
		}
	}
	else
	{
		shape->updateShapeAABBs(getTransform(), type);
		world->broadPhase.createShapeProxy(type, shape, shape->fatAABB, shape->filter.categoryBits, forcePairCreation);
	}
}

void SqBody::removeShapeProxy(SqShape *shape)
{
	if (shape->type == sq_chainShape)
	{
		SqChainShape *chainShape = (SqChainShape *)shape;
		for (int i = 0; i < chainShape->segments.getCount(); ++i)
		{
			SqShape *segment = chainShape->segments.get(i);
			world->broadPhase.destroyShapeProxy(segment);
		}
	}
	else
	{
		world->broadPhase.destroyShapeProxy(shape);
	}
}

const SqTransform &SqBody::getTransform()
{
	// printf("????? SqTransform sim %p %d %d %d \n",this, bodyIndex, setIndex, simIndex);
	SqSolverSet *set = world->solverSets.get(setIndex);
	SqBodySim *bodySim = set->bodySims.get(simIndex);
	return bodySim->transform;
}

void SqBody::setTransform(const SqVec2 &position, const SqRot &rotation)
{
	SQ_ASSERT(SqVec2::isValid(position));
	SQ_ASSERT(SqRot::isValid(rotation));
	SQ_ASSERT(world->lock == false);

	// if (bodyIndex == 0)
	// {
	// 	printf("SqBody::setTransform %f %f %f %f \n", position.x, position.y, rotation.c, rotation.s);
	// }

	SqBodySim *bodySim = getBodySim();

	bodySim->transform.p = position;
	bodySim->transform.q = rotation;
	bodySim->center = SqTransform::transformPoint(bodySim->transform, bodySim->localCenter);
	bodySim->prevSim.rotation = bodySim->transform.q;
	bodySim->prevSim.center = bodySim->center;

	if (isEnabled())
		updateShapeProxy(true);
}

void SqBody::setScale(float sx, float sy)
{
	if (scaleX == sx && scaleY == sy)
		return;

	/**
	 * 记住这里有一个累加缩放的作用
	 *
	 * 比如有一个点x为5
	 * 1、第一次你调用setScale设置为0.5,则 5 * 0.5 = 2.5
	 * 2、然后你不小外部又调用一次设置为0.2，由于上一次缩放得到的点是2.5，则在此基础上累计 2.5 * 0.2 = 0.5
	 *  这很明显不是我们想要的，所以我们先要还原原来的点，在在此基础上缩放才行
	 *
	 *  还原：
	 *   2.5 * （1/0.5）= 5
	 *  然后：
	 * 	 5 * 0.2 这样才符合
	 *
	 */

	float ssx = (1.0f / scaleX) * sx;
	float ssy = (1.0f / scaleY) * sy;
	scaleX = sx;
	scaleY = sy;

	// printf("body shapes %p %p \n", this, headShape);

	SqShape *shape = headShape;
	while (shape)
	{
		// printf("process shape %p %p %d \n",this,shape,shape->type);
		shape->scale(ssx, ssy);
		shape = shape->next;
	}

	// printf("===============set body %d scale %f %f %f %f\n", bodyIndex, sx, sy, ssx, ssy);

	int jointKey = headJointKey;
	while (jointKey != SQ_NULL_INDEX)
	{
		int jointId = jointKey >> 1;
		int edgeIndex = jointKey & 1;
		SqJoint *joint = world->getJoint(jointId);
		jointKey = joint->edges[edgeIndex].nextKey;

		SqTransform *localTransform = nullptr;
		if (joint->edges[0].bodyId == bodyIndex)
		{
			localTransform = &(SqTransform &)joint->getLocalFrameATransform();
		}
		else
		{
			SQ_ASSERT(joint->edges[1].bodyId == bodyIndex);
			localTransform = &(SqTransform &)joint->getLocalFrameBTransform();
		}

		// printf("   set joint scale before %f %f \n", localTransform->p.x, localTransform->p.y);

		localTransform->p.x *= ssx;
		localTransform->p.y *= ssy;

		// printf("   set after joint scale %f %f \n", localTransform->p.x, localTransform->p.y);
	}

	updateShapeProxy(true);
	updateBodyMassData();
}

bool SqBody::wakeBody()
{
	if (setIndex >= sq_firstSleepingSet)
	{

		// 从Sleep中醒来，则重置进入Sleep的时间
		sleepTime = 0.f;
		world->moveSleepSetToAwakeSet(setIndex);
		return true;
	}
	return false;
}

void SqBody::setAwake(bool awake)
{

	if (awake && setIndex >= sq_firstSleepingSet)
	{
		wakeBody();
	}
	else if (awake == false && setIndex == sq_awakeSet)
	{
		SqIsland *island = world->getIsland(islandId);
		if (island->constraintRemoveCount > 0)
		{
			island->split();
		}

		// split操作会将原来的销毁的，需要获取新的island实例
		island = world->getIsland(islandId);
		island->sleep();
	}
}

bool SqBody::isStatic()
{
	return setIndex == SqSetType::sq_staticSet;
}

bool SqBody::isAwake()
{
	return setIndex == SqSetType::sq_awakeSet;
}

bool SqBody::isSleep()
{
	return setIndex >= SqSetType::sq_firstSleepingSet;
}

bool SqBody::isEnabled()
{
	return setIndex != SqSetType::sq_disabledSet;
}

void SqBody::enable()
{
	if (setIndex != SqSetType::sq_disabledSet)
	{
		return;
	}

	// printf("+++++++++++enable body %p bodyId %d \n", this, bodyIndex);
	// TRACE();
	
	// 从disabledSet中移除，加入到awakeSet或staticSet中
	SqSolverSet *disabledSet = world->solverSets.get(SqSetType::sq_disabledSet);
	int setId = type == sq_staticBody ? sq_staticSet : sq_awakeSet;
	SqSolverSet *targetSet = world->solverSets.get(setId);
	disabledSet->transferBody(targetSet, this);
	SqShape *shape = headShape;
	while (shape)
	{
		createShapeProxy(shape, false);
		shape = shape->next;
	}

	if (setIndex != sq_staticSet)
	{
		createIslandForBody(setIndex);
	}

	// 在Joint中，只要有一方的Body是Disable的，则整个Joint都设置为Disable
	// 当前这个Body被激活了，则检测一下是否需要激活Joint
	int jointKey = headJointKey;
	while (jointKey != SQ_NULL_INDEX)
	{
		int jointId = jointKey >> 1;
		int edgeIndex = jointKey & 1;

		SqJoint *joint = world->getJoint(jointId);

		// printf(">>>>>>>>>>> %d %d \n", jointId, joint->setIndex);

		SQ_ASSERT(!joint->isEnable());
		SQ_ASSERT(joint->islandId == SQ_NULL_INDEX);
		jointKey = joint->edges[edgeIndex].nextKey;

		SqBody *bodyA = world->getBody(joint->edges[0].bodyId);
		SqBody *bodyB = world->getBody(joint->edges[1].bodyId);

		// 另外一个Body还是Disable，那么Joint保持Disable
		if (!bodyA->isEnabled() || !bodyB->isEnabled())
		{
			continue;
		}

		// 那么激活Joint
		int jointSetId;
		if (bodyA->setIndex == sq_staticSet && bodyB->setIndex == sq_staticSet)
		{
			jointSetId = sq_staticSet;
		}
		else if (bodyA->setIndex == sq_staticSet)
		{
			// 如果A为静态，而B不是静态，则使用B动态的set进行模拟
			jointSetId = bodyB->setIndex;
		}
		else
		{
			// 如果B为静态，而A不是静态，则使用A动态的set进行模拟
			jointSetId = bodyA->setIndex;
		}

		SqSolverSet *set = world->getSloverSet(jointSetId);
		SqSolverSet *currentSet = world->getSloverSet(joint->setIndex);
		currentSet->transferJoint(set, joint);

		if (joint->isEnable())
		{
			world->linkJointToIsland(joint, false);
		}
	}
}

void SqBody::disable()
{
	if (setIndex == SqSetType::sq_disabledSet)
	{
		return;
	}

	// printf("+++++++++++disable body %p bodyId %d \n", this, bodyIndex);

	SqSolverSet *targetSet = world->solverSets.get(SqSetType::sq_disabledSet);
	SqSolverSet *srcSet = world->solverSets.get(setIndex);
	srcSet->transferBody(targetSet, this);
	SqShape *shape = headShape;
	while (shape)
	{
		removeShapeProxy(shape);
		shape = shape->next;
	}

	removeAllContacts(false);

	// 在Joint中，只要有一方的Body是Disable的，则整个Joint都设置为Disable
	// 当前这个Body被Disable，则Joint也需要被Disable
	SqSolverSet *disableSet = world->getSloverSet(sq_disabledSet);
	int jointKey = headJointKey;
	while (jointKey != SQ_NULL_INDEX)
	{
		int jointId = jointKey >> 1;
		int edgeIndex = jointKey & 1;

		SqJoint *joint = world->getJoint(jointId);
		jointKey = joint->edges[edgeIndex].nextKey;

		// printf(">>>>>>>>>>> %d %d \n", jointId, joint->setIndex);

		if (!joint->isEnable())
		{
			continue;
		}

		if (joint->islandId != SQ_NULL_INDEX)
		{
			world->unlinkJointFromIsland(joint);
		}
		SqSolverSet *currentSet = world->getSloverSet(joint->setIndex);
		currentSet->transferJoint(disableSet, joint);
	}

	// Island必须在最后移除
	removeIsland();
}

bool SqBody::isEnableSleep()
{
	SqBodySim *bodySim = getBodySim();
	return bodySim->flags & sq_enableSleep;
}

void SqBody::setEnableSleep(bool e)
{
	SqBodySim *bodySim = getBodySim();
	if (e)
	{
		bodySim->flags |= sq_enableSleep;
	}
	else
	{
		bodySim->flags &= ~sq_enableSleep;
	}
}

bool SqBody::isEnableContinuous()
{
	SqBodySim *bodySim = getBodySim();
	return bodySim->flags & sq_enableContinuous;
}

void SqBody::setEnableContinuous(bool enable)
{
	SqBodySim *bodySim = getBodySim();
	if (enable)
	{
		bodySim->flags |= sq_enableContinuous;
	}
	else
	{
		bodySim->flags &= ~sq_enableContinuous;
	}
}

SqBodySim *SqBody::getBodySim()
{
	SqSolverSet *set = world->solverSets.get(setIndex);
	SqBodySim *bodySim = set->bodySims.get(simIndex);
	return bodySim;
}

void SqBody::updateExtent(const SqVec2 &localCenter)
{
	/**
	 * 一个Body上可以组合多个Shape，从而组成一个大的形状，
	 * 所以这个大的形状有自己的质心，这里就是计算shape距离这个质心最远和最近的点的距离
	 */

	SqBodySim *bodySim = getBodySim();
	SqShape *shape = headShape;
	while (shape)
	{

		if (shape->type == SqShapeType::sq_chainShape)
		{
			SqChainShape *chainShape = (SqChainShape *)shape;
			const SqTransform &transform = getTransform();
			for (int i = 0; i < chainShape->segments.getCount(); ++i)
			{
				SqShape *segment = chainShape->segments.get(i);
				SqShapeExtent extent = segment->computeShapeExtent(localCenter);
				bodySim->minExtent = min(bodySim->minExtent, extent.minExtent);
				bodySim->maxExtent = max(bodySim->maxExtent, extent.maxExtent);
			}
		}
		else
		{
			SqShapeExtent extent = shape->computeShapeExtent(localCenter);
			bodySim->minExtent = min(bodySim->minExtent, extent.minExtent);
			bodySim->maxExtent = max(bodySim->maxExtent, extent.maxExtent);
		}
		shape = shape->next;
	}
}

/**
 * 计算Body的质量、质心、转动惯性
 */
void SqBody::updateBodyMassData()
{
	SqBodySim *bodySim = getBodySim();

	this->mass = 0.0f;
	this->inertia = 0.0f;

	bodySim->invMass = 0.0f;
	bodySim->invInertia = 0.0f;
	bodySim->localCenter.zero();
	bodySim->minExtent = SqConfig::getInstance()->getHugValue();
	bodySim->maxExtent = 0.0f;

	// 对于Static 和 kinematic 类型的Body他们的Mass为0
	if (type != sq_dynamicBody)
	{
		bodySim->center = bodySim->transform.p;

		// Need extents for kinematic bodies for sleeping to work correctly.
		updateExtent(SqVec2());
		return;
	}

	SqVec2 localCenter;
	SqShape *shape = headShape;
	while (shape)
	{
		// 密度为0则没有质量，不需要计算
		if (shape->density == 0.0f)
		{
			continue;
		}

		SqMassData massData = shape->computeShapeMass();
		this->mass += massData.mass;
		localCenter = SqVec2::MulAdd(localCenter, massData.mass, massData.center);
		this->inertia += massData.rotationalInertia;
		shape = headShape->next;
	}

	/**
	 * 质心坐标计算公式 ： 各个纬度上总重量/总质量
	 */

	if (this->mass > 0.0f)
	{
		bodySim->invMass = 1.0f / this->mass;
		localCenter = SqVec2::MulSV(bodySim->invMass, localCenter);
	}

	if (this->inertia > 0.0f)
	{
		// 计算转动惯性
		this->inertia -= this->mass * SqVec2::Dot(localCenter, localCenter);
		SQ_ASSERT(inertia > 0.0f);
		bodySim->invInertia = 1.0f / this->inertia;
	}
	else
	{
		this->inertia = 0.0f;
		bodySim->invInertia = 0.0f;
	}

	SqVec2 oldCenter = bodySim->center;
	bodySim->localCenter = localCenter;
	bodySim->center = SqTransform::transformPoint(bodySim->transform, bodySim->localCenter);
	bodySim->prevSim.center = bodySim->center;

	// Update center of mass velocity
	// b2BodyState *state = b2GetBodyState(world, body);
	// if (state != NULL)
	// {
	// 	b2Vec2 deltaLinear = b2CrossSV(state->angularVelocity, b2Sub(bodySim->center, oldCenter));
	// 	state->linearVelocity = b2Add(state->linearVelocity, deltaLinear);
	// }

	// Compute body extents relative to center of mass
	updateExtent(localCenter);
}

void SqBody::createIslandForBody(int setIndex)
{
	SQ_ASSERT(islandId == SQ_NULL_INDEX);
	SQ_ASSERT(islandPrev == SQ_NULL_INDEX);
	SQ_ASSERT(islandNext == SQ_NULL_INDEX);
	SQ_ASSERT(setIndex != sq_disabledSet);
	SqIsland *island = world->createIsland(setIndex);
	islandId = island->islandId;
	island->headBody = bodyIndex;
	island->tailBody = bodyIndex;
	island->bodyCount = 1;
}

void SqBody::setType(SqBodyType type)
{
	if (world->lock)
		return;

	if (this->type == type)
	{
		return;
	}

	if (setIndex == SqSetType::sq_disabledSet)
	{
		// 被禁用的Body先不对type处理，在enable的时候再处理
		this->type = type;
		updateBodyMassData();
		return;
	}

	SqBodyType originalType = this->type;

	bool wakeBodies = false;
	removeAllContacts(wakeBodies);

	// Wake this body because we assume below that it is awake or static.
	wakeBody();

	{
		int jointKey = this->headJointKey;
		while (jointKey != SQ_NULL_INDEX)
		{
			int jointId = jointKey >> 1;
			int edgeIndex = jointKey & 1;

			// 从旧的Island中移除，因为接下来的逻辑会为Body创建新的Island
			SqJoint *joint = world->getJoint(jointId);
			if (joint->islandId != SQ_NULL_INDEX)
			{
				world->unlinkJointFromIsland(joint);
			}

			// A body going from static to dynamic or kinematic goes to the awake set
			// and other attached bodies must be awake as well. For consistency, this is
			// done for all cases.
			SqBody *bodyA = world->getBody(joint->edges[0].bodyId);
			SqBody *bodyB = world->getBody(joint->edges[1].bodyId);
			bodyA->wakeBody();
			bodyB->wakeBody();
			jointKey = joint->edges[edgeIndex].nextKey;
		}
	}

	this->type = type;

	if (originalType == sq_staticBody)
	{
		// 处理表示Body从sq_staticBody 转换为 sq_kinematicBody或sq_dynamicBody
		SQ_ASSERT(setIndex == sq_staticSet);

		SqSolverSet *staticSet = world->getSloverSet(sq_staticBody);
		SqSolverSet *awakeSet = world->getSloverSet(sq_awakeSet);

		// 从静态Set移动到wakeSet，相当于激活Body
		staticSet->transferBody(awakeSet, this);

		createIslandForBody(sq_awakeSet);

		// Transfer static joints to awake set
		int jointKey = this->headJointKey;
		while (jointKey != SQ_NULL_INDEX)
		{
			int jointId = jointKey >> 1;
			int edgeIndex = jointKey & 1;

			SqJoint *joint = world->getJoint(jointId);

			// Transfer the joint if it is in the static set
			if (joint->setIndex == sq_staticSet)
			{
				staticSet->transferJoint(awakeSet, joint);
			}
			else if (joint->setIndex == sq_awakeSet)
			{
				// In this case the joint must be re-inserted into the constraint graph to ensure the correct
				// graph color.

				// First transfer to the static set.
				// b2TransferJoint(world, staticSet, awakeSet, joint);

				// // Now transfer it back to the awake set and into the graph coloring.
				// b2TransferJoint(world, awakeSet, staticSet, joint);
			}
			else
			{
				// Otherwise the joint must be disabled.
				SQ_ASSERT(joint->setIndex == sq_disabledSet);
			}

			jointKey = joint->edges[edgeIndex].nextKey;
		}
		updateShapeProxy();
	}
	else if (type == sq_staticBody)
	{
		// 处理由其他状态转换到静态的Body

		SQ_ASSERT(setIndex == sq_awakeSet);

		SqSolverSet *staticSet = world->solverSets.get(sq_staticSet);
		SqSolverSet *awakeSet = world->solverSets.get(sq_awakeSet);
		awakeSet->transferBody(staticSet, this);

		// 静态Body不需要模拟，所以不需要Island
		removeIsland();

		SqBodySim *bodySim = staticSet->bodySims.get(simIndex);

		// bodySim->flags &= ~sq_isFast;

		// Maybe transfer joints to static set.
		int jointKey = this->headJointKey;
		while (jointKey != SQ_NULL_INDEX)
		{
			int jointId = jointKey >> 1;
			int edgeIndex = jointKey & 1;

			SqJoint *joint = world->getJoint(jointId);
			jointKey = joint->edges[edgeIndex].nextKey;

			int otherEdgeIndex = edgeIndex ^ 1;
			SqBody *otherBody = world->getBody(joint->edges[otherEdgeIndex].bodyId);

			// Skip disabled joint
			if (joint->setIndex == SqSetType::sq_disabledSet)
			{
				// Joint is disable, should be connected to a disabled body
				SQ_ASSERT(otherBody->setIndex == sq_disabledSet);
				continue;
			}

			// Since the body was not static, the joint must be awake.
			SQ_ASSERT(joint->setIndex == sq_awakeSet);

			if (otherBody->setIndex == sq_staticSet)
			{
				// 相当于Joint的两个Body都是静态的，那么这个Joint才移到静态Set中
				awakeSet->transferJoint(staticSet, joint);
			}
			else
			{
				// 当前的Body为静态，但Joint的另外Body为动态，那么Joint还是需要在Awake Set中
				// The other body must be awake.
				SQ_ASSERT(otherBody->setIndex == sq_awakeSet);

				// The joint must live in a graph color.
				// SQ_ASSERT(0 <= joint->colorIndex && joint->colorIndex < B2_GRAPH_COLOR_COUNT);

				// In this case the joint must be re-inserted into the constraint graph to ensure the correct
				// graph color.

				// First transfer to the static set.
				// b2TransferJoint(world, staticSet, awakeSet, joint);

				// Now transfer it back to the awake set and into the graph coloring.
				// b2TransferJoint(world, awakeSet, staticSet, joint);
			}
		}
		updateShapeProxy();
	}
	else
	{
		// 处理sq_dynamicBody和sq_kinematicBody互换的情况
		SQ_ASSERT(originalType == sq_dynamicBody || originalType == sq_kinematicBody);
		SQ_ASSERT(type == sq_dynamicBody || type == sq_kinematicBody);
		updateShapeProxy();
	}

	// Relink all joints
	{
		int jointKey = this->headJointKey;
		while (jointKey != SQ_NULL_INDEX)
		{
			int jointId = jointKey >> 1;
			int edgeIndex = jointKey & 1;

			SqJoint *joint = world->getJoint(jointId);
			jointKey = joint->edges[edgeIndex].nextKey;

			int otherEdgeIndex = edgeIndex ^ 1;
			int otherBodyId = joint->edges[otherEdgeIndex].bodyId;
			SqBody *otherBody = world->getBody(otherBodyId);

			if (otherBody->setIndex == sq_disabledSet)
			{
				continue;
			}

			if (this->type == sq_staticBody && otherBody->type == sq_staticBody)
			{
				continue;
			}
			world->linkJointToIsland(joint, false);
		}

		world->mergeAwakeIslands();
	}

	updateBodyMassData();

	// b2ValidateSolverSets(world);
}

void SqBody::setMassData(const SqMassData &massData)
{
	SQ_ASSERT(isValidFloat(massData.mass) && massData.mass >= 0.0f);
	SQ_ASSERT(isValidFloat(massData.rotationalInertia) && massData.rotationalInertia >= 0.0f);
	SQ_ASSERT(SqVec2::isValid(massData.center));
	SqBodySim *bodySim = getBodySim();

	mass = massData.mass;
	inertia = massData.rotationalInertia;
	bodySim->localCenter = massData.center;

	SqVec2 center = SqTransform::transformPoint(bodySim->transform, massData.center);
	bodySim->center = center;
	bodySim->prevSim.center = center;

	bodySim->invMass = mass > 0.0f ? 1.0f / mass : 0.0f;
	bodySim->invInertia = inertia > 0.0f ? 1.0f / inertia : 0.0f;
}

SqMassData SqBody::getMassData()
{
	SqBodySim *bodySim = getBodySim();
	SqMassData massData = {mass, bodySim->localCenter, inertia};
	return massData;
}

float SqBody::getMass()
{
	return mass;
}

void SqBody::updateShapeProxy(bool checkSmallMove)
{
	const SqTransform &transform = getTransform();
	SqShape *shape = headShape;
	while (shape)
	{
		if (checkSmallMove)
		{
			// shape->aabb = shape->computeShapeAABB(transform);
			// // 发生了的变换可能是非常微小，还在fatAABB内，则不需要更新tree，提升性能
			// if (SqAABB::Contains(shape->fatAABB, shape->aabb) == false)
			// {
			// 	shape->updateFatAABB(type);
			// 	if (shape->proxyKey != SQ_NULL_INDEX)
			// 	{
			// 		updateShapeProxy(shape);
			// 	}
			// }

			if (shape->type == sq_chainShape)
			{
				SqChainShape *chainShape = (SqChainShape *)shape;
				const SqTransform &transform = getTransform();
				for (int i = 0; i < chainShape->segments.getCount(); ++i)
				{
					SqShape *segment = chainShape->segments.get(i);
					segment->aabb = segment->computeShapeAABB(transform);

					// 发生了的变换可能是非常微小，还在fatAABB内，则不需要更新tree，提升性能
					if (SqAABB::Contains(segment->fatAABB, segment->aabb) == false)
					{
						segment->updateFatAABB(type);
						world->broadPhase.updateProxy(segment->proxyKey, segment->fatAABB);
					}
				}
			}
			else
			{

				// printf("updateShapeProxy p(%f %f) q(%f %f)\n",transform.p.x,transform.p.y,transform.q.c,transform.q.s);

				shape->aabb = shape->computeShapeAABB(transform);
				if (SqAABB::Contains(shape->fatAABB, shape->aabb) == false)
				{
					shape->updateFatAABB(type);
					if (shape->proxyKey != SQ_NULL_INDEX)
					{
						world->broadPhase.updateProxy(shape->proxyKey, shape->fatAABB);
					}
				}
			}
		}
		else
		{
			world->broadPhase.destroyShapeProxy(shape);
			shape->updateShapeAABBs(transform, type);
			createShapeProxy(shape, true);
		}
		shape = shape->next;
	}
}

void SqBody::setLinearDamping(float linearDamping)
{
	SQ_ASSERT(isValidFloat(linearDamping) && linearDamping >= 0.0f);
	SqBodySim *bodySim = getBodySim();
	bodySim->linearDamping = linearDamping;
}

float SqBody::getLinearDamping()
{
	SqBodySim *bodySim = getBodySim();
	return bodySim->linearDamping;
}

void SqBody::setAngularDamping(float angularDamping)
{
	SQ_ASSERT(isValidFloat(angularDamping) && angularDamping >= 0.0f);
	SqBodySim *bodySim = getBodySim();
	bodySim->angularDamping = angularDamping;
}

float SqBody::getAngularDamping()
{
	SqBodySim *bodySim = getBodySim();
	return bodySim->angularDamping;
}

void SqBody::applyLinearImpulse(const SqVec2 &impulse, const SqVec2 &point, bool wake)
{

	if (type != sq_dynamicBody || setIndex == SqSetType::sq_disabledSet)
	{
		return;
	}

	if (wake && isSleep())
	{
		wakeBody();
	}

	if (setIndex == sq_awakeSet)
	{
		SqBodySim *sim = getBodySim();

		// 因为冲量是力在时间上的累积。所以可以认为冲量就是力。所以计算的需要加的速度为： F/m = a = v
		sim->linearVelocity = SqVec2::MulAdd(sim->linearVelocity, sim->invMass, impulse);

		// 不作用在质心的冲量（力）会产生力矩，从而产生转动
		sim->angularVelocity += sim->invInertia * SqVec2::Cross(SqVec2::Sub(point, sim->center), impulse);

		// b2LimitVelocity(state, world->maxLinearSpeed);
	}
}

void SqBody::applyLinearImpulseToCenter(const SqVec2 &impulse, bool wake)
{

	if (type != sq_dynamicBody || setIndex == SqSetType::sq_disabledSet)
	{
		return;
	}

	if (wake && isSleep())
	{
		wakeBody();
	}

	if (setIndex == sq_awakeSet)
	{
		SqBodySim *sim = getBodySim();
		sim->linearVelocity = SqVec2::MulAdd(sim->linearVelocity, sim->invMass, impulse);
		// b2LimitVelocity(state, world->maxLinearSpeed);
	}
}

void SqBody::applyAngularImpulse(float impulse, bool wake)
{
	if (type != sq_dynamicBody || setIndex == SqSetType::sq_disabledSet)
	{
		return;
	}

	if (wake && isSleep())
	{
		wakeBody();
	}

	if (setIndex == sq_awakeSet)
	{
		SqBodySim *sim = getBodySim();
		sim->angularVelocity += sim->invInertia * impulse;
	}
}

void SqBody::setGravityScale(float gravityScale)
{
	SQ_ASSERT(isValidFloat(gravityScale));
	SqBodySim *bodySim = getBodySim();
	bodySim->gravityScale = gravityScale;
}

float SqBody::getGravityScale()
{
	SqBodySim *bodySim = getBodySim();
	return bodySim->gravityScale;
}

void SqBody::applyForce(const SqVec2 &force, const SqVec2 &point, bool wake)
{

	// disable和不是动态的body直接忽略
	if (type != sq_dynamicBody || setIndex == SqSetType::sq_disabledSet)
	{
		return;
	}

	if (wake && isSleep())
	{
		wakeBody();
	}

	if (setIndex == sq_awakeSet)
	{
		SqBodySim *bodySim = getBodySim();
		bodySim->force = SqVec2::Add(bodySim->force, force);

		// 力如果不应用在质心上，则会对物体产生力矩，从而转到，这里就是计算力矩
		// T = r x F 这是一个叉积，在二维的情况叉积是标量
		bodySim->torque += SqVec2::Cross(SqVec2::Sub(point, bodySim->center), force);
	}
}

void SqBody::applyForceToCenter(const SqVec2 &force, bool wake)
{
	if (type != sq_dynamicBody || setIndex == SqSetType::sq_disabledSet)
	{
		return;
	}

	if (wake && isSleep())
	{
		wakeBody();
	}

	if (setIndex == sq_awakeSet)
	{
		SqBodySim *bodySim = getBodySim();
		bodySim->force = SqVec2::Add(bodySim->force, force);
	}
}

void SqBody::applyTorque(float torque, bool wake)
{

	if (type != sq_dynamicBody || setIndex == SqSetType::sq_disabledSet)
	{
		return;
	}

	if (wake && isSleep())
	{
		wakeBody();
	}

	if (setIndex == sq_awakeSet)
	{
		SqBodySim *bodySim = getBodySim();
		bodySim->torque += torque;
	}
}

const SqVec2 &SqBody::getLinearVelocity()
{
	return getBodySim()->linearVelocity;
}

/**
 *
 * 给定一个世界空间中的点 worldPoint，计算这个点在刚体 bodyId 上的速度。这个速度综合了刚体的线速度和角速度引起的旋转速度。
 * 背后的物理公式（刚体动力学）：
 * v(P) = v(C) + ω × r
 * 其中：
		v(P)：点 P 的速度（即我们要求的结果）
		v(C)：质心的线速度（m_linearVelocity）
		ω：角速度（m_angularVelocity，是标量，在2D中逆时针为正）
		r = P - C：从质心 C 到点 P 的矢量（即 worldPoint - position）
		ω × r：角速度引起的切向线速度

 * 即角速度会在切线方向为物体添加一个速度，所以切线的速度+质心速度 = 物体上某点的速度
 *
 */
SqVec2 SqBody::getLinearVelocityFromWorldPoint(const SqVec2 &worldPoint)
{
	const SqVec2 &m_linearVelocity = getLinearVelocity();
	float m_angularVelocity = getAngularVelocity();
	return SqVec2::Add(m_linearVelocity, SqVec2::CrossSV(m_angularVelocity, SqVec2::Sub(worldPoint, getBodySim()->center)));
}

float SqBody::getAngularVelocity()
{
	return getBodySim()->angularVelocity;
}

void SqBody::setLinearVelocity(const SqVec2 &linearVelocity)
{

	if (type == sq_staticBody)
	{
		return;
	}

	if (SqVec2::LengthSquared(linearVelocity) > 0.0f)
	{
		wakeBody();
	}
	getBodySim()->linearVelocity = linearVelocity;
}

void SqBody::setAngularVelocity(float angularVelocity)
{
	SqBodySim *bodySim = getBodySim();
	if (type == sq_staticBody || (bodySim->flags & SqBodyFlags::sq_lockAngularZ))
	{
		return;
	}

	if (angularVelocity != 0.0f)
	{
		wakeBody();
	}

	getBodySim()->angularVelocity = angularVelocity;
}

void SqBody::setBullet(bool flag)
{
	SqBodySim *bodySim = getBodySim();
	if (flag)
	{
		bodySim->flags |= SqBodyFlags::sq_isBullet;
	}
	else
	{
		bodySim->flags &= ~SqBodyFlags::sq_isBullet;
	}
}

bool SqBody::isBullet()
{
	SqBodySim *bodySim = getBodySim();
	return (bodySim->flags & SqBodyFlags::sq_isBullet) != 0;
}

void SqBody::setMotionLocks(SqBodyMotionLocks locks)
{

	uint32_t newFlags = 0;
	newFlags |= locks.linearX ? sq_lockLinearX : 0;
	newFlags |= locks.linearY ? sq_lockLinearY : 0;
	newFlags |= locks.angularZ ? sq_lockAngularZ : 0;
	SqBodySim *bodySim = getBodySim();

	if ((bodySim->flags & sq_allLocks) != newFlags)
	{
		bodySim->flags &= ~sq_allLocks;
		bodySim->flags |= newFlags;
		if (locks.linearX)
		{
			bodySim->linearVelocity.x = 0.0f;
		}

		if (locks.linearY)
		{
			bodySim->linearVelocity.y = 0.0f;
		}

		if (locks.angularZ)
		{
			bodySim->angularVelocity = 0.0f;
		}
		updateBodyMassData();
	}
}

SqBodyMotionLocks SqBody::getMotionLocks()
{
	SqBodySim *bodySim = getBodySim();
	SqBodyMotionLocks locks;
	locks.linearX = (bodySim->flags & SqBodyFlags::sq_lockLinearX);
	locks.linearY = (bodySim->flags & SqBodyFlags::sq_lockLinearY);
	locks.angularZ = (bodySim->flags & SqBodyFlags::sq_lockAngularZ);
	return locks;
}

/**
 * 将Body从Island上移除
 */
void SqBody::removeIsland()
{
	if (islandId != SQ_NULL_INDEX)
	{
		world->getIsland(islandId)->removeBody(this);
	}
}

/**
 * 单纯将Joint加入到Body链表中
 */
void SqBody::addJointToLink(SqJoint *joint, int index)
{
	int jointId = joint->jointIndex;
	SQ_ASSERT(jointId != SQ_NULL_INDEX);
	SQ_ASSERT(index == 0 || index == 1);

	// 与jont的edge配合，构造成链表的存储方式
	joint->edges[index].bodyId = bodyIndex;
	joint->edges[index].prevKey = SQ_NULL_INDEX;
	joint->edges[index].nextKey = headJointKey;

	// 第一位存储index，表示body存储在Joint的第几个index中
	// 后面的位存储jointId
	// 所以这个key包含了：当前的Body存储在哪个joint的那个Edge中
	int key = (jointId << 1) | index;
	if (headJointKey != SQ_NULL_INDEX)
	{
		SqJoint *jointA = world->getJoint(headJointKey >> 1);
		SqJointEdge *edgeA = jointA->edges + (headJointKey & 1);
		edgeA->prevKey = key;
	}

	headJointKey = key;
	jointCount += 1;
}

/**
 * 单单是从Body的链表中移除Joint
 * 不是真正移除这个joint，只是从Body移除而已，joint上的Island等都不会被移除
 */
void SqBody::removeJointFromLink(SqJoint *joint)
{

	// printf("removeJointFromLink %p \n",joint);

	SqJointEdge *edge = joint->edges + 0;
	int index = 0;
	if (edge->bodyId != bodyIndex)
	{
		edge = joint->edges + 1;

		SQ_ASSERT(edge->bodyId == bodyIndex); // 从此Body上找不到Joint，那么可能重复移除Body或Joint了？

		if (edge->bodyId != bodyIndex)
		{
			// 表示Joint已经从Body上移除了
			return;
		}
		index = 1;
	}

	// jont存储在当前body中的key
	int savekey = SQ_NULL_INDEX;
	int edgeKey = (joint->jointIndex << 1) | index;

	if (edge->prevKey != SQ_NULL_INDEX)
	{
		SqJoint *prevJoint = world->getJoint(edge->prevKey >> 1);
		SqJointEdge *prevEdge = prevJoint->edges + (edge->prevKey & 1);
		savekey = prevEdge->nextKey; // 前一个的next肯定为当前的key
		prevEdge->nextKey = edge->nextKey;
	}

	if (edge->nextKey != SQ_NULL_INDEX)
	{
		SqJoint *nextJoint = world->getJoint(edge->nextKey >> 1);
		SqJointEdge *nextEdge = nextJoint->edges + (edge->nextKey & 1);
		savekey = nextEdge->prevKey; // 后一个的prev肯定为当前的key
		nextEdge->prevKey = edge->prevKey;
	}

	// 表示没有prev和next，那么当前的肯定为根节点了
	if (savekey == SQ_NULL_INDEX)
	{
		savekey = headJointKey;
	}

	SQ_ASSERT(savekey == edgeKey);

	if (headJointKey == edgeKey)
	{
		headJointKey = edge->nextKey;
	}

	jointCount -= 1;
}

void SqBody::removeAllJoint()
{
	while (headJointKey != SQ_NULL_INDEX)
	{
		int jointId = headJointKey >> 1;
		SqJoint *joint = world->getJoint(jointId);

		// 在destroyJonint里面会调用上面的removeJointFromLink，使得headJointKey更新
		world->destroyJoint(joint, false);
	}
}

void SqBody::removeContact(SqContact *contact, bool wakeBodies)
{
	world->narrowPhase.removeContactById(contact->contactId, wakeBodies);
}

void SqBody::removeAllContacts(bool wakeBodies)
{
	world->narrowPhase.removeContact(bodyIndex, wakeBodies);
}

void SqBody::destroy()
{

	removeAllContacts(false);
	removeAllJoint();
	SqShape *shape = headShape;
	while (shape)
	{
		if (shape->isSensor())
		{
			world->sensor.removeSenesor(shape->getSensorIndex());
		}
		if (shape->type == SqShapeType::sq_chainShape)
		{
			SqChainShape *chain = (SqChainShape *)shape;
			for (int i = 0; i < chain->segments.getCount(); ++i)
			{
				world->broadPhase.destroyShapeProxy(chain->segments.get(i));
			}
		}
		else
		{
			world->broadPhase.destroyShapeProxy(shape);
		}
		SqShape *deleteShape = shape;
		shape = shape->next;
		delete deleteShape;
	}

	removeIsland();
	SqSolverSet *set = world->getSloverSet(setIndex);
	set->removeBody(simIndex);
	if (set->setIndex >= sq_firstSleepingSet && set->bodySims.getCount() == 0)
	{
		world->removeSloverSet(set->setIndex);
	}
	reset();
	// b2ValidateSolverSets(world);
}

void SqBody::enableBeginContactEvent(bool b)
{
	if (b)
	{
		bodyFlag |= SqBodyFlags::sq_enableContactBeginEvent;
	}
	else
	{
		bodyFlag &= ~SqBodyFlags::sq_enableContactBeginEvent;
	}
}

void SqBody::enableEndContactEvent(bool b)
{
	if (b)
	{
		bodyFlag |= SqBodyFlags::sq_enableContactEndEvent;
	}
	else
	{
		bodyFlag &= ~SqBodyFlags::sq_enableContactEndEvent;
	}
}

void SqBody::enableHitUpdateContactEvent(bool b)
{
	if (b)
	{
		bodyFlag |= SqBodyFlags::sq_enableContactHitEvent;
	}
	else
	{
		bodyFlag &= ~SqBodyFlags::sq_enableContactHitEvent;
	}
}

void SqBody::enableSensorBeginEvent(bool b)
{
	if (b)
	{
		bodyFlag |= SqBodyFlags::sq_enableSensorBeginEvent;
	}
	else
	{
		bodyFlag &= ~SqBodyFlags::sq_enableSensorBeginEvent;
	}
}

void SqBody::enableSensorEndEvent(bool b)
{
	if (b)
	{
		bodyFlag |= SqBodyFlags::sq_enableSensorEndEvent;
	}
	else
	{
		bodyFlag &= ~SqBodyFlags::sq_enableSensorEndEvent;
	}
}

SqVec2 SqBody::getLocalCenterOfMass()
{
	SqBodySim *bodySim = getBodySim();
	return bodySim->localCenter;
}

SqVec2 SqBody::getWorldCenterOfMass()
{
	SqBodySim *bodySim = getBodySim();
	return bodySim->center;
}

void SqBody::setEnableParticlePressure(bool b)
{
	if (b)
	{
		bodyFlag |= SqBodyFlags::sq_enableParticlePressure;
	}
	else
	{
		bodyFlag &= ~SqBodyFlags::sq_enableParticlePressure;
	}
}

void SqBody::setEnablePartcileDamping(bool b)
{
	if (b)
	{
		bodyFlag |= SqBodyFlags::sq_enableParticleDamping;
	}
	else
	{
		bodyFlag &= ~SqBodyFlags::sq_enableParticleDamping;
	}
}