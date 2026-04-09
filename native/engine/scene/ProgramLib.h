#pragma once

#include "../gfx/gfx.h"
#include <map>
#include "../assets/EffectAsset.h"
#include "../core/core.h"

class ITemplateInfo
{
public:
    struct MacroInfo
    {
        /**
         * 传统的做法是通过将宏名字拼接起来作为Shader变体唯一的key，这造成这个Key可能非常长浪费内存，
         * 而且与C++交互也不好操作，所以这里会为每个宏名字动态生成一个唯一的数字的key，在同一个Effect资源中宏key是唯一的
         */
        int flag{-1};

        std::string marcoName;
        MacroInfo(){}
        MacroInfo(int flag, std::string &name) : flag(flag), marcoName(name) {}
    };

    ShaderInfo shaderInfo;
    std::vector<const DescriptorSetLayoutBinding *> bindings;

    // 资源中所有的出现的宏必须注册到这里
    std::vector<MacroInfo> macros;

    // 记录当前着色器模板创建出来的所有的Shader
    std::unordered_map<sqstd::hash_t, Shader *> shaders;

    int findMacroFlag(const std::string &macroName) const;

    ~ITemplateInfo();
};

class ProgramLib
{
private:
    std::unordered_map<sqstd::hash_t, ITemplateInfo *> shaderMap;

public:
    static ProgramLib *getInstnace();
    ITemplateInfo *define(IShaderDefine *shaderInfo, EffectAsset *asset, int shadrIndex);
    void remove(int effectAssetId, int shaderIndex);
    static sqstd::hash_t getShaderKey(int effectAssetId, int shaderIndex);
    ITemplateInfo *getTemplateInfo(int effectAssetId, int shaderIndex);
    DescriptorSetLayout *getDescriptorSetLayout(int effectAssetId, int shaderIndex);
    Shader *getGFXShader(Device *, const MacroRecord &macroDefines, int effectAssetId, int shaderIndex);
    void removeGFXShader(const MacroRecord &macroDefines, int effectAssetId, int shaderIndex);
};
