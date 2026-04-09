#ifndef _DESCRIPTOR_SET_LAYOUT_H_
#define _DESCRIPTOR_SET_LAYOUT_H_
#include "define.h"

/**
 * 其中DescriptorSetLayout和DescriptorSet都是Vulkan中的概念，这里模拟一下。
 * 
 * 描述着色器中的Uniform、Uniform块的内存应该如何分配
 * 用于在应用端设置Uniform变量的值。需要结合DescritorSet来完成
*/
class DescriptorSetLayout{
  protected:
    std::vector<const DescriptorSetLayoutBinding*> bindings;
  public:
    DescriptorSetLayout();
    const std::vector<const DescriptorSetLayoutBinding*>& getBindings() { return bindings;};
    virtual void initialize (const DescriptorSetLayoutInfo& info);
    virtual ~DescriptorSetLayout();
};

#endif