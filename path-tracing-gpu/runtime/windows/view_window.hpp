#pragma once

#include <directx12-wrapper/swap_chain.hpp>

#include "../../cores/noncopyable.hpp"
#include "../../cores/types.hpp"

#include <string>

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

		void update(float delta);
		
		void present(bool sync = true) const;

		bool living() const noexcept;

		view_window& operator=(view_window&& window) noexcept;
	private:
		wrapper::directx12::swap_chain mSwapChain;

		handle mHandle = nullptr;
		bool mLiving = false;
	};
	
}
