#pragma once
#include "../common/collection/SqArray.h"
#include "./sim/SqBodySim.h"
#include "./sim/SqContactSim.h"
#include "./sim/SqJointSim.h"
#include "./sim/SqIslandSim.h"

namespace phxy
{

    class SqSolverSet
    {
    private:
        SqArray<SqIslandSim> islandSims;

    public:
        int setIndex{SQ_NULL_INDEX};
        SqWorld *world{nullptr};
        SqArray<SqBodySim> bodySims;
        SqArray<SqContactSim> contactSims;
        SqArray<SqJointSim> jointSims;

        ~SqSolverSet();

        void removeContact(int);
        SqContactSim *moveToContact(SqContactSim *);

        void removeBody(int);
        SqBodySim *moveToBody(SqBodySim *);

        void removeJoint(int);
        SqJointSim *moveToJoint(SqJointSim *);

        void removeIsland(int);
        SqIslandSim *moveToIsland(SqIslandSim *);
        inline SqIslandSim *getIslandSim(int index) { return islandSims.get(index); };
        int getIslandSimCount() { return islandSims.getCount(); };
        int addIslandSim();

        void shrink();
        void copyTo(SqSolverSet *);
        void transferBody(SqSolverSet *targetSet, SqBody *body);
        void transferJoint(SqSolverSet *targetSet, SqJoint *);
    };
}