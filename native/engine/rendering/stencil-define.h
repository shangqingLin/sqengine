#pragma once
#include "../core/sqstd/typedefs.h"

namespace pipeline
{
    enum class StencilStage
    {
        // 关闭模板测试
        DISABLED = 0,

        // 处于清除模板缓冲区阶段
        CLEAR = 1,

        // Entering a new level, should handle new stencil
        ENTER_LEVEL = 2,

        // In content
        ENABLED = 3,

        // Exiting a level, should restore old stencil or disable
        EXIT_LEVEL = 4,

        // Clear stencil buffer & USE INVERTED
        CLEAR_INVERTED = 5,

        // Entering a new level & USE INVERTED
        ENTER_LEVEL_INVERTED = 6,
    };
    SQ_ENUM_CONVERSION_OPERATOR(StencilStage);
}