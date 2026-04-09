#include "SqWorldDraw.h"
#include "SqWorld.h"
#include "../common/math/SqMath.h"
#include <stdio.h>
#include "../geometry/shape/SqCapsuleShape.h"
#include "../geometry/shape/SqCircleShape.h"
#include "../geometry/shape/SqPolygonShape.h"
#include "../geometry/shape/SqEdgeShape.h"
#include "../geometry/shape/SqChainEdgeShape.h"
#include "../geometry/shape/SqChainShape.h"
#include "./joint/SqDistanceJoint.h"
#include "./joint/SqPrismaticJoint.h"
#include "./joint/SqRevoluteJoint.h"
#include "./joint/SqWeldJoint.h"
#include "./joint/SqWheelJoint.h"
#include "./joint/SqMouseJoint.h"
#include "./joint/SqWheelJoint.h"

using namespace phxy;

void drawDistanceJoint(SqDebugDraw *draw, SqDistanceJoint *joint, SqTransform transformA, SqTransform transformB)
{
    SqJointSim *base = joint->getJointSim();
    SqVec2 pA = transformA.transformPoint(base->localFrameA.p);
    SqVec2 pB = transformB.transformPoint(transformB, base->localFrameB.p);

    SqVec2 axis = SqVec2::Normalize(SqVec2::Sub(pB, pA));

    float minLength = joint->GetMinLength();
    float maxLength = joint->GetMaxLength();

    if (minLength < maxLength && joint->IsLimitEnabled())
    {
        SqVec2 pMin = SqVec2::MulAdd(pA, joint->GetMinLength(), axis);
        SqVec2 pMax = SqVec2::MulAdd(pA, joint->GetMaxLength(), axis);
        SqVec2 offset = SqVec2::MulSV(0.05f, SqVec2::RightPerp(axis));

        if (minLength > SQ_LINEAR_SLOP)
        {
            // draw->DrawPoint(pMin, 4.0f, c2, draw->context);
            draw->DrawSegmentFcn(SqVec2::Sub(pMin, offset), SqVec2::Add(pMin, offset), sq_colorLightGreen, draw->context);
        }

        if (maxLength < SQ_HUGE)
        {
            // draw->DrawPoint(pMax, 4.0f, c3, draw->context);
            draw->DrawSegmentFcn(SqVec2::Sub(pMax, offset), SqVec2::Add(pMax, offset), sq_colorRed, draw->context);
        }

        if (minLength > SQ_LINEAR_SLOP && maxLength < SQ_HUGE)
        {
            draw->DrawSegmentFcn(pMin, pMax, sq_colorGray, draw->context);
        }
    }

    draw->DrawSegmentFcn(pA, pB, sq_colorWhite, draw->context);
    draw->DrawPointFcn(pA, 0.08f, sq_colorWhite, draw->context);
    draw->DrawPointFcn(pB, 0.08f, sq_colorWhite, draw->context);

    if (joint->GetSpringHertz() > 0.0f && joint->IsSpringEnabled())
    {
        SqVec2 pRest = SqVec2::MulAdd(pA, joint->GetLength(), axis);
        draw->DrawPointFcn(pRest, 0.08f, sq_colorBlue, draw->context);
    }
}

void drawPrismaticJoint(SqDebugDraw *draw, SqPrismaticJoint *joint, SqTransform transformA, SqTransform transformB, float drawSize)
{

    SqJointSim *base = joint->getJointSim();
    SqTransform frameA = SqTransform::Mul(transformA, base->localFrameA);
    SqTransform frameB = SqTransform::Mul(transformB, base->localFrameB);
    SqVec2 axisA = SqRot::transformVector(frameA.q, SqVec2(1.0f, 0.0f));

    draw->DrawSegmentFcn(frameA.p, frameB.p, sq_colorDimGray, draw->context);

    if (joint->IsLimitEnabled())
    {
        float b = 0.25f * drawSize;
        SqVec2 lower = SqVec2::MulAdd(frameA.p, joint->GetLowerLimit(), axisA);
        SqVec2 upper = SqVec2::MulAdd(frameA.p, joint->GetUpperLimit(), axisA);
        SqVec2 perp = SqVec2::LeftPerp(axisA);
        draw->DrawSegmentFcn(lower, upper, sq_colorGray, draw->context);
        draw->DrawSegmentFcn(SqVec2::MulSub(lower, b, perp), SqVec2::MulAdd(lower, b, perp), sq_colorGreen, draw->context);
        draw->DrawSegmentFcn(SqVec2::MulSub(upper, b, perp), SqVec2::MulAdd(upper, b, perp), sq_colorRed, draw->context);
    }
    else
    {
        draw->DrawSegmentFcn(SqVec2::MulSub(frameA.p, 1.0f, axisA), SqVec2::MulAdd(frameA.p, 1.0f, axisA), sq_colorGray, draw->context);
    }

    if (joint->IsSpringEnabled())
    {
        SqVec2 p = SqVec2::MulAdd(frameA.p, joint->GetTargetTranslation(), axisA);
        draw->DrawPointFcn(p, 0.08f, sq_colorViolet, draw->context);
    }

    draw->DrawPointFcn(frameA.p, 0.1f, sq_colorGray, draw->context);
    draw->DrawPointFcn(frameB.p, 0.1f, sq_colorBlue, draw->context);
}

