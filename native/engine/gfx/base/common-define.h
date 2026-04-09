#ifndef _COMMON_DEFINE_H_
#define _COMMON_DEFINE_H_
#include "../../core/core.h"

namespace gfx{
    
    enum class BlendFactor : uint32_t {
        ZERO,
        ONE,
        SRC_ALPHA,
        DST_ALPHA,
        ONE_MINUS_SRC_ALPHA,
        ONE_MINUS_DST_ALPHA,
        SRC_COLOR,
        DST_COLOR,
        ONE_MINUS_SRC_COLOR,
        ONE_MINUS_DST_COLOR,
        SRC_ALPHA_SATURATE,
        CONSTANT_COLOR,
        ONE_MINUS_CONSTANT_COLOR,
        CONSTANT_ALPHA,
        ONE_MINUS_CONSTANT_ALPHA,
    };
    SQ_ENUM_CONVERSION_OPERATOR(BlendFactor);

    enum class BlendOp : uint32_t {
        ADD,
        SUB,
        REV_SUB,
        MIN,
        MAX,
    };
    SQ_ENUM_CONVERSION_OPERATOR(BlendOp);

    enum class ColorMask : uint32_t {
        NONE = 0x0,
        R = 0x1,
        G = 0x2,
        B = 0x4,
        A = 0x8,
        ALL = R | G | B | A,
    };
    SQ_ENUM_CONVERSION_OPERATOR(ColorMask);

    enum class Type:int {
        UNKNOWN,
        BOOL,
        BOOL2,//bvec2
        BOOL3,//bvec3
        BOOL4,//bvec4
        INT,//int
        INT2,//ivec2
        INT3,//ivec3
        INT4,//ivec4
        UINT,//uint
        UINT2,//uvec2
        UINT3,//uvec3
        UINT4,//uvec4
        FLOAT, //float
        FLOAT2,//vec2 
        FLOAT3,//vec3
        FLOAT4,//vec4
        MAT2,
        MAT2X3,
        MAT2X4,
        MAT3X2,
        MAT3,
        MAT3X4,
        MAT4X2,
        MAT4X3,
        MAT4,
        // combined image samplers
        SAMPLER1D,
        SAMPLER1D_ARRAY,
        SAMPLER2D,
        SAMPLER2D_ARRAY,
        SAMPLER3D,
        SAMPLER_CUBE,
        // sampler
        SAMPLER,
        // sampled textures
        TEXTURE1D,
        TEXTURE1D_ARRAY,
        TEXTURE2D,
        TEXTURE2D_ARRAY,
        TEXTURE3D,
        TEXTURE_CUBE,
        // storage images
        IMAGE1D,
        IMAGE1D_ARRAY,
        IMAGE2D,
        IMAGE2D_ARRAY,
        IMAGE3D,
        IMAGE_CUBE,
        // input attachment
        SUBPASS_INPUT,
        COUNT
    };

    SQ_ENUM_CONVERSION_OPERATOR(Type);

    extern const char TYPE_SIZES[46];

    int getTypeSize(Type type);
}
#endif