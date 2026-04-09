#ifndef _GFX_DEFINE_H_
#define _GFX_DEFINE_H_

#include <vector>
#include "common-define.h"
#include <string>
#include "../../core/core.h"
#include <optional>

/**
 * 本文件为对外定义：当创建图形Object时所需要的描述数据结构
 */

// 使用纹理的方式定义数据类型
enum class Format
{
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
    COUNT
};
SQ_ENUM_CONVERSION_OPERATOR(Format);

int GetFormatSize(Format);

void formatAlignment(Format format, int &width, int &height);

// 计算返回alignment的倍数，如果size已经是alignment的倍数，则直接返回size
int alignTo(int size, int alignment);

// 获取纹理大小，返回字节大小
int formatTexSize(Format format, int width, int height, int depth);

struct FormatInfo
{
    int size;
    int count;
};

extern FormatInfo FormatInfos[116];

//===========================================

/**
 * Buffer Object缓冲区的修改标记，OpenGL定义的内存修改方式
 * glBufferData等方法中的usage参数
 */
enum class BufferUsage
{
    NONE = 0,
    DYNAMIC_DRAW = 1,
    STATIC_DRAW = 2
};

/**
 * Buffer Object的类型
 */
enum class BufferType
{
    NONE = 0,
    TRANSFER_SRC = 0x1,
    TRANSFER_DST = 0x2,
    INDEX = 0x4,
    VERTEX = 0x8,
    UNIFORM = 0x10,
    STORAGE = 0x20,
    INDIRECT = 0x40,
};

/**
 * OpenGL允许着色器中声明的变量的数据了下和真实设置到vertexAttribPointer不一致的
 * 使用自定义的可以实现一下特殊的效果，比如color使用一个数字表示，但在着色器中是vec4,省内存
 */
struct AttributeCustomSetting
{

    int offset = 0;

    // 数字个数
    int count = 0;

    // 字节个数
    int stride = 0;

    Format type;

    // 如果大于0，表示在多实例绘制中对vertexAttribDivisor的设置
    int instanceStride = 0;
};

/**
 * 代表这顶点着色器的定时属性的顶用
 * 每一个顶点属性就是一个Attribute实例
 */
struct Attribute
{

    std::string name;

    // 使用TextureFormat來表示
    // 可以表示数据类型和数组，比如RG32F 可以表示为vec2
    // 使用这个可以方便顶点属性分配
    Format format;

    /**
     * 1、默认的情况下，会统计Shader所有的Attribute，然后认为这些Attribute都在一个Buffer中进行分配，
     *    即所有的Attribute共用一个Buffer。会根据Attribute声明的数据类型来进行计算
     *
     * 2、所以，如果你有多个Buffer，每个属性使用不同的Buffer或每个属性有不同的分配，那么你就需要设置
     *    custom来自定义自己的属性分配方式
     */
    std::optional<AttributeCustomSetting> custom;
    Attribute() {};
    Attribute(const std::string &name, Format format) : name(name), format(format)
    {
    }
    Attribute(const std::string &name, Format format, const AttributeCustomSetting &custom) : name(name), format(format), custom(custom)
    {
    }
};

int getAttributeStride(const Attribute &attribute);
int getAttributeStride(const std::vector<Attribute> &attributes);

/**
 * 创建BufferObject需要制定的信息
 */
struct BufferInfo
{
    BufferUsage usage = BufferUsage::NONE;
    BufferType type = BufferType::NONE;
    int size = 0;

    /**
     * 如果设置了这个，则表示当前的Buffer Object不是真正上的OpenGL Buffer Object
     * 它只是映射了一段真正的Buffer Object上的内存。
     */
    sqstd::ByteBlockChunk *bufferView = nullptr;

    /**
     * 如果当前的Buffer为VBO，则必须指定。其他情况为空
     */
    std::vector<Attribute> *attrs = nullptr;

