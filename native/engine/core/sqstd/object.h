#pragma once

/**
 * 类型识别系统
 */
#define CLASS(m_class, m_inherits) \
public:                            \
	static std::string CLASS_NAME = #m_class																										   \