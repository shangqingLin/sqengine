#pragma once
#include "../../../math/Vec2.h"
#include "../../../sqstd/Array.h"

namespace d2
{

    /**
     * 计算点到线段的距离
     */
    float squaredDistanceToLineSegment(Vec2 &point, Vec2 &segmentPoint1, Vec2 &segmentPoint2);

    /**
     * 线段是通过一组点来指定的，但同一组点是无法渲染线段的（线段有粗细、厚度），
     * 所以这个函数用来以这些点为中心构建一个可以渲染的Mesh点，即生成一个一个小的矩形拼接起来，从而渲染线段
     * @param points 线段的点
     * @param resultPoint 生成的顶点结果
     * @param index 生成的绘制索引
     * @param lineWidth 设置线段的粗细
     * @param loop 线段是否是首尾链接的
     */
    void buildLine(sqstd::Array<Vec2> &points, sqstd::Array<Vec2> &resultPoint, sqstd::Array<unsigned short> &index, float lineWidth, bool loop);
}
