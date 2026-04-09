#pragma once
#include "Texture2d.h"
#include <optional>
#include "../core/math/Vec4.h"
#include "../core/math/Rect.h"

class SpriteFrame
{
public:
    Texture2d *texture;
    Rect<float> rect;
    bool rotate;
    float uv[8];

    // 9宫格设置
    std::optional<Vec4> grid9;
    // 9宫的uv
    struct SpriteFrameSlicedUv
    {
        float uv[32];
    };
    std::optional<SpriteFrameSlicedUv> slicedUv;

    SpriteFrame();
};
