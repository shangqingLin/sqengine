#pragma once
#include "../common/math/SqVec2.h"
#include "../common/math/SqRot.h"

namespace phxy
{
    class SqBody;
    enum SqBodyType
    {
        sq_staticBody = 0,
        sq_kinematicBody = 1,
        sq_dynamicBody = 2,
        sq_bodyTypeCount,
    };

    typedef struct SqBodyMotionLocks
    {
        bool linearX{false};
        bool linearY{false};
        bool angularZ{false};
    } SqBodyMotionLocks;

    struct SqMassData
    {
        // 物体质量
        float mass{0.f};

        // Local Center质心坐标
        SqVec2 center;

        // 转动惯性
        float rotationalInertia{0.f};
    };

    enum SqBodyFlags
    {
        // This body has fixed rotation
        sq_lockAngularZ = 0x00000001,

        // This body has fixed translation along the x-axis
        sq_lockLinearX = 0x00000002,

        // This body has fixed translation along the y-axis
        sq_lockLinearY = 0x00000004,

        // All lock flags
        sq_allLocks = sq_lockAngularZ | sq_lockLinearX | sq_lockLinearY,

        // This flag is used for debug draw
        sq_isFast = 0x00000008,

        // This dynamic body does a final CCD pass against all body types, but not other bullets
        sq_isBullet = 0x00000010,

        // This body has hit the maximum linear or angular velocity
        sq_isSpeedCapped = 0x00000040,

        // This body has no limit on angular velocity
        sq_allowFastRotation = 0x00000080,

        // This body need's to have its AABB increased
        sq_enlargeBounds = 0x00000100,

        // 需要执行TOI运算
        sq_needTOI = 0x200, // 1 << 9

        sq_enableContinuous = 1 << 10,

        sq_enableSleep = 1 << 11,

        sq_enableContactBeginEvent = 1 << 13,
        sq_enableContactHitEvent = 1 << 14,
        sq_enableContactEndEvent = 1 << 15,
        sq_enableSensorBeginEvent = 1 << 16,
        sq_enableSensorEndEvent = 1 << 17,
        sq_enableParticlePressure = 1 << 18,
        sq_enableParticleDamping = 1 << 19,
        sq_node_transform_pos_rot = 1 << 20,
        sq_node_transform_scale = 1 << 21,
        sq_node_transform = sq_node_transform_pos_rot | sq_node_transform_scale
    };

    struct SqBodyDef
    {
        SqBodyType type{SqBodyType::sq_staticBody};
        SqVec2 position;
        SqRot rotation;
        SqVec2 linearVelocity;
        float angularVelocity{0.f};
        float linearDamping{0.f};
        float angularDamping{0.f};
        float gravityScale{1.0f};
        float sleepThreshold;
        const char *name;
        void *userData{nullptr};

        bool lockLinearX{false};
        bool lockLinearY{false};
        bool loclAngularZ{false};

        bool enableSleep{true};
        bool isAwake{true};
        bool isBullet{false};
        bool isEnabled{true};
        bool allowFastRotation{true};
        bool enableContinuous{true};

        bool enableParticlePressure{true};
        bool enableParticleDamping{true};
    };
}