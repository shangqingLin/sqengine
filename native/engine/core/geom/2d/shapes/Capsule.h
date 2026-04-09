#ifndef _2D_CAPSULE_H_
#define _2D_CAPSULE_H_

#include "ShapePrimitive.h"

namespace d2
{
    class Capsule : public ShapePrimitive
    {
    private:
        float upperCenterX;
        float upperCenterY;
        float lowerCenterX;
        float lowerCenterY;
        float radius;
        // 胶囊体的轴面向左手边的法线
        Vec2 axisLeftNormal;
        Vec2 axisUp;
        void build(sqstd::Array<Vec2> &);

    public:
        Capsule();
        Capsule(float c1x, float c1y, float c2x, float c2y, float radius);
        void set(float c1x, float c1y, float c2x, float c2y, float radius);
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