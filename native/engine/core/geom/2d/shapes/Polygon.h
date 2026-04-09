#pragma once
#include "ShapePrimitive.h"

namespace d2
{
    class Polygon : public ShapePrimitive
    {
    public:
        bool closed = false;
        Polygon();
        virtual bool contains(float x, float y);
        virtual bool strokeContains(float x, float y, float strokeWidth); 
        virtual void fill(sqstd::Array<Vec2> &, sqstd::Array<unsigned short> &);
        virtual void stroke(sqstd::Array<Vec2> &, sqstd::Array<unsigned short> &, float);
        virtual void queryFill(float x, float y, d2::QueryShapeResult &);
        virtual void queryStroke(float x, float y, d2::QueryShapeResult &, float);
        virtual void getBounds(Bound2 &out);
        virtual void reset();
    };
}
