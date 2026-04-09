#include "Material.h"
#include "../scene/ProgramLib.h"
#include "../rendering/buildin-block-define.h"
#include "AssetManager.h"
#include "Texture2d.h"
#include "../core/core.h"

IMaterialInfo::IMaterialInfo()
{
}
IMaterialInfo::~IMaterialInfo()
{
    // printf("===destroy IMaterialInfo \n");
}

void IMaterialInfo::pushState(PassStates *state)
{
    states.push_back(state);
}

void IMaterialInfo::pushMacro(char *name, int value)
{
    if (!macroDefines.has_value())
    {
        macroDefines = MacroRecord();
    }
    macroDefines.value()[name] = value;
}

//==================Material===================================
Material::Material(int id) : Asset(id),
                             effect(NULL),
                             states(NULL)
{
    // printf("+++++++++++++++create material %d \n",id);
}

void Material::setEffectAsset(EffectAsset *effect)
{
    // printf("  setEffectAsset   %d effect %d \n", getId(), effect->getId());
    this->effect = effect;
}

void Material::initialize(IMaterialInfo *info)
{

    SQ_ASSERT(effect);

    // printf(" Material::initialize %d \n", getId());

    if (!marcroDefines.empty())
    {
        marcroDefines.clear();
    }

    this->states = info->states;
    if (info->macroDefines.has_value())
    {
        // printf(" Material::initialize hasMacro \n");
        marcroDefines.resize(effect->passes.size());
        for (int i = 0; i < effect->passes.size(); ++i)
        {
            marcroDefines[i] = info->macroDefines.value();
        }
    }
    createPass();
}

void Material::createPass()
{
    SQ_ASSERT(effect);

    // printf(" createPass %p %p %s %d \n", this, effect, effect->getKeyUrl().c_str(), effect->passes.size());

    passes.reserve(effect->passes.size());
    for (int i = 0; i < effect->passes.size(); ++i)
    {
        PassInfo &passInfo = effect->passes[i];

        if (i < marcroDefines.size())
            passInfo.defines = marcroDefines[i];
        Pass *pass = new Pass();
        pass->initialize(&passInfo);
        if (i < states.size())
        {
            PassStates *state = this->states[i];
            pass->fillPipelineInfo(*state);
        }
        passes.push_back(pass);
    }
}

void Material::setTexture(std::string name, TextureBase *texture, int passIndex)
{
    Pass *pass = passes[passIndex];
    const UniformSamplerTexture *uniformTexture = findSamperProperty(name, passIndex);

    // printf(" setTexture %s %p \n", name.c_str(), texture);

    if (uniformTexture)
    {
        pass->bindTexture(uniformTexture->binding, texture->getGFXTexture());
        pass->bindSampler(uniformTexture->binding, texture->getGFXSampler());
        return;
    }
    std::string msg = "Not Found Material Property:";
    msg.append(name);
    ERROR(true, msg.c_str());
}

void Material::setTexture(std::string name, TextureBase **texture, int textArrayNum, int passIndex)
{
    Pass *pass = passes[passIndex];
    const UniformSamplerTexture *uniformTexture = findSamperProperty(name, passIndex);

    // printf(" setTextures %s %p \n", name.c_str(), uniformTexture);

    if (uniformTexture)
    {

        SQ_ASSERT(uniformTexture->count > 1);

        // if(name == "particleTexture")
        //  printf(" setProperty bind texture %d %s %d \n", texture->binding, texture->name.c_str(),texture->count);

        for (int m = 0; m < textArrayNum; ++m)
        {
            pass->bindTexture(uniformTexture->binding, texture[m]->getGFXTexture(), m);
            pass->bindSampler(uniformTexture->binding, texture[m]->getGFXSampler(), m);
        }
        return;
    }

    std::string msg = "Not Found Material Property:";
    msg.append(name);
    ERROR(true, msg.c_str());
}

void Material::setProperty(std::string name, MaterialProperty value)
{
    MaterialProperty v[1];
    v[0] = value;
    setProperty(name, v, 1);
}

