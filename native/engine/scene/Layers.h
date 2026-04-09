#ifndef _SCENE_LAYERS_H_
#define _SCENE_LAYERS_H_

#include "../core/sqstd/typedefs.h"

enum class Layers : int
{

    /**
     * 继承父节点的Layer
     * Default
     */
    INHERIT = 0, // Default
    GROUND = 3,
    UI = 5,
    TRUCK = 6,
    MAP = 7,
    EFFECT = 8,
    BACKGROUND_FRA = 9,   // 远景
    BACKGROUND_MID = 10,  // 中景
    BACKGROUND_NEAR = 11, // 近景
    BACKGROUND_FORE = 12, // 前景
    FLUID = 13,
    TRUCK_SENSOR = 14,
    EMITTER_WATER = 15,
    SCENE_OBJECT = 16,
    FLMAE = 17
};
SQ_ENUM_CONVERSION_OPERATOR(Layers);
#endif