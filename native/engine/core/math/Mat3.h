#ifndef _MAT3_H_H
#define _MAT3_H_H
#include "Vec2.h"

/**
 * [
 *  cos sin
 *  -sin cos
 *  tx ty
 * ]
 */
class Mat3
{  
public:
   float data[6];
   Mat3();
   void setTRS(const Vec2& t, const Vec2& r , const Vec2& s);
   void mul2(Mat3& lmat,const Mat3& rmat);
   void mul(const Mat3& rhs);
   void setIdentity();
   void setTranslate(float tx, float ty);
   void setScale(float x, float y);
   void translate(float x, float y);
   void scale(float x, float y);
   void rotate(float angle);
   void setRotate(float angle);
   void transformPoint(const Vec2& vec,Vec2& res) const;
   void transformVector(const Vec2& vec,Vec2& res) const;
   void invert();
   void copy(const Mat3&);
   void print() const;
   void getTranslation(Vec2&) const;
   void getTranslation(float&,float&) const;
   void getScale(Vec2& out) const;
   void getScale(float&,float&) const;
   void getRotation(float& x,float& y,bool rad = true) const;
   bool operator!=(const Mat3&) const;
};

#endif