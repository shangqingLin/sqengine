
#include "geometry2d-common.h"
#include <stdio.h>

/**
 * 使用同向法判断一个点是否在三角形内
*/
bool geometry2d::pointInTriangle (const Vec2& vPoint1,const Vec2& vPoint2,const Vec2& vPoint3,const Vec2& aimPoint) {
    return (vPoint3.x - aimPoint.x) * (vPoint1.y - aimPoint.y) - (vPoint1.x - aimPoint.x) * (vPoint3.y - aimPoint.y) >= 0
           && (vPoint1.x - aimPoint.x) * (vPoint2.y - aimPoint.y) - (vPoint2.x - aimPoint.x) * (vPoint1.y - aimPoint.y) >= 0
           && (vPoint2.x - aimPoint.x) * (vPoint3.y - aimPoint.y) - (vPoint3.x - aimPoint.x) * (vPoint2.y - aimPoint.y) >= 0;
}

bool geometry2d::isTriangleReflex(const Vec2& a, const Vec2& b, const Vec2& c) {
    /*
     计算三角形面积并判断正负
     1、如果面积结果为正，则说明顶点顺序是顺时针的，三角形是凹的,（即“反射”三角形）
     2、如果面积结果为负，则说明顶点顺序是逆时针的，三角形是凸的。
    */
    return geometry2d::triangleArea(a,b,c) >= 0;
}

float geometry2d::triangleArea(const Vec2& a, const Vec2& b, const Vec2& c) {
    //计算叉积，二维向量的叉积就是有向面积
    return (b.y - a.y) * (c.x - b.x) - (b.x - a.x) * (c.y - b.y);
}


int geometry2d::polygonSignedArea(const sqstd::Array<Vec2*>& points){
    //如果是逆时针，则这里计算的结果为正
    //如果是顺时针，则这里的计算结果为负
    int sum = 0;
    for (int i = 0,j = points.getCount() - 1; i < points.getCount(); ++i) {
        const Vec2& point = *points[i];
        const Vec2& pointNext = *points[j];
        // sum += pointNext.y * point.x +  point.y * pointNext.x;
        sum += (pointNext.x - point.x) * (point.y + pointNext.y);
         j = i;
    }
    return sum;
}

int geometry2d::polygonSignedArea(const sqstd::Array<Vec2>& points){
    int sum = 0;
    for (int i = 0,j = points.getCount() - 1; i < points.getCount(); ++i) {
        const Vec2& point = points[i];
        const Vec2& pointNext = points[j];
        // sum += pointNext.y * point.x +  point.y * pointNext.x;
        sum += (pointNext.x - point.x) * (point.y + pointNext.y);
        j = i;
    }
    return sum;
}

bool geometry2d::polygonVertSeq(const sqstd::Array<Vec2*>& points){
    if(points.getCount() == 3){
        return isTriangleReflex(*points[0],*points[1],*points[2]);
    }
    return geometry2d::polygonSignedArea(points) < 0;
}

bool geometry2d::polygonVertSeq(const sqstd::Array<Vec2>& points){
    if(points.getCount() == 3){
        //如果是三角形，直接使用三角形判断方式
        return isTriangleReflex(points[0],points[1],points[2]);
    }
    return geometry2d::polygonSignedArea(points) < 0;
}

bool geometry2d::segmentsIntersects (const Vec2 p1, const Vec2 q1 ,const Vec2 p2, const Vec2 q2){
    if ((((Vec2&)p1).equals(q1) && ((Vec2&)p2).equals(q2))
        || (((Vec2&)p1).equals(q2) && ((Vec2&)p2).equals(q1))) {
        return true;
    }
    return geometry2d::triangleArea(p1, q1, p2) > 0 != geometry2d::triangleArea(p1, q1, q2) > 0
           && geometry2d::triangleArea(p2, q2, p1) > 0 != geometry2d::triangleArea(p2, q2, q1) > 0;
}
