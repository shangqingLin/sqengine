#pragma once

#include "math/SqVec2.h"
#include "math/SqRot.h"
#include "math/SqAABB.h"
#include "math/SqTransform.h"
#include "./constants-define.h"
#include "../dynamics/SqStepContext.h"
#include "../collision/narrowphase/collision.h"
#include "../collision/narrowphase/SqManifold.h"

namespace phxy
{
    typedef void BeforeSolveCustomFcn(SqStepContext *context);

    //====================Shape=====================================

    enum SqHexColor
    {
        sq_colorAliceBlue = 0xF0F8FF,
        sq_colorAntiqueWhite = 0xFAEBD7,
        sq_colorAqua = 0x00FFFF,
        sq_colorAquamarine = 0x7FFFD4,
        sq_colorAzure = 0xF0FFFF,
        sq_colorBeige = 0xF5F5DC,
        sq_colorBisque = 0xFFE4C4,
        sq_colorBlack = 0x000000,
        sq_colorBlanchedAlmond = 0xFFEBCD,
        sq_colorBlue = 0x0000FF,
        sq_colorBlueViolet = 0x8A2BE2,
        sq_colorBrown = 0xA52A2A,
        sq_colorBurlywood = 0xDEB887,
        sq_colorCadetBlue = 0x5F9EA0,
        sq_colorChartreuse = 0x7FFF00,
        sq_colorChocolate = 0xD2691E,
        sq_colorCoral = 0xFF7F50,
        sq_colorCornflowerBlue = 0x6495ED,
        sq_colorCornsilk = 0xFFF8DC,
        sq_colorCrimson = 0xDC143C,
        sq_colorCyan = 0x00FFFF,
        sq_colorDarkBlue = 0x00008B,
        sq_colorDarkCyan = 0x008B8B,
        sq_colorDarkGoldenRod = 0xB8860B,
        sq_colorDarkGray = 0xA9A9A9,
        sq_colorDarkGreen = 0x006400,
        sq_colorDarkKhaki = 0xBDB76B,
        sq_colorDarkMagenta = 0x8B008B,
        sq_colorDarkOliveGreen = 0x556B2F,
        sq_colorDarkOrange = 0xFF8C00,
        sq_colorDarkOrchid = 0x9932CC,
        sq_colorDarkRed = 0x8B0000,
        sq_colorDarkSalmon = 0xE9967A,
        sq_colorDarkSeaGreen = 0x8FBC8F,
        sq_colorDarkSlateBlue = 0x483D8B,
        sq_colorDarkSlateGray = 0x2F4F4F,
        sq_colorDarkTurquoise = 0x00CED1,
        sq_colorDarkViolet = 0x9400D3,
        sq_colorDeepPink = 0xFF1493,
        sq_colorDeepSkyBlue = 0x00BFFF,
        sq_colorDimGray = 0x696969,
        sq_colorDodgerBlue = 0x1E90FF,
        sq_colorFireBrick = 0xB22222,
        sq_colorFloralWhite = 0xFFFAF0,
        sq_colorForestGreen = 0x228B22,
        sq_colorFuchsia = 0xFF00FF,
        sq_colorGainsboro = 0xDCDCDC,
        sq_colorGhostWhite = 0xF8F8FF,
        sq_colorGold = 0xFFD700,
        sq_colorGoldenRod = 0xDAA520,
        sq_colorGray = 0x808080,
        sq_colorGreen = 0x008000,
        sq_colorGreenYellow = 0xADFF2F,
        sq_colorHoneyDew = 0xF0FFF0,
        sq_colorHotPink = 0xFF69B4,
        sq_colorIndianRed = 0xCD5C5C,
        sq_colorIndigo = 0x4B0082,
        sq_colorIvory = 0xFFFFF0,
        sq_colorKhaki = 0xF0E68C,
        sq_colorLavender = 0xE6E6FA,
        sq_colorLavenderBlush = 0xFFF0F5,
        sq_colorLawnGreen = 0x7CFC00,
        sq_colorLemonChiffon = 0xFFFACD,
        sq_colorLightBlue = 0xADD8E6,
        sq_colorLightCoral = 0xF08080,
        sq_colorLightCyan = 0xE0FFFF,
        sq_colorLightGoldenRodYellow = 0xFAFAD2,
        sq_colorLightGray = 0xD3D3D3,
        sq_colorLightGreen = 0x90EE90,
        sq_colorLightPink = 0xFFB6C1,
        sq_colorLightSalmon = 0xFFA07A,
        sq_colorLightSeaGreen = 0x20B2AA,
        sq_colorLightSkyBlue = 0x87CEFA,
        sq_colorLightSlateGray = 0x778899,
        sq_colorLightSteelBlue = 0xB0C4DE,
        sq_colorLightYellow = 0xFFFFE0,
        sq_colorLime = 0x00FF00,
        sq_colorLimeGreen = 0x32CD32,
        sq_colorLinen = 0xFAF0E6,
        sq_colorMagenta = 0xFF00FF,
        sq_colorMaroon = 0x800000,
        sq_colorMediumAquaMarine = 0x66CDAA,
        sq_colorMediumBlue = 0x0000CD,
        sq_colorMediumOrchid = 0xBA55D3,
        sq_colorMediumPurple = 0x9370DB,
        sq_colorMediumSeaGreen = 0x3CB371,
        sq_colorMediumSlateBlue = 0x7B68EE,
        sq_colorMediumSpringGreen = 0x00FA9A,
        sq_colorMediumTurquoise = 0x48D1CC,
        sq_colorMediumVioletRed = 0xC71585,
        sq_colorMidnightBlue = 0x191970,
        sq_colorMintCream = 0xF5FFFA,
        sq_colorMistyRose = 0xFFE4E1,
        sq_colorMoccasin = 0xFFE4B5,
        sq_colorNavajoWhite = 0xFFDEAD,
        sq_colorNavy = 0x000080,
        sq_colorOldLace = 0xFDF5E6,
        sq_colorOlive = 0x808000,
        sq_colorOliveDrab = 0x6B8E23,
        sq_colorOrange = 0xFFA500,
        sq_colorOrangeRed = 0xFF4500,
        sq_colorOrchid = 0xDA70D6,
        sq_colorPaleGoldenRod = 0xEEE8AA,
        sq_colorPaleGreen = 0x98FB98,
        sq_colorPaleTurquoise = 0xAFEEEE,
        sq_colorPaleVioletRed = 0xDB7093,
        sq_colorPapayaWhip = 0xFFEFD5,
        sq_colorPeachPuff = 0xFFDAB9,
        sq_colorPeru = 0xCD853F,
        sq_colorPink = 0xFFC0CB,
        sq_colorPlum = 0xDDA0DD,
        sq_colorPowderBlue = 0xB0E0E6,
        sq_colorPurple = 0x800080,
        sq_colorRebeccaPurple = 0x663399,
        sq_colorRed = 0xFF0000,
        sq_colorRosyBrown = 0xBC8F8F,
        sq_colorRoyalBlue = 0x4169E1,
        sq_colorSaddleBrown = 0x8B4513,
        sq_colorSalmon = 0xFA8072,
        sq_colorSandyBrown = 0xF4A460,
        sq_colorSeaGreen = 0x2E8B57,
        sq_colorSeaShell = 0xFFF5EE,
        sq_colorSienna = 0xA0522D,
        sq_colorSilver = 0xC0C0C0,
        sq_colorSkyBlue = 0x87CEEB,
        sq_colorSlateBlue = 0x6A5ACD,
        sq_colorSlateGray = 0x708090,
        sq_colorSnow = 0xFFFAFA,
        sq_colorSpringGreen = 0x00FF7F,
        sq_colorSteelBlue = 0x4682B4,
        sq_colorTan = 0xD2B48C,
        sq_colorTeal = 0x008080,
        sq_colorThistle = 0xD8BFD8,
        sq_colorTomato = 0xFF6347,
        sq_colorTurquoise = 0x40E0D0,
        sq_colorViolet = 0xEE82EE,
        sq_colorWheat = 0xF5DEB3,
        sq_colorWhite = 0xFFFFFF,
        sq_colorWhiteSmoke = 0xF5F5F5,
        sq_colorYellow = 0xFFFF00,
        sq_colorYellowGreen = 0x9ACD32,

