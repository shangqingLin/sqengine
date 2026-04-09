#include "SqSolver.h"
#include "../SqWorld.h"

using namespace phxy;

SqSolver::SqSolver(SqWorld *world) : world(world),
                                     bodySovler(world),
                                     TOISlover(world),
                                     jointSolver(world),
                                     contanctSlover(world)
{
}

void SqSolver::islandSplitProcess(SqStepContext &context)
{

    if (context.splitIslandId != SQ_NULL_INDEX)
    {
        SqSolverSet *awakeSet = world->getSloverSet(sq_awakeSet);
        SqIslandSim *sim = awakeSet->getIslandSim(context.splitIslandId);
        SqIsland *island = world->getIsland(sim->islandId);
        island->sleep();
        context.splitIslandId = SQ_NULL_INDEX;
        context.splitSleepTime = 0.f;
    }
}

void SqSolver::islandSleepProcess(SqStepContext &context)
{

    SqSolverSet *awakeSet = context.world->getSloverSet(sq_awakeSet);
    int count = awakeSet->getIslandSimCount();

    // printf("===== count %d \n", count);

    // for (int islandIndex = count - 1; islandIndex >= 0; islandIndex -= 1)
    // {
    //     SqIslandSim *sim = awakeSet->islandSims.get(islandIndex);
    //     SqIsland *island = context.world->getIsland(sim->islandId);
    //     printf("fuck %d %d %d %d \n", islandIndex, sim->islandId, island->simIndex, island->setIndex);
    // }

    // 这里倒序处理，因为sleep操作是将数据从awake中移到sleep set中，会修改islandSims数组的
    for (int islandIndex = count - 1; islandIndex >= 0; islandIndex -= 1)
    {
        SqIslandSim *sim = awakeSet->getIslandSim(islandIndex);

        if (sim->flag & SqIslandSimFlag::sq_notNeedToSleep)
        {
            sim->flag &= ~SqIslandSimFlag::sq_notNeedToSleep;
            continue;
        }

        SqIsland *island = context.world->getIsland(sim->islandId);

        island->sleep();
    }
    // printf("=====end\n");
}

void SqSolver::solve(SqStepContext &context)
{
    world->mergeAwakeIslands();
    SqSolverSet *awakeSet = world->getSloverSet(sq_awakeSet);
    int awakeBodyCount = awakeSet->bodySims.getCount();
    if (awakeBodyCount == 0)
    {
        return;
    }

    {
        SqArray<SqContactSim> &awakeContactSims = context.world->solverSets.get(sq_awakeSet)->contactSims;
        int contactCount = awakeContactSims.getCount();

        if (contactCount > 0)
        {
            SqContactSim **contactSims = (SqContactSim **)sqstd::StackTempArenaAllocator::getInstance()->allocateChunk(contactCount * sizeof(SqContactSim *));
            context.contacts = contactSims;
            context.contactCount = contactCount;
            for (int i = 0; i < contactCount; ++i)
            {
                contactSims[i] = awakeContactSims.get(i);
            }
        }
    }

    /**
     * 必须严格按照如下顺序求解
     */

    islandSplitProcess(context);

    /**
     * 因为为了保持稳定性，会solve context.subStepCount次的，即接下来的slove
     * 方法会被调用context.subStepCount的次数，但其实有很多的数据没必要在slove阶段计算多次，
     * 这些数据只需要计算一次，然后solve中使用即可，所以我们这里规定了一个prepare阶段预先计算一些
     * 在solve阶段使用的数据，这样就避免多次计算这种一样的数据，提升性能了。
     */
    jointSolver.prepare(context);
    contanctSlover.prepare(context);
    bodySovler.solveBodyVelocities(context);

    jointSolver.warmStart(context);
    contanctSlover.warmStart(context);

    // 必须先求解Joint，再求解contact，因为Jonit可能打破碰撞约束造成穿墙的
    for (int i = 0; i < context.subStepCount; ++i)
    {
        jointSolver.solve(context, true);
    }
    contanctSlover.solve(context, true);

    // float h = context.h;
    // float inv_h = context.inv_h;
    // context.h = h / 8.0f;
    // context.inv_h = 1.0f / context.h;

    // context.h = h / 8.0f;
    // context.inv_h = 1.0f / context.h;

    // jointSolver.warmStart(context);
    // jointSolver.solve(context);

    // contanctSlover.postSolve(context);
    bodySovler.sloveBodyPosition(context);

    /**
     * 称为Relax阶段
     * 因为上面的sloveBodyPosition会让Body的位置发生变化，
     * 从而导致约束被破坏，重新计算一下让约束更加稳定
     */
    jointSolver.solve(context, false);
    contanctSlover.solve(context, false);
    bodySovler.sloveTransform(context);
    TOISlover.solve(context);
    bodySovler.syncProxy(context);

    /**
     * 这个步骤必须在整个流程的最后执行，因为Island的Sleep逻辑会让Shape的AABB失效
     */
    if (world->enableSleep)
    {
        islandSleepProcess(context);
    }

    if (context.contacts)
    {
        sqstd::StackTempArenaAllocator::getInstance()->freeChunk(context.contacts);
    }

    // bodySovler.showBodyInfo(context);
}
