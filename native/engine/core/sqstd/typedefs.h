
#pragma once

#include <cstdint>
#include <type_traits>

#define SQ_ENUM_CONVERSION_OPERATOR(T) \
    inline std::underlying_type<T>::type toNumber(const T v) { return static_cast<std::underlying_type<T>::type>(v); }

#ifndef _STR
#define _STR(m_x) #m_x
#endif

#if defined(__GNUC__)
#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)
#else
#define likely(x) x
#define unlikely(x) x
#endif

//=================模板类型操作=====================

namespace sqstd
{
    /**
     * 检查T的类型是否为CT
     */
    template <typename T, typename CT>
    bool typeIs()
    {
        using check = std::remove_reference_t<T>;
        return std::is_same<check, CT>::value;
    };
}