        sq_colorBox2DRed = 0xDC3132,
        sq_colorBox2DBlue = 0x30AEBF,
        sq_colorBox2DGreen = 0x8CC924,
        sq_colorBox2DYellow = 0xFFEE8C
    };

    struct SqDebugDraw
    {
        /// Draw a closed polygon provided in CCW order.
        void (*DrawPolygonFcn)(const SqVec2 *vertices, int vertexCount, SqHexColor color, void *context);

        /// Draw a solid closed polygon provided in CCW order.
        void (*DrawSolidPolygonFcn)(SqTransform transform, const SqVec2 *vertices, int vertexCount, float radius, SqHexColor color,
                                    void *context);

        /// Draw a circle.
        void (*DrawCircleFcn)(SqVec2 center, float radius, SqHexColor color, void *context);

        /// Draw a solid circle.
        void (*DrawSolidCircleFcn)(SqTransform transform, float radius, SqHexColor color, void *context);

        /// Draw a solid capsule.
        void (*DrawSolidCapsuleFcn)(SqVec2 p1, SqVec2 p2, float radius, SqHexColor color, void *context);

        /// Draw a line segment.
        void (*DrawSegmentFcn)(SqVec2 p1, SqVec2 p2, SqHexColor color, void *context);

        /// Draw a transform. Choose your own length scale.
        void (*DrawTransformFcn)(SqTransform transform, void *context);

