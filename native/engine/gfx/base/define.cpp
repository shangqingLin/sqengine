#include "define.h"
#include <cmath>

using namespace gfx;

FormatInfo FormatInfos[116] = {
    {0, 0}, // UNKNOWN
    {1, 1}, // A8
    {1, 2}, // LA8
    {1, 1}, // R8
    {1, 1}, // R8SN
    {1, 1}, // R8UI
    {1, 1}, // R8I
    {2, 1}, // R16F
    {2, 1}, // R16UI
    {2, 1}, // R16I
    {4, 1}, // R32F
    {4, 1}, // R32UI
    {4, 1}, // R32I

    {2, 2}, // RG8
    {2, 2}, // RG8SN
    {2, 2}, // RG8UI
    {2, 2}, // RG8I
    {4, 2}, // RG16F
    {4, 2}, // RG16UI
    {4, 2}, // RG16I
    {8, 2}, // RG32F
    {8, 2}, // RG32UI
    {8, 2}, // RG32I

    {3, 3}, // RGB8
    {3, 3}, // SRGB8
    {3, 3}, // RGB8SN
    {3, 3}, // RGB8UI
    {3, 3}, // RGB8I

    {6, 3}, // RGB16F
    {6, 3}, // RGB16UI
    {6, 3}, // RGB16I

    {12, 3}, // RGB32F
    {12, 3}, // RGB32UI
    {12, 3}, // RGB32I

    {4, 4}, // RGBA8
    {4, 4}, // BGRA8
    {4, 4}, // SRGB8_A8
    {4, 4}, // RGBA8SN
    {4, 4}, // RGBA8UI
    {4, 4}, // RGBA8I

    {8, 4}, // RGBA16F
    {8, 4}, // RGBA16UI
    {8, 4}, // RGBA16I

    {16, 4}, // RGBA32F
    {16, 4}, // RGBA32UI
    {16, 4}, // RGBA32I

    {2, 3}, // R5G6B5
    {4, 3}, // R11G11B10F
    {2, 4}, // RGB5A1
    {2, 4}, // RGBA4
    {2, 4}, // RGB10A2
    {2, 4}, // RGB10A2UI
    {2, 4}, // RGB9E5

    {4, 1}, // DEPTH
    {5, 2}, // DEPTH_STENCIL

    {1, 3}, // BC1
    {1, 4}, // BC1_ALPHA
    {1, 3}, // BC1_SRGB
    {1, 4}, // BC1_SRGB_ALPHA
    {1, 4}, // BC2
    {1, 4}, // BC2_SRGB

    {1, 4}, // BC3
    {1, 4}, // BC3_SRGB
    {1, 1}, // BC4
    {1, 1}, // BC4_SNORM
    {1, 2}, // BC5
    {1, 2}, // BC5_SNORM
    {1, 3}, // BC6H_UF16
    {1, 3}, // BC6H_SF16
    {1, 4}, // BC7
    {1, 4}, // BC7_SRGB

    {1, 3}, // ETC_RGB8
    {1, 3}, // ETC2_RGB8
    {1, 3}, // ETC2_SRGB8
    {1, 4}, // ETC2_RGB8_A1
    {1, 4}, // ETC2_SRGB8_A1
    {2, 4}, // ETC2_RGBA8
    {2, 4}, // ETC2_SRGB8_A8
    {1, 1}, // EAC_R11
    {1, 1}, // EAC_R11SN
    {2, 2}, // EAC_RG11
    {2, 2}, // EAC_RG11SN

    {2, 3}, // PVRTC_RGB2
    {2, 4}, // PVRTC_RGBA2
    {2, 3}, // PVRTC_RGB4
    {2, 4}, // PVRTC_RGBA4
    {2, 4}, // PVRTC2_2BPP
    {2, 4}, // PVRTC2_4BPP

    {1, 4}, // ASTC_RGBA_4x4
    {1, 4}, // ASTC_RGBA_5x4
    {1, 4}, // ASTC_RGBA_5x5
    {1, 4}, // ASTC_RGBA_6x5
    {1, 4}, // ASTC_RGBA_6x6
    {1, 4}, // ASTC_RGBA_8x5
    {1, 4}, // ASTC_RGBA_8x6
    {1, 4}, // ASTC_RGBA_8x8
    {1, 4}, // ASTC_RGBA_10x5
    {1, 4}, // ASTC_RGBA_10x6
    {1, 4}, // ASTC_RGBA_10x8
    {1, 4}, // ASTC_RGBA_10x10
    {1, 4}, // ASTC_RGBA_12x10
    {1, 4}, // ASTC_RGBA_12x12

    {1, 4}, // ASTC_SRGBA_4x4
    {1, 4}, // ASTC_SRGBA_5x4
    {1, 4}, // ASTC_SRGBA_5x5
    {1, 4}, // ASTC_SRGBA_6x5
    {1, 4}, // ASTC_SRGBA_6x6
    {1, 4}, // ASTC_SRGBA_8x5
    {1, 4}, // ASTC_SRGBA_8x6
    {1, 4}, // ASTC_SRGBA_8x8
    {1, 4}, // ASTC_SRGBA_10x5
    {1, 4}, // ASTC_SRGBA_10x6
    {1, 4}, // ASTC_SRGBA_10x8
    {1, 4}, // ASTC_SRGBA_10x10
    {1, 4}, // ASTC_SRGBA_12x10
    {1, 4}  // ASTC_SRGBA_12x12
};