void drawWeldJoint(SqDebugDraw *draw, SqWeldJoint *joint, SqTransform transformA, SqTransform transformB, float drawSize)
{

    SqJointSim *base = joint->getJointSim();
    SqTransform frameA = SqTransform::Mul(transformA, base->localFrameA);
    SqTransform frameB = SqTransform::Mul(transformB, base->localFrameB);

    SqPolygonShape box;
    box.setAsBox(0.25f * drawSize, 0.125f * drawSize, SqVec2(), SqRot());

    SqVec2 points[4];

    for (int i = 0; i < 4; ++i)
    {
        points[i] = frameA.transformPoint(box.vertices[i]);
    }
    draw->DrawPolygonFcn(points, 4, sq_colorDarkOrange, draw->context);

    for (int i = 0; i < 4; ++i)
    {
        points[i] = frameB.transformPoint(box.vertices[i]);
    }

    draw->DrawPolygonFcn(points, 4, sq_colorDarkCyan, draw->context);
}

void drawWheelJoint(SqDebugDraw *draw, SqWheelJoint *joint, SqTransform transformA, SqTransform transformB)
{
    SqJointSim *base = joint->getJointSim();
    SqTransform frameA = SqTransform::Mul(transformA, base->localFrameA);
    SqTransform frameB = SqTransform::Mul(transformB, base->localFrameB);
    SqVec2 axisA = SqRot::transformVector(frameA.q, SqVec2(1.0f, 0.0f));

    SqHexColor c1 = sq_colorGray;
    SqHexColor c2 = sq_colorGreen;
    SqHexColor c3 = sq_colorRed;
    SqHexColor c4 = sq_colorDimGray;
    SqHexColor c5 = sq_colorBlue;

    draw->DrawSegmentFcn(frameA.p, frameB.p, c5, draw->context);

    if (joint->IsLimitEnabled())
    {
        SqVec2 lower = SqVec2::MulAdd(frameA.p, joint->GetLowerLimit(), axisA);
        SqVec2 upper = SqVec2::MulAdd(frameA.p, joint->GetUpperLimit(), axisA);
        SqVec2 perp = SqVec2::LeftPerp(axisA);
        draw->DrawSegmentFcn(lower, upper, c1, draw->context);
        draw->DrawSegmentFcn(SqVec2::MulSub(lower, 0.1f, perp), SqVec2::MulAdd(lower, 0.1f, perp), c2, draw->context);
        draw->DrawSegmentFcn(SqVec2::MulSub(upper, 0.1f, perp), SqVec2::MulAdd(upper, 0.1f, perp), c3, draw->context);
    }
    else
    {
        draw->DrawSegmentFcn(SqVec2::MulSub(frameA.p, 1.0f, axisA), SqVec2::MulAdd(frameA.p, 1.0f, axisA), c1, draw->context);
    }

    draw->DrawPointFcn(frameA.p, 0.08f, c1, draw->context);
    draw->DrawPointFcn(frameB.p, 0.08f, c4, draw->context);
}

void drawRevoluteJoint(SqDebugDraw *draw, SqRevoluteJoint *joint, SqTransform transformA, SqTransform transformB, float drawSize)
{

    SqJointSim *base = joint->getJointSim();

    SqTransform frameA = SqTransform::Mul(transformA, base->localFrameA);
    SqTransform frameB = SqTransform::Mul(transformB, base->localFrameB);

    const float radius = 0.25f * drawSize;
    draw->DrawCircleFcn(frameB.p, radius, sq_colorGray, draw->context);

    SqVec2 rx = {radius, 0.0f};
    SqVec2 r = SqRot::transformVector(frameA.q, rx);
    draw->DrawSegmentFcn(frameA.p, SqVec2::Add(frameA.p, r), sq_colorGray, draw->context);

    r = SqRot::transformVector(frameB.q, rx);
    draw->DrawSegmentFcn(frameB.p, SqVec2::Add(frameB.p, r), sq_colorBlue, draw->context);

    if (draw->drawJointExtras)
    {
        float jointAngle = SqRot::RelativeAngle(frameA.q, frameB.q);
        char buffer[32];
        snprintf(buffer, 32, " %.1f deg", 180.0f * jointAngle / SQ_PI);
        draw->DrawStringFcn(SqVec2::Add(frameA.p, r), buffer, sq_colorWhite, draw->context);
    }

    float lowerAngle = joint->getLowerLimit();
    float upperAngle = joint->getUpperLimit();

    if (joint->isLimitEnabled())
    {
        SqRot rotLo = SqRot::Mul(frameA.q, SqRot(lowerAngle));
        SqVec2 rlo = SqRot::transformVector(rotLo, rx);

        SqRot rotHi = SqRot::Mul(frameA.q, SqRot(upperAngle));
        SqVec2 rhi = SqRot::transformVector(rotHi, rx);

        draw->DrawSegmentFcn(frameB.p, SqVec2::Add(frameB.p, rlo), sq_colorGreen, draw->context);
        draw->DrawSegmentFcn(frameB.p, SqVec2::Add(frameB.p, rhi), sq_colorRed, draw->context);
    }

    if (joint->isSpringEnabled())
    {
        SqRot q = SqRot::Mul(frameA.q, SqRot(joint->getTargetAngle()));
        SqVec2 v = SqRot::transformVector(q, rx);
        draw->DrawSegmentFcn(frameB.p, SqVec2::Add(frameB.p, v), sq_colorViolet, draw->context);
    }

    SqHexColor color = sq_colorGold;
    draw->DrawSegmentFcn(transformA.p, frameA.p, color, draw->context);
    draw->DrawSegmentFcn(frameA.p, frameB.p, color, draw->context);
    draw->DrawSegmentFcn(transformB.p, frameB.p, color, draw->context);

    // char buffer[32];
    // sprintf(buffer, "%.1f", b2Length(joint->impulse));
    // draw->DrawString(pA, buffer, draw->context);
}

