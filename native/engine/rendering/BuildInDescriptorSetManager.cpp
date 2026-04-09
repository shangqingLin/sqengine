#include "BuildInDescriptorSetManager.h"
#include "buildin-block-define.h"

using namespace pipeline;

static BuildInDescriptorSetManager *_ins = new BuildInDescriptorSetManager();

BuildInDescriptorSetManager *BuildInDescriptorSetManager::getIntance()
{
    return _ins;
}

void BuildInDescriptorSetManager::initialize()
{

    // Piple可能多次调用，所以判断一下是否初始化过了
    if (globalBlockMap.size() > 0)
        return;

    for (int i = 0; i < UBOCamera::LAYOUT.members.size(); ++i)
    {
        UniformBlock &block = (UniformBlock &)UBOCamera::LAYOUT;
        block.members[i].block = &UBOCamera::LAYOUT;
    }

    for (int i = 0; i < UBOGlobal::LAYOUT.members.size(); ++i)
    {
        UniformBlock &block = (UniformBlock &)UBOGlobal::LAYOUT;
        block.members[i].block = &UBOGlobal::LAYOUT;
    }

    for (int i = 0; i < UBOLocal::LAYOUT.members.size(); ++i)
    {
        UniformBlock &block = (UniformBlock &)UBOLocal::LAYOUT;
        block.members[i].block = &UBOLocal::LAYOUT;
    }

    globalBlockMap[UBOCamera::NAME] = &UBOCamera::LAYOUT;
    globalBlockMap[UBOGlobal::NAME] = &UBOGlobal::LAYOUT;

    localBlockMap[UBOLocal::NAME] = &UBOLocal::LAYOUT;
    localSamplerMap[LOCAL_SPRITE_TEXTURE::NAME] = (UniformSamplerTexture *)(&LOCAL_SPRITE_TEXTURE::LAYOUT);

    localSetLayoutBindings.push_back(&LOCAL_SPRITE_TEXTURE::DESCRIPTOR);
    localSetLayoutBindings.push_back(&UBOLocal::DESCRIPTOR);
}