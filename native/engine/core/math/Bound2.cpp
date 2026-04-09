#include "Bound2.h"

Bound2::Bound2() : minX(0), minY(0), maxX(0), maxY(0)
{
}

Bound2::Bound2(float minX, float minY, float maxX, float maxY)
    : minX(minX), minY(minY), maxX(maxX), maxY(maxY)
{
}

void Bound2::set(float minX, float minY, float maxX, float maxY)
{
    this->minX = minX;
    this->minY = minY;
    this->maxX = maxX;
    this->maxY = maxY;
}

void Bound2::addFrame(float x0, float y0, float x1, float y1)
{
    if (x0 < minX)
        minX = x0;
    if (y0 < minY)
        minY = y0;
    if (x1 > maxX)
        maxX = x1;
    if (y1 > maxY)
        maxY = y1;
}

void Bound2::addFrame(float x0,float y0)
{
    if (x0 < minX)
        minX = x0;
    if (y0 < minY)
        minY = y0;
    if (x0 > maxX)
        maxX = x0;
    if (y0 > maxY)
        maxY = y0;
}

void Bound2::addRect(Rect<float> &rect)
{
    this->addFrame(rect.x, rect.y, rect.x + rect.width, rect.y + rect.height);
}

void Bound2::addBound(const Bound2& b)
{
    addFrame(b.minX,b.minY,b.maxX,b.maxY);
}

bool Bound2::containsPoint(float x , float y) const
{
    return minX <= x && minY <= y && maxX >= x && maxY >= y;
}