int GetFormatSize(Format f)
{
    return FormatInfos[toNumber(f)].size;
}

void formatAlignment(Format format, int &width, int &height)
{
    switch (format)
    {
    case Format::BC1:
    case Format::BC1_ALPHA:
    case Format::BC1_SRGB:
    case Format::BC1_SRGB_ALPHA:
    case Format::BC2:
    case Format::BC2_SRGB:
    case Format::BC3:
    case Format::BC3_SRGB:
    case Format::BC4:
    case Format::BC4_SNORM:
    case Format::BC6H_SF16:
    case Format::BC6H_UF16:
    case Format::BC7:
    case Format::BC7_SRGB:
    case Format::BC5:
    case Format::BC5_SNORM:
    case Format::ETC_RGB8:
    case Format::ETC2_RGB8:
    case Format::ETC2_SRGB8:
    case Format::ETC2_RGB8_A1:
    case Format::EAC_R11:
    case Format::EAC_R11SN:
    case Format::ETC2_RGBA8:
    case Format::ETC2_SRGB8_A1:
    case Format::EAC_RG11:
    case Format::EAC_RG11SN:
    case Format::PVRTC_RGB4:
    case Format::PVRTC_RGBA4:
    case Format::PVRTC2_4BPP:
    case Format::ASTC_RGBA_4X4:
    case Format::ASTC_SRGBA_4X4:
        width = 4;
        height = 4;
        return;

    case Format::PVRTC_RGB2:
    case Format::PVRTC_RGBA2:
    case Format::PVRTC2_2BPP:
        width = 8;
        height = 8;
        return;

    case Format::ASTC_RGBA_5X4:
    case Format::ASTC_SRGBA_5X4:
        width = 5;
        height = 4;
    case Format::ASTC_RGBA_5X5:
    case Format::ASTC_SRGBA_5X5:
        width = 5;
        height = 5;
        return;
    case Format::ASTC_RGBA_6X5:
    case Format::ASTC_SRGBA_6X5:
        width = 6;
        height = 5;
        return;
    case Format::ASTC_RGBA_6X6:
    case Format::ASTC_SRGBA_6X6:
        width = 6;
        height = 6;
        return;
    case Format::ASTC_RGBA_8X5:
    case Format::ASTC_SRGBA_8X5:
    case Format::ASTC_RGBA_8X8:
    case Format::ASTC_SRGBA_8X8:
        width = 8;
        height = 8;
        return;
    case Format::ASTC_RGBA_8X6:
    case Format::ASTC_SRGBA_8X6:
        width = 8;
        height = 6;
        return;
    case Format::ASTC_RGBA_10X5:
    case Format::ASTC_SRGBA_10X5:
        width = 10;
        height = 15;
        return;
    case Format::ASTC_RGBA_10X6:
    case Format::ASTC_SRGBA_10X6:
        width = 10;
        height = 6;
        return;
    case Format::ASTC_RGBA_10X8:
    case Format::ASTC_SRGBA_10X8:
        width = 10;
        height = 8;
        return;
    case Format::ASTC_RGBA_10X10:
    case Format::ASTC_SRGBA_10X10:
        width = 10;
        height = 10;
        return;
    case Format::ASTC_RGBA_12X10:
    case Format::ASTC_SRGBA_12X10:
        width = 12;
        height = 10;
        return;
    case Format::ASTC_RGBA_12X12:
    case Format::ASTC_SRGBA_12X12:
        width = 12;
        height = 12;
        return;
    default:
        width = 1;
        height = 1;
    }
}

