#pragma once

#include "../../../math/Vec2.h"
#include "../../../math/Bound2.h"
#include "../../../sqstd/Array.h"
#include "./ShapeEnum.h"

namespace d2
{

    struct QueryShapeResult
    {

        // 如果点击是多边形的边，则记录边两个端点
        int linePoint1Index = -1;
        int linePoint2Index = -1;

        // 记录是第几个绘制的图形
        int gemoetryIndex = -1;
        int pathIndex = -1;
    };

    class ShapePrimitive
    {
    public:
        const int shapeType;

        sqstd::Array<Vec2> points;

        ShapePrimitive(int shapeType) : shapeType(shapeType) {};

        virtual ~ShapePrimitive();

        /**
         * 计算点是否在图形内
         */
        virtual bool contains(float x, float y) = 0;

        /**
         * 计算点是否在图形的边界上
         * strokeWidth 为线段的宽度
         */
        virtual bool strokeContains(float x, float y, float strokeWidth) = 0;

        virtual void fill(sqstd::Array<Vec2> &, sqstd::Array<unsigned short> &) = 0;
        virtual void stroke(sqstd::Array<Vec2> &, sqstd::Array<unsigned short> &, float) = 0;
        virtual void queryFill(float x, float y, d2::QueryShapeResult &) = 0;
        virtual void queryStroke(float x, float y, d2::QueryShapeResult &, float) = 0;
        virtual void getBounds(Bound2 &out) = 0;
        virtual inline void reset() { points.clear(); };
    };
}
