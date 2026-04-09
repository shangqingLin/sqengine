
#pragma once
#include <wasm_simd128.h>
// #ifdef __wasm_simd128__

/**
 *  WASM中所有的数据类型都是统一使用v128_t容器来表示，
 *  然后会根据你调用的函数自动具体决定什么样的类型。
 * 例如：
 * v128_t wasm_i32x4_add(v128_t a, v128_t b);
 * v128_t wasm_i16x8_mul(v128_t a, v128_t b);
 * v128_t wasm_i8x16_eq(v128_t a, v128_t b);
 */
using SIMD_128f = v128_t; //__m128
using SIMD_128i = v128_t; //__m128i
using SIMD_128d = v128_t; //__m128d
using SIMD_128u = v128_t;

//==================== set系列函数 ==========================

// float
#define simd_set_ps(x, y, z, w) wasm_f32x4_make(x, y, z, w)
#define simd_set1_ps(v) wasm_f32x4_splat(v)
#define simd_set_ps1(v) wasm_f32x4_splat(v)

// short
#define simd_set_epi16(e7, e6, e5, e4, e3, e2, e1, e0) wasm_i16x8_make(e7, e6, e5, e4, e3, e2, e1, e0)

// int
#define simd_set_epi32(e3, e2, e1, e0) wasm_i32x4_make(e3, e2, e1, e0)

//__m64 e1, __m64 e0
#define simd_set_epi64(a, b) wasm_i64x2_make(a, b)

// char
#define simd_set_epi8(e15, e14, e13, e12, e11, e10, e9, e8, e7, e6, e5, e4, e3, e2, e1, e0) wasm_i8x16_make(e15, e14, e13, e12, e11, e10, e9, e8, e7, e6, e5, e4, e3, e2, e1, e0)

// double
#define simd_set_pd(e1, e0) wasm_f64x2_make(e1, e0)

// short
#define simd_set1_epi16(a) wasm_i16x8_splat(a)

#define simd_set1_epi32(a) wasm_i32x4_splat(a)

//__m64
#define simd_set1_epi64(a) wasm_i64x2_splat(a)

// char
#define simd_set1_epi8(a) wasm_i8x16_splat(a)

// double
#define simd_set1_pd(a) wasm_f64x2_splat(a)

// char
#define simd_setr_epi16(e7, e6, e5, e4, e3, e2, e1, e0) wasm_i16x8_make(e7, e6, e5, e4, e3, e2, e1, e0)

// int
#define simd_setr_epi32(e3, e2, e1, e0) wasm_i32x4_make(e3, e2, e1, e0)

// unsiged int
#define simd_setr_epu32(e3, e2, e1, e0) wasm_u32x4_make(e3, e2, e1, e0)

//__m64
#define simd_setr_epi64(a, b) wasm_i64x2_make(a, b)

// char
#define simd_setr_epi8(e15, e14, e13, e12, e11, e10, e9, e8, e7, e6, e5, e4, e3, e2, e1, e0) wasm_i8x16_make(e15, e14, e13, e12, e11, e10, e9, e8, e7, e6, e5, e4, e3, e2, e1, e0)

// double
#define simd_setr_pd(a, b) wasm_f64x2_make(a, b)

//================== stroe和extract系列函数 ============================

// float* mem, v128_t a
#define simd_store_ps(mem, a) wasm_v128_store(mem, a)
#define simd_store_epu32(mem, a) wasm_v128_store(mem, a)
#define simd_store_epi32(mem, a) wasm_v128_store(mem, a)

//__m128 a, const int imm8
#define _mm_extract_ps(a, imm8) wasm_f32x4_extract_lane(a, imm8)

#define simd_extract_epi32(a, imm8) wasm_i32x4_extract_lane(a, imm8)

#define simd_extract_epi64(a, imm8) wasm_i64x2_extract_lane(a, imm8)

#define simd_extract_epi8(a, imm8) wasm_u8x16_extract_lane(a, imm8)

