
// 扩展支持的枚举
export enum WebGL2EXT {
    COMPRESSED_RGB_S3TC_DXT1_EXT = 0x83F0,
    COMPRESSED_RGBA_S3TC_DXT1_EXT = 0x83F1,
    COMPRESSED_RGBA_S3TC_DXT3_EXT = 0x83F2,
    COMPRESSED_RGBA_S3TC_DXT5_EXT = 0x83F3,

    COMPRESSED_SRGB_S3TC_DXT1_EXT = 0x8C4C,
    COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT = 0x8C4D,
    COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT = 0x8C4E,
    COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT = 0x8C4F,

    COMPRESSED_RGB_PVRTC_4BPPV1_IMG = 0x8C00,
    COMPRESSED_RGB_PVRTC_2BPPV1_IMG = 0x8C01,
    COMPRESSED_RGBA_PVRTC_4BPPV1_IMG = 0x8C02,
    COMPRESSED_RGBA_PVRTC_2BPPV1_IMG = 0x8C03,

    COMPRESSED_RGB_ETC1_WEBGL = 0x8D64,

    COMPRESSED_R11_EAC = 0x9270,
    COMPRESSED_SIGNED_R11_EAC = 0x9271,
    COMPRESSED_RG11_EAC = 0x9272,
    COMPRESSED_SIGNED_RG11_EAC = 0x9273,
    COMPRESSED_RGB8_ETC2 = 0x9274,
    COMPRESSED_SRGB8_ETC2 = 0x9275,
    COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2 = 0x9276,
    COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2 = 0x9277,
    COMPRESSED_RGBA8_ETC2_EAC = 0x9278,
    COMPRESSED_SRGB8_ALPHA8_ETC2_EAC = 0x9279,

    COMPRESSED_RGBA_ASTC_4x4_KHR = 0x93B0,
    COMPRESSED_RGBA_ASTC_5x4_KHR = 0x93B1,
    COMPRESSED_RGBA_ASTC_5x5_KHR = 0x93B2,
    COMPRESSED_RGBA_ASTC_6x5_KHR = 0x93B3,
    COMPRESSED_RGBA_ASTC_6x6_KHR = 0x93B4,
    COMPRESSED_RGBA_ASTC_8x5_KHR = 0x93B5,
    COMPRESSED_RGBA_ASTC_8x6_KHR = 0x93B6,
    COMPRESSED_RGBA_ASTC_8x8_KHR = 0x93B7,
    COMPRESSED_RGBA_ASTC_10x5_KHR = 0x93B8,
    COMPRESSED_RGBA_ASTC_10x6_KHR = 0x93B9,
    COMPRESSED_RGBA_ASTC_10x8_KHR = 0x93BA,
    COMPRESSED_RGBA_ASTC_10x10_KHR = 0x93BB,
    COMPRESSED_RGBA_ASTC_12x10_KHR = 0x93BC,
    COMPRESSED_RGBA_ASTC_12x12_KHR = 0x93BD,

    COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR = 0x93D0,
    COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR = 0x93D1,
    COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR = 0x93D2,
    COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR = 0x93D3,
    COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR = 0x93D4,
    COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR = 0x93D5,
    COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR = 0x93D6,
    COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR = 0x93D7,
    COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR = 0x93D8,
    COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR = 0x93D9,
    COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR = 0x93DA,
    COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR = 0x93DB,
    COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR = 0x93DC,
    COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR = 0x93DD,
}

export interface IWebGL2Extensions {
    EXT_texture_filter_anisotropic: EXT_texture_filter_anisotropic | null;
    EXT_color_buffer_half_float: EXT_color_buffer_half_float | null;
    EXT_color_buffer_float: EXT_color_buffer_float | null;
    WEBGL_compressed_texture_etc1: WEBGL_compressed_texture_etc1 | null;
    WEBGL_compressed_texture_etc: WEBGL_compressed_texture_etc | null;
    WEBGL_compressed_texture_pvrtc: WEBGL_compressed_texture_pvrtc | null;
    WEBGL_compressed_texture_astc: WEBGL_compressed_texture_astc | null;
    WEBGL_compressed_texture_s3tc: WEBGL_compressed_texture_s3tc | null;
    WEBGL_compressed_texture_s3tc_srgb: WEBGL_compressed_texture_s3tc_srgb | null;
    WEBGL_debug_shaders: WEBGL_debug_shaders | null;
    WEBGL_lose_context: WEBGL_lose_context | null;
    WEBGL_debug_renderer_info: WEBGL_debug_renderer_info | null;
    WEBGL_color_buffer_float:WEBGL_color_buffer_float | null;
    OES_texture_half_float_linear: OES_texture_half_float_linear | null;
    OES_texture_float_linear: OES_texture_float_linear | null;
    PARALLEL_SHADER_COMPLIE: KHR_parallel_shader_compile | null;
    EXT_float_blend:EXT_float_blend | null;
      //@ts-ignore
    EXT_disjoint_timer_query_webgl2 : EXT_disjoint_timer_query_webgl2 | null
    useVAO: boolean;
}