void SqWorldDraw::drawJoint(SqDebugDraw *draw, SqWorld *world, SqJoint *joint)
{
    SqBody *bodyA = world->getBody(joint->edges[0].bodyId);
    SqBody *bodyB = world->getBody(joint->edges[1].bodyId);
    if (bodyA->setIndex == sq_disabledSet || bodyB->setIndex == sq_disabledSet)
    {
        return;
    }

    SqJointSim *jointSim = joint->getJointSim();

    SqTransform transformA = bodyA->getTransform();
    SqTransform transformB = bodyB->getTransform();
    SqVec2 pA = transformA.transformPoint(jointSim->localFrameA.p);
    SqVec2 pB = transformB.transformPoint(jointSim->localFrameB.p);

    SqHexColor color = sq_colorDarkSeaGreen;

    switch (joint->getType())
    {
    case sq_distanceJoint:
        drawDistanceJoint(draw, static_cast<SqDistanceJoint *>(joint), transformA, transformB);
        break;

    case sq_mouseJoint:
        draw->DrawPointFcn(pA, 0.1f, sq_colorYellowGreen, draw->context);
        draw->DrawPointFcn(pB, 0.1f, sq_colorYellowGreen, draw->context);
        draw->DrawSegmentFcn(pA, pB, sq_colorLightGray, draw->context);
        break;

    case sq_filterJoint:
        draw->DrawSegmentFcn(pA, pB, sq_colorGold, draw->context);
        break;

    case sq_prismaticJoint:
        drawPrismaticJoint(draw, static_cast<SqPrismaticJoint *>(joint), transformA, transformB, 1);
        break;

    case sq_revoluteJoint:
        drawRevoluteJoint(draw, static_cast<SqRevoluteJoint *>(joint), transformA, transformB, 1);
        break;

    case sq_weldJoint:
        drawWeldJoint(draw, static_cast<SqWeldJoint *>(joint), transformA, transformB, 1);
        break;

    case sq_wheelJoint:
        drawWheelJoint(draw, static_cast<SqWheelJoint *>(joint), transformA, transformB);
        break;

    default:
        // printf("====draw distance %f \n", SqVec2::Length( SqVec2::Sub(pA,pB)));
        draw->DrawSegmentFcn(transformA.p, pA, color, draw->context);
        draw->DrawSegmentFcn(pA, pB, color, draw->context);
        draw->DrawSegmentFcn(transformB.p, pB, color, draw->context);
        break;
    }

    // if (draw->drawGraphColors)
    // {
    //     SqHexColor colors[B2_GRAPH_COLOR_COUNT] = {sq_colorRed, sq_colorOrange, sq_colorYellow, sq_colorGreen,
    //                                                sq_colorCyan, sq_colorBlue, sq_colorViolet, sq_colorPink,
    //                                                sq_colorChocolate, sq_colorGoldenRod, sq_colorCoral, sq_colorBlack};

    //     int colorIndex = joint->colorIndex;
    //     if (colorIndex != SQ_NULL_INDEX)
    //     {
    //         SqVec2 p = SqVec2::Lerp(pA, pB, 0.5f);
    //         draw->DrawPointFcn(p, 5.0f, colors[colorIndex], draw->context);
    //     }
    // }

    // if (draw->drawJointExtras)
    // {
    //     SqVec2 force = b2GetJointConstraintForce(world, joint);
    //     float torque = b2GetJointConstraintTorque(world, joint);
    //     SqVec2 p = SqVec2::Lerp(pA, pB, 0.5f);

    //     draw->DrawSegmentFcn(p, SqVec2::MulAdd(p, 0.001f, force), sq_colorAzure, draw->context);

    //     char buffer[64];
    //     snprintf(buffer, 64, "f = [%g, %g], t = %g", force.x, force.y, torque);
    //     draw->DrawStringFcn(p, buffer, sq_colorAzure, draw->context);
    // }
}

