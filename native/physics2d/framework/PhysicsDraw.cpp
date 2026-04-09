#include "PhysicsDraw.h"
#include "PhysicsWorld.h"
#include "PhysicsSystem.h"
#include <float.h>

#ifdef PHYSICS_DEBUG
using namespace physics2d;

void DrawPolygonFcn(const phxy::SqVec2 *vertices, int vertexCount, phxy::SqHexColor color, void *context)
{
    Color eColor;
    eColor.fromHEX(color);
    eColor.a = 0.5;
    static_cast<PhysicsDraw *>(context)->DrawPolygon(vertices, vertexCount, eColor);
}

void DrawSolidPolygonFcn(phxy::SqTransform transform, const phxy::SqVec2 *vertices, int vertexCount, float radius, phxy::SqHexColor color,
                         void *context)
{
    Color eColor;
    eColor.fromHEX(color);
    eColor.a = 0.5;
    static_cast<PhysicsDraw *>(context)->DrawSolidPolygon(transform, vertices, vertexCount, radius, eColor);
}

void DrawCircleFcn(phxy::SqVec2 center, float radius, phxy::SqHexColor color, void *context)
{
    Color eColor;
    eColor.fromHEX(color);
    eColor.a = 0.5;
    static_cast<PhysicsDraw *>(context)->DrawCircle(center, radius, eColor);
}

void DrawSolidCircleFcn(phxy::SqTransform transform, float radius, phxy::SqHexColor color, void *context)
{
    Color eColor;
    eColor.fromHEX(color);
    eColor.a = 0.5;
    static_cast<PhysicsDraw *>(context)->DrawSolidCircle(transform, phxy::SqVec2(), radius, eColor);
}

void DrawCapsuleFcn(phxy::SqVec2 p1, phxy::SqVec2 p2, float radius, phxy::SqHexColor color, void *context)
{
    Color eColor;
    eColor.fromHEX(color);
    eColor.a = 0.5;
    static_cast<PhysicsDraw *>(context)->DrawCapsule(p1, p2, radius, eColor);
}

void DrawSolidCapsuleFcn(phxy::SqVec2 p1, phxy::SqVec2 p2, float radius, phxy::SqHexColor color, void *context)
{
    Color eColor;
    eColor.fromHEX(color);
    eColor.a = 0.5;
    static_cast<PhysicsDraw *>(context)->DrawSolidCapsule(p1, p2, radius, eColor);
}

void DrawSegmentFcn(phxy::SqVec2 p1, phxy::SqVec2 p2, phxy::SqHexColor color, void *context)
{
    Color eColor;
    eColor.fromHEX(color);
    eColor.a = 0.5;
    static_cast<PhysicsDraw *>(context)->DrawSegment(p1, p2, eColor);
}

void DrawTransformFcn(phxy::SqTransform transform, void *context)
{
    static_cast<PhysicsDraw *>(context)->DrawTransform(transform);
}

void DrawPointFcn(phxy::SqVec2 p, float size, phxy::SqHexColor color, void *context)
{
    Color eColor;
    eColor.fromHEX(color);
    eColor.a = 0.5;
    static_cast<PhysicsDraw *>(context)->DrawPoint(p, size, eColor);
}

void DrawStringFcn(phxy::SqVec2 p, const char *s, phxy::SqHexColor color, void *context)
{
    static_cast<PhysicsDraw *>(context)->DrawString(p, s);
}

PhysicsDraw::PhysicsDraw(Graphics *graphics, PhysicsWorld *world) : graphics(graphics), world(world)
{

    phxy::SqAABB bounds = {{-FLT_MAX, -FLT_MAX}, {FLT_MAX, FLT_MAX}};

    m_debugDraw = {DrawPolygonFcn,
                   DrawSolidPolygonFcn,
                   DrawCircleFcn,
                   DrawSolidCircleFcn,
                   DrawCapsuleFcn,
                   DrawSegmentFcn,
                   DrawTransformFcn,
                   DrawPointFcn,
                   DrawStringFcn,
                   bounds,
                   false, // drawUsingBounds
                   true,  // shapes
                   true,  // joints
                   false, // joint extras
                   false, // aabbs
                   false, // mass
                   false, // drawBodyNames
                   false, // contacts
                   false, // colors
                   false, // normals
                   true,  // impulse
                   false, // drawContactFeatures
                   false, // friction
                   false,
                   this};
}

void PhysicsDraw::SetDrawFlag(unsigned int flag)
{
    m_debugDraw.useDrawingBounds = flag & (1 << 1);
    m_debugDraw.drawShapes = flag & (1 << 2);
    m_debugDraw.drawJoints = flag & (1 << 3);
    m_debugDraw.drawJointExtras = flag & (1 << 4);
    m_debugDraw.drawBounds = flag & (1 << 5);
    m_debugDraw.drawMass = flag & (1 << 6);
    m_debugDraw.drawContacts = flag & (1 << 7);
    m_debugDraw.drawContactNormals = flag & (1 << 8);
    m_debugDraw.drawContactImpulses = flag & (1 << 9);
    m_debugDraw.drawContactFeatures = flag & (1 << 10);
    m_debugDraw.drawFrictionImpulses = flag & (1 << 11);
    m_debugDraw.drawIslands = flag & (1 << 12);
}

