#pragma once

#include <stdint.h>

// 整数数值为-1 表示引用变量中的NULL
#define SQ_NULL_INDEX (-1)

#define SQ_GRAPH_COLOR_COUNT 12

/**
 * 一般不用修改，增大会对性能有影响
 */
#define SQ_MAX_POLYGON_VERTICES 8
#define SQ_DEFAULT_CATEGORY_BITS 1
#define SQ_DEFAULT_MASK_BITS 0xfffffff

#define SQ_SIMD_WIDTH 4

#define SQ_GRAPH_COLOR_COUNT 12

// Body静止的时间超过这里设置的时间则进入Sleep
#define SQ_TIME_TO_SLEEP 0.5f



/**
 * 在物理引擎中，处理的单位：
 * 1、长度单位：米
 * 2、角度：弧度
 * 3、重量为：千克
 * 4、时间为：秒
 * 
 * 如果你使用物理引擎的时候以像素作为单位的，则需要设置多少个像素为1米
 * 如果以米为单位，则不需要设置此参数
 */
extern float sq_lengthUnitsPerMeter;

void sqSetLengthUnitsPerMeter(float lengthUnits);

float sqGetLengthUnitsPerMeter(void);

/**
 * 表示Box2D整个世界最大的宽度和长度
 * 默认值为 100km
 */
#define SQ_HUGE (100000.0f * sq_lengthUnitsPerMeter)

/**
 * 默认值为2cm
 */
#define SQ_LINEAR_SLOP (0.005f * sq_lengthUnitsPerMeter)