void Material::setProperty(std::string name, MaterialProperty *value, int valueNum)
{
    // printf(" setProperty %s effect %p %p passsize %d \n", name.c_str(), this, this->effect, passes.size());

    SQ_ASSERT(passes.size());

    for (int i = 0; i < passes.size(); ++i)
    {
        setProperty(name, value, valueNum, i);
    }
}

void Material::setProperty(std::string name, MaterialProperty value, int passIndex)
{
    MaterialProperty v[1];
    v[0] = value;
    setProperty(name, v, 1, passIndex);
}

void Material::setProperty(std::string name, MaterialProperty *value, int valueSize, int passIndex)
{

    SQ_ASSERT(passes.size());

    // printf();
    Pass *pass = passes[passIndex];

    SQ_ASSERT(pass);

    const Uniform *uniform = findProperty(name, passIndex);
    if (uniform)
    {

        if (uniform->count > 1)
        {
            std::vector<MaterialProperty> values;
            values.reserve(valueSize);
            for (int m = 0; m < valueSize; ++m)
            {
                values.push_back(value[m]);
            }
            pass->setUniformArray(*uniform, values);
        }
        else
        {
            pass->setUniform(*uniform, value[0]);
        }
        return;
    }

    std::string msg = "Not Found Material Property:";
    msg.append(name);
    ERROR(true, msg.c_str());
}

const UniformSamplerTexture *Material::findSamperProperty(std::string name, int passIndex)
{
    Pass *pass = passes[passIndex];

    ITemplateInfo *templateInfo = ProgramLib::getInstnace()->getTemplateInfo(pass->info->effectAssetId, pass->info->shaderIndex);
    for (int i = 0; i < templateInfo->shaderInfo.samplerTextures.size(); ++i)
    {
        const UniformSamplerTexture *uniform = templateInfo->shaderInfo.samplerTextures[i];
        if (uniform->name == name)
        {
            // printf(" Material::findSamperProperty %s %d %d %d find %d \n", name.c_str(), getId(), pass->info->effectAssetId, pass->info->shaderIndex, uniform->binding);
            // for (int f = 0; f < templateInfo->shaderInfo.samplerTextures.size(); ++f)
            // {
            //     const UniformSamplerTexture *uniforms = templateInfo->shaderInfo.samplerTextures[f];
            //     printf("     %s %d    \n", uniforms->name.c_str(), uniforms->binding);
            // }
            return uniform;
        }
    }
    return nullptr;
}

const Uniform *Material::findProperty(std::string name, int passIndex)
{

    Pass *pass = passes[passIndex];
    // printf(" Material::findProperty %s %d %d %d \n", name.c_str(), getId(), pass->info->effectAssetId, pass->info->shaderIndex);
    ITemplateInfo *templateInfo = ProgramLib::getInstnace()->getTemplateInfo(pass->info->effectAssetId, pass->info->shaderIndex);
    for (int i = 0; i < templateInfo->shaderInfo.blocks.size(); ++i)
    {
        const UniformBlock *block = templateInfo->shaderInfo.blocks[i];
        if (block->set == toNumber(pipeline::SetIndex::MATERIAL))
        {
            for (int n = 0; n < block->members.size(); ++n)
            {
                Uniform &uniform = (Uniform &)block->members[n];
                if (uniform.name == name)
                {
                    return &uniform;
                }
            }
        }
    }

    return nullptr;
}

Material::~Material()
{
    this->effect = NULL;
    for (int i = 0; i < states.size(); ++i)
    {
        delete states[i];
    }
    states.clear();

    for (int i = 0; i < passes.size(); ++i)
    {
        delete passes[i];
    }
    passes.clear();
}

void Material::setPropertyBridgeJsTexture(char *name, int textureAssetId)
{
    Texture2d *texture = dynamic_cast<Texture2d *>(AssetManager::getInstance()->findById(textureAssetId));
    ERR_FAIL_NULL_V(texture);
    setTexture(name, texture);
}

void Material::setPropertyBridgeJsFloat(char *name, float value)
{
    setProperty(name, value);
}

void Material::setPropertyBridgeJsMat4(char *name, void *value)
{
    // MaterialProperty vv = (float *)value;
    setProperty(name, (float *)value);
}