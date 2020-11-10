#pragma once

#include <directx12-wrapper/raytracings/accleration.hpp>
#include <directx12-wrapper/commands/command_queue.hpp>
#include <directx12-wrapper/resources/buffer.hpp>

#include "../../cores/noncopyable.hpp"
#include "../../cores/types.hpp"

namespace path_tracing::runtime::render {

	class render_device final : public noncopyable {
	public:
		render_device() = default;

		render_device(const D3D_FEATURE_LEVEL& level);

		~render_device() = default;

		void wait() const;

		wrapper::directx12::command_queue queue() const noexcept;

		wrapper::directx12::device device() const noexcept;

		wrapper::directx12::fence fence() const noexcept;
	private:
		wrapper::directx12::command_queue mMainQueue;

		wrapper::directx12::device mDevice;
		wrapper::directx12::fence mFence;
	};
	
}
