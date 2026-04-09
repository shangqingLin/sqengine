#pragma once

#include "../core/core.h"
#include "../gfx/gfx.h"
#include <unordered_map>

/**
 *  定义各种内置的Block
 */

namespace pipeline {

    extern std::unordered_map<std::string,const UniformBlock*> globalBlockMap;
    extern std::unordered_map<std::string,const UniformBlock*> localBlockMap;
    extern std::unordered_map<std::string,UniformSamplerTexture*> localSamplerMap;
    extern std::vector<const DescriptorSetLayoutBinding*> localSetLayoutBindings;
    
    enum class SetIndex {
        MATERIAL,
        GLOBAL,
        LOCAL
    };
    SQ_ENUM_CONVERSION_OPERATOR(SetIndex);

   //定义全局的的Block的binding值。这种类型的block称为Global
    enum class PipelineGlobalBindings {
        UBO_GLOBAL,
        UBO_CAMERA
    };
    SQ_ENUM_CONVERSION_OPERATOR(PipelineGlobalBindings);

    //定义与模型自身有关的Block的binding值。这种类型的block称为Local
    enum class ModelLocalBindings {
        SAMPLER_SPRITE,
        UBO_LOCAL
    };
    SQ_ENUM_CONVERSION_OPERATOR(ModelLocalBindings);

    /**
     * 内置的全局UBOGlobal。属于Global的block
     * 用于传递帧时间等
     */
    struct  UBOGlobal
    {
        static const std::string NAME;
        static const DescriptorSetLayoutBinding DESCRIPTOR;
        static const PipelineGlobalBindings BINDING;
        static const UniformBlock LAYOUT;
    };

 
    
    /**
     * 内置的UBOCamera。属于Global的block
     * 用于传递照相机的相关数据
     */
    struct UBOCamera
    {
        static const std::string NAME;
        static const DescriptorSetLayoutBinding DESCRIPTOR;
        static const PipelineGlobalBindings BINDING;
        static const UniformBlock LAYOUT;
    };
    
    /**
     * 内置的模型上的数据的block，属于Local的block
     * 如：模型的世界变换矩阵等
     */
    struct UBOLocal
    {
        static const std::string NAME;
        static const int SIZE;
        static const DescriptorSetLayoutBinding DESCRIPTOR;
        static const ModelLocalBindings BINDING;
        static const UniformBlock LAYOUT;
    };


    /**
     * 表示纹理采样器block。实际上Shader语法中纹理采样器是不能放到block中，
     * 但为了能够统一思路处理，在程序中认为一个纹理采样器就是一个Block
     */

    struct LOCAL_SPRITE_TEXTURE {
        static const std::string NAME;
        static const DescriptorSetLayoutBinding DESCRIPTOR;
        static const UniformSamplerTexture LAYOUT;
        static const ModelLocalBindings BINDING;
    };
    
};

