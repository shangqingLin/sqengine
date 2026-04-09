import { Color } from "../../core";


/**
 * 记录当前图形API支持的能力如何
 */
export class DeviceCaps {

    /**
     * 顶点着色器可以声明顶点属性的最多多少个
     */
    public maxVertexAttributes: number = 0;


    public maxVertexUniformVectors: number = 0;
    public maxFragmentUniformVectors: number = 0;

    /**
     * 支持的纹理单元的个数
     */
    public maxTextureUnits: number = 0;

    /**
     * 支持的图像个数
     */
    public maxImageUnits: number = 0;

    /**
     * 顶点着色器中可以支持多个纹理单元
     */
    public maxVertexTextureUnits: number = 0;
    public maxColorRenderTargets: number = 0;
    public maxShaderStorageBufferBindings: number = 0;
    public maxShaderStorageBlockSize: number = 0;

    /**
     * 是否支持32位浮点数纹理
     */
    public support32FloatTexture: boolean;

    /**
     * 是否支持16位浮点纹理
     */
    public support16FloatTexture: boolean;

    /**
     * 是否支持离屏幕渲染到32位浮点数纹理上。
     * 好操蛋，WebGL2.0虽然支持了32位浮点纹理了，
     * 但不一定支持将32位浮点纹理作为RenderTexture，需要扩展支持才行
     */
    public support32FloatRenderTexture: boolean;

    /**
     * 是否支持离屏幕渲染到16位浮点数纹理上
     */
    public support16FloatRenderTexture: boolean;


    /**
     * Uniform block支持最多的个数。WebGL1.0不支持Uniform块，所以这个为0
     */
    public maxUniformBufferBindings: number = 0;
    public maxUniformBlockSize: number = 0;

    /**
     * 支持的纹理最大的大小是多少（宽高）
     */
    public maxTextureSize: number = 0;
    public maxCubeMapTextureSize: number = 0;
    public maxArrayTextureLayers: number = 0;
    // public max3DTextureSize: number = 0;
    public uboOffsetAlignment: number = 1;
    public maxComputeSharedMemorySize: number = 0;
    public maxComputeWorkGroupInvocations: number = 0;

    public supportQuery: boolean = false;
    public supportGPUDriven: boolean = false;
    public clipSpaceMinZ: number = -1;
    public screenSpaceSignY: number = 1;
    public clipSpaceSignY: number = 1;

    //融合相关

    /**
     * 是否支持Min和Max融合方程
     */
    public supportBlendMaxMinEquation: boolean;

    /**
     * 是否支持在32位浮点数纹理上进行Blend操作
     */
    public supportBlendWith32FloatRenderTexture: boolean
}





//--------------------------------纹理相关的定义---------------------------
export enum TextureType {
    TEX1D,
    TEX2D,
    TEX3D,
    CUBE,
    TEX1D_ARRAY,
    TEX2D_ARRAY,
}

// export enum TextureUsage {
//     NONE = 0,
//     TRANSFER_SRC = 0x1,
//     TRANSFER_DST = 0x2,
//     SAMPLED = 0x4,
//     STORAGE = 0x8,
//     COLOR_ATTACHMENT = 0x10,
//     DEPTH_STENCIL_ATTACHMENT = 0x20,
//     INPUT_ATTACHMENT = 0x40,
// }

// export enum TextureFlag {
//     NONE = 0,
//     GEN_MIPMAP = 0x1,     // Generate mipmaps using bilinear filter
//     GENERAL_LAYOUT = 0x2, // For inout framebuffer attachments
//     EXTERNAL_OES = 0x4, // External oes texture
//     EXTERNAL_NORMAL = 0x8, // External normal texture
//     LAZILY_ALLOCATED = 0x10, // Try lazily allocated mode.
//     MUTABLE_VIEW_FORMAT = 0x40, // texture view as different format
//     MUTABLE_STORAGE = 0x80, // mutable storage for gl
// }


export enum Format {

    UNKNOWN,

    A8,
    L8,
    LA8,

    R8,
    R8SN,
    R8UI,
    R8I,
    R16F,
    R16UI,
    R16I,
    R32F,
    R32UI,
    R32I,

    RG8,
    RG8SN,
    RG8UI,
    RG8I,
    RG16F,
    RG16UI,
    RG16I,
    RG32F,
    RG32UI,
    RG32I,

    RGB8,
    SRGB8,
    RGB8SN,
    RGB8UI,
    RGB8I,
    RGB16F,
    RGB16UI,
    RGB16I,
    RGB32F,
    RGB32UI,
    RGB32I,

