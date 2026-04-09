#include "SqBodySolver.h"
#include "../SqWorld.h"
#include <stdio.h>
#include "../../common/math/SqMath.h"
#include <engine/scene/Node.h>

using namespace phxy;

SqBodySolver::SqBodySolver(SqWorld *world)
    : world(world)
{
}

/**
 * 对刚体应用外力（Force）、力矩（torque）、重力和阻尼
 */
void SqBodySolver::bodyVelocities(SqBodySim *bodySim, SqStepContext &context)
{
    const SqVec2 &gravity = context.world->getGravity();

    SqVec2 v = bodySim->linearVelocity;
    float w = bodySim->angularVelocity;

    // printf("solver velocity before %d v:(%f %f) w:%f \n", bodySim->bodyIndex, v.x, v.y, w);

    float linearDamping = 1.0f / (1.0f + context.h * bodySim->linearDamping);
    float angularDamping = 1.0f / (1.0f + context.h * bodySim->angularDamping);

    float gravityScale = bodySim->invMass > 0.0f ? bodySim->gravityScale : 0.0f;

    /**
     * 计算线性加速度：
     * 牛顿第二定律 F=ma,所以 a = F/a 得出合力之下产生的加速度
     * 所以 a = F/m * dt
     */
    SqVec2 linearVelocityDelta = SqVec2::Add(SqVec2::MulSV(context.h * bodySim->invMass, bodySim->force), SqVec2::MulSV(context.h * gravityScale, gravity));

    // SqVec2 test = SqVec2::MulSV(context.h * bodySim->invMass, bodySim->force);
    // printf("solve %d %f %f %f \n",bodySim->bodyIndex,test.x,test.y,SqVec2::Length(test));

    // printf("sim body force %f %f \n", bodySim->force.x, bodySim->force.y);

    // printf("linearVelocityDelta %f %f f: %f %f \n",linearVelocityDelta.x,linearVelocityDelta.y,bodySim->force.x,bodySim->force.y);

    // printf("%f %f \n",gravityScale,bodySim->gravityScale);

    // printf("l %f %f dt %f mass %f gs %f graw %f %f\n", linearVelocityDelta.x, linearVelocityDelta.y,
    //     context.dt,
    //     bodySim->invMass,bodySim->gravityScale ,gravity.x,gravity.y);

    /**
     * 计算转动加速度
     *
     * 角加速度 = 力矩/转动惯性
     */
    float angularVelocityDelta = context.h * bodySim->invInertia * bodySim->torque;

    // 应用阻尼
    v = SqVec2::MulAdd(linearVelocityDelta, linearDamping, v);
    w = angularVelocityDelta + angularDamping * w;

    // printf("linearDamping %f velocity %f %f \n",linearDamping,v.x,v.y);

    // if (b2Dot(v, v) > maxLinearSpeedSquared)
    // {
    //     float ratio = maxLinearSpeed / b2Length(v);
    //     v = b2MulSV(ratio, v);
    //     bodySim->flags |= b2_isSpeedCapped;
    // }

    // if (w * w > maxAngularSpeedSquared && (bodySim->flags & b2_allowFastRotation) == 0)
    // {
    //     float ratio = maxAngularSpeed / b2AbsFloat(w);
    //     w *= ratio;
    //     bodySim->flags |= b2_isSpeedCapped;
    // }

    if (bodySim->flags & sq_lockLinearX)
    {
        v.x = 0.0f;
    }

    if (bodySim->flags & sq_lockLinearY)
    {
        v.y = 0.0f;
    }

    if (bodySim->flags & sq_lockAngularZ)
    {
        w = 0.0f;
    }

    // 外部不正确的h可能产生不正确的数值，这里检查一下
    SQ_ASSERT(SqVec2::isValid(v));
    SQ_ASSERT(isValidFloat(w));

    bodySim->linearVelocity = v;
    bodySim->angularVelocity = w;

    // SqBody* body = context.world->getBody(bodySim->bodyIndex);
    // Node* node = (Node*)body->getUserData();
    // if(node->nativeId == 19){
    //     printf("solver velocity w %f \n",bodySim->angularVelocity);
    // }
    // printf("solver velocity after %d %f %f w %f \n", bodySim->bodyIndex, bodySim->linearVelocity.x, bodySim->linearVelocity.y, bodySim->angularVelocity);
}

