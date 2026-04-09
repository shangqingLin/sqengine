#pragma once
#include <optional>
#include "./ShapePrimitive.h"
#include "./Polygon.h"
#include "../../../sqstd/sqstd.h"
#include <unordered_map>

namespace d2
{
    enum class GraphicsPathAction
    {
        NONE,
        FILL,
        STROKE
    };

    /**
     * 填充绘制的样式
     */
    struct FillStyle
    {
        std::optional<unsigned int> color;
    };

    /**
     * 线条的样式
     */
    struct StrokeStyle
    {
        std::optional<unsigned int> color;
        std::optional<float> width;
    };

    class Path
    {
    private:
        sqstd::Array<d2::ShapePrimitive *> shapePrimitives;
        sqstd::Array<d2::ShapePrimitive *> cacheShape;
        d2::Polygon *_currentPolygonShape = nullptr;
        void addPoint(float &x, float &y);
        void endShape(bool closed = false);
        ShapePrimitive *getShapeFromCache(ShapeType);

    public:
        GraphicsPathAction action = GraphicsPathAction::NONE;
        bool dirty = false;
        std::optional<StrokeStyle> strokeStyle;
        std::optional<FillStyle> fillStyle;

        ~Path();

        void moveTo(float x, float y);
        void lineTo(float x, float y);
        void updateLinePoint(int pointIndex, float x, float y);
        void insertAfterLinePoint(int insertAfterpointIndex, float x, float y);

        /**
         * @param cx 椭圆圆心
         * @param cy 椭圆圆心
         * @param rx 水平半径
         * @param ry 垂直的半径
         * 如果rx = ry 则表示绘制圆形了
         */
        void drawEllipse(float cx, float cy, float rx, float ry);

        void deleteLinePoint(int pointIndex);

        /**
         * 绘制贝塞尔曲线
         */
        void bezierCurveTo(float ctX1, float ctY1, float ctX2, float ctY2, float endX, float endY);

        void drawCircle(float cx, float cy, float r);

        void drawRect(float x, float y, float w, float h);

        void drawCapsule(float c1x, float c1y, float c2x, float c2y, float radius);

        void close();

        void fill();
        void stroke();
        void fill(FillStyle &fillStyle);
        void stroke(StrokeStyle &strokeStyle);

        void clear();
        void build(sqstd::Array<Vec2> &, sqstd::Array<unsigned short> &, float);
        void getBound(Bound2 &);
        bool containsPoint(float x, float y);
        void queryGemoetry(float localX, float localY, d2::QueryShapeResult &result);
    };

}