int alignTo(int size, int alignment)
{
    return ceil(size / alignment) * alignment;
}

int formatTexSize(Format format, int width, int height, int depth)
{
    switch (format)
    {
    case Format::BC1:
    case Format::BC1_ALPHA:
    case Format::BC1_SRGB:
    case Format::BC1_SRGB_ALPHA:
        return ceil(width / 4) * ceil(height / 4) * 8 * depth;
    case Format::BC2:
    case Format::BC2_SRGB:
    case Format::BC3:
    case Format::BC3_SRGB:
    case Format::BC4:
    case Format::BC4_SNORM:
    case Format::BC6H_SF16:
    case Format::BC6H_UF16:
    case Format::BC7:
    case Format::BC7_SRGB:
        return ceil(width / 4) * ceil(height / 4) * 16 * depth;
    case Format::BC5:
    case Format::BC5_SNORM:
        return ceil(width / 4) * ceil(height / 4) * 32 * depth;

    case Format::ETC_RGB8:
    case Format::ETC2_RGB8:
    case Format::ETC2_SRGB8:
    case Format::ETC2_RGB8_A1:
    case Format::EAC_R11:
    case Format::EAC_R11SN:
        return ceil(width / 4) * ceil(height / 4) * 8 * depth;
    case Format::ETC2_RGBA8:
    case Format::ETC2_SRGB8_A1:
    case Format::EAC_RG11:
    case Format::EAC_RG11SN:
        return ceil(width / 4) * ceil(height / 4) * 16 * depth;

    case Format::PVRTC_RGB2:
    case Format::PVRTC_RGBA2:
    case Format::PVRTC2_2BPP:
        return ceil(width / 8) * ceil(height / 4) * 8 * depth;

    case Format::PVRTC_RGB4:
    case Format::PVRTC_RGBA4:
    case Format::PVRTC2_4BPP:
        return ceil(width / 4) * ceil(height / 4) * 8 * depth;

    case Format::ASTC_RGBA_4X4:
    case Format::ASTC_SRGBA_4X4:
        return ceil(width / 4) * ceil(height / 4) * 16 * depth;
    case Format::ASTC_RGBA_5X4:
    case Format::ASTC_SRGBA_5X4:
        return ceil(width / 5) * ceil(height / 4) * 16 * depth;
    case Format::ASTC_RGBA_5X5:
    case Format::ASTC_SRGBA_5X5:
        return ceil(width / 5) * ceil(height / 5) * 16 * depth;
    case Format::ASTC_RGBA_6X5:
    case Format::ASTC_SRGBA_6X5:
        return ceil(width / 6) * ceil(height / 5) * 16 * depth;
    case Format::ASTC_RGBA_6X6:
    case Format::ASTC_SRGBA_6X6:
        return ceil(width / 6) * ceil(height / 6) * 16 * depth;
    case Format::ASTC_RGBA_8X5:
    case Format::ASTC_SRGBA_8X5:
        return ceil(width / 8) * ceil(height / 5) * 16 * depth;
    case Format::ASTC_RGBA_8X6:
    case Format::ASTC_SRGBA_8X6:
        return ceil(width / 8) * ceil(height / 6) * 16 * depth;
    case Format::ASTC_RGBA_8X8:
    case Format::ASTC_SRGBA_8X8:
        return ceil(width / 8) * ceil(height / 8) * 16 * depth;
    case Format::ASTC_RGBA_10X5:
    case Format::ASTC_SRGBA_10X5:
        return ceil(width / 10) * ceil(height / 5) * 16 * depth;
    case Format::ASTC_RGBA_10X6:
    case Format::ASTC_SRGBA_10X6:
        return ceil(width / 10) * ceil(height / 6) * 16 * depth;
    case Format::ASTC_RGBA_10X8:
    case Format::ASTC_SRGBA_10X8:
        return ceil(width / 10) * ceil(height / 8) * 16 * depth;
    case Format::ASTC_RGBA_10X10:
    case Format::ASTC_SRGBA_10X10:
        return ceil(width / 10) * ceil(height / 10) * 16 * depth;
    case Format::ASTC_RGBA_12X10:
    case Format::ASTC_SRGBA_12X10:
        return ceil(width / 12) * ceil(height / 10) * 16 * depth;
    case Format::ASTC_RGBA_12X12:
    case Format::ASTC_SRGBA_12X12:
        return ceil(width / 12) * ceil(height / 12) * 16 * depth;

    default:
        return (width * height * depth * FormatInfos[toNumber(format)].size);
    }
}

