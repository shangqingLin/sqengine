#pragma once
#include "../../engine/2d/components/graphics/Graphics.h"
#include "../../engine/core/math/Color.h"
#include "../phxy/sq-phxy.h"

#ifdef PHYSICS_DEBUG
namespace physics2d
{
    class PhysicsWorld;
    class PhysicsDraw
    {
    private:
        Graphics *graphics;
        phxy::SqDebugDraw m_debugDraw;
        PhysicsWorld *world;

    public:
        PhysicsDraw(Graphics *graphics, PhysicsWorld *world);
        inline phxy::SqDebugDraw &getDraw() { return m_debugDraw; };
        void DrawPolygon(const phxy::SqVec2 *vertices, int vertexCount, const Color &color);
        void DrawSolidPolygon(phxy::SqTransform &transform, const phxy::SqVec2 *vertices, int vertexCount, float radius, const Color &color);
        void DrawCircle(const phxy::SqVec2 &center, float radius, const Color &color);
        void DrawSolidCircle(phxy::SqTransform &transform, const phxy::SqVec2 &center, float radius, const Color &color);
        void DrawCapsule(phxy::SqVec2 p1, phxy::SqVec2 p2, float radius, const Color &color);
        void DrawSolidCapsule(phxy::SqVec2 p1, phxy::SqVec2 p2, float radius, const Color &color);
        void DrawSegment(phxy::SqVec2 p1, phxy::SqVec2 p2, const Color &color);
        void DrawTransform(const phxy::SqTransform &xf);
        void DrawPoint(phxy::SqVec2 p, float size, const Color &color);
        void DrawString(const phxy::SqVec2 &p, const char *s);
        void SetDrawFlag(unsigned int flag);
    };
}
#endif
