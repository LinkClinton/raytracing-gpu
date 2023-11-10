#pragma once

#pragma warning(disable : 4275)

#include <spdlog/spdlog.h>

namespace raytracing::extensions::spdlog
{

	template <typename... Args>
	void debug(::spdlog::format_string_t<Args...> format, Args&& ... args) { ::spdlog::debug(format, std::forward<Args>(args)...); }

	template <typename... Args>
	void info(::spdlog::format_string_t<Args...> format, Args&& ... args) { ::spdlog::info(format, std::forward<Args>(args)...); }

	template <typename... Args>
	void warn(::spdlog::format_string_t<Args...> format, Args&& ... args) { ::spdlog::warn(format, std::forward<Args>(args)...); }

	template <typename... Args>
	void error(::spdlog::format_string_t<Args...> format, Args&& ... args) { ::spdlog::error(format, std::forward<Args>(args)...); }

}
