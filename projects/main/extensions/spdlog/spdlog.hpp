#pragma once

#pragma warning(disable : 4275)

#include <spdlog/spdlog.h>

namespace raytracing::extensions::spdlog
{

	template <typename... Args>
	void debug(const std::string_view& format, const Args& ... args) { ::spdlog::debug(format, args...); }

	template <typename... Args>
	void info(const std::string_view& format, const Args& ... args) { ::spdlog::info(format, args...); }

	template <typename... Args>
	void warn(const std::string_view& format, const Args& ... args) { ::spdlog::warn(format, args...); }

	template <typename... Args>
	void error(const std::string_view& format, const Args& ... args) { ::spdlog::error(format, args...); }

}
