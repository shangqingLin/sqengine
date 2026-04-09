#ifndef _2D_CIRCLE_H_
#define _2D_CIRCLE_H_
#include "ShapePrimitive.h"

namespace d2
{
    class Circle : public ShapePrimitive
    {
    private:
        float x, y, radius;
        void build(sqstd::Array<Vec2> &);

    public:
        Circle();
        Circle(float x, float y, float radius);
        void set(float x, float y, float radius);
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