    /**
     * 由于是异步执行模式。
     * 所以对于一些共享的Uniform，比如Global UBO，在多照相机的情况下，投影矩阵总是被最后一次的照相机覆盖
     * 所以，针对这种情况，需要在glUniform*的时候copy到native内存，后续的覆盖就不会有影响之前已经存储在glUniform中的数据了
     */
    bool webglSysToJsByCopy = false;
};

struct UniformBlock;

/**
 * 代表着色器中Uniform变量
 */
struct Uniform
{
    // uniform数据类型
    gfx::Type type;

    // 如果是数组的，则表示数组的大小
    // 比如uniform vec4 a[10]; 则这里count=10
    // 如果不是数组，则必须设置为1
    int count = 1;

    // 在block中的偏移量，字节个数
    int offset = 0;

    std::string name;

    // 这个Uniform位于哪个Block中
    const UniformBlock *block = NULL;
};

struct UniformBlock
{
    int binding = 0;

    // 如果是块数组，则表示这个数组的大小。如果不是数组，则为1
    // 目前还不支持块数组，所以恒定为1
    int count = 1;
    int set;

    std::vector<Uniform> members;

    bool buildin = false;
};

/**
 * 着色器中声明的smapler2D采样器
 */
struct UniformSamplerTexture
{
    std::string name;
    int binding = 0;
    int type;
    int set;

    // 如果是纹理数组的，则记录这个数组的大小
    int count = 0;

    bool buildin = false;
};

struct ShaderInfo
{

    /**
     * Effect资源ID
     */
    int assetId{-1};

    /**
     * 因为一个Effect中可以有多个Shader，这里记录引用Effect中哪个Shader
     * 创建Effect的时候，是第几个IShaderInfo
     * 即在Effect中的顺序
     */
    int shaderIndex{-1};

    // 用于告诉js端使用哪些宏
    // 目前只能支持32个宏，因为这个flag只能存储32位，看看后面怎么支持更加多
    unsigned int macroFlags{0};

    std::vector<Attribute> attributes;
    std::vector<const UniformBlock *> blocks;
    std::vector<UniformSamplerTexture *> samplerTextures;
};

enum class ShaderStage
{
    NONE = 0x0,
    VERTEX = 0x1,
    CONTROL = 0x2,
    EVALUATION = 0x4,
    GEOMETRY = 0x8,
    FRAGMENT = 0x10,
    COMPUTE = 0x20,
    ALL = 0x3f,
};

/**
 * DescriptorSetLayoutBinding指定的Uniform的类型
 */
enum class DescriptorType
{
    UNKNOWN = 0,

    // 表示Uniform块
    UNIFORM_BUFFER = 0x1,
    DYNAMIC_UNIFORM_BUFFER = 0x2,
    STORAGE_BUFFER = 0x4,
    DYNAMIC_STORAGE_BUFFER = 0x8,

    // 表示sampler2D类型
    SAMPLER_TEXTURE = 0x10
};

struct DescriptorSetLayoutBinding
{

    /**
     * Uniform变量或Uniform block在着色器中的位置，通过这个位置才能设置Uniform的值
     */
    int binding{0};

    // 如果为数组的话，则表示数组的大小
    // 比如说"uniform sampler2D textures[10];"则这里的count为10
    // 如果是Uniform Block，由于块不支持数组，则固定为1
    int count{0};

    // Uniform的类型
    DescriptorType descriptorType{DescriptorType::UNKNOWN};
};

struct DescriptorSetLayoutInfo
{
    std::vector<const DescriptorSetLayoutBinding *> *bindings = NULL;
};

class DescriptorSetLayout;
struct DescriptorSetInfo
{
    DescriptorSetLayout *layout = NULL;
};

//========================渲染状态的定义==============
enum class ClearFlags
{
    NONE = 0,
    COLOR = 0x1,
    DEPTH = 0x2,
    STENCIL = 0x4,
    DEPTH_STENCIL = DEPTH | STENCIL,
    ALL = COLOR | DEPTH | STENCIL,
};

enum class CullMode
{
    NONE,
    FRONT,
    BACK
};
SQ_ENUM_CONVERSION_OPERATOR(CullMode);