static void drawShape2(SqDebugDraw *draw, SqShape *shape, SqTransform xf, SqHexColor color)
{
    // printf("+++++++++drawShape2\n");
    switch (shape->type)
    {
    case sq_capsuleShape:
    {
        SqCapsuleShape *capsule = static_cast<SqCapsuleShape *>(shape);
        SqVec2 p1 = SqTransform::transformPoint(xf, capsule->center1);
        SqVec2 p2 = SqTransform::transformPoint(xf, capsule->center2);
        draw->DrawSolidCapsuleFcn(p1, p2, capsule->radius, color, draw->context);
    }
    break;

    case sq_circleShape:
    {
        SqCircleShape *circle = static_cast<SqCircleShape *>(shape);
        xf.p = xf.transformPoint(circle->center);
        draw->DrawSolidCircleFcn(xf, circle->radius, color, draw->context);
    }
    break;

    case sq_polygonShape:
    case sq_boxShape:
    {
        SqPolygonShape *poly = static_cast<SqPolygonShape *>(shape);
        draw->DrawSolidPolygonFcn(xf, poly->vertices, poly->count, poly->radius, color, draw->context);
    }
    break;

    case sq_segmentShape:
    {
        SqEdgeShape *segment = static_cast<SqEdgeShape *>(shape);
        SqVec2 p1 = xf.transformPoint(segment->point1);
        SqVec2 p2 = xf.transformPoint(segment->point2);
        draw->DrawSegmentFcn(p1, p2, color, draw->context);
    }
    break;

    case sq_chainSegmentShape:
    {
        // printf("sq_chainSegmentShape transform %f %f %f %f\n",xf.p.x,xf.p.y, xf.q.c,xf.q.s);
        SqChainEdgeShape *segment = static_cast<SqChainEdgeShape *>(shape);
        SqVec2 p1 = xf.transformPoint(segment->point1);
        SqVec2 p2 = xf.transformPoint(segment->point2);
        draw->DrawSegmentFcn(p1, p2, color, draw->context);
        draw->DrawPointFcn(p2, 0.08f, color, draw->context);
        draw->DrawSegmentFcn(p1, SqVec2::Lerp(p1, p2, 0.1f), sq_colorPaleGreen, draw->context);
    }
    break;

    default:
        break;
    }
}

void SqWorldDraw::drawShape(SqDebugDraw *draw, SqShape *shape, SqTransform xf, SqHexColor color)
{
    if (shape->type == sq_chainShape)
    {
        SqChainShape *chainShape = (SqChainShape *)shape;
        for (int i = 0; i < chainShape->count; ++i)
        {
            drawShape2(draw, chainShape->segments.get(i), xf, color);
        }
    }
    else
    {
        drawShape2(draw, shape, xf, color);
    }
}

struct DrawContext
{
    SqWorld *world;
    SqDebugDraw *draw;
    SqWorldDraw *worldDraw;
    SqBitSet debugBodySet;
};

static bool DrawQueryCallback(int proxyId, void *userData, void *context)
{

    SqShape *shape = (SqShape *)shape;

    struct DrawContext *drawContext = (DrawContext *)context;
    SqWorld *world = drawContext->world;
    SqDebugDraw *draw = drawContext->draw;

    drawContext->debugBodySet.addValue(shape->getBody());

    if (draw->drawShapes)
    {
        SqBody *body = world->getBody(shape->getBody());
        SqBodySim *bodySim = body->getBodySim();

        SqHexColor color;

        if (shape->getCustomColor() != 0)
        {
            color = SqHexColor(shape->getCustomColor());
        }
        else if (body->getType() == sq_dynamicBody && body->getMass() == 0.0f)
        {
            // Bad body
            color = sq_colorRed;
        }
        else if (!body->isEnabled())
        {
            color = sq_colorSlateGray;
        }
        // else if (shape->sensorIndex != B2_NULL_INDEX)
        // {
        //     color = sq_colorWheat;
        // }
        else if ((bodySim->flags & sq_isBullet) && body->isAwake())
        {
            color = sq_colorTurquoise;
        }
        // else if (body->isSpeedCapped)
        // {
        //     color = sq_colorYellow;
        // }
        else if (bodySim->flags & sq_isFast)
        {
            color = sq_colorSalmon;
        }
        else if (body->getType() == sq_staticBody)
        {
            color = sq_colorPaleGreen;
        }
        else if (body->getType() == sq_kinematicBody)
        {
            color = sq_colorRoyalBlue;
        }
        else if (body->isAwake())
        {
            color = sq_colorPink;
        }
        else
        {
            color = sq_colorGray;
        }
        drawContext->worldDraw->drawShape(draw, shape, bodySim->transform, color);
    }

    if (draw->drawBounds)
    {
        SqAABB aabb = shape->getFatAABB();

        SqVec2 vs[4] = {{aabb.lowerBound.x, aabb.lowerBound.y},
                        {aabb.upperBound.x, aabb.lowerBound.y},
                        {aabb.upperBound.x, aabb.upperBound.y},
                        {aabb.lowerBound.x, aabb.upperBound.y}};
        draw->DrawPolygonFcn(vs, 4, sq_colorGold, draw->context);
    }

    return true;
}

/**
 * 通过指定一个AABB来查询动态树，在这个AABB范围内的所有Shape都会绘制出来
 */
