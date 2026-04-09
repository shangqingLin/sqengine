#include "ProgramLib.h"
#include "../rendering/buildin-block-define.h"
#include <sstream>

static ProgramLib *_ins = new ProgramLib();

//==========================

int ITemplateInfo::findMacroFlag(const std::string &macroName) const
{
    for (int i = 0; i < macros.size(); ++i)
    {
        // printf(" +++++++ findMacroFlag %d %s \n",i,macros[i].marcoName.c_str());
        if (macros[i].marcoName == macroName)
        {
            return macros[i].flag;
        }
    }
    return -1;
}

ITemplateInfo::~ITemplateInfo()
{

    std::unordered_map<sqstd::hash_t, Shader *>::iterator shaderIt = shaders.begin();
    while (shaderIt != shaders.end())
    {
        delete shaderIt->second;
        ++shaderIt;
    }
    shaders.clear();

    for (int i = 0; i < shaderInfo.blocks.size(); ++i)
    {
        if (!shaderInfo.blocks[i]->buildin)
        {
            delete shaderInfo.blocks[i];
        }
    }
    shaderInfo.blocks.clear();

    for (int i = 0; i < shaderInfo.samplerTextures.size(); ++i)
    {
        if (!shaderInfo.samplerTextures[i]->buildin)
        {
            delete shaderInfo.samplerTextures[i];
        }
    }
    shaderInfo.samplerTextures.clear();

    for (int i = 0; i < bindings.size(); ++i)
    {
        delete bindings[i];
    }
    bindings.clear();
}
//=================================

ProgramLib *ProgramLib::getInstnace()
{
    return _ins;
}

