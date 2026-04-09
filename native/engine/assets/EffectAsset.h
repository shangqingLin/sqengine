#pragma once

#include <vector>
#include "../gfx/gfx.h"
#include <map>
#include "Asset.h"
#include "../scene/PassDefine.h"

class EffectAsset;


struct IUniform
{
    std::string name;

    // uniform数据类型
    int type;

    // 如果是数组的，则表示数组的大小
    int count = 1;

    void setName(char *name) { this->name = name; };
};

/**
 * 顶点着色器声明的顶点属性
 */
struct IAttributeInfo
{
    std::string name;
    int format;
    int location;
    inline void setName(char *name) { this->name = name; };
};

/**
 * 着色器中声明的Uniform块
 */
class IBlockInfo
{
public:
    int binding = -1;
    std::string name;
    std::vector<IUniform *> members;
    bool buildin = false;
    ~IBlockInfo();
    inline void pushMembers(IUniform *u) { members.push_back(u); };
    inline void setBlockName(char *name) { this->name = name; };
};

struct ISamplerTextureInfo
{
    int binding = -1;
    std::string name;
    int type = 0;
    int count = 0;
    bool buildin = false;
    inline void setName(char *name) { this->name = name; };
};

class IShaderDefine
{
public:
    std::vector<IAttributeInfo *> attributes;
    std::vector<IBlockInfo *> blocks;
    std::vector<ISamplerTextureInfo *> samplerTextures;
    std::unordered_map<int, std::string> macros;

    ~IShaderDefine();
    inline void pushAttribute(IAttributeInfo *attr) { attributes.push_back(attr); };
    inline void pushBlock(IBlockInfo *block) { blocks.push_back(block); };
    inline void pushSamplerTexture(ISamplerTextureInfo *info) { samplerTextures.push_back(info); };
    inline void pushMacro(char *name, int flag) { macros[flag] = std::string(name); };
};

class IPassInfo
{
public:
    int shaderIndex = -1;

    // 直接存储到一个Buffer中吧，因为状态数据太多了，对外提供API非常麻烦
    char *passStateBuffer = nullptr;
};

class IEffectDefine
{
public:
    std::vector<IShaderDefine *> shaders;
    std::vector<IPassInfo *> passes;

    ~IEffectDefine();
    inline void pushShaderDefine(IShaderDefine *define) { shaders.push_back(define); };
    inline void pushPassInfo(IPassInfo *pass) { passes.push_back(pass); }
};

class EffectAsset : public Asset
{
public:
    std::vector<PassInfo> passes;

    EffectAsset(int id);
    ~EffectAsset();

    void initialize(IEffectDefine *info);
};
