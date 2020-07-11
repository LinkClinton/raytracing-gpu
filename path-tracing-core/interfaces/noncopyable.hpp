#pragma once

namespace path_tracing::core::interfaces {

	class noncopyable {
	public:
		noncopyable() = default;
		virtual ~noncopyable() = default;

		noncopyable(const noncopyable&) = delete;
		noncopyable(noncopyable&&) noexcept = default;

		noncopyable& operator=(const noncopyable&) = delete;
		noncopyable& operator=(noncopyable&&) = default;
	};

}