    RGBA8,
    BGRA8,
    SRGB8_A8,
    RGBA8SN,
    RGBA8UI,
    RGBA8I,
    RGBA16F,
    RGBA16UI,
    RGBA16I,
    RGBA32F,
    RGBA32UI,
    RGBA32I,

    // Special Format
    R5G6B5,
    R11G11B10F,
    RGB5A1,
    RGBA4,
    RGB10A2,
    RGB10A2UI,
    RGB9E5,

    // Depth-Stencil Format
    DEPTH,
    DEPTH_STENCIL,

    // Compressed Format

    // Block Compression Format, DDS (DirectDraw Surface)
    // DXT1: 3 channels (5:6:5), 1/8 original size, with 0 or 1 bit of alpha
    BC1,
    BC1_ALPHA,
    BC1_SRGB,
    BC1_SRGB_ALPHA,
    // DXT3: 4 channels (5:6:5), 1/4 original size, with 4 bits of alpha
    BC2,
    BC2_SRGB,
    // DXT5: 4 channels (5:6:5), 1/4 original size, with 8 bits of alpha
    BC3,
    BC3_SRGB,
    // 1 channel (8), 1/4 original size
    BC4,
    BC4_SNORM,
    // 2 channels (8:8), 1/2 original size
    BC5,
    BC5_SNORM,
    // 3 channels (16:16:16), half-floating point, 1/6 original size
    // UF16: unsigned float, 5 exponent bits + 11 mantissa bits
    // SF16: signed float, 1 signed bit + 5 exponent bits + 10 mantissa bits
    BC6H_UF16,
    BC6H_SF16,
    // 4 channels (4~7 bits per channel) with 0 to 8 bits of alpha, 1/3 original size
    BC7,
    BC7_SRGB,

    // Ericsson Texture Compression Format
    ETC_RGB8,
    ETC2_RGB8,
    ETC2_SRGB8,
    ETC2_RGB8_A1,
    ETC2_SRGB8_A1,
    ETC2_RGBA8,
    ETC2_SRGB8_A8,
    EAC_R11,
    EAC_R11SN,
    EAC_RG11,
    EAC_RG11SN,

    // PVRTC (PowerVR)
    PVRTC_RGB2,
    PVRTC_RGBA2,
    PVRTC_RGB4,
    PVRTC_RGBA4,
    PVRTC2_2BPP,
    PVRTC2_4BPP,

    // ASTC (Adaptive Scalable Texture Compression)
    ASTC_RGBA_4X4,
    ASTC_RGBA_5X4,
    ASTC_RGBA_5X5,
    ASTC_RGBA_6X5,
    ASTC_RGBA_6X6,
    ASTC_RGBA_8X5,
    ASTC_RGBA_8X6,
    ASTC_RGBA_8X8,
    ASTC_RGBA_10X5,
    ASTC_RGBA_10X6,
    ASTC_RGBA_10X8,
    ASTC_RGBA_10X10,
    ASTC_RGBA_12X10,
    ASTC_RGBA_12X12,

    // ASTC (Adaptive Scalable Texture Compression) SRGB
    ASTC_SRGBA_4X4,
    ASTC_SRGBA_5X4,
    ASTC_SRGBA_5X5,
    ASTC_SRGBA_6X5,
    ASTC_SRGBA_6X6,
    ASTC_SRGBA_8X5,
    ASTC_SRGBA_8X6,
    ASTC_SRGBA_8X8,
    ASTC_SRGBA_10X5,
    ASTC_SRGBA_10X6,
    ASTC_SRGBA_10X8,
    ASTC_SRGBA_10X10,
    ASTC_SRGBA_12X10,
    ASTC_SRGBA_12X12,

    // Total count
    COUNT,
}

export enum TextureWrapMode {
    NONE,
    REPEAT,
    MIRROR_REPEAT,
    CLAMP_EDGE
}


export enum TextureFilter {
    NONE,
    LINEAR,
    NEAREST,
    NEAREST_MIPMAP_NEAREST,
    NEAREST_MIPMAP_LINEAR,
    LINEAR_MIPMAP_LINEAR
}

export enum PrimitiveMode {
    POINT_LIST,
    LINE_LIST,
    LINE_STRIP,
    LINE_LOOP,
    LINE_LIST_ADJACENCY,
    LINE_STRIP_ADJACENCY,
    ISO_LINE_LIST,
    TRIANGLE_LIST,
    TRIANGLE_STRIP,
    TRIANGLE_FAN,
    TRIANGLE_LIST_ADJACENCY,
    TRIANGLE_STRIP_ADJACENCY,
    TRIANGLE_PATCH_ADJACENCY,
    QUAD_PATCH_LIST,
}

export interface Uniform {
    name: string;
    type: Type;
    count: number;
}