void SqWorldDraw::drawWithBounds(SqWorld *world, SqDebugDraw *draw)
{
    SQ_ASSERT(SqAABB::isValid(draw->drawingBounds));

    const float k_impulseScale = 1.0f;
    const float k_axisScale = 0.3f;
    SqHexColor speculativeColor = sq_colorGainsboro;
    SqHexColor addColor = sq_colorGreen;
    SqHexColor persistColor = sq_colorBlue;
    SqHexColor normalColor = sq_colorDimGray;
    SqHexColor impulseColor = sq_colorMagenta;
    SqHexColor frictionColor = sq_colorYellow;

    // SqHexColor graphColors[B2_GRAPH_COLOR_COUNT] = {b2_colorRed, b2_colorOrange, b2_colorYellow, b2_colorGreen,
    //                                                 b2_colorCyan, b2_colorBlue, b2_colorViolet, b2_colorPink,
    //                                                 b2_colorChocolate, b2_colorGoldenRod, b2_colorCoral, b2_colorBlack};

    // int bodyCapacity = b2GetIdCapacity(&world->bodyIdPool);
    // b2SetBitCountAndClear(&world->debugBodySet, bodyCapacity);

    // int jointCapacity = b2GetIdCapacity(&world->jointIdPool);
    // b2SetBitCountAndClear(&world->debugJointSet, jointCapacity);

    // int contactCapacity = b2GetIdCapacity(&world->contactIdPool);
    // b2SetBitCountAndClear(&world->debugContactSet, contactCapacity);

    SqBitSet debugJointSet;
    debugJointSet.setAlloc(sqstd::StackTempArenaAllocator::getInstance());

    SqBitSet debugContactSet;
    debugContactSet.setAlloc(sqstd::StackTempArenaAllocator::getInstance());

    struct DrawContext drawContext = {world, draw, this};
    drawContext.debugBodySet.setAlloc(sqstd::StackTempArenaAllocator::getInstance());

    for (int i = 0; i < sq_bodyTypeCount; ++i)
    {
        world->getBroadPhase().getTree(i).Query(draw->drawingBounds, SQ_DEFAULT_MASK_BITS, &DrawQueryCallback, &drawContext);
    }

    uint32_t wordCount = drawContext.debugBodySet.getBlockCount();
    uint64_t *bits = drawContext.debugBodySet.getData();
    for (uint32_t k = 0; k < wordCount; ++k)
    {
        uint64_t word = bits[k];
        while (word != 0)
        {
            uint32_t ctz = sqCTZ64(word);
            uint32_t bodyId = 64 * k + ctz;

            SqBody *body = world->getBody(bodyId);

            // if (draw->drawBodyNames && body->name[0] != 0)
            // {
            //     SqVec2 offset = {0.1f, 0.1f};
            //     b2BodySim *bodySim = b2GetBodySim(world, body);

            //     SqDebugDraw transform = {bodySim->center, bodySim->transform.q};
            //     SqVec2 p = b2TransformPoint(transform, offset);
            //     draw->DrawStringFcn(p, body->name, b2_colorBlueViolet, draw->context);
            // }

            if (draw->drawMass && body->getType() == sq_dynamicBody)
            {
                SqVec2 offset = {0.1f, 0.1f};
                SqBodySim *bodySim = body->getBodySim();

                SqTransform transform = {bodySim->center, bodySim->transform.q};
                draw->DrawTransformFcn(transform, draw->context);

                SqVec2 p = transform.transformPoint(offset);

                char buffer[32];
                snprintf(buffer, 32, "  %.2f", body->getMass());
                draw->DrawStringFcn(p, buffer, sq_colorWhite, draw->context);
            }

            if (draw->drawJoints)
            {
                int jointKey = body->headJointKey;
                while (jointKey != SQ_NULL_INDEX)
                {
                    int jointId = jointKey >> 1;
                    int edgeIndex = jointKey & 1;
                    SqJoint *joint = world->getJoint(jointId);

                    // 避免重复绘制Joint
                    if (debugJointSet.hasValue(jointId) == false)
                    {
                        drawJoint(draw, world, joint);
                        debugJointSet.addValue(jointId);
                    }
                    else
                    {
                        // todo testing
                        edgeIndex += 0;
                    }

                    jointKey = joint->edges[edgeIndex].nextKey;
                }
            }

            const float linearSlop = SqConfig::getInstance()->getLinearSlop();
            if (draw->drawContacts && body->getType() == sq_dynamicBody && body->isAwake())
            {
                int contactKey = body->headContactKey;
                while (contactKey != SQ_NULL_INDEX)
                {
                    int contactId = contactKey >> 1;
                    int edgeIndex = contactKey & 1;
                    SqContact *contact = world->narrowPhase.getContact(contactId);
                    contactKey = contact->edges[edgeIndex].nextKey;

                    if (contact->setIndex != sq_awakeSet || contact->colorIndex == SQ_NULL_INDEX)
                    {
                        continue;
                    }

                    // 避免重复绘制Contact
                    if (debugContactSet.hasValue(contactId) == false)
                    {
                        // B2_ASSERT(0 <= contact->colorIndex && contact->colorIndex < B2_GRAPH_COLOR_COUNT);

                        // b2GraphColor *gc = world->constraintGraph.colors + contact->colorIndex;
                        // b2ContactSim *contactSim = b2ContactSimArray_Get(&gc->contactSims, contact->localIndex);
                        SqContactSim *contactSim = world->getSloverSet(contact->setIndex)->contactSims.get(contact->simIndex);
                        int pointCount = contactSim->manifold.pointCount;
                        SqVec2 normal = contactSim->manifold.normal;
                        char buffer[32];

                        for (int j = 0; j < pointCount; ++j)
                        {
                            SqManifoldPoint *point = contactSim->manifold.points + j;

                            if (draw->drawGraphColors)
                            {
                                // graph color
                                // float pointSize = contact->colorIndex == B2_OVERFLOW_INDEX ? 7.5f : 5.0f;
                                // draw->DrawPointFcn(point->point, pointSize, graphColors[contact->colorIndex], draw->context);
                                draw->DrawPointFcn(point->point, 5.0f, sq_colorBlue, draw->context);
                                // m_context->draw.DrawString(point->position, "%d", point->color);
                            }
                            else if (point->separation > linearSlop)
                            {
                                // Speculative
                                draw->DrawPointFcn(point->point, 5.0f, speculativeColor, draw->context);
                            }
                            else if (point->persisted == false)
                            {
                                // Add
                                draw->DrawPointFcn(point->point, 10.0f, addColor, draw->context);
                            }
                            else if (point->persisted == true)
                            {
                                // Persist
                                draw->DrawPointFcn(point->point, 5.0f, persistColor, draw->context);
                            }

                            if (draw->drawContactNormals)
                            {
                                SqVec2 p1 = point->point;
                                SqVec2 p2 = SqVec2::MulAdd(p1, k_axisScale, normal);
                                draw->DrawSegmentFcn(p1, p2, normalColor, draw->context);
                            }
                            else if (draw->drawContactImpulses)
                            {
                                SqVec2 p1 = point->point;
                                SqVec2 p2 = SqVec2::MulAdd(p1, k_impulseScale * point->normalImpulse, normal);
                                draw->DrawSegmentFcn(p1, p2, impulseColor, draw->context);
                                // snprintf(buffer, B2_ARRAY_COUNT(buffer), "%.1f", 1000.0f * point->normalImpulse);
                                draw->DrawStringFcn(p1, buffer, sq_colorWhite, draw->context);
                            }

                            if (draw->drawContactFeatures)
                            {
                                // snprintf(buffer, B2_ARRAY_COUNT(buffer), "%d", point->id);
                                draw->DrawStringFcn(point->point, buffer, sq_colorOrange, draw->context);
                            }

                            if (draw->drawFrictionImpulses)
                            {
                                SqVec2 tangent = SqVec2::RightPerp(normal);
                                SqVec2 p1 = point->point;
                                SqVec2 p2 = SqVec2::MulAdd(p1, k_impulseScale * point->tangentImpulse, tangent);
                                draw->DrawSegmentFcn(p1, p2, frictionColor, draw->context);
                                // snprintf(buffer, B2_ARRAY_COUNT(buffer), "%.1f", 1000.0f * point->tangentImpulse);
                                draw->DrawStringFcn(p1, buffer, sq_colorWhite, draw->context);
                            }
                        }
                        debugContactSet.addValue(contactId);
                    }
                    else
                    {
                        // todo testing
                        edgeIndex += 0;
                    }

                    contactKey = contact->edges[edgeIndex].nextKey;
                }
            }

            // Clear the smallest set bit
            word = word & (word - 1);
        }
    }

    debugJointSet.clearData();
    debugContactSet.clearData();
}

