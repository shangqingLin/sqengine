#include "PassUtils.h"

namespace PassUtils
{

    /**
     * 计算Uniform占用的空间大小
     */
    int getUniformSize(std::vector<UniformBlock> &blocks)
    {
        int size = 0;
        for (int i = 0; i < blocks.size(); ++i)
        {
            size += getUniformSize(blocks[i]);
        }
        return size;
    }

    int getUniformSize(std::vector<const UniformBlock *> &blocks)
    {
        int size = 0;
        for (int i = 0; i < blocks.size(); ++i)
        {
            size += getUniformSize(*blocks[i]);
        }
        return size;
    }

    int getUniformSize(const UniformBlock &block)
    {
        int size = 0;
        for (int i = 0; i < block.members.size(); ++i)
        {
            size += getTypeSize(block.members[i].type) * block.members[i].count;
        }
        return size;
    }

    // int getBindingFromHandle(int& handle){
    //     int bindingMask = 0x03f00000;
    //    return (handle & bindingMask) >> 20;
    // }

    // int getTypeFromHandle(int& handle){
    //      int typeMask = 0xfc000000;
    //     return (handle & typeMask) >> 26;
    // }

    // int getOffsetFromHandle(int& handle){
    //     int offsetMask  = 0x00000fff;
    //     return handle & offsetMask;
    // }

    // int getCountFromHandle(int& handle){
    //     int countMask = 0x000ff000;
    //     return (handle & countMask) >> 12;
    // }

    const std::unordered_map<gfx::Type, GFXTypeWriterCallback> type2writer = {
        {gfx::Type::UNKNOWN, [](sqstd::ByteBlockChunk *a, const MaterialProperty &, int offset) {

         }},

        {gfx::Type::INT, [](sqstd::ByteBlockChunk *a, const MaterialProperty &v, int offset)
         {
             SQ_ASSERT(std::get_if<int>(&v) != nullptr);
             a->buffer.setValue(offset, std::get<int>(v));
         }},
        {gfx::Type::INT2, [](sqstd::ByteBlockChunk *a, const MaterialProperty &v, int offset)
         {
             SQ_ASSERT(std::get_if<int *>(&v));
             int *data = std::get<int *>(v);
             a->buffer.setValue(offset, data[0]);
             a->buffer.setValue(offset + 4, data[1]);
         }},
        {gfx::Type::INT3, [](sqstd::ByteBlockChunk *a, const MaterialProperty &v, int offset)
         {
             int *data = std::get<int *>(v);
             a->buffer.setValue(offset, data[0]);
             a->buffer.setValue(offset + 4, data[1]);
             a->buffer.setValue(offset + 8, data[2]);
         }},
        {gfx::Type::INT4, [](sqstd::ByteBlockChunk *a, const MaterialProperty &v, int offset)
         {
             SQ_ASSERT(std::get_if<int *>(&v));
             int *data = std::get<int *>(v);
             a->buffer.setValue(offset, data[0]);
             a->buffer.setValue(offset + 4, data[1]);
             a->buffer.setValue(offset + 8, data[2]);
             a->buffer.setValue(offset + 12, data[3]);
         }},
        {gfx::Type::FLOAT, [](sqstd::ByteBlockChunk *a, const MaterialProperty &v, int offset)
         {
             SQ_ASSERT(std::get_if<float>(&v));
             a->buffer.setValue(offset, std::get<float>(v));
         }},
        {gfx::Type::FLOAT2, [](sqstd::ByteBlockChunk *a, const MaterialProperty &v, int offset)
         {
             SQ_ASSERT(std::get_if<float *>(&v));
             float *data = std::get<float *>(v);
             a->buffer.setValue(offset, data[0]);
             a->buffer.setValue(offset + 4, data[1]);
         }},
        {gfx::Type::FLOAT3, [](sqstd::ByteBlockChunk *a, const MaterialProperty &v, int offset)
         {
             SQ_ASSERT(std::get_if<float *>(&v));
             float *data = std::get<float *>(v);
             a->buffer.setValue(offset, data[0]);
             a->buffer.setValue(offset + 4, data[1]);
             a->buffer.setValue(offset + 8, data[2]);
         }},
        {gfx::Type::FLOAT4, [](sqstd::ByteBlockChunk *a, const MaterialProperty &v, int offset)
         {
             SQ_ASSERT(std::get_if<float *>(&v));
             float *data = std::get<float *>(v);
             a->buffer.setValue(offset, data[0]);
             a->buffer.setValue(offset + 4, data[1]);
             a->buffer.setValue(offset + 8, data[2]);
             a->buffer.setValue(offset + 12, data[3]);
         }},
        {gfx::Type::MAT3, [](sqstd::ByteBlockChunk *a, const MaterialProperty &v, int offset)
         {
             SQ_ASSERT(std::get_if<float *>(&v));
             float *data = std::get<float *>(v);
             for (int i = 0; i < 9; ++i)
             {
                 a->buffer.setValue(offset, data[i]);
                 offset += 4;
             }
         }},
        {gfx::Type::MAT4, [](sqstd::ByteBlockChunk *a, const MaterialProperty &v, int offset)
         {
             SQ_ASSERT(std::get_if<float *>(&v));
             float *data = std::get<float *>(v);
             for (int i = 0; i < 16; ++i)
             {
                //  printf("read mat4 i %d v %f \n", i, data[i]);
                 a->buffer.setValue(offset, data[i]);
                 offset += 4;
             }
         }},
    };

}