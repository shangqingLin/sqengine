#pragma once

#include "Rect.h"

/**
 * AABB包围盒
 */
class Bound2{
    public:
        float minX;
        float minY;
        float maxX;
        float maxY;
        Bound2();
        Bound2(float minX,float minY,float maxX,float maxY);
        void set(float minX,float minY,float maxX,float maxY);
        void addFrame(float x0,float y0,float x1,float y1);
        void addFrame(float x0,float y0);
        void addBound(const Bound2&);
        void addRect(Rect<float>&);
        bool containsPoint(float x , float y) const;
};

