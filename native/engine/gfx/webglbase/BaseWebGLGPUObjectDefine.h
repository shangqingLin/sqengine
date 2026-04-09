#ifndef _BASE_WEBGL_GPU_OBJECT_DEFINE_H_
#define _BASE_WEBGL_GPU_OBJECT_DEFINE_H_
#include "../../core/common/ArrayBuffer.h"
#include "../../core/math/Rect.h"
#include "../../core/sqstd/Array.h"
#include "../base/SamplerObject.h"
#include "../base/RenderPass.h"
#include "../base/define.h"
#include "WebGLAPIDefine.h"
#include <optional>

///=================着色器中存储的 Shader 信息====================
struct IGLGPUUniform
{
    unsigned short blockIndex{0};
    unsigned short memberIndex{0};
    unsigned short location{0};
    // 在ubo中数据的偏移
    unsigned int offset{0};

    // 缓存Uniform设置的值
    sqstd::ByteBlockChunk *cache{nullptr};
    bool dataInit{false};
};

struct IGLGPUUniformSamplerTexture
{
    // 通过index获取Effect上着色器的信息
    unsigned short index{0};
    unsigned short location{0};

    // 当前这个纹理绑定到哪个纹理单元上
    // 支持纹理数组
    std::vector<int> texUnits;
};

struct IGLGPUAttribute
{

    // 与着色器资源中关联的
    Attribute *attribute{nullptr};
    unsigned short location{0};
    unsigned short count{0};
    unsigned int stride{0};
    unsigned int size{0};
    unsigned int offset{0};
    Format type;
};

struct IGLGPUShader
{
    GLAPI::GLProgram glProgram = 0;

    // 在Effect资源中的顺序
    int shaderIndex{-1};
    int effectAssetId{-1};
    unsigned int macroFlags{0};

    // 在effect资源中着色器的信息
    std::vector<Attribute> *attributes{nullptr};
    std::vector<const UniformBlock *> *blocks{nullptr};
    std::vector<UniformSamplerTexture *> *samplerTextures{nullptr};

    // 编译着色器之后的信息。
    // 着色器编译之后可能由于宏、Uniform没有被使用等原因，真正使用的Uniform和在effect中声明的uniform个数是不一样的
    std::vector<IGLGPUUniform> glActiveUniforms;
    // 缓存所有的Uniform的值，如果一直不变就不需要去调用API设置，优化！！
    sqstd::ByteBlock *uniformBuffer{nullptr};

    std::vector<IGLGPUUniformSamplerTexture> glSamplerTextures;
    std::vector<IGLGPUAttribute> glActiveAttributes;

    // 使用多少个纹理单元
    int numTexUnitUse{0};
};

//===========================end==================================================

struct IGLGPUSamplerObject
{
    // WebGL2.0才有
    // GLAPI::GLSampler sampler{0};
    int sampler{0};

    // SamplerObject中的hash值
    unsigned int hash{0};
};

struct IGLGPUTexture
{
    TextureType type;

    int width{0};
    int height{0};
    int mipLevel{0};
    int size = 0;
    bool isPowerOf2 = false;

    GLAPI::GLenum glTarget;

    // 内部格式
    Format glInternalFmt;

    // 外部格式
    Format glFormat;

    // 外部格式中像素的数据类型
    Format glType;

    int glUsage = 0;

    GLAPI::GLTexture glTexture = 0;

    /**
     * 表示这张是一个用于存储RBO的纹理
     * 用于DEPTH_STENCIL 绑定点的纹理，但不是真正的纹理，这些绑定点本来就不需要纹理，但引擎为了和颜色绑定点
     * 统一，所以所有都绑定点都可以有一张纹理。但对于DEPTH_STENCIL绑定点来说纹理可以有，也可以没有
     * 即glTexture为空
     */
    GLAPI::GLenum glRenderbuffer = 0;

    // 当前纹理绑定到的纹理单元
    //-1表示还没有绑定
    int texUnit = -1;

    // 当前的纹理所使用的SamplerObject
    IGLGPUSamplerObject *cacheBindSampler{nullptr};
};

struct IGLGPUBuffer
{

    BufferType type = BufferType::NONE;

    BufferUsage memUsage = BufferUsage::NONE;

    // 缓冲区大小
    int size{0};

    // 作为结果返回
    GLAPI::GLenum glTarget{0};
    GLAPI::GLBuffer glBuffer{0};

    /**
     *如果设置了bufferView，则表示这个Buffer Object不是真正的OpenGL Buffer Object
     * 只是映射了ArrayBuffer中的一段内存
     * bufferView和buffer只能选择其中一个
     */
    sqstd::ByteBlockChunk *bufferView = nullptr;
    sqstd::Byte *buffer = nullptr;

    /**
     * 由于是异步执行模式。
     * 所以对于一些共享的Uniform，比如Global UBO，在多照相机的情况下，投影矩阵总是被最后一次的照相机覆盖
     * 所以，针对这种情况，需要在glUniform*的时候copy到native内存，后续的覆盖就不会有影响之前已经存储在glUniform中的数据了
     */
    bool sysToJsByCopy = false;
};

struct IGLGPUFrameBuffer
{
    std::vector<IGLGPUTexture> colorTextures;
    std::optional<IGLGPUTexture> depthStencilTexture;
    GLAPI::GLFramebuffer glFramebuffer = 0;
};

/**
 * 保存渲染管线的当前状态的值
 * 用于实现判断如果值没有改变，则不需要调用图形API，减少图形API的调用
 * 提升性能
 */
class GLGPUStateCache
{
public:
    PrimitiveMode primitive;
    GLAPI::GLuint glArrayBuffer = 0;
    GLAPI::GLuint glElementArrayBuffer = 0;
    GLAPI::GLuint glUniformBuffer = 0;
    GLAPI::GLVAO glVAO = 0;
    sqstd::Array<GLAPI::GLTexture> texUintBind;
    sqstd::Array<GLAPI::GLenum> texUintBindTarget;
    sqstd::Array<GLAPI::GLuint> glSamplers;
    GLAPI::GLProgram glProgram = 0;
    GLAPI::GLuint glFramebuffer = 0;
    gfx::RenderPass *renderPass = nullptr;
    GLAPI::GLuint glRenderbuffer = 0;
    Rect<float> viewport;
    Rect<float> scissor;
    RasterizerState rs;
    DepthState ds;
    StencilState ss;
    BlendState bs;

    bool isCullFaceEnabled = true;
    bool isStencilTestEnabled = false;
};

#endif