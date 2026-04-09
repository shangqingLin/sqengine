#pragma once

class Vec2
{
public:
  float x;
  float y;
  Vec2();
  Vec2(float);
  Vec2(float x, float y);
  Vec2(const Vec2 &);
  void set(float x, float y);
  bool equals(const Vec2 &);
  float len() const;
  float lengthSqr() const;
  float normalize();
  void copy(const Vec2 &vec);
  float cross(const Vec2 &rhs);
  float dot(const Vec2 &rhs);
  void mul2(const Vec2 &lhs, const Vec2 &rhs);
  void scale(float scalar);
  void mul(const Vec2 &rhs);
  void mul(float s);
  void sub(const Vec2 &rhs);
  void sub2(const Vec2 &lhs, const Vec2 &rhs);
  void add2(const Vec2 &lhs, const Vec2 &rhs);
  void add(const Vec2 &rhs);
  void skew();
  static float length(const Vec2 &v1, const Vec2 &v2);
  static float dot(const Vec2 &v1, const Vec2 &v2);
  static float lengthSqr(const Vec2 &v);
  static float normalize(const Vec2 &a, Vec2 &out);
  static float cross(const Vec2 &v1, const Vec2 &v2);
  static void lerp(const Vec2 &v1, const Vec2 &v2, float alpha, Vec2 &out);
  static void abs(Vec2 &v1);
  static void max(Vec2 &v1, float v);
  static void leftPerp(const Vec2 &v, Vec2 &out);
  static void rightPerp(const Vec2 &v, Vec2 &out);
  Vec2 operator*(const Vec2 &v1) const;
  Vec2 operator*(float) const;
  Vec2 &operator*=(float);
  Vec2 operator+(const Vec2 &v1) const;
  Vec2 operator+(float) const;
  Vec2 &operator+=(float);
  Vec2 &operator+=(const Vec2 &);
  Vec2 operator-(const Vec2 &v1) const;
  bool operator==(const Vec2 &) const;
  bool operator!=(const Vec2 &) const;
  Vec2 operator/(float v) const;
};
