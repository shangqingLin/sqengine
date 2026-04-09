#pragma once

#include <stdint.h>
#include "../typedefs.h"

#ifdef __GNUC__
// #define FUNCTION_STR __PRETTY_FUNCTION__ - too annoying
#define FUNCTION_STR __FUNCTION__
#else
#define FUNCTION_STR __FUNCTION__
#endif

void _err_print_error(const char *p_function, const char *p_file, int p_line, const char *param, const char *p_error);

#if ENABLE_ERROR_CHECK
#define ERROR(condition, MSG) \
	if (condition)            \
		printf("ERROR : %s\n", MSG);
#define ERR_FAIL_NULL_V_MSG(m_param, m_msg)                                                                    \
	if (unlikely(m_param == nullptr))                                                                          \
	{                                                                                                          \
		_err_print_error(FUNCTION_STR, __FILE__, __LINE__, "Parameter \"" _STR(m_param) "\" is null.", m_msg); \
	}                                                                                                          \
	else                                                                                                       \
		((void)0)

#define ERR_FAIL_NULL_V(m_param)                                                                                 \
	if (unlikely(m_param == nullptr))                                                                            \
	{                                                                                                            \
		_err_print_error(FUNCTION_STR, __FILE__, __LINE__, "Parameter \"" _STR(m_param) "\" is null.", nullptr); \
	}                                                                                                            \
	else                                                                                                         \
		((void)0)
#else
#define ERROR(...) ((void)0)
#define ERR_FAIL_NULL_V_MSG(m_param, m_msg) ((void)0)
#define ERR_FAIL_NULL_V(m_param) ((void)0)
#endif
