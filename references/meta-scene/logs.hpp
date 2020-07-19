#pragma once

#define __SPDLOG_IMPLEMENTATION__
#ifndef __SPDLOG_IMPLEMENTATION__
#else

#pragma warning(disable : 4275)

#include <spdlog/spdlog.h>

#endif

namespace metascene {

	namespace logs {

#ifdef __SPDLOG_IMPLEMENTATION__

		template <typename... Args>
		void debug(const std::string& format, const Args& ... args)
		{
			spdlog::debug(format.c_str(), args...);
		}

		template <typename ... Args>
		void info(const std::string& format, const Args&... args)
		{
			spdlog::info(format.c_str(), args...);
		}

		template <typename ... Args>
		void warn(const std::string& format, const Args&... args)
		{
			spdlog::warn(format.c_str(), args...);
		}

		template <typename ... Args>
		void error(const std::string& format, const Args&... args)
		{
			spdlog::error(format, args...);
		}
		
#endif
		
	}
}
