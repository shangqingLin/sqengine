#pragma once

#include "../../../engine/framework/component/Component.h"
#include "../../../engine/core/core.h"
#include <unordered_map>
#include "../../phxy/sq-phxy.h"

/**
 * 一个特殊的Joint组件
 * 为了更加灵活使用Joint，这个组件你可以创建任何的类型的Joint
 * 并且可以设置连接您想要的任何节点
 */

namespace physics2d
{

    enum class JointType
    {
        DISTANCE,
        MOTOR,
        MOUSE,
        WELD,
        WHEEL,
        PRISMATIC,
        REVOLUTE
    };
    SQ_ENUM_CONVERSION_OPERATOR(JointType)

    class JointConnect2DComponent : public Component
    {
    private:
        std::unordered_map<sqstd::hash_t, phxy::SqJoint *> jointMap;
        sqstd::hash_t getHash(JointType,Node *, Node *);

    public:
        void create(JointType, Node *, Node *, phxy::SqBaseJointDef &def);
        phxy::SqJoint* findJoint(JointType, Node *, Node *);
        void remove(JointType,Node *, Node *);
        void removeAll();
        void setEnableSim(bool);
        virtual ~JointConnect2DComponent();
    };
}