#pragma once

/**
 * 1、实测SIMD好像对小游戏增益不大，以后再慢慢测试吧
 * 2、SIMD不能支持动态切换，必须打一个SIMD的包和一个非SIMD，非常麻烦
 */
#ifdef SIMD_WASM
#include "./wasm/wasm-define.h"

// extern bool USE_SIMD;
#endif