#define simd_extract_ps(a, imm8) wasm_i32x4_extract_lane(a, imm8)

// =====浮点数 算术操作==========
#define simd_movemask_ps(a) wasm_i32x4_bitmask(a)

// 浮点数加法操作
#define simd_add_ps(a, b) wasm_f32x4_add(a, b)

// 浮点数减法
#define simd_sub_ps(a, b) wasm_f32x4_sub(a, b)

// 浮点数乘法
#define simd_mul_ps(a, b) wasm_f32x4_mul(a, b)

// 浮点数除法
#define simd_div_ps(a, b) wasm_f32x4_div(a, b)

// 浮点数开方
#define simd_sqrt_ps(a) wasm_f32x4_sqrt(a)

#define simd_ceil_ps(a) wasm_f32x4_ceil(a)

// float const * mem_addr
#define simd_broadcast_ss(mem_addr) wasm_v32x4_load_splat(mem_addr)

//取两个数中最大的值作为结果存储到寄存器中
#define simd_max_ps(a, b) wasm_f32x4_max(a, b)

//取两个数中最小的值作为结果存储到寄存器中
#define simd_min_ps(a, b) wasm_f32x4_min(a, b)



//==== 浮点数比较运算=======

/**
 * 逐通道比较两个 128 位向量（包含 4 个单精度浮点数），判断对应通道中第一个向量的值是否小于第二个向量的值。
 */
#define simd_cmplt_ps(a, b) wasm_f32x4_lt(a, b)

/**
 * 小于
 */
#define simd_cmple_ps(a, b) wasm_f32x4_le(a, b)

/**
 * 等于
 */
#define simd_cmpeq_ps(a, b) wasm_f32x4_eq(a, b)

#define simd_cmpge_ps(a, b) wasm_f32x4_ge(a, b)

#define simd_cmpgt_ps(a, b) wasm_f32x4_gt(a, b)
#define simd_cmpneq_ps(a, b) wasm_f32x4_ne(a, b)

//-----整数算术运算

// 16位整数相加
#define simd_add_epi16(a, b) wasm_i16x8_add(a, b)

// 16位整数相乘
#define simd_mullo_epi16(a, b) wasm_i16x8_mul(a, b)
// 16位相减
#define simd_sub_epi16(a, b) wasm_i16x8_sub(a, b)

// 32位整数相加
#define simd_add_epi32(a, b) wasm_i32x4_add(a, b)

// 32位整数相减
#define simd_sub_epi32(a, b) wasm_i32x4_sub(a, b)

// 64位整数相加
#define simd_add_epi64(a, b) wasm_i64x2_add(a, b)

// 64位整数相减
#define simd_sub_epi64(a, b) wasm_i64x2_sub(a, b)

// 8位整数相加
#define simd_add_epi8(a, b) wasm_i8x16_add(a, b)

//
#define simd_sub_epi8(a, b) wasm_i8x16_sub(a, b)

//
#define simd_subs_epi16(a, b) wasm_i16x8_sub_sat(a, b)

#define simd_subs_epi8(a, b) wasm_i8x16_sub_sat(a, b)

#define simd_subs_epu16(a, b) wasm_u16x8_sub_sat(a, b)

#define simd_subs_epu8(a, b) wasm_u8x16_sub_sat(a, b)

#define simd_abs_epi8(a) wasm_i8x16_abs(a)

#define simd_abs_epi16(a) wasm_i16x8_abs(a)

#define simd_abs_epi32 wasm_i32x4_abs(a)

// 将 a 中的低 32 位整数复制到 dst。
#define simd_cvtsi128_si32(a) wasm_i32x4_extract_lane(a)

#define simd_cvtsi128_si64(a) wasm_i64x2_extract_lane(a)

#define simd_madd_epi16(a, b) wasm_i32x4_dot_i16x8(a, b)

#define simd_lddqu_si128(a) wasm_v128_load(a)