void SetBlendMode(BlendMode model, BlendTarget &target)
{
    switch (model)
    {
    case BlendMode::ADD:
    {
        target.blendEq = gfx::BlendOp::ADD;
        target.blendAlphaEq = gfx::BlendOp::ADD;
        target.blendSrc = gfx::BlendFactor::SRC_ALPHA;
        target.blendDst = gfx::BlendFactor::ONE;
        target.blendSrcAlpha = gfx::BlendFactor::SRC_ALPHA;
        target.blendDstAlpha = gfx::BlendFactor::ONE;

        // target.blendEq = gfx::BlendOp::ADD;
        // target.blendAlphaEq = gfx::BlendOp::ADD;
        // target.blendSrc = gfx::BlendFactor::SRC_ALPHA;
        // target.blendDst = gfx::BlendFactor::ONE;
        // target.blendSrcAlpha = gfx::BlendFactor::SRC_ALPHA;
        // target.blendDstAlpha = gfx::BlendFactor::ONE_MINUS_SRC_ALPHA;

        break;
    }
    case BlendMode::MIX:
    {

        target.blendEq = gfx::BlendOp::ADD;
        target.blendAlphaEq = gfx::BlendOp::ADD;
        target.blendSrc = gfx::BlendFactor::SRC_ALPHA;
        target.blendDst = gfx::BlendFactor::ONE_MINUS_SRC_ALPHA;
        target.blendSrcAlpha = gfx::BlendFactor::ONE;
        target.blendDstAlpha = gfx::BlendFactor::ONE_MINUS_SRC_ALPHA;
        break;
    }

    case BlendMode::SUB:
    {
        target.blendEq = gfx::BlendOp::REV_SUB;
        target.blendAlphaEq = gfx::BlendOp::REV_SUB;
        target.blendSrc = gfx::BlendFactor::SRC_ALPHA;
        target.blendDst = gfx::BlendFactor::ONE;
        target.blendSrcAlpha = gfx::BlendFactor::SRC_ALPHA;
        target.blendDstAlpha = gfx::BlendFactor::ONE;
        break;
    }
    case BlendMode::MUL:
    {
        target.blendEq = gfx::BlendOp::ADD;
        target.blendAlphaEq = gfx::BlendOp::ADD;
        target.blendSrc = gfx::BlendFactor::DST_COLOR;
        target.blendDst = gfx::BlendFactor::ZERO;
        target.blendSrcAlpha = gfx::BlendFactor::DST_ALPHA;
        target.blendDstAlpha = gfx::BlendFactor::ZERO;
        break;
    }

    case BlendMode::ALPHA_TO_COVERAGE:
    {
        target.blendEq = gfx::BlendOp::ADD;
        target.blendAlphaEq = gfx::BlendOp::ADD;
        target.blendSrc = gfx::BlendFactor::SRC_ALPHA;
        target.blendDst = gfx::BlendFactor::ONE_MINUS_SRC_ALPHA;
        target.blendSrcAlpha = gfx::BlendFactor::ONE;
        target.blendDstAlpha = gfx::BlendFactor::ZERO;
        break;
    }

    case BlendMode::PREMULTIPLIED_ALPHA:
    {
        target.blendEq = gfx::BlendOp::ADD;
        target.blendAlphaEq = gfx::BlendOp::ADD;
        target.blendSrc = gfx::BlendFactor::ONE;
        target.blendDst = gfx::BlendFactor::ONE_MINUS_SRC_ALPHA;
        target.blendSrcAlpha = gfx::BlendFactor::ONE;
        target.blendDstAlpha = gfx::BlendFactor::ONE_MINUS_SRC_ALPHA;
        break;
    }
    default:
        break;
    }
}

sqstd::hash_t BlendState::serializeHash()
{
    // sqstd::hash_detail::hash_combine();
    return 0;
}

sqstd::hash_t StencilState::serializeHash()
{
    // sqstd::hash_detail::hash_combine();
    return 0;
}

sqstd::hash_t RasterizerState::serializeHash()
{
    // sqstd::hash_detail::hash_combine();
    return 0;
}

int getAttributeStride(const Attribute &attr)
{
    FormatInfo &info = FormatInfos[toNumber(attr.format)];
    return info.count;
}

int getAttributeStride(const std::vector<Attribute> &attributes)
{
    int count = 0;
    for (int i = 0; i < attributes.size(); ++i)
    {
        const Attribute &attr = attributes[i];
        FormatInfo &info = FormatInfos[toNumber(attr.format)];
        count += info.size;
    }
    return count;
}
