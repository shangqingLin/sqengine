#include "bezier2D.h"
#include "../../math/math.h"

void d2::casteljauBerzier2D(
    float x1, float y1, // 起始点
    float x2, float y2, // 控制点1
    float x3, float y3, // 控制点2
    float x4, float y4, // 结束点
    int level,
    sqstd::Array<Vec2> &points)
{

    if (level > 10)
    {
        return;
    }

    // float L1X = x1;
    // float L1Y = y1;

    float L2X = (x1 + x2) * 0.5;
    float L2Y = (y1 + y2) * 0.5;

    float HX = (x2 + x3) * 0.5;
    float HY = (y2 + y3) * 0.5;

    float R3X = (x3 + x4) * 0.5;
    float R3Y = (y3 + y4) * 0.5;

    float L3X = (L2X + HX) * 0.5;
    float L3Y = (L2Y + HY) * 0.5;

    // 计算控制点(x2,y2)和(x3,y3)到直线有端点(x1,y1)和(x4,y4)组成的直线的直线距离
    // 如果这个直线距离小于指定的大小，则就认为这个(x1,y1)和(x4,y4)这两个点离得非常非常近了
    // 则(x4,y4)就表示为曲线上的一个点了，加到结果中
    float dx = x4 - x1;
    float dy = y4 - y1;
    float d2 = abs((x2 - x4) * dy - (y2 - y4) * dx);
    float d3 = abs((x3 - x4) * dy - (y3 - y4) * dx);

    if ((d2 + d3) * (d2 + d3) < 0.25 * (dx * dx + dy * dy))
    {
        points.push(Vec2(x4, y4));
        return;
    }

    float R2X = (R3X + HX) * 0.5;
    float R2Y = (R3Y + HY) * 0.5;

    float L4X = (L3X + R2X) * 0.5;
    float L4Y = (L3Y + R2Y) * 0.5;

    // R1X = L4X;
    // R1Y = L4Y;
    // R4X = x4;
    // R4Y = y4;
    // 计算左半边
    d2::casteljauBerzier2D(x1, y1, L2X, L2Y, L3X, L3Y, L4X, L4Y, level + 1, points);

    // 计算右半边
    d2::casteljauBerzier2D(L4X, L4Y, R2X, R2Y, R3X, R3Y, x4, y4, level + 1, points);
}

Vec2 d2::cubicBezier(float t, const Vec2 &p0, const Vec2 &p1, const Vec2 &p2, const Vec2 &p3)
{
    float one_minus_t = 1.0 - t;
    float one_minus_t2 = one_minus_t * one_minus_t;
    float t2 = t * t;

    return p0 * one_minus_t2 * one_minus_t +
           p1 * 3.0 * t * one_minus_t2 +
           p2 * 3.0 * t2 * one_minus_t +
           p3 * t2 * t;
}

Vec2 d2::GenerateCubicBezier(float percent, const Vec2 &p0, const Vec2 &p1, float controllerOffsetAngleLeft, float controllerOffsetAngleRight)
{

    if (percent <= 0.f)
        return p0;
    if (percent >= 1.f)
        return p1;

    // x值只需要在水平方向上进行插值就可以了，y值是通过贝塞尔曲线计算出来的
    // 因为曲线的起伏高低由y值决定
    float d = Math::abs(p1.x - p0.x);

    // 下面生成三次贝塞尔曲线的控制点

    // 三阶贝塞尔曲线是有4个点，形成三个区间，控制点偏移取平均偏移就可以了，当然你可以取任何你想要的，但这个比较合理一点
    //  float cDist = d / 3.0;
    float cxDist = d * 0.333333;

    // 角度转弧度
    // Math::PI / 180 = 0.017453292519943295

    // 以起点为原点，controllerOffsetAngleLeft为控制点与起点连线的夹角
    Vec2 cp0(p0.x + cxDist, 0.0f);
    cp0.y = p0.y - cxDist * tan(Math::angleToRadian(controllerOffsetAngleLeft)); // tangent right

    /// 以终点为原点，controllerOffsetAngleRight为控制点与起点连线的夹角
    Vec2 cp1(p1.x - cxDist, 0.0f);
    cp1.y = p1.y + cxDist * tan(Math::angleToRadian(controllerOffsetAngleRight)); // tangent left

    // printf("cp0 %f %f cp1 %f %f cxDist %f \n", cp0.x, cp0.y, cp1.x, cp1.y,cxDist);

    return d2::cubicBezier(percent, p0, cp0, cp1, p1);
}

void d2::CubicBerzierCurve::addPoint(float x, float y)
{
    if (points.size() == points.capacity())
    {
        points.resize(points.size() * 2);
    }
    points.emplace_back(x, y);
    dirty = true;
}

void d2::CubicBerzierCurve::samplerCasteljauBerzier2D(sqstd::Array<Vec2> &points) {}

Vec2 d2::CubicBerzierCurve::samplercubicBezier(float t)
{
}

void d2::CubicBerzierCurve::addGenerateCubiceControllerAngle(float angle)
{
    angles.push_back(angle);
}

Vec2 d2::CubicBerzierCurve::samplerGenerateCubicBezier(float t)
{
    SQ_ASSERT(points.size() > 1);

    if (dirty)
    {
        dirty = false;

        // 按照x值从小大小，按照y值从大到小排序
        std::sort(points.begin(), points.end(), [](const Vec2 &a, const Vec2 &b)
                  { return a.x == b.x ? a.y > b.y : a.x < b.x; });
    }

    if (t == 0)
    {
        return points[0];
    }
    else if (t >= 1.0f)
    {
        return points[points.size() - 1];
    }

    const Vec2 &firstPoint = points[0];
    const Vec2 &lastPoint = points[points.size() - 1];

    float dx = firstPoint.x + (lastPoint.x - firstPoint.x) * t;

    int beginPointIndex, endPointIndex;
    for (int i = 0; i < points.size() - 1; ++i)
    {
        if (points[i].x <= dx && dx <= points[i + 1].x)
        {
            beginPointIndex = i;
            endPointIndex = i + 1;
            break;
        }
    }

    const Vec2 &beginPoint = points[beginPointIndex];
    const Vec2 &endPoint = points[endPointIndex];

    float xxLength = Math::abs(dx - firstPoint.x);
    float xxxLength = Math::abs(firstPoint.x - lastPoint.x);

    // float t2 = dx - xxLength;
    // t2 /= xxxLength;
    float t2 = xxLength / xxxLength;

    printf("samplerGenerateCubicBezier  %f\n", t2);

    float rightAngle = angles.size() > 0 ? angles[Math::min((int)(angles.size() - 1), beginPointIndex)] : 0.f;
    float leftAngle = angles.size() > 0 ? angles[Math::min((int)(angles.size() - 1), endPointIndex)] : 0.f;
    return d2::GenerateCubicBezier(t2, beginPoint, endPoint, leftAngle, rightAngle);
}