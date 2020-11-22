#pragma once

#include <directx12-wrapper/swap_chain.hpp>

#include "../../interfaces/noncopyable.hpp"
#include "../../types.hpp"

#include <string>

namespace path_tracing::runtime {

	struct runtime_service;
	struct runtime_frame;
	
}

namespace path_tracing::runtime::windows {

	using handle = void*;

	class view_window final : public noncopyable {
	public:
		view_window() = default;

		view_window(
			const wrapper::directx12::command_queue& queue,
			const wrapper::directx12::device& device,
			const std::string& name, uint32 size_x, uint32 size_y);

		view_window(view_window&& window) noexcept;

		~view_window();

		void update(const runtime_service& service, const runtime_frame& frame);

		void present(bool sync = true) const;

		bool living() const noexcept;

		uint32 size_x() const noexcept;

		uint32 size_y() const noexcept;
		
		view_window& operator=(view_window&& window) noexcept;

		friend class window_system;
	private:
		wrapper::directx12::swap_chain mSwapChain;

		uint32 mSizeX = 0;
		uint32 mSizeY = 0;
		
		handle mHandle = nullptr;
		bool mLiving = false;
	};

}
