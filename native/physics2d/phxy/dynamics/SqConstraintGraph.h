#pragma once
#include "../common/constants-define.h"

namespace phxy
{

    class SqWorld;
    class SqContactSim;
    class SqContact;
    class SqJoint;
    class SqJointSim;

    class SqGraphColor
    {
    };

    class SqConstraintGraph
    {
    private:
        SqGraphColor colors[SQ_GRAPH_COLOR_COUNT];
        SqWorld *world;
    public:
        SqConstraintGraph(SqWorld *);
        // void addContactToGraph(SqContactSim *contactSim, SqContact *contact);
        // void removeContactFromGraph(int bodyIdA, int bodyIdB, int colorIndex, int localIndex);
        // SqJointSim *createJointInGraph(SqJoint *joint);
        // void addJointToGraph(SqJointSim *jointSim, SqJoint *joint);
        // void removeJointFromGraph(int bodyIdA, int bodyIdB, int colorIndex, int localIndex);
    };

}