enum class ComparisonFunc : int
{
    NEVER,
    LESS,
    EQUAL,
    LESS_EQUAL,
    GREATER,
    NOT_EQUAL,
    GREATER_EQUAL,
    ALWAYS,
};
SQ_ENUM_CONVERSION_OPERATOR(ComparisonFunc);

enum class StencilOp : int
{
    ZERO,
    KEEP,
    REPLACE,
    INCR,
    DECR,
    INVERT,
    INCR_WRAP,
    DECR_WRAP,
};
SQ_ENUM_CONVERSION_OPERATOR(StencilOp);

class DepthState
{
public:
    // 是否开启深度测试
    bool depthTest = false;

    // 是否开启深度写入
    bool depthWrite = false;

    // 深度测试的比较函数
    ComparisonFunc depthFunc = ComparisonFunc::LESS;
    sqstd::hash_t serializeHash();
};

struct StencilState
{

    // 绘制正面时使用的模板设置
    bool stencilTestFront = false;
    ComparisonFunc stencilFuncFront = ComparisonFunc::ALWAYS;
    unsigned int stencilReadMaskFront{0xffffffff};
    unsigned int stencilWriteMaskFront{0xffffffff};
    StencilOp stencilFailOpFront = StencilOp::KEEP;
    StencilOp stencilZFailOpFront = StencilOp::KEEP;
    StencilOp stencilPassOpFront = StencilOp::KEEP;
    unsigned int stencilRefFront{0xffffffff};

    // 绘制背面时使用的模板设置
    bool stencilTestBack = false;
    ComparisonFunc stencilFuncBack = ComparisonFunc::ALWAYS;
    unsigned int stencilRefBack{0xffffffff};
    unsigned int stencilReadMaskBack{0xffffffff};
    unsigned int stencilWriteMaskBack{0xffffffff};
    StencilOp stencilFailOpBack = StencilOp::KEEP;
    StencilOp stencilZFailOpBack = StencilOp::KEEP;
    StencilOp stencilPassOpBack = StencilOp::KEEP;
    sqstd::hash_t serializeHash();
};

class RasterizerState
{
public:
    CullMode cullMode = CullMode::NONE;

    // 绘制三角形的时候，是按顺序还是逆时针取顶点
    bool isFrontFaceCCW{true};

    // 这两个属性为多边形偏移的设置
    float depthBias{0.0};
    float depthBiasSlop{0.0};

    // 如果当前绘制的线条，则设置线条的宽度
    float lineWidth{0.0};

    sqstd::hash_t serializeHash();
};

struct BlendTarget
{
    /**
     * 是否启用混合
     */
    bool blend{false};

    /**
     * 设置源颜色的源因子
     */
    gfx::BlendFactor blendSrc{gfx::BlendFactor::SRC_ALPHA};
    gfx::BlendFactor blendSrcAlpha{gfx::BlendFactor::SRC_ALPHA};

    /**
     * 设置目标颜色的目标因子
     */
    gfx::BlendFactor blendDst{gfx::BlendFactor::ONE_MINUS_SRC_ALPHA};
    gfx::BlendFactor blendDstAlpha{gfx::BlendFactor::ONE_MINUS_SRC_ALPHA};

    /**
     * 设置混合方程
     */
    gfx::BlendOp blendEq{gfx::BlendOp::ADD};      // 颜色混合方程
    gfx::BlendOp blendAlphaEq{gfx::BlendOp::ADD}; // 透明度混合方程

    /**
     * 设置当前颜色哪个分量可以写入到帧缓冲区中
     * 使用gfx::ColorMask 枚举的多个合成员合成的值
     */
    unsigned int blendColorMask{toNumber(gfx::ColorMask::ALL)};
};

enum BlendMode
{
    ADD,
    SUB,
    MIX,
    MUL,
    ALPHA_TO_COVERAGE,
    PREMULTIPLIED_ALPHA
};
void SetBlendMode(BlendMode model, BlendTarget &);

