#ifndef _RECT_H_
#define _RECT_H_

template <typename T>
class Rect
{
public:
  T x, y, width, height;
  Rect(T x, T y, T width, T height) : x(x),
                                      y(y),
                                      width(width),
                                      height(height)
  {

  }
  
  Rect(){};
  
  bool contain(Rect<T> &rect)
  {
    return (rect.x >= x && rect.y >= y &&
          rect.x + rect.width <= x + width && rect.y + rect.height <= y + height);
  }

  void set(T x, T y, T width, T height)
  {
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
  }
};
#endif