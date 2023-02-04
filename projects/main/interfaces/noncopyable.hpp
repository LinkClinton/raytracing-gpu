#pragma once

namespace raytracing::interfaces {

	class noncopyable {
	public:
		noncopyable() = default;
		virtual ~noncopyable() = default;

		noncopyable(const noncopyable&) = delete;
		noncopyable(noncopyable&&) noexcept = default;

		noncopyable& operator=(const noncopyable&) = delete;
		noncopyable& operator=(noncopyable&&) noexcept = default;
	};

}

using namespace raytracing::interfaces;