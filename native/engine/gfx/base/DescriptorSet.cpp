#include "DescriptorSet.h"
#include "../../core/core.h"

// int fuckId = 0;
DescriptorSet::DescriptorSet() : layout(NULL)
{
   // id = ++fuckId;
   // printf("=====create DescriptorSet %d \n", id);
}

DescriptorSet::~DescriptorSet()
{
   delete layout;
   layout = NULL;
}

void assignVector(int needSize, UniformValueArray &values)
{
   if (values.size() < needSize)
   {
      values.resize(needSize, UniformValue());
   }
}

void DescriptorSet::initialize(DescriptorSetInfo &info)
{

   layout = info.layout;
   const std::vector<const DescriptorSetLayoutBinding *> &bindings = layout->getBindings();
   for (int i = 0; i < bindings.size(); ++i)
   {
      // 图形API允许Binding重复的，但这有一定的条件(数据类型相同的Block)
      // SQ_ASSERT(bindingValueMap.find(bindings[i]->binding) ==  bindingValueMap.end());
      if (bindingValueMap.find(bindings[i]->binding) == bindingValueMap.end())
      {

         bindingValueMap[bindings[i]->binding] = UniformValueArray();
         if (bindings[i]->descriptorType == DescriptorType::SAMPLER_TEXTURE)
         {
            samplers[bindings[i]->binding] = sqstd::Array<gfx::SamplerObject *>();
         }
      }
   }
}

void DescriptorSet::bindBuffer(int binding, BufferObject *buffer, int index)
{
   //  printf("DescriptorSet bindBuffer %d %p\n",binding,this);
   ERROR(!buffer, "Error: DescriptorSet bindBuffer by NULL");
   SQ_ASSERT(bindingValueMap.find(binding) != bindingValueMap.end());
   UniformValueArray &values = bindingValueMap[binding];
   assignVector(index + 1, values);
   values[index] = buffer;
}

void DescriptorSet::bindTexture(int binding, gfx::Texture *texutre, int index)
{
   // printf("DescriptorSet bindTexture %p binding %d texture %p index %d\n", this, binding, texutre, index);
   ERROR(!texutre, "Error: DescriptorSet bindTexture by NULL");
   SQ_ASSERT(bindingValueMap.find(binding) != bindingValueMap.end());
   UniformValueArray &values = bindingValueMap[binding];
   assignVector(index + 1, values);
   values[index] = texutre;

   // printf("DescriptorSet::bindTexture %p %p %d %d \n", this, texutre, binding, index);
}

void DescriptorSet::bindSampler(int binding, gfx::SamplerObject *sampler, int index)
{
   SQ_ASSERT(samplers.find(binding) != samplers.end());
   sqstd::Array<gfx::SamplerObject *> &array = samplers[binding];
   if (array.getCount() - 1 < index)
   {
      array.Add();
   }
   array.set(index, sampler);
   // printf("DescriptorSet::bindSampler %p %p  %d %d %d \n", this, sampler, array.getCount(), binding, index);
}

gfx::SamplerObject *DescriptorSet::getBindingSamplerObject(int binding, int index)
{
   SQ_ASSERT(samplers.find(binding) != samplers.end());
   sqstd::Array<gfx::SamplerObject *> &array = samplers.find(binding)->second;
   // printf("DescriptorSet::getBindingSamplerObject %p %d %d %d \n", this, array.getCount(), binding, index);
   return array.getCount() > 0 ? *array.get(index) : nullptr;
}

int DescriptorSet::findBindTextureIndex(int binding, gfx::Texture *texutre)
{
   SQ_ASSERT(bindingValueMap.find(binding) != bindingValueMap.end());
   UniformValueArray &values = bindingValueMap[binding];
   int index = -1;

   // printf("findBindTextureIndex %p binding %d size %d texture %p\n", this, binding, values.size(), texutre);

   for (int i = 0; i < values.size(); ++i)
   {
      // printf("  get value %d %p %p\n", i, *std::get_if<gfx::Texture *>(&values[i]), std::get<gfx::Texture *>(values[i]));

      gfx::Texture **value = std::get_if<gfx::Texture *>(&values[i]);
      if (value != nullptr && *value == texutre)
      {
         index = i;
         break;
      }
   }
   return index;
}

int DescriptorSet::findEmtyTextureIndex(int binding)
{
   SQ_ASSERT(bindingValueMap.find(binding) != bindingValueMap.end());
   UniformValueArray &values = bindingValueMap[binding];
   int index = -1;
   for (int i = 0; i < values.size(); ++i)
   {
      if (!std::get_if<gfx::Texture *>(&values[i]))
      {
         index = i;
         break;
      }
   }
   return index;
}

int DescriptorSet::getSize(int binding)
{
   SQ_ASSERT(bindingValueMap.find(binding) != bindingValueMap.end());
   UniformValueArray &values = bindingValueMap[binding];
   return values.size();
}

bool DescriptorSet::hasValue(int binding, int index)
{
   if (bindingValueMap.find(binding) != bindingValueMap.end())
   {
      UniformValueArray &values = bindingValueMap[binding];
      // printf("hasValue %d \n",values.size());
      return values.size() >= (index + 1);
   }
   return false;
}

void DescriptorSet::clear()
{

   BindingValueMapTypeIterator it = bindingValueMap.begin();
   while (it != bindingValueMap.end())
   {
      it->second.clear();
      ++it;
   }

   SetSamplerType::iterator it1 = samplers.begin();
   while (it1 != samplers.end())
   {
      it1->second.clear();
      ++it1;
   }
}