void SqBodySolver::bodyPosition(SqBodySim *bodySim, SqStepContext &context)
{

    // SqBody *body = context.world->getBody(bodySim->bodyIndex);
    // Node *node = (Node *)body->userData;
    // if (node->nativeId == 10000)
    // {
    //     printf("bodyPosition %d \n",bodySim->flags);
    // }

    if (bodySim->flags & sq_lockLinearX)
    {
        bodySim->linearVelocity.x = 0.0f;
    }

    if (bodySim->flags & sq_lockLinearY)
    {
        bodySim->linearVelocity.y = 0.0f;
    }

    if (bodySim->flags & sq_lockAngularZ)
    {
        bodySim->angularVelocity = 0.0f;
    }

    bodySim->deltaPosition = SqVec2::MulAdd(bodySim->deltaPosition, context.h, bodySim->linearVelocity);

    // printf("bodyPosition h %f v %f %f deltaPosition %f %f deLength %f \n",
    //        context.h,
    //        bodySim->linearVelocity.x, bodySim->linearVelocity.y,
    //        bodySim->deltaPosition.x, bodySim->deltaPosition.y,
    //        SqVec2::Length(bodySim->deltaPosition));

    if (bodySim->angularVelocity != 0.0f)
    {
        bodySim->deltaRotation = SqRot::IntegrateRotation(bodySim->deltaRotation, context.h * bodySim->angularVelocity);
    }
}

void SqBodySolver::finalizeTransform(SqBodySim *bodySim, SqStepContext &context)
{
    SqBody *body = context.world->getBody(bodySim->bodyIndex);
    bodySim->center = SqVec2::Add(bodySim->center, bodySim->deltaPosition);
    bodySim->transform.q = SqRot::Normalize(SqRot::Mul(bodySim->deltaRotation, bodySim->transform.q));
    bodySim->transform.p = bodySim->center;

    Node *node = (Node *)body->getUserData();
    // if (node->nativeId == 16)
    // printf("finalizeTransform %d delta %f %f p %f %f q %f %f \n", node->nativeId,
    //        bodySim->deltaPosition.x, bodySim->deltaPosition.y,
    //        bodySim->transform.p.x, bodySim->transform.p.y,
    //        bodySim->transform.q.c, bodySim->transform.q.s);

    bodySim->flags |= SqBodyFlags::sq_enlargeBounds;

    // 计算当前Body距离质心最远的那个点可以产生的速度的大小
    //  线速度 = 角速度 x R
    float maxVelocity = SqVec2::Length(bodySim->linearVelocity) + abs(bodySim->angularVelocity) * bodySim->maxExtent;

    // 计算当前Body距离质心最远的那个点可以产生最大的位移，这只是一个大概的值，当对于Sleep来足够了
    float maxDeltaPosition = SqVec2::Length(bodySim->deltaPosition) + abs(bodySim->deltaRotation.s) * bodySim->maxExtent;

    // context.inv_dt * maxDeltaPosition 距离/时间=速度
    // positionSleepFactor 告诉这个位移在Sleep判断不那么重要，最主要还是看maxVelocity
    float positionSleepFactor = 0.5f;
    float sleepVelocity = max(maxVelocity, positionSleepFactor * context.inv_dt * maxDeltaPosition);

    bodySim->deltaPosition.zero();
    bodySim->deltaRotation.identity();
    bodySim->force.zero();
    bodySim->torque = 0.0f;

    // 可能之前的执行添加了这些标记，这里清空掉，重新进行判断是否需要
    bodySim->flags &= ~sq_needTOI;

    if (context.world->enableSleep == false || body->isEnableSleep() == false || sleepVelocity > body->getSleepThreshold())
    {

        // 快速移动或enableSpleep = false的则不需要进入Sleep状态
        body->sleepTime = 0.0f;

        if (body->getType() == sq_dynamicBody && context.world->enableContinuous && body->isEnableContinuous() && maxVelocity * context.dt > 0.5f * bodySim->minExtent)
        {
            // This flag is only retained for debug draw
            // sim->flags |= b2_isFast;

            if (body->isBullet())
            {
                // int bulletIndex = b2AtomicFetchAddInt(&stepContext->bulletBodyCount, 1);
                // stepContext->bulletBodies[bulletIndex] = simIndex;
            }
            else
            {
                bodySim->flags |= sq_needTOI;
            }
        }
        else
        {
            // Body is safe to advance
            bodySim->prevSim.center = bodySim->center;
            bodySim->prevSim.rotation = bodySim->transform.q;
        }
    }
    else
    {
        bodySim->prevSim.center = bodySim->center;
        bodySim->prevSim.rotation = bodySim->transform.q;
        body->sleepTime += context.dt;
    }

    // printf("++++++finalizeTransform %p %d bodyIndex %d \n", body, body->setIndex,body->bodyIndex);
    SqIsland *island = world->getIsland(body->islandId);

    if (body->sleepTime < SQ_TIME_TO_SLEEP)
    {
        // 注意，一个Island中有许多Body，只有所有的Body都进入了SleepTime才能执行将Island设置为Sleep的操作
        // 所以这里只要有其中一个Body不需要sleep则将整个island设置为不需要sleep
        // printf("++++++++++++ %d %d %d \n", island->islandId, island->setIndex, island->simIndex);
        SqIslandSim *islandSim = island->getSim();
        islandSim->flag |= SqIslandSimFlag::sq_notNeedToSleep;
    }
    else if (island->constraintRemoveCount > 0) // 这个用于判断有约束移除的时候就需要Split
    {
        if (body->sleepTime > context.splitSleepTime)
        {

            // 我们不是每帧都需要进行Split Island，因为这是一个比较重度的操作
            // 每次只能拆分一个，并且是有body想进入Sleep状态再执行拆分操作
            context.splitIslandId = body->islandId;

            // 控制最早想进入Sleep状态的Island对其进行拆分
            // 否则他永远都是找到最后一个想进入Sleep状态的Island进行拆分了
            context.splitSleepTime = body->sleepTime;
        }
    }
}

