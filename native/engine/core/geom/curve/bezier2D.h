#pragma once
#include "../../sqstd/Array.h"
#include "../../math/Vec2.h"
#include <vector>

namespace d2
{
    /**
     * De Casteljau算法计算三阶贝塞尔曲线的点
     * 这种算法不需要指定进度t，就可以生成整条贝塞尔曲线上的点
     */
    void casteljauBerzier2D(
        float x1, float y1, // 起点
        float x2, float y2, // 第一个控制点
        float x3, float y3, // 第二个控制点
        float x4, float y4, // 终点
        int level,
        sqstd::Array<Vec2> &points);

    /**
     * 计算三阶贝塞尔曲线的点
     * 这里的算法需要指定进度t，取样贝塞尔曲线上的一个点
     * @param t 进度
     * @param p0 曲线起点
     * @param p1 第一个控制点
     * @param p2 第二个控制点
     * @param p3 终点
     */
    Vec2 cubicBezier(float t, const Vec2 &p0, const Vec2 &p1, const Vec2 &p2, const Vec2 &p3);

    /**
     * 上面两个函数都需要指定控制点，为了方便，这里提供了一个函数只需要指定起点p0和终点p1
     * 之后这个函数内部自动计算控制点，生成三阶贝塞尔曲线
     * 然后取t进度处的点坐标
     */
    Vec2 GenerateCubicBezier(float t, const Vec2 &p0, const Vec2 &p1,float controllerOffsetAngleLeft,float controllerOffsetAngleRight);
    

    class CubicBerzierCurve
    {
        private:
            std::vector<float> angles;
            std::vector<Vec2> points;
            bool dirty = false;
        public:
           
            void addPoint(float x,float y);
            inline int getPointCount() { return points.size(); };

            void samplerCasteljauBerzier2D(sqstd::Array<Vec2> &points);
            Vec2 samplercubicBezier(float t);

            void addGenerateCubiceControllerAngle(float angle);
            Vec2 samplerGenerateCubicBezier(float t);
    };
}
