#pragma once

namespace sqstd{
    
    using hash_t = std::uint32_t;   

    namespace hash_detail
    {
        
    #define CCSTD_FUNCTIONAL_HASH_ROTL32(x, r) (x << r) | (x >> (32 - r))

    template<hash_t Bits> 
    struct hash_combine_impl
    {
        template <typename SizeT>
        inline static SizeT fn(SizeT seed, SizeT value)
        {
            seed ^= value + 0x9e3779b9 + (seed<<6) + (seed>>2);
            return seed;
        }
    };

    template<>
    struct hash_combine_impl<32>
    {
        inline static std::uint32_t fn(std::uint32_t h1, std::uint32_t k1)
        {
            const std::uint32_t c1 = 0xcc9e2d51;
            const std::uint32_t c2 = 0x1b873593;

            k1 *= c1;
            k1 = CCSTD_FUNCTIONAL_HASH_ROTL32(k1,15);
            k1 *= c2;

            h1 ^= k1;
            h1 = CCSTD_FUNCTIONAL_HASH_ROTL32(h1,13);
            h1 = h1*5+0xe6546b64;

            return h1;
        }
    };

    template<> 
    struct hash_combine_impl<64>
    {
        inline static std::uint64_t fn(std::uint64_t h, std::uint64_t k)
        {
            const std::uint64_t m = (std::uint64_t(0xc6a4a793) << 32) + 0x5bd1e995;
            const int r = 47;

            k *= m;
            k ^= k >> r;
            k *= m;

            h ^= k;
            h *= m;

            // Completely arbitrary number, to prevent 0's
            // from hashing to 0.
            h += 0xe6546b64;

            return h;
        }
    };
   }

    template <class T>
    inline void hash_combine(hash_t& seed, T const& v)
    {
        seed = hash_detail::hash_combine_impl<sizeof(hash_t) * CHAR_BIT>::fn(seed,v);
    }
}