void SqBodySolver::solveBodyVelocities(SqStepContext &context)
{
    SqSolverSet *awakeSet = world->getSloverSet(sq_awakeSet);
    int awakeBodyCount = awakeSet->bodySims.getCount();
    for (int i = 0; i < awakeBodyCount; ++i)
    {
        bodyVelocities(awakeSet->bodySims.get(i), context);
    }
}

void SqBodySolver::sloveBodyPosition(SqStepContext &context)
{
    SqSolverSet *awakeSet = world->getSloverSet(sq_awakeSet);
    int awakeBodyCount = awakeSet->bodySims.getCount();
    for (int i = 0; i < awakeBodyCount; ++i)
    {
        bodyPosition(awakeSet->bodySims.get(i), context);
    }
}

void SqBodySolver::sloveTransform(SqStepContext &context)
{
    SqSolverSet *awakeSet = world->getSloverSet(sq_awakeSet);
    int awakeBodyCount = awakeSet->bodySims.getCount();
    for (int i = 0; i < awakeBodyCount; ++i)
    {
        finalizeTransform(awakeSet->bodySims.get(i), context);
    }
}

void SqBodySolver::syncProxy(SqStepContext &context)
{
    SqSolverSet *awakeSet = world->getSloverSet(sq_awakeSet);
    int awakeBodyCount = awakeSet->bodySims.getCount();
    for (int i = 0; i < awakeBodyCount; ++i)
    {
        SqBodySim *bodySim = awakeSet->bodySims.get(i);
        if (bodySim->flags & SqBodyFlags::sq_enlargeBounds)
        {
            bodySim->flags &= ~SqBodyFlags::sq_enlargeBounds;
            world->getBody(bodySim->bodyIndex)->updateShapeProxy(true);
        }
    }
}

void SqBodySolver::showBodyInfo(SqStepContext &)
{
    SqSolverSet *awakeSet = world->getSloverSet(sq_awakeSet);
    int awakeBodyCount = awakeSet->bodySims.getCount();
    for (int i = 0; i < awakeBodyCount; ++i)
    {
        SqBodySim *bodySim = awakeSet->bodySims.get(i);
        // printf("show %d v:(%f %f) w:%f \n", bodySim->bodyIndex, bodySim->linearVelocity.x, bodySim->linearVelocity.y, bodySim->angularVelocity);
    }
}