#pragma once

// 用于判断当前是什么平台的宏 platform
#if defined(_WIN32) || defined(_WIN64)
	#define SQ_PLATFORM_WINDOWS
#elif defined( __ANDROID__ )
	#define SQ_PLATFORM_ANDROID
#elif defined( __linux__ )
	#define SQ_PLATFORM_LINUX
#elif defined( __APPLE__ )
	#include <TargetConditionals.h>
	#if defined( TARGET_OS_IPHONE ) && !TARGET_OS_IPHONE
		#define SQ_PLATFORM_MACOS
	#else
		#define SQ_PLATFORM_IOS
	#endif
#elif defined( __EMSCRIPTEN__ )
	#define SQ_PLATFORM_WASM
#else
	#define SQ_PLATFORM_UNKNOWN
#endif

// 定义判断当前编译使用的编译器
#if defined( __clang__ )
	#define SQ_COMPILER_CLANG
#elif defined( __GNUC__ )
	#define SQ_COMPILER_GCC
#elif defined( _MSC_VER )
	#define SQ_COMPILER_MSVC
#endif

#if ENABLE_ASSERTS
#include <assert.h>
#define SQ_ASSERT( condition ) assert(condition) 																								 
#else
	#define SQ_ASSERT( ... ) ( (void)0 )
#endif


