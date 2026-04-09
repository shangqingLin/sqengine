#pragma once

#include "../../../engine/2d/components/Transform2DComponent.h"
#include <unordered_map>
#include "../../phxy/sq-phxy.h"

namespace physics2d
{
    void setDefBody(phxy::SqBaseJointDef &def, Node *nodeA, Node *nodeB);
    void DeleteJointCallback(void *context);

    class Joint2DComponent : public Component
    {
    protected:
        phxy::SqBaseJointDef *tempDef = nullptr;
        phxy::SqJoint *joint = nullptr;
        Node *connectNode = nullptr;
        void setBody(Node *nodeA, Node *nodeB, phxy::SqBaseJointDef &def);
        virtual void onSwitchConnectNode() = 0;
        virtual phxy::SqBaseJointDef *onCreateJointDef() = 0;
        virtual void destroyJoint();

        friend void DeleteJointCallback(void *context);

        void onCreateJoint();

    public:
        void setConnectNode(Node *node);
        void setCollideConnected(bool collide);
        void setConnectNodeLocalAnchor(const Vec2 &);
        void setOwnerLocalAnchor(const Vec2 &);
        void setConnectNodeRotate(float rotate);
        void setOwnerNodeRotate(float rotate);
        void setConstraintHertz(float);
        void setConstraintDampingRatio(float);
        void setForceThreshold(float);
        void setEnableSim(bool);
        void setTorqueThreshold(float);
        virtual void onEnable();
        virtual ~Joint2DComponent();
    };
}