export enum Type {
    UNKNOWN,
    BOOL,
    BOOL2,
    BOOL3,
    BOOL4,
    INT,
    INT2,
    INT3,
    INT4,
    UINT,
    UINT2,
    UINT3,
    UINT4,
    FLOAT,
    FLOAT2,
    FLOAT3,
    FLOAT4,
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
    COUNT,
}

const _type2size = [
    0,  // UNKNOWN
    4,  // BOOL
    8,  // BOOL2
    12, // BOOL3
    16, // BOOL4
    4,  // INT
    8,  // INT2
    12, // INT3
    16, // INT4
    4,  // UINT
    8,  // UINT2
    12, // UINT3
    16, // UINT4
    4,  // FLOAT
    8,  // FLOAT2
    12, // FLOAT3
    16, // FLOAT4
    16, // MAT2
    24, // MAT2X3
    32, // MAT2X4
    24, // MAT3X2
    36, // MAT3
    48, // MAT3X4
    32, // MAT4X2
    48, // MAT4X3
    64, // MAT4
    4,  // SAMPLER1D
    4,  // SAMPLER1D_ARRAY
    4,  // SAMPLER2D
    4,  // SAMPLER2D_ARRAY
    4,  // SAMPLER3D
    4,  // SAMPLER_CUBE
];

export function GetTypeSize(type: Type): number {
    return _type2size[type] || 0;
}


export enum MemoryAccess {
    NONE = 0,
    READ_ONLY = 0x1,
    WRITE_ONLY = 0x2,
    READ_WRITE = READ_ONLY | WRITE_ONLY,
}


export enum API {
    UNKNOWN,
    GLES2,
    GLES3,
    VULKAN,
    WEBGL,
    WEBGL2,
    WEBGPU
}

export interface GPUObject { }

export interface GPUShader extends GPUObject {
    nativePointer: number;
}

export enum ClearFlags {
    NONE = 0,
    COLOR = 0x1,
    DEPTH = 0x2,
    STENCIL = 0x4,
    DEPTH_STENCIL = DEPTH | STENCIL,
    ALL = COLOR | DEPTH | STENCIL,
}


export enum ColorMask {
    NONE = 0x0,
    R = 0x1,
    G = 0x2,
    B = 0x4,
    A = 0x8,
    ALL = R | G | B | A,
};

/**
 * 混合因子
 */
export enum BlendFactor {
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

export enum BlendOp {
    ADD,
    SUB,
    REV_SUB,
    MIN,
    MAX
};

export enum AttachmentOp {

    /**
     * 保留原样,不做任何操作
     */
    LOAD,

    /**
     * 清除原有数据，使用Camera上的值填充
     */
    CLEAR,

    DISCARD
};

export interface BlendTarget {
    /**
     * 是否启用混合，默认true
     */
    blend?: boolean;

    /**
     * 设置源颜色的源因子
     */
    blendSrc?: BlendFactor; //默认BlendFactor:: SRC_ALPHA
    blendSrcAlpha?: BlendFactor; //默认BlendFactor:: SRC_ALPHA };

    /**
     * 设置目标颜色的目标因子
     */
    blendDst?: BlendFactor; //默认 BlendFactor:: ONE_MINUS_SRC_ALPHA };
    blendDstAlpha?: BlendFactor;// 默认BlendFactor:: ONE_MINUS_SRC_ALPHA };

    /**
     * 设置混合方程
     */
    blendEq?: BlendOp; // 设置RGB分量混合方程。默认是BlendOp:: ADD };
    blendAlphaEq?: BlendOp;// 设置Aplha混合方程 。默认是BlendOp:: ADD };

    /**
     * 设置当前颜色哪个分量可以写入到帧缓冲区中
     */
    blendColorMask?: ColorMask;//{ gfx:: ColorMask:: ALL };
};


export enum BlendMode {
    ADD,
    SUB,
    MIX,
    MUL,
    ALPHA_TO_COVERAGE,
    PREMULTIPLIED_ALPHA
}

export interface BlendState {
    blendColor?: Color;

    /**
     * blendTarget和blendMode选择其中一个，blendTarget优先
     */
    blendMode?: BlendMode;
    blendTarget?: BlendTarget;
};
export interface RasterizerState { }
export interface DepthStencilState { }

export enum ComparisonFunc {
    NEVER,
    LESS,
    EQUAL,
    LESS_EQUAL,
    GREATER,
    NOT_EQUAL,
    GREATER_EQUAL,
    ALWAYS,
};

export enum StencilOp {
    ZERO,
    KEEP,
    REPLACE,
    INCR,
    DECR,
    INVERT,
    INCR_WRAP,
    DECR_WRAP,
};
