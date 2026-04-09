#include "DescriptorSetLayout.h"
DescriptorSetLayout::DescriptorSetLayout() {}

void DescriptorSetLayout::initialize(const DescriptorSetLayoutInfo &info)
{
    bindings.reserve(info.bindings->size());
    for (int i = 0; i < info.bindings->size(); ++i)
    {
        bindings.push_back((*info.bindings)[i]);
    }
}

DescriptorSetLayout::~DescriptorSetLayout()
{
    bindings.clear();
}