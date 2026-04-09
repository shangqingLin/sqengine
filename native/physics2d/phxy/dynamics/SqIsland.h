#pragma once
#include "./sim/SqIslandSim.h"

namespace phxy
{
    class SqContact;
    class SqWorld;
    class SqJoint;
    class SqBody;
    class SqIsland
    {
    private:
        // 在SqWorld数组中的index作为ID
        int islandId;
        int parentIsland;

        // 存储Island中的碰撞点
        int headContact;

        // 位于链表最尾端的Contact
        // 如果只有一个Contact，则tailContact=headContact
        int tailContact;
        int contactCount;

        //存储Island中的Joint
        int headJoint;
        int tailJoint;
        int jointCount;

        //用于标记当前Island中移除了多少个约束，即添加了之后移除了多少个
        //约束包括：Joint和Contact。
        //使用这个变量用于判断当前Island是否需要执行某些操作
        //比如有移除约束，那么我们才执行拆分当前的Island的操作，因为约束没有啦，它们不属于同一个Island了
        int constraintRemoveCount;

        // 链表头Body。链表中第一个Body
        int headBody;

        // 链表最尾的Body
        // 如果只有一个Body，则tailBody=headBody
        int tailBody;
        
        int bodyCount;

        /**
         * SqIslandSim所在的Set
         */
        int setIndex;
        int simIndex;
        SqWorld *world;

    public:
        friend class SqWorld;
        friend class SqSolverSet;
        friend class SqBody;
        friend class SqBodySolver;
        friend class SqWorldDraw;
        friend class SqSolver;
        SqIsland();
        void reset();
        void addContact(SqContact *contact);
        void removeContact(SqContact *contact);
        void addJoint(SqJoint *joint);
        void removeJoint(SqJoint *joint);
        void addBody(SqBody *);
        void removeBody(SqBody *);
        SqIslandSim* getSim();
        SqIsland *findRootIsland();
        void mergeToParent();
        void split();
        void sleep();
    };

}