/**
 * 对两个128位向量中的每个16位有符号整数通道进行相加。
 * 如果结果超出16位有符号整数范围（即小于-32768或大于32767），则结果会被饱和截断到该范围（下溢为-32768，上溢为32767）
 */
#define simd_adds_epi16(a, b) wasm_i16x8_add_sat(a, b)

#define simd_adds_epi8(a, b) wasm_i8x16_add_sat(a, b)

#define simd_adds_epu16(a, b) wasm_u16x8_add_sat(a, b)

#define simd_adds_epu8(a, b) wasm_u8x16_add_sat(a, b)

#define simd_avg_epu16(a, b) wasm_u16x8_avgr(a, b)

//== 整型比较运算符==========
#define simd_cmpeq_epi16(a, b) wasm_i16x8_eq(a, b)

#define simd_cmpeq_epi32(a, b) wasm_i32x4_eq(a, b)

#define simd_cmpeq_epi8(a, b) wasm_i8x16_eq(a, b)

#define simd_cmpeq_pd(a, b) wasm_f64x2_eq(a, b)

#define simd_cmpge_pd(a, b) wasm_f64x2_ge(a, b)

#define simd_cmpgt_epi16(a, b) wasm_i16x8_gt(a, b)

#define simd_cmpgt_epi32(a, b) wasm_i32x4_gt(a, b)

#define simd_cmpgt_epi8(a, b) wasm_i8x16_gt(a, b)

#define simd_cmpgt_pd(a, b) wasm_f64x2_gt(a, b)

#define simd_cmple_pd(a, b) wasm_f64x2_le(a, b)

#define simd_cmplt_epi16 wasm_i16x8_lt(a, b)

#define simd_cmplt_epi32(a, b) wasm_i32x4_lt(a, b)

#define simd_cmplt_epi8(a, b) wasm_i8x16_lt(a, b)

#define simd_cmplt_pd(a, b) wasm_f64x2_lt(a, b)

#define simd_cmpneq_pd(a, b) wasm_f64x2_ne(a, b)

#define simd_cvtepi32_pd(a, b) wasm_f64x2_convert_low_i32x4(a, b)

#define simd_cvtepi32_ps(a, b) wasm_f32x4_convert_i32x4(a, b)

#define simd_cvtepi16_epi32(a) wasm_i32x4_widen_low_i16x8(a)

#define simd_cvtepi8_epi16(a) wasm_i16x8_widen_low_i8x16(a)

#define simd_cvtepu16_epi32(a) wasm_u32x4_extend_low_u16x8(a)

#define simd_cvtepu8_epi16(a) wasm_u16x8_extend_low_u8x16(a)

#define simd_insert_epi32(a, i, imm8) wasm_i32x4_replace_lane(a, i, imm8)

#define simd_insert_epi64(a, i, imm8) wasm_i64x2_replace_lane(a, i, imm8)

#define simd_insert_epi8(a, i, imm8) wasm_i8x16_replace_lane(a, i, imm8)

#define simd_max_epi32(a, b) wasm_i32x4_max(a, b)

#define simd_max_epi8(a, b) wasm_i8x16_max(a, b)

#define simd_max_epu16(a, b) wasm_u16x8_max(a, b)

#define simd_max_epu32(a, b) wasm_u32x4_max(a, b)

#define simd_min_epi32(a, b) wasm_i32x4_min(a, b)

#define simd_min_epi8(a, b) wasm_i8x16_min(a, b)

#define simd_min_epu16(a, b) wasm_u16x8_min(a, b)

#define simd_min_epu32(a, b) wasm_u32x4_min(a, b)

#define simd_mullo_epi32(a, b) wasm_i32x4_mul(a, b)

#define simd_packus_epi32(a, b) wasm_u16x8_narrow_i32x4(a, b)

//__m128d a, int rounding
#define simd_round_pd(a, rounding) wasm_f64x2_ceil(a, rounding) // wasm_f64x2_floor, wasm_f64x2_nearest, wasm_f64x2_trunc

