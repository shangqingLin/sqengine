#ifndef _GEMOTERY2D_COMMON_H_
#define _GEMOTERY2D_COMMON_H_
#include "../../../math/Vec2.h"
#include "../../../sqstd/Array.h"

namespace geometry2d{
    /**
     * 判断一个三角形是否是反射三角形,如果是反射三角形，则顶点的顺序为顺时针，并且为凹三角形
     * 
     * 
     * 下面形状为非反射三角形,abc为逆时针
     *    a
     *   /|
     *  / |
     *  b |     
     *  \ |   
     *   \|
     *    c
     * 
     * 下面是反射三角形,abc为顺时针
     *    a
     *   |\  
     *   | \   
     *   | / b
     *   |/  
     *    c
     *   
     **/ 
    bool isTriangleReflex(const Vec2& a, const Vec2& b, const Vec2& c);

    float triangleArea(const Vec2& a, const Vec2& b, const Vec2& c);

    /**
     * 获取多边形顶点的顺序
     * 返回true为顺时针
     * 返回false为逆时针
    */
    bool polygonVertSeq(const sqstd::Array<Vec2*>& points);
    bool polygonVertSeq(const sqstd::Array<Vec2>& points);

    /**
     * 计算多边形面积，有符号的
    */
    int  polygonSignedArea(const sqstd::Array<Vec2*>& points);
    int  polygonSignedArea(const sqstd::Array<Vec2>& points);

    /**
     * 判断一个点是否在三角形内
    */
    bool pointInTriangle (const Vec2& vPoint1,const Vec2& vPoint2,const Vec2& vPoint3,const Vec2& aimPoint);

    /**
     * 判断两条线段是否相交
     * 参数自定线段的端点
    */
    bool segmentsIntersects (const Vec2 p1, const Vec2 q1 ,const Vec2 p2, const Vec2 q2);

    
}
#endif