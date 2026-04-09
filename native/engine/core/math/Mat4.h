#ifndef _MAT4_H_
#define _MAT4_H_
#include "Mat3.h"
#include "Vec3.h"
#include <string>

class Mat4
{

public:
    
   float data[16];

   Mat4();  

   void setIdentity();

   /**
    * 矩阵乘法
    * 将第一个参数的矩阵乘以第二个参数的矩阵，结果保存到当前的矩阵中
   */
   void mul2(Mat4&,Mat4&);
   void mul(Mat4&);

   /**
    * 将该矩阵设置为透视投影矩阵
    * @param fov 过视点作垂直于近平面的直线。视点到近平面的半平面夹角
    * @param aspect 近平面的宽高比
    * @param znear 视点到近平面的垂直距离
    * @param zfar 视点到远平面的垂直距离
    * @param fovIsHorizontal fov夹角是否是垂直于水平的方式
   */
   void setPerspective(float fov, float aspect, float znear,float zfar, bool fovIsHorizontal = false);

   /**
    * 通过设置透视投影的视锥体来设置透视投影矩阵 
    **/ 
   void setFrustum(float left, float right,float bottom, float top, float znear, float zfar);

   /**
    * 将该矩阵设置为正投影矩阵
   */
   void setOrtho(float left,float right,float bottom, float top, float near, float far);

   void setFromMat3(Mat3&);

   void transformPoint(Vec3& vec,Vec3& res);
   void transformVector(Vec3& vec,Vec3& res);

   /**
    * 求逆矩阵
    */
   void invert();

   void copy(Mat4&);

   void toString();
};

#endif