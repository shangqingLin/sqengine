#ifndef _DESCRIPTOR_SET_H
#define _DESCRIPTOR_SET_H
#include "define.h"
#include "DescriptorSetLayout.h"
#include "../../core/common/BufferView.h"
#include "../../core/base/config.h"
#include "./Texture.h"
#include "BufferObject.h"
#include "SamplerObject.h"
#include <unordered_map>
#include <variant>

using UniformValue = std::variant<BufferObject *, gfx::Texture *>;

typedef std::unordered_map<int, std::vector<UniformValue>> BindingValueMapType;
typedef std::unordered_map<int, std::vector<UniformValue>>::iterator BindingValueMapTypeIterator;
typedef std::vector<UniformValue> UniformValueArray;
typedef std::unordered_map<int, sqstd::Array<gfx::SamplerObject *>> SetSamplerType;
/**
 * 为Shader中的Uniform参数设置值
 */
class DescriptorSet
{
protected:
   /**
    * 用于描述Uniform变量在Shader中的分布
    * 因为我们设置参数的需要他的分布
    */
   DescriptorSetLayout *layout;

   // 存储当前设置的值
   BindingValueMapType bindingValueMap;
   SetSamplerType samplers;

   // int id;
public:
   DescriptorSet();
   ~DescriptorSet();

   // 子类覆盖这个方法之后必须调用父类的
   virtual void initialize(DescriptorSetInfo &info);

   void bindBuffer(int binding, BufferObject *buffer, int index = 0);
   void bindTexture(int binding, gfx::Texture *texutre, int index = 0);

   /**
    * SamplerObject的binging必须和Texture上的一致，表示是对这张纹理进行设置
    */
   void bindSampler(int binding, gfx::SamplerObject *, int index = 0);
   int findBindTextureIndex(int binding, gfx::Texture *texutre);
   int findEmtyTextureIndex(int binding);
   int getSize(int binding);

   gfx::SamplerObject* getBindingSamplerObject(int binding,int index);

   template <typename T>
   T *getBindingValue(int binding, int index = 0)
   {
      SQ_ASSERT(bindingValueMap.find(binding) != bindingValueMap.end());
      UniformValueArray &values = bindingValueMap[binding];
      T *value = std::get<T *>(values[index]);
      return value;
   }

   const UniformValueArray getBindingValue(int binding)
   {
      return bindingValueMap[binding];
   }

   bool hasValue(int binding, int index = 0);
   void clear();
   const DescriptorSetLayout &getLayout() { return *layout; }
};

#endif