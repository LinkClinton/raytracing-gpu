#pragma once

#include <directx12-wrapper/extensions/imgui.hpp>
#include <directx12-wrapper/swap_chain.hpp>

#include "../../runtime/render/render_device.hpp"
#include "../../cores/types.hpp"

namespace path_tracing::runtime {

	struct runtime_service;
	
}

namespace path_tracing::extensions::imgui {

	using runtime::render::render_device;
	using runtime::runtime_service;
	
	class imgui_renderer final {
	public:
		imgui_renderer() = default;

		~imgui_renderer() = default;

		void resolve(
			const wrapper::directx12::swap_chain& swap_chain,
			const render_device& device, uint32 font_size);

		void update(const runtime_service& service, real delta);

		void render(const runtime_service& service, real delta);
		
		ImTextureID allocate(const wrapper::directx12::texture2d& resource, size_t index) const;
	private:
		wrapper::directx12::command_allocator mCommandAllocator;
		wrapper::directx12::graphics_command_list mCommandList;

		wrapper::directx12::descriptor_heap mRenderTargetViewHeap;
		wrapper::directx12::descriptor_heap mImGuiDescriptorHeap;

		wrapper::directx12::swap_chain mSwapChain;

		wrapper::directx12::device mDevice;
	};
	
}
