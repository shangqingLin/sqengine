#include "buildin-block-define.h"

namespace pipeline
{
    // UBOCamera
    const std::string UBOCamera::NAME = "CCCamera";
    const PipelineGlobalBindings UBOCamera::BINDING = PipelineGlobalBindings::UBO_CAMERA;
    const DescriptorSetLayoutBinding UBOCamera::DESCRIPTOR = {
        toNumber(PipelineGlobalBindings::UBO_CAMERA),
        1,
        DescriptorType::UNIFORM_BUFFER};
    const UniformBlock UBOCamera::LAYOUT = {
        toNumber(PipelineGlobalBindings::UBO_CAMERA),
        1,
        toNumber(SetIndex::GLOBAL),
        {{gfx::Type::MAT4,
          1,
          0,
          "cc_matViewProj",
          NULL}},
        true};

    // UBOGlobal
    const std::string UBOGlobal::NAME = "CCGlobal";
    const PipelineGlobalBindings UBOGlobal::BINDING = PipelineGlobalBindings::UBO_GLOBAL;
    const DescriptorSetLayoutBinding UBOGlobal::DESCRIPTOR = {
        toNumber(PipelineGlobalBindings::UBO_GLOBAL),
        1,
        DescriptorType::UNIFORM_BUFFER};
    const UniformBlock UBOGlobal::LAYOUT = {
        toNumber(PipelineGlobalBindings::UBO_GLOBAL),
        1,
        toNumber(SetIndex::GLOBAL),
        {
            // 游戏帧时间
            {
                gfx::Type::FLOAT,
                1,
                0,
                "cc_time"},

            // 游戏从开始一直累加的时间
            {
                gfx::Type::FLOAT,
                1,
                4,
                "sq_gameDuration"},

            // 屏幕分辨率
            {
                gfx::Type::FLOAT2,
                1,
                8,
                "cc_screenSize"},
        },
        true};

    // LOCAL_SPRITE_TEXTURE
    const std::string LOCAL_SPRITE_TEXTURE::NAME = "cc_spriteTexture";
    const ModelLocalBindings LOCAL_SPRITE_TEXTURE::BINDING = ModelLocalBindings::SAMPLER_SPRITE;
    const DescriptorSetLayoutBinding LOCAL_SPRITE_TEXTURE::DESCRIPTOR = {
        toNumber(ModelLocalBindings::SAMPLER_SPRITE),
        10,
        DescriptorType::SAMPLER_TEXTURE};
    const UniformSamplerTexture LOCAL_SPRITE_TEXTURE::LAYOUT = {
        "cc_spriteTexture",
        toNumber(ModelLocalBindings::SAMPLER_SPRITE),
        0,
        toNumber(SetIndex::LOCAL),
        10,
        true};

    // UBOLocal
    const std::string UBOLocal::NAME = "CCLocal";
    const int UBOLocal::SIZE = 64;
    const ModelLocalBindings UBOLocal::BINDING = ModelLocalBindings::UBO_LOCAL;
    const DescriptorSetLayoutBinding UBOLocal::DESCRIPTOR = {
        toNumber(ModelLocalBindings::UBO_LOCAL),
        1,
        DescriptorType::UNIFORM_BUFFER};
    const UniformBlock UBOLocal::LAYOUT = {
        toNumber(ModelLocalBindings::UBO_LOCAL),
        1,
        toNumber(SetIndex::LOCAL),
        {{gfx::Type::MAT4,
          1,
          0,
          "cc_matWorld",
          NULL}},
        true};

    std::unordered_map<std::string, const UniformBlock *> globalBlockMap;
    std::unordered_map<std::string, const UniformBlock *> localBlockMap;
    std::unordered_map<std::string, UniformSamplerTexture *> localSamplerMap;
    std::vector<const DescriptorSetLayoutBinding *> localSetLayoutBindings;

} // namespace pipeline
