#include "Vec3.h"


Vec3::Vec3():
x(0),y(0),z(0)
{}

void Vec3::set(float x,float y,float z){
    this->x = x;
    this->y = y;
    this->z = z;
}