void SqWorldDraw::debugDraw(SqWorld *world, SqDebugDraw *draw)
{
    if (draw->useDrawingBounds)
    {
        drawWithBounds(world, draw);
        return;
    }

    if (draw->drawShapes)
    {
        int setCount = world->solverSets.getCount();

        for (int setIndex = 0; setIndex < setCount; ++setIndex)
        {
            SqSolverSet *set = world->solverSets.get(setIndex);
            int bodyCount = set->bodySims.getCount();

            // printf("bodyCount %d %d \n", bodyCount,set->setIndex);

            for (int bodyIndex = 0; bodyIndex < bodyCount; ++bodyIndex)
            {
                SqBodySim *bodySim = set->bodySims.get(bodyIndex);
                SqBody *body = world->getBody(bodySim->bodyIndex);

                SQ_ASSERT(body->setIndex == setIndex);

                SqTransform xf = bodySim->transform;

                SqShape *shape = body->headShape;

                while (shape)
                {
                    SqHexColor color;

                    if (shape->customColor != 0)
                    {
                        color = SqHexColor(shape->customColor);
                    }
                    else if (body->type == sq_dynamicBody && body->mass == 0.0f)
                    {
                        // 不合法的Body
                        color = sq_colorRed;
                    }
                    else if (body->setIndex == sq_disabledSet)
                    {
                        color = sq_colorSlateGray;
                    }
                    // else if (shape->sensorIndex != SQ_NULL_INDEX)
                    // {
                    //     color = sq_colorWheat;
                    // }
                    else if ((bodySim->flags & sq_isBullet) && body->setIndex == sq_awakeSet)
                    {
                        color = sq_colorTurquoise;
                    }
                    // else if (body->isSpeedCapped)
                    // {
                    //     color = sq_colorYellow;
                    // }
                    else if (bodySim->flags & sq_isFast)
                    {
                        color = sq_colorSalmon;
                    }
                    else if (body->type == sq_staticBody)
                    {
                        color = sq_colorPaleGreen;
                    }
                    else if (body->type == sq_kinematicBody)
                    {
                        color = sq_colorRoyalBlue;
                    }
                    else if (body->setIndex == sq_awakeSet)
                    {
                        color = sq_colorPink;
                    }
                    else
                    {
                        color = sq_colorGray;
                    }

                    drawShape(draw, shape, xf, color);
                    shape = shape->next;
                }
            }
        }
    }

    if (draw->drawJoints)
    {
        int count = world->jointArray.getCount();
        for (int i = 0; i < count; ++i)
        {
            SqJoint *joint = world->getJoint(i);
            if (joint == nullptr || joint->setIndex == SQ_NULL_INDEX)
            {
                continue;
            }

            drawJoint(draw, world, joint);
        }
    }

    
    if (draw->drawBounds)
    {
        SqHexColor color = sq_colorGold;

        int setCount = world->solverSets.getCount();
        for (int setIndex = 0; setIndex < setCount; ++setIndex)
        {
            SqSolverSet *set = world->solverSets.get(setIndex);
            int bodyCount = set->bodySims.getCount();
            for (int bodyIndex = 0; bodyIndex < bodyCount; ++bodyIndex)
            {
                SqBodySim *bodySim = set->bodySims.get(bodyIndex);

                // char buffer[32];
                // snprintf(buffer, 32, "%d", bodySim->bodyId);
                // draw->DrawStringFcn(bodySim->center, buffer, sq_colorWhite, draw->context);

                SqBody *body = world->getBody(bodySim->bodyIndex);

                SQ_ASSERT(body->setIndex == setIndex);

                SqShape *shape = body->headShape;
                while (shape)
                {

                    if (shape->type == sq_chainShape)
                    {
                        SqChainShape *chainShape = (SqChainShape *)shape;

                        for (int i = 0; i < chainShape->count; ++i)
                        {
                            SqAABB aabb = chainShape->segments.get(i)->fatAABB;
                            SqVec2 vs[4] = {{aabb.lowerBound.x, aabb.lowerBound.y},
                                            {aabb.upperBound.x, aabb.lowerBound.y},
                                            {aabb.upperBound.x, aabb.upperBound.y},
                                            {aabb.lowerBound.x, aabb.upperBound.y}};

                            // printf("chain i %d lowerBound %f %f upperBound %f %f \n",i,aabb.lowerBound.x, aabb.lowerBound.y,aabb.upperBound.x, aabb.upperBound.y);
                            draw->DrawPolygonFcn(vs, 4, color, draw->context);
                        }
                    }
                    else
                    {
                        SqAABB aabb = shape->fatAABB;
                        SqVec2 vs[4] = {{aabb.lowerBound.x, aabb.lowerBound.y},
                                        {aabb.upperBound.x, aabb.lowerBound.y},
                                        {aabb.upperBound.x, aabb.upperBound.y},
                                        {aabb.lowerBound.x, aabb.upperBound.y}};

                        draw->DrawPolygonFcn(vs, 4, color, draw->context);
                    }

                    shape = shape->next;
                }
            }
        }
    }

    // if (draw->drawBodyNames)
    // {
    //     SqVec2 offset = {0.05f, 0.05f};
    //     int count = world->bodies.count;
    //     for (int i = 0; i < count; ++i)
    //     {
    //         SqBody *body = world->bodies.data + i;
    //         if (body->setIndex == SQ_NULL_INDEX)
    //         {
    //             continue;
    //         }

    //         if (body->name[0] == 0)
    //         {
    //             continue;
    //         }

    //         SqBodySim *bodySim = b2GetBodySim(world, body);

    //         SqTransform transform = {bodySim->center, bodySim->transform.q};
    //         SqVec2 p = transform.transformPoint(offset);
    //         draw->DrawStringFcn(p, body->name, sq_colorBlueViolet, draw->context);
    //     }
    // }

    
    if (draw->drawMass)
    {
        SqVec2 offset = {0.1f, 0.1f};
        int setCount = world->solverSets.getCount();
        for (int setIndex = 0; setIndex < setCount; ++setIndex)
        {
            SqSolverSet *set = world->solverSets.get(setIndex);
            int bodyCount = set->bodySims.getCount();
            for (int bodyIndex = 0; bodyIndex < bodyCount; ++bodyIndex)
            {
                SqBodySim *bodySim = set->bodySims.get(bodyIndex);

                SqTransform transform = {bodySim->center, bodySim->transform.q};
                draw->DrawTransformFcn(transform, draw->context);

                SqVec2 p = transform.transformPoint(offset);

                char buffer[32];
                float mass = bodySim->invMass > 0.0f ? 1.0f / bodySim->invMass : 0.0f;
                snprintf(buffer, 32, "  %.2f", mass);
                draw->DrawStringFcn(p, buffer, sq_colorWhite, draw->context);
            }
        }
    }

    if (draw->drawContacts)
    {
        const float k_impulseScale = 1.0f;
        const float k_axisScale = 0.3f;
        const float linearSlop = SQ_LINEAR_SLOP;

        SqHexColor speculativeColor = sq_colorLightGray;
        SqHexColor addColor = sq_colorGreen;
        SqHexColor persistColor = sq_colorBlue;
        SqHexColor normalColor = sq_colorDimGray;
        SqHexColor impulseColor = sq_colorMagenta;
        SqHexColor frictionColor = sq_colorYellow;

        int contactCount = world->narrowPhase.contactManager.contacts.getCount();
        for (int contactId = 0; contactId < contactCount; ++contactId)
        {
            SqContact *contact = world->narrowPhase.contactManager.contacts.get(contactId);
            SqContactSim *contactSim = world->getSloverSet(contact->setIndex)->contactSims.get(contact->simIndex);

            int pointCount = contactSim->manifold.pointCount;
            SqVec2 normal = contactSim->manifold.normal;
            char buffer[32];

            for (int j = 0; j < pointCount; ++j)
            {
                SqManifoldPoint *point = contactSim->manifold.points + j;

                if (point->separation > linearSlop)
                {
                    // Speculative
                    draw->DrawPointFcn(point->point, 5.0f, speculativeColor, draw->context);
                }
                else if (point->persisted == false)
                {
                    // Add
                    draw->DrawPointFcn(point->point, 10.0f, addColor, draw->context);
                }
                else if (point->persisted == true)
                {
                    // Persist
                    draw->DrawPointFcn(point->point, 5.0f, persistColor, draw->context);
                }
                else
                {
                    draw->DrawPointFcn(point->point, 1, sq_colorBlue, draw->context);
                }

                if (draw->drawContactNormals)
                {
                    SqVec2 p1 = point->point;
                    SqVec2 p2 = SqVec2::MulAdd(p1, k_axisScale, normal);
                    draw->DrawSegmentFcn(p1, p2, normalColor, draw->context);
                }
                else if (draw->drawContactImpulses)
                {
                    SqVec2 p1 = point->point;
                    SqVec2 p2 = SqVec2::MulAdd(p1, k_impulseScale * point->totalNormalImpulse, normal);
                    draw->DrawSegmentFcn(p1, p2, impulseColor, draw->context);
                    // snprintf(buffer, B2_ARRAY_COUNT(buffer), "%.2f", 1000.0f * point->totalNormalImpulse);
                    // draw->DrawStringFcn(p1, buffer, sq_colorWhite, draw->context);
                }

                if (draw->drawContactFeatures)
                {
                    // snprintf(buffer, B2_ARRAY_COUNT(buffer), "%d", point->id);
                    draw->DrawStringFcn(point->point, buffer, sq_colorOrange, draw->context);
                }

                if (draw->drawFrictionImpulses)
                {
                    SqVec2 tangent = SqVec2::RightPerp(normal);
                    SqVec2 p1 = point->point;
                    SqVec2 p2 = SqVec2::MulAdd(p1, k_impulseScale * point->tangentImpulse, tangent);
                    draw->DrawSegmentFcn(p1, p2, frictionColor, draw->context);
                    // snprintf(buffer, B2_ARRAY_COUNT(buffer), "%.2f", point->tangentImpulse);
                    // draw->DrawStringFcn(p1, buffer, sq_colorWhite, draw->context);
                }
            }
        }
    }

    if (draw->drawIslands)
    {
        int count = world->islandArray.getCount();
        for (int i = 0; i < count; ++i)
        {
            SqIsland *island = world->islandArray.get(i);
            if (island->setIndex == SQ_NULL_INDEX)
            {
                continue;
            }

            int shapeCount = 0;
            SqAABB aabb = {
                {FLT_MAX, FLT_MAX},
                {-FLT_MAX, -FLT_MAX}};

            int bodyId = island->headBody;
            while (bodyId != SQ_NULL_INDEX)
            {
                SqBody *body = world->getBody(bodyId);
                SqShape *shape = body->headShape;
                while (shape)
                {
                    aabb = SqAABB::Union(aabb, shape->fatAABB);
                    shapeCount += 1;
                    shape = shape->next;
                }

                bodyId = body->islandNext;
            }

            if (shapeCount > 0)
            {
                SqVec2 vs[4] = {{aabb.lowerBound.x, aabb.lowerBound.y},
                                {aabb.upperBound.x, aabb.lowerBound.y},
                                {aabb.upperBound.x, aabb.upperBound.y},
                                {aabb.lowerBound.x, aabb.upperBound.y}};

                draw->DrawPolygonFcn(vs, 4, sq_colorOrangeRed, draw->context);
            }
        }
    }
}
