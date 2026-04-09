#pragma once
#include <optional>
#include "../gfx/gfx.h"
#include "../assets/EffectAsset.h"

// 宏定义的数据类型
using MacroValue = std::variant<int32_t, bool, std::string>;
using MacroRecord = std::unordered_map<std::string, MacroValue>;

struct PassStates
{
    std::optional<RasterizerState> rasterizerState;
    std::optional<DepthState> depthState;
    std::optional<StencilState> stencilState;
    std::optional<BlendState> blendState;
    std::optional<PrimitiveMode> primitive;
};

struct PassInfo
{
    /**
     * 当这个Pass引用Effect资源中哪个Shader
     */
    int shaderIndex;
    int effectAssetId;

    PassStates state;

    // 宏定义
    MacroRecord defines;
};
