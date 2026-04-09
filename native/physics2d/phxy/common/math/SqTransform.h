#pragma once
#include "SqRot.h"
#include "SqVec2.h"

namespace phxy
{
    class SqTransform
    {
    public:
        SqVec2 p;
        SqRot q;

        SqTransform();
        SqTransform(SqVec2,SqRot);
        void identity();
        SqVec2 transformPoint(const SqVec2 &p) const;
        SqVec2 transformVector(const SqVec2 &v) const;
        SqVec2 invTransformVector(const SqVec2 &v) const;
        SqVec2 invTransformPoint(const SqVec2 &v) const;

        static SqVec2 transformPoint(const SqTransform &A,const SqVec2 &p);
        static SqVec2 transformVector(const SqTransform &A,const SqVec2 &v);
        static SqTransform InvMulTransforms(const SqTransform &A, const SqTransform &B);
        static SqTransform Mul( const SqTransform& A,const SqTransform& B );
    };

}