        /// Draw a point.
        void (*DrawPointFcn)(SqVec2 p, float size, SqHexColor color, void *context);

        /// Draw a string in world space
        void (*DrawStringFcn)(SqVec2 p, const char *s, SqHexColor color, void *context);

        SqAABB drawingBounds;

        bool useDrawingBounds;

        bool drawShapes;

        bool drawJoints;

        bool drawJointExtras;

        bool drawBounds;

        bool drawMass;

        bool drawBodyNames;

        bool drawContacts;

        bool drawGraphColors;

        bool drawContactNormals;

        bool drawContactImpulses;

        bool drawContactFeatures;

        bool drawFrictionImpulses;

        bool drawIslands;

        void *context;
    };

    typedef float SqFrictionCallback(float frictionA, int userMaterialIdA, float frictionB, int userMaterialIdB);
    typedef float SqRestitutionCallback(float restitutionA, int userMaterialIdA, float restitutionB, int userMaterialIdB);

    //======================== 事件==================

    /**
     * 对外部提供的碰撞事件——开始发生碰撞触发的事件
     */
    struct SqContactBeginEvent
    {
        int contactId{SQ_NULL_INDEX};
        SqManifold *maniflod{nullptr};
        SqShape *shapeA{nullptr};
        SqShape *shapeB{nullptr};
    };

    struct SqContactEndEvent
    {
        int contactId{SQ_NULL_INDEX};
        int bodyAId{SQ_NULL_INDEX};
        int bodyBId{SQ_NULL_INDEX};
        SqShape *shapeA{nullptr};
        SqShape *shapeB{nullptr};
    };

    struct SqContactHitUpdateEvent
    {
        int contactId{SQ_NULL_INDEX};
        SqManifold *maniflod{nullptr};
        SqShape *shapeA{nullptr};
        SqShape *shapeB{nullptr};
    };

    struct SqSensorBeginEvent
    {
        SqShape *shapeA{nullptr};
        SqShape *shapeB{nullptr};
    };

    struct SqSensorEndEvent
    {
        SqShape *shapeA{nullptr};
        SqShape *shapeB{nullptr};
    };

}