#define simd_round_ps(a, rounding) wasm_f32x4_ceil(a, rounding) // wasm_f32x4_floor, wasm_f32x4_nearest, wasm_f32x4_trunc

#define simd_max_epi16(a, b) wasm_i16x8_max(a, b)

#define simd_max_epu8(a, b) wasm_u8x16_max(a, b)

#define simd_min_epi16(a, b) wasm_i16x8_min(a, b)

#define simd_min_epu8(a, b) wasm_u8x16_min(a, b)

#define simd_movemask_epi8(a, b) wasm_i8x16_bitmask(a, b)

#define simd_movemask_pd(a, b) wasm_i64x2_bitmask(a, b)

// 从 imm8 选择的 a 中提取一个 16 位整数，并将结果存储在 dst 的低位元素中。
#define simd_extract_epi16(a, b) wasm_u16x8_extract_lane(a, b)

// 将 a 复制到 dst，并将 16 位整数 i 插入到 dst 中 imm8 指定的位置。
#define simd_insert_epi16(a, i, imm8) wasm_i16x8_replace_lane(a, i, imm8)

#define simd_packs_epi16(a, b) wasm_i8x16_narrow_i16x8(a, b)

#define simd_packs_epi32(a, b) wasm_i16x8_narrow_i32x4(a, b)

#define simd_packus_epi16(a, b) wasm_u8x16_narrow_i16x8(a, b)

//== 位运算 =======

//浮点数位运算
#define simd_or_ps(a,b) wasm_v128_or(a,b)
#define simd_xor_ps(a,b) wasm_v128_xor(a,b)
#define simd_and_ps(a,b) wasm_v128_and(a,b)
#define simd_andnot_ps(a,b) wasm_v128_andnot(a,b)

//双精度
#define simd_or_pd(a,b) wasm_v128_or(a,b)
#define simd_xor_pd(a,b) wasm_v128_xor(a,b)
#define simd_and_pd(a,b) wasm_v128_and(a,b)
#define simd_andnot_pd(a,b) wasm_v128_and(a,b)

//整数运算
#define simd_or_si128(a,b) wasm_v128_or(a,b)
#define simd_xor_si128(a,b) wasm_v128_xor(a,b)
#define simd_and_si128(a,b) wasm_v128_and(a,b)
#define simd_andnot_si128(a,b) wasm_v128_andnot(a,b)


//无符号整数运算
#define simd_or_su128(a,b) wasm_v128_or(a,b)
#define simd_xor_su128(a,b) wasm_v128_xor(a,b)
#define simd_and_su128(a,b) wasm_v128_and(a,b)
#define simd_andnot_su128(a,b) wasm_v128_andnot(a,b)


//=============== 双精度 =======================================
// 双精度相加
#define simd_add_pd(a, b) wasm_f64x2_add(a, b)

// 双精度相乘
#define simd_mul_pd(a, b) wasm_f64x2_mul(a, b)

// 双精度相减
#define simd_sub_pd(a, b) wasm_f64x2_sub(a, b)

// 双精度相除
#define simd_div_pd(a, b) wasm_f64x2_div(a, b)

#define simd_sqrt_pd(a) wasm_f64x2_sqrt(a)

#define simd_ceil_pd(a) wasm_f64x2_ceil(a)

/**
 * 将 a 中的压缩双精度（64 位）浮点元素转换为压缩单精度（32 位）浮点元素，并将结果存储在 dst 中。
 */
#define simd_cvtpd_ps(a) wasm_f32x4_demote_f64x2_zero(a)

/**
 * 将 a 中的压缩单精度（32 位）浮点元素转换为压缩双精度（64 位）浮点元素，并将结果存储在 dst 中。
 */
#define simd_cvtps_pd(a) wasm_f64x2_promote_low_f32x4(a)

/**
 * 将 a 的低双精度（64 位）浮点元素复制到 dst。
 */
#define simd_cvtsd_f64(a) wasm_f64x2_extract_lane(a)
