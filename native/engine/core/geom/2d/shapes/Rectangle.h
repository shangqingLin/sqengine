#ifndef _2D_RECTANGLE_H_
#define _2D_RECTANGLE_H_
#include "./ShapePrimitive.h"

namespace d2
{
    class Rectangle : public ShapePrimitive
    {
    private:
        float x, y, w, h;
        void build(sqstd::Array<Vec2> &points);

    public:
        Rectangle();
        Rectangle(float x, float y, float w, float h);
        void set(float x, float y, float w, float h);
        virtual bool contains(float x, float y);

        virtual bool strokeContains(float x, float y, float strokeWidth);

        virtual void fill(sqstd::Array<Vec2> &, sqstd::Array<unsigned short> &);

        virtual void stroke(sqstd::Array<Vec2> &, sqstd::Array<unsigned short> &, float);

        virtual void queryFill(float x, float y, d2::QueryShapeResult &);

        virtual void queryStroke(float x, float y, d2::QueryShapeResult &, float);

        virtual void getBounds(Bound2 &out);
    };
}

#endif