void PhysicsDraw::DrawPolygon(const phxy::SqVec2 *vertices, int vertexCount, const Color &color)
{

    float units = PhysicsSystem::getInstance()->getUnitsPerMeter();
    d2::StrokeStyle style;
    style.color = Color::combineToNum(color);
    for (int i = 0; i < vertexCount; ++i)
    {
        const phxy::SqVec2 &v = vertices[i];
        if (i == 0)
        {
            graphics->moveTo(v.x * units, v.y * units);
        }
        else
        {
            graphics->lineTo(v.x * units, v.y * units);
        }
    }
    graphics->close();
    graphics->stroke(style);
}

void PhysicsDraw::DrawSolidPolygon(phxy::SqTransform &transform, const phxy::SqVec2 *vertices, int vertexCount, float radius, const Color &color)
{

    // printf("DrawSolidPolygon: %d %d\n",vertexCount,color);
    float units = PhysicsSystem::getInstance()->getUnitsPerMeter();
    d2::FillStyle fill;
    fill.color = Color::combineToNum(color);
    phxy::SqVec2 out;
    for (int i = 0; i < vertexCount; ++i)
    {
        const phxy::SqVec2 &v = vertices[i];
        phxy::SqVec2 out = transform.transformPoint(v);
        out.x *= units;
        out.y *= units;
        if (i == 0)
        {
            graphics->moveTo(out.x, out.y);
        }
        else
        {
            graphics->lineTo(out.x, out.y);
        }
    }
    graphics->close();
    graphics->fill(fill);
}

void PhysicsDraw::DrawCircle(const phxy::SqVec2 &center, float radius, const Color &color)
{
    //  printf("--------------------DrawCircle %f %f %f %d\n",center.x,center.y,radius,color);
    float units = PhysicsSystem::getInstance()->getUnitsPerMeter();
    d2::StrokeStyle style;
    style.color = Color::combineToNum(color);
    graphics->drawCircle(center.x * units, center.y * units, radius * units);
    graphics->stroke(style);
}

void PhysicsDraw::DrawSolidCircle(phxy::SqTransform &transform, const phxy::SqVec2 &center, float radius, const Color &color)
{
    //  printf("--------------------DrawSolidCircle %f %f %f\n",center.x,center.y,radius);
    float units = PhysicsSystem::getInstance()->getUnitsPerMeter();
    d2::FillStyle fill;
    fill.color = Color::combineToNum(color);
    phxy::SqVec2 out = transform.transformPoint(center);
    graphics->drawCircle(out.x * units, out.y * units, radius * units);
    graphics->fill(fill);
}

void PhysicsDraw::DrawSegment(phxy::SqVec2 p1, phxy::SqVec2 p2, const Color &color)
{
    d2::StrokeStyle style;
    style.color = Color::combineToNum(color);
    float units = PhysicsSystem::getInstance()->getUnitsPerMeter();
    p1.x *= units;
    p1.y *= units;
    p2.x *= units;
    p2.y *= units;

    if (p1.x == p2.x && p1.y == p2.y)
    {
        d2::FillStyle fill;
        fill.color = style.color;
        graphics->drawCircle(p1.x, p1.y, 0.0625 * units);
        graphics->fill(fill);
        return;
    }
    // printf("--------------------line %f %f %f %f\n",p1.x,p1.y,p2.x,p2.y);
    graphics->moveTo(p1.x, p1.y);
    graphics->lineTo(p2.x, p2.y);
    graphics->stroke(style);
}

void PhysicsDraw::DrawTransform(const phxy::SqTransform &xf)
{
}

void PhysicsDraw::DrawPoint(phxy::SqVec2 p, float size, const Color &color)
{
    float units = PhysicsSystem::getInstance()->getUnitsPerMeter();
    p.x *= units;
    p.y *= units;
    size *= units;

    d2::FillStyle fill;
    fill.color = Color::combineToNum(color);
    graphics->drawCircle(p.x, p.y, size);
    graphics->fill(fill);
}

void PhysicsDraw::DrawSolidCapsule(phxy::SqVec2 p1, phxy::SqVec2 p2, float radius, const Color &color)
{
    float units = PhysicsSystem::getInstance()->getUnitsPerMeter();
    p1.x *= units;
    p1.y *= units;
    p2.x *= units;
    p2.y *= units;
    radius *= units;

    d2::StrokeStyle fill;
    fill.color = Color::combineToNum(color);
    graphics->drawCapsule(p1.x, p1.y, p2.x, p2.y, radius);
    // graphics->fill(fill);
    graphics->stroke(fill);

    // printf("DrawSolidCapsule %f %f %f %f %f\n",p1.x,p1.y,p2.x, p2.y, radius);
}

void PhysicsDraw::DrawCapsule(phxy::SqVec2 p1, phxy::SqVec2 p2, float radius, const Color &color)
{
    float units = PhysicsSystem::getInstance()->getUnitsPerMeter();
    p1.x *= units;
    p1.y *= units;
    p2.x *= units;
    p2.y *= units;
    radius *= units;
    d2::StrokeStyle fill;
    fill.color = Color::combineToNum(color);
    graphics->drawCapsule(p1.x, p1.y, p2.x, p2.y, radius);
    graphics->stroke(fill);
}

void PhysicsDraw::DrawString(const phxy::SqVec2 &p, const char *s)
{
}
#endif