#pragma once
#include "../../common/math/SqTransform.h"
#include "../../common/constants-define.h"
#include <stdint.h>

namespace phxy
{
    class SqBody;

    /**
     * 保存Body上一帧的模拟的数据
     */
    struct SqPrevBodySim
    {
        /**
         * CCD要求存储上一帧的变换和质心
         * 在大多数的情况下,transform.p == center的，按照道理来说我们只需要存储一个transform而不是像下面分开存储，
         * 但实际情况是当前帧执行之后可能外部改变执行位置，这时有两种情况：
         * 1、可能外部一些调用（如果Body的updateMass、setTransform）改变了质心的位置，CCD要求必须使用最新的质心位置
         * 2、质心没有被外部改变，则就需要使用上一帧的质心
         *
         * 你可能会问：干脆Transform和center一起保存，这样做确实可以，但浪费内存没有必须要。
         * 既然没有保存上一帧的transform，又要需要使用最新的质心，那么我们需要根据rotation和center还原原来的Transform
         *
         *  center的位置世界坐标正常是这样计算：center = q * localcenter + p；
         *  现在已知q了，需要求p，则p = center - q * localcenter;
         *
         *  所以我们会看见如下代码：
         * 	SqTransform xf1;
         *   xf1.q = rotation;
         *   xf1.p = b2Sub(center, SqRot::rotateVector(rotation, localCenter ) );
         *
         */
        // SqTransform transform;
        SqRot rotation;
        SqVec2 center;
    };

    struct SqBodySim
    {

        /**
         * 当前Body的位置变换矩阵，外部通过这个得到Body的位置
         */
        SqTransform transform;

        /**
         * 一个Body上可以组合多个Shape，从而组成一个大的形状，这个就是这个大的形状的质心
         * 局部质心坐标. 是所有的Shape的平均质心坐标
         */
        SqVec2 localCenter;

        /**
         * localCenter转换到世界坐标系下的坐标
         */
        SqVec2 center;

        SqPrevBodySim prevSim;

        int bodyIndex{SQ_NULL_INDEX}; // 指向SqBody的index

        /**
         * @type {SqBodyFlags}
         */
        uint32_t flags{0};

        /**
         * 当前Body所受到的合外力
         */
        SqVec2 force;
        float invMass{0.f};
        float invInertia{0.f};
        float linearDamping{0.f};

        /**
         * 当前Body所受到的合力矩
         */
        float torque{0.f};
        float angularDamping{0.f};
        float gravityScale{1.0};

        SqVec2 linearVelocity;
        float angularVelocity{0.f};

        /**
         * 因为Body的计算可能分布到不同的阶段中
         * 不同的阶段都可能计算出一个位移和旋转，所以这里暂时存储流程中的结果
         * 在阶段的最终再同步到上面的transform中
         */
        SqVec2 deltaPosition;
        SqRot deltaRotation;

        float maxExtent{0.f};
        float minExtent{0.f};

        SqBodySim();

        void reset();
    };

}