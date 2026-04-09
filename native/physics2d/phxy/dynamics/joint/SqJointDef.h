#pragma once
#include "../../common/math/SqTransform.h"
#include <float.h>
namespace phxy
{

    class SqBody;

    enum SqJointType
    {
        sq_distanceJoint,
        sq_filterJoint,
        sq_motorJoint,
        sq_mouseJoint,
        sq_prismaticJoint,
        sq_revoluteJoint,
        sq_weldJoint,
        sq_wheelJoint,
        sq_revoluteFixedAngleJoint,
        sq_jointTypeCount
    };

    struct SqBaseJointDef
    {
        /// User data pointer
        void *userData{nullptr};

        /// The first attached body
        int bodyAId = -1;

        /// The second attached body
        int bodyBId = -1;

        /// The first local joint frame
        SqTransform localFrameA;

        /// The second local joint frame
        SqTransform localFrameB;

        /// Force threshold for joint events
        float forceThreshold{FLT_MAX};

        /// Torque threshold for joint events
        float torqueThreshold{FLT_MAX};

        /**
         * 是否开启下面的constraintHertz和constraintDampingRatio约束
         * 即弹簧效果
         */
        bool enableSoftness{true};

        /// Constraint hertz (advanced feature)
        float constraintHertz{60.f};

        /// Constraint damping ratio (advanced feature)
        float constraintDampingRatio{0.f};

        /// Debug draw scale
        float drawScale{0.f};

        /**
         * 设置链接的两个刚体是否可以相互碰撞
         */
        bool collideConnected = false;
        
        ~SqBaseJointDef() = default;
    };

    struct SqRevoluteJointDef : public SqBaseJointDef
    {

        /// The target angle for the joint in radians. The spring-damper will drive
        /// to this angle.
        float targetAngle{0.f};

        /// Enable a rotational spring on the revolute hinge axis
        bool enableSpring{false};

        /// The spring stiffness Hertz, cycles per second
        float hertz{0.f};

        /// The spring damping ratio, non-dimensional
        float dampingRatio{0.f};

        /**
         * 是否限制他们之间的角度
         * 即让他们保持在一定的角度
         **/
        bool enableLimit{false};

        /**
         * lowerAngle和upperAngle范围是[-0.99 * pi ,0.99 * pi]
         * 并且要求lowerAngle必须<=upperAngle
         *
         * lowerAngle 设置他们之间保持的最小角度
         * upperAngle 设置他们之间保持的最大角度
         *
         * 即他们之间的夹角可以在[lowerAngle,upperAngle]范围内
         *
         * 如果 lowerAngle = upperAngle ，则表示两个Body固定在lowerAngle角度
         */
        float lowerAngle{0.f};
        float upperAngle{0.f};

        /**
         * BodyA和BodyB的相对速度限制在motorSpeed
         */
        bool enableMotor{false};

        /// The maximum motor torque, typically in newton-meters
        float maxMotorTorque{0.f};

        /**
         * 如果enableMotor设置为true，则表示旋转速度。
         * 指定的为弧度
         */
        float motorSpeed{0.f};
    };

    struct SqRevoluteFixedAngleJointDef : public SqBaseJointDef
    {
        float fixedAngle = 0.f;
        bool fixedEnable = false;
        float fixedAngleRange = 0.34;
    };

    struct SqDistanceJointDef : public SqBaseJointDef
    {

        /// The rest length of this joint. Clamped to a stable minimum value.
        float length{1.0f};

        /// Enable the distance constraint to behave like a spring. If false
        /// then the distance joint will be rigid, overriding the limit and motor.
        bool enableSpring{false};

        /// The spring linear stiffness Hertz, cycles per second
        float hertz{0.f};

        /// The spring linear damping ratio, non-dimensional
        float dampingRatio{0.f};

        /// Enable/disable the joint limit
        bool enableLimit{false};

        /// Minimum length. Clamped to a stable minimum value.
        float minLength{0.f};

        /// Maximum length. Must be greater than or equal to the minimum length.
        float maxLength{FLT_MAX};

        /// Enable/disable the joint motor
        bool enableMotor{false};

        /// The maximum motor force, usually in newtons
        float maxMotorForce{0.f};

        /// The desired motor speed, usually in meters per second
        float motorSpeed{0.f};
    };

    struct SqMotorJointDef : public SqBaseJointDef
    {

        /// The maximum motor force in newtons
        float maxForce{1.0f};

        /// The maximum motor torque in newton-meters
        float maxTorque{10.f};

        /// Position correction factor in the range [0,1]
        float correctionFactor{0.3f};
    };

    struct SqMouseJointDef : public SqBaseJointDef
    {

        /// Stiffness in hertz
        float hertz{4.0f};

        /// Damping ratio, non-dimensional
        float dampingRatio{1.0f};

        /**
         * -1表示没有最大的限制
         */
        /// Maximum force, typically in newtons
        float maxForce{1.0f};
    };

    /**
     * BodyB固定在BodyA上移动，BodyA的transform就是用来定义轴
     */
    struct SqPrismaticJointDef : public SqBaseJointDef
    {

        /**
         * 如果设置为true, BodyB的角度和BodyA的角度保持一致
         */
        bool enableSyncAngle = true;

        /// Enable a linear spring along the prismatic joint axis
        bool enableSpring{false};

        /// The spring stiffness Hertz, cycles per second
        float hertz{0.f};

        /// The spring damping ratio, non-dimensional
        float dampingRatio{0.f};

        /// The target translation for the joint in meters. The spring-damper will drive
        /// to this translation.
        float targetTranslation{0.f};

        /// Enable/disable the joint limit
        bool enableLimit{false};

        /// The lower translation limit
        float lowerTranslation{0.f};

        /// The upper translation limit
        float upperTranslation{0.f};

        /// Enable/disable the joint motor
        bool enableMotor{false};

        /// The maximum motor force, typically in newtons
        float maxMotorForce{0.f};

        /// The desired motor speed, typically in meters per second
        float motorSpeed{0.f};
    };

    struct SqWeldJointDef : public SqBaseJointDef
    {
        /// Linear stiffness expressed as Hertz (cycles per second). Use zero for maximum stiffness.
        float linearHertz{0.f};

        /// Angular stiffness as Hertz (cycles per second). Use zero for maximum stiffness.
        float angularHertz{0.f};

        /// Linear damping ratio, non-dimensional. Use 1 for critical damping.
        float linearDampingRatio{0.f};

        /// Linear damping ratio, non-dimensional. Use 1 for critical damping.
        float angularDampingRatio{0.f};
    };

    struct SqWheelJointDef : public SqBaseJointDef
    {

        /// Enable a linear spring along the local axis
        bool enableSpring{false};

        /// Spring stiffness in Hertz
        float hertz{1.0f};

        /// Spring damping ratio, non-dimensional
        float dampingRatio{0.7f};

        /// Enable/disable the joint linear limit
        bool enableLimit{false};

        /// The lower translation limit
        float lowerTranslation{0.f};

        /// The upper translation limit
        float upperTranslation{0.f};

        /// Enable/disable the joint rotational motor
        bool enableMotor{false};

        /// The maximum motor torque, typically in newton-meters
        float maxMotorTorque{0.f};

        /**
         * 单位是：弧度/秒
         */
        /// The desired motor speed in radians per second
        float motorSpeed{0.f};
    };

}