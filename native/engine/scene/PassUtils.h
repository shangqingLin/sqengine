#ifndef _PROGRAM_UTIL_H_
#define _PROGRAM_UTIL_H_

#include "../gfx/gfx.h"
#include <unordered_map>
#include "../core/sqstd/ByteBlock.h"
#include <variant>
#include "../core/math/Vec2.h"

using MaterialProperty = std::variant<float,int,unsigned int,float*,int*,unsigned int*>;

namespace PassUtils{


    /**
     * 计算Uniform占用的空间大小
    */
    int getUniformSize(std::vector<UniformBlock>& blocks);
    int getUniformSize(std::vector<const UniformBlock*>& blocks);
    int getUniformSize(const UniformBlock& block);

    using GFXTypeWriterCallback = void (*)(sqstd::ByteBlockChunk*, const MaterialProperty &,int offset);
    extern const std::unordered_map<gfx::Type, GFXTypeWriterCallback> type2writer;
};
#endif