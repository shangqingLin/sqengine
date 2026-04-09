#include "line2D.h"
#include <stdio.h>

/**
 * 使用向量法来计算点到直线的距离
 */
float d2::squaredDistanceToLineSegment(Vec2 &point, Vec2 &segmentPoint1, Vec2 &segmentPoint2)
{
   float a = point.x - segmentPoint1.x;
   float b = point.y - segmentPoint1.y;

   float c = segmentPoint2.x - segmentPoint1.x;
   float d = segmentPoint2.y - segmentPoint1.y;

   float dot = (a * c) + (b * d);
   float lenSq = (c * c) + (d * d);
   float param = -1;

   if (lenSq != 0)
   {
      param = dot / lenSq;
   }

   float xx, yy;

   if (param < 0)
   {
      xx = segmentPoint1.x;
      yy = segmentPoint1.y;
   }
   else if (param > 1)
   {
      xx = segmentPoint2.x;
      yy = segmentPoint2.y;
   }
   else
   {
      xx = segmentPoint1.x + (param * c);
      yy = segmentPoint1.y + (param * d);
   }

   float dx = point.x - xx;
   float dy = point.y - yy;

   return (dx * dx) + (dy * dy);
}

void _createLinePoint(sqstd::Array<Vec2> &resultPoint, const Vec2 &p1, const Vec2 &p2, float lineWidth, bool reverse = false)
{
   // 将线段转换为矩形，使用矩形来表示线段

   // 线段变为向量
   Vec2 direct;
   if (reverse)
   {
      direct.set(p1.x - p2.x, p1.y - p2.y);
   }
   else
   {
      direct.set(p2.x - p1.x, p2.y - p1.y);
   }
   direct.normalize();

   // px和py表示为垂直于direct的向量
   float px = direct.y;
   float py = -direct.x;

   // 在direct向量两边延伸两个顶点，制作矩形的顶点
   px *= lineWidth;
   py *= lineWidth;

   Vec2 p;
   p.x = p1.x - px;
   p.y = p1.y - py;
   resultPoint.push(p);

   p.x = p1.x + px;
   p.y = p1.y + py;
   resultPoint.push(p);

   p.x = p2.x - px;
   p.y = p2.y - py;
   resultPoint.push(p);

   p.x = p2.x + px;
   p.y = p2.y + py;
   resultPoint.push(p);
}

void d2::buildLine(sqstd::Array<Vec2> &points, sqstd::Array<Vec2> &resultPoint, sqstd::Array<unsigned short> &triangles, float lineWidth, bool loop)
{
   int pointSize = points.getCount();
   lineWidth *= 0.5;
   unsigned short vertexOffset = resultPoint.getCount();

   // 长方形的个数
   int rectSize = 0;

   for (int c = 0; c < pointSize - 1; ++c)
   {
      Vec2 &p1 = *points.get(c);
      Vec2 &p2 = *points.get(c + 1);
      _createLinePoint(resultPoint, p1, p2, lineWidth);
   }

   if (loop)
   {
      Vec2 &p1 = *points.getLast();
      Vec2 &p2 = *points.get(0);
      _createLinePoint(resultPoint, p1, p2, lineWidth);
      rectSize = pointSize;
   }
   else
   {
      rectSize = pointSize - 1;
   }

   unsigned short beginVertex = 0;
   for (int i = 0; i < rectSize; ++i)
   {

      triangles.push(vertexOffset + beginVertex);
      triangles.push(vertexOffset + beginVertex + 1);
      triangles.push(vertexOffset + beginVertex + 3);

      triangles.push(vertexOffset + beginVertex);
      triangles.push(vertexOffset + beginVertex + 3);
      triangles.push(vertexOffset + beginVertex + 2);
      beginVertex += 4;
   }
}