class BlendState
{
public:
    Color blendColor; // 设置gl.blendColor
    BlendTarget blendTarget;
    sqstd::hash_t serializeHash();
};

//======================Texture====================

enum class TextureType
{
    TEX1D,
    TEX2D,
    TEX3D,
    CUBE,
    TEX1D_ARRAY,
    TEX2D_ARRAY
};

// enum class TextureUsage
// {
//     NONE = 0,
//     TRANSFER_SRC = 0x1,
//     TRANSFER_DST = 0x2,
//     SAMPLED = 0x4,
//     STORAGE = 0x8,
//     COLOR_ATTACHMENT = 0x10,
//     DEPTH_STENCIL_ATTACHMENT = 0x20,
//     INPUT_ATTACHMENT = 0x40,
// };

// enum class TextureFlag
// {
//     NONE = 0,
//     GEN_MIPMAP = 0x1,           // Generate mipmaps using bilinear filter
//     GENERAL_LAYOUT = 0x2,       // For inout framebuffer attachments
//     EXTERNAL_OES = 0x4,         // External oes texture
//     EXTERNAL_NORMAL = 0x8,      // External normal texture
//     LAZILY_ALLOCATED = 0x10,    // Try lazily allocated mode.
//     MUTABLE_VIEW_FORMAT = 0x40, // texture view as different format
//     MUTABLE_STORAGE = 0x80,     // mutable storage for gl
// };

struct TextureInfo
{
    TextureType type{TextureType::TEX2D};
    Format format{Format::UNKNOWN};
    int width{0};
    int height{0};
    // TextureFlag flags = TextureFlag::NONE;
    int levelCount{1};
};

// 纹理参数定义

enum class TextureWrapMode
{
    NONE,
    REPEAT,
    MIRROR_REPEAT,
    CLAMP_EDGE
};

SQ_ENUM_CONVERSION_OPERATOR(TextureWrapMode);

enum class TextureFilter
{
    NONE,
    LINEAR,
    NEAREST,
    NEAREST_MIPMAP_NEAREST,
    NEAREST_MIPMAP_LINEAR,
    LINEAR_MIPMAP_LINEAR
};

SQ_ENUM_CONVERSION_OPERATOR(TextureFilter);

struct SamplerInfo
{
    TextureFilter minFilter{TextureFilter::LINEAR};
    TextureFilter magFilter{TextureFilter::LINEAR};
    TextureWrapMode wrapS{TextureWrapMode::CLAMP_EDGE};
    TextureWrapMode wrapT{TextureWrapMode::CLAMP_EDGE};
    TextureWrapMode wrapR{TextureWrapMode::CLAMP_EDGE};
    SamplerInfo() {};
    SamplerInfo(TextureFilter minFilter, TextureFilter magFilter) : minFilter(minFilter), magFilter(magFilter) {};
};

struct BufferTextureCopyRegion
{
    // 由于该引擎的方法是C++与JS是异步交互的，这里的buffer可能是栈内存的(超出作用域就被回收了)
    // 所以需要将栈内存copy到异步交互的缓冲区中
    bool buffCopyToNative{false};

    // 这里都是按照像素单位来指定的，不是字节
    uint32_t texOffsetX{0};
    uint32_t texOffsetY{0};
    uint32_t texWidth{0};
    uint32_t texHeight{0};
};

//--------------------
enum class PrimitiveMode
{
    POINT_LIST,
    LINE_LIST,
    LINE_STRIP,
    LINE_LOOP,
    LINE_LIST_ADJACENCY,
    LINE_STRIP_ADJACENCY,
    ISO_LINE_LIST,
    // raycast detectable:
    TRIANGLE_LIST,
    TRIANGLE_STRIP,
    TRIANGLE_FAN,
    TRIANGLE_LIST_ADJACENCY,
    TRIANGLE_STRIP_ADJACENCY,
    TRIANGLE_PATCH_ADJACENCY,
    QUAD_PATCH_LIST,
};
SQ_ENUM_CONVERSION_OPERATOR(PrimitiveMode);
#endif