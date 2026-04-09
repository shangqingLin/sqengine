#pragma once

#include "../../core/core.h"
#include "./define.h"

namespace gfx
{
    class SamplerObject
    {
    protected:
        /**
         * 将所有设置合并为一个值，使用3个位表示一个flag,即最大为7,
         */
        unsigned int hash = 0;

    public:
        TextureWrapMode getWrapS();
        TextureWrapMode getWrapT();
        TextureFilter getMinFilter();
        TextureFilter getMagFilter();
        virtual ~SamplerObject() = default;
        static unsigned int computeHash(const SamplerInfo &);
        static void unpackComputeHash(SamplerInfo &, unsigned int hash);
        inline unsigned int getHash() { return hash; };
    };
}
