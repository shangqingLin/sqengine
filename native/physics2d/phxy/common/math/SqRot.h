#pragma once
#include "SqVec2.h"

namespace phxy
{
    class SqRot
    {
    public:
        /**
         * c 为 cos之后的值
         * s 为 sin之后的值
         * 
         * 构建矩阵：
         * [qc -qs]
         * [qs  qc] 
         * 
         */
        float c, s;
        SqRot();
        SqRot(float radius);
        SqRot(float, float);

        void setRadius(float);
        void identity();

        /**
         * 归一化旋转
         * 构建单位矩阵时需要对旋转的地方也要归一化
         */
        static SqRot Normalize(const SqRot &q);
        static bool isValid(const SqRot &q);
        static bool isNormalized(const SqRot &q);
        static SqRot InvMulRot(const SqRot &a, const SqRot &b);
        static SqRot Mul(const SqRot &q, const SqRot &r);
        static SqRot IntegrateRotation(const SqRot &q1, float deltaAngle);
        static SqVec2 transformVector(const SqRot &q, const SqVec2 &v);
        static SqVec2 invRotateVector(const SqRot &q,const SqVec2 &v);
        static SqRot NLerp(const SqRot &q1, const SqRot &q2, float t);
        static float RelativeAngle(const SqRot &a, const SqRot &b);
        static float GetAngle(const SqRot &q);
    };

    static const SqRot SqRot_identity = {1.0f, 0.0f};
}