ITemplateInfo *ProgramLib::define(IShaderDefine *shaderInfo, EffectAsset *asset, int shaderIndex)
{

    // std::map<double, ITemplateInfo *>::iterator it = shaderMap.find(shaderInfo->name);
    // if (it != shaderMap.end())
    // {
    //     return;
    // }

    ITemplateInfo *templateInfo = new ITemplateInfo();
    templateInfo->shaderInfo.assetId = asset->getId();
    templateInfo->shaderInfo.shaderIndex = shaderIndex;

    sqstd::hash_t key = ProgramLib::getShaderKey(asset->getId(), shaderIndex);
    shaderMap[key] = templateInfo;

    if (shaderInfo->macros.size() > 0)
    {
        templateInfo->macros.resize(shaderInfo->macros.size());
        std::unordered_map<int, std::string>::iterator it = shaderInfo->macros.begin();
        while (it != shaderInfo->macros.end())
        {
            // printf("register macro %d %s \n",shaderInfo->macros.size(), it->second.c_str());
            templateInfo->macros.emplace_back(ITemplateInfo::MacroInfo(it->first, it->second));
            ++it;
        }
    }

    int blockSize = shaderInfo->blocks.size();

    if (blockSize > 0)
    {
        templateInfo->shaderInfo.blocks.reserve(blockSize);
        for (int i = 0; i < blockSize; ++i)
        {
            IBlockInfo &block = *shaderInfo->blocks[i];
            if (block.buildin)
            {
                auto infoIt = pipeline::globalBlockMap.find(block.name);
                if (infoIt != pipeline::globalBlockMap.end())
                {
                    templateInfo->shaderInfo.blocks.push_back(infoIt->second);
                    continue;
                }

                auto infoIt2 = pipeline::localBlockMap.find(block.name);

                // printf("block name %s \n",block.name.c_str());

                if (infoIt2 != pipeline::localBlockMap.end())
                {
                    templateInfo->shaderInfo.blocks.push_back(infoIt2->second);
                    continue;
                }

                SQ_ASSERT(false);

                continue;
            }

            DescriptorSetLayoutBinding *binding = new DescriptorSetLayoutBinding();
            binding->binding = block.binding;
            binding->count = 1;
            binding->descriptorType = DescriptorType::UNIFORM_BUFFER;
            templateInfo->bindings.push_back(binding);

            UniformBlock *shaderInfoBlock = new UniformBlock();
            templateInfo->shaderInfo.blocks.push_back(shaderInfoBlock);
            shaderInfoBlock->binding = block.binding;
            shaderInfoBlock->count = 1;
            shaderInfoBlock->set = toNumber(pipeline::SetIndex::MATERIAL);
            int membersSize = block.members.size();
            if (membersSize > 0)
            {
                shaderInfoBlock->members.assign(membersSize, Uniform());
                int offset = 0;
                for (int n = 0; n < membersSize; ++n)
                {
                    IUniform &u = *block.members[n];
                    Uniform &shaderUniform = shaderInfoBlock->members[n];

                    SQ_ASSERT(u.count);
                    SQ_ASSERT(u.type);
                    shaderUniform.count = u.count;
                    shaderUniform.name = u.name;
                    shaderUniform.type = gfx::Type(u.type);
                    shaderUniform.offset = offset;
                    shaderUniform.block = shaderInfoBlock;
                    offset += gfx::getTypeSize(shaderUniform.type) * shaderUniform.count;

                    // printf("shader Info key %u %d %d  uniform name %s \n", key, asset->getId(), shaderIndex, u.name.c_str());
                }
            }
        }
    }

    {
        int size = shaderInfo->samplerTextures.size();
        if (size > 0)
        {
            templateInfo->shaderInfo.samplerTextures.assign(size, NULL);
            for (int i = 0; i < size; ++i)
            {
                ISamplerTextureInfo &s = *shaderInfo->samplerTextures[i];

#if ENABLE_ASSERTS
                for (int j = 0; j < templateInfo->shaderInfo.samplerTextures.size(); ++j)
                {
                    UniformSamplerTexture *shaderSampler = templateInfo->shaderInfo.samplerTextures[j];
                    if (shaderSampler && shaderSampler->binding == s.binding)
                    {
                        SQ_ASSERT(false);
                    }
                }
#endif

                if (s.buildin)
                {
                    auto infoIt1 = pipeline::localSamplerMap.find(s.name);
                    if (infoIt1 != pipeline::localSamplerMap.end())
                    {
                        templateInfo->shaderInfo.samplerTextures[i] = infoIt1->second;
                    }
                    continue;
                }

                DescriptorSetLayoutBinding *binding = new DescriptorSetLayoutBinding();
                binding->binding = s.binding;
                binding->count = s.count;
                binding->descriptorType = DescriptorType::SAMPLER_TEXTURE;
                templateInfo->bindings.push_back(binding);

                UniformSamplerTexture *shaderSampler = new UniformSamplerTexture();
                templateInfo->shaderInfo.samplerTextures[i] = shaderSampler;
                shaderSampler->name = s.name;
                shaderSampler->binding = s.binding;
                shaderSampler->count = s.count;
                shaderSampler->type = s.type;
                shaderSampler->set = toNumber(pipeline::SetIndex::MATERIAL);

                // printf("effect %d add texture to shader info %s binding %d \n", asset->getId(), shaderSampler->name.c_str(), shaderSampler->binding);
            }
        }
    }

    {
        int size = shaderInfo->attributes.size();
        if (size > 0)
        {
            templateInfo->shaderInfo.attributes.assign(size, Attribute());
            for (int i = 0; i < size; ++i)
            {
                IAttributeInfo &a = *shaderInfo->attributes[i];
                Attribute &attribute = templateInfo->shaderInfo.attributes[i];
                SQ_ASSERT(a.format);
                attribute.format = Format(a.format);
                attribute.name = a.name;
            }
        }
    }

    return templateInfo;
}

DescriptorSetLayout *ProgramLib::getDescriptorSetLayout(int effectAssetId, int shaderIndex)
{
    std::unordered_map<sqstd::hash_t, ITemplateInfo *>::iterator it = shaderMap.find(ProgramLib::getShaderKey(effectAssetId, shaderIndex));
    if (it != shaderMap.end())
    {
        ITemplateInfo *templateInfo = it->second;
        DescriptorSetLayoutInfo info;
        info.bindings = &templateInfo->bindings;
        DescriptorSetLayout *layout = DeviceManager::getInstance()->device->createDescriptorSetLayout(info);
        return layout;
    }
    return NULL;
}

sqstd::hash_t getVariantShaderKey(ITemplateInfo *templet, MacroRecord &macroDefines, unsigned int &flags)
{
    sqstd::hash_t seed = templet->shaderInfo.assetId;
    sqstd::hash_combine(seed, templet->shaderInfo.shaderIndex);
    MacroRecord::iterator it = macroDefines.begin();
    while (it != macroDefines.end())
    {

        bool *b = std::get_if<bool>(&it->second);
        if (b != nullptr && *b == true)
        {
            int flag = templet->findMacroFlag(it->first);
            ERROR(flag == -1, "Error：Macro Not Found");
            flags |= 1 << flag;
            sqstd::hash_combine(seed, flag);
        }
        else
        {
            int32_t *b = std::get_if<int32_t>(&it->second);
            if (b != nullptr && *b >= 1)
            {
                int flag = templet->findMacroFlag(it->first);
                flags |= 1 << flag;
                ERROR(flag == -1, "Error：Macro Not Found");
                sqstd::hash_combine(seed, flag);
            }
        }

        // printf("getVariantShaderKey %s %u\n", it->first.c_str(),flags);
        ++it;
    }
    return seed;
}

Shader *ProgramLib::getGFXShader(Device *device, const MacroRecord &macroDefines, int effectAssetId, int shaderIndex)
{
    std::unordered_map<sqstd::hash_t, ITemplateInfo *>::iterator it = shaderMap.find(ProgramLib::getShaderKey(effectAssetId, shaderIndex));

    unsigned int flags = 0;
    sqstd::hash_t shaderId = getVariantShaderKey(it->second, (MacroRecord &)macroDefines, flags);

    std::unordered_map<sqstd::hash_t, Shader *>::iterator shaderIt = it->second->shaders.find(shaderId);
    if (shaderIt != it->second->shaders.end())
    {
        return shaderIt->second;
    }

    ITemplateInfo *templateInfo = it->second;
    templateInfo->shaderInfo.macroFlags = flags;
    // printf("getGFXShader %d %d %u\n",effectAssetId, shaderIndex,flags);
    Shader *shader = device->createShader(templateInfo->shaderInfo);
    templateInfo->shaderInfo.macroFlags = 0;
    templateInfo->shaders[shaderId] = shader;
    return shader;
}

ITemplateInfo *ProgramLib::getTemplateInfo(int effectAssetId, int shaderIndex)
{
    // printf("getTemplateInfo %u %d %d \n", ProgramLib::getShaderKey(effectAssetId, shaderIndex), effectAssetId, shaderIndex);
    std::unordered_map<sqstd::hash_t, ITemplateInfo *>::iterator it = shaderMap.find(ProgramLib::getShaderKey(effectAssetId, shaderIndex));
    return it != shaderMap.end() ? it->second : NULL;
}

void ProgramLib::removeGFXShader(const MacroRecord &macroDefines, int effectAssetId, int shaderIndex)
{
    std::unordered_map<sqstd::hash_t, ITemplateInfo *>::iterator it = shaderMap.find(ProgramLib::getShaderKey(effectAssetId, shaderIndex));
    unsigned int flags = 0;
    sqstd::hash_t shaderId = getVariantShaderKey(it->second, (MacroRecord &)macroDefines, flags);
    std::unordered_map<sqstd::hash_t, Shader *>::iterator shaderIt = it->second->shaders.find(shaderId);
    if (shaderIt != it->second->shaders.end())
    {
        delete shaderIt->second;
        it->second->shaders.erase(shaderId);
    }
}

void ProgramLib::remove(int effectAssetId, int shaderIndex)
{

    std::unordered_map<sqstd::hash_t, ITemplateInfo *>::iterator it = shaderMap.find(ProgramLib::getShaderKey(effectAssetId, shaderIndex));
    if (it != shaderMap.end())
    {
        ITemplateInfo *templateInfo = it->second;
        delete templateInfo;
        shaderMap.erase(it);
    }
}

sqstd::hash_t ProgramLib::getShaderKey(int effectAssetId, int shaderIndex)
{
    sqstd::hash_t seed = effectAssetId;
    sqstd::hash_combine(seed, shaderIndex);
    return seed;
}