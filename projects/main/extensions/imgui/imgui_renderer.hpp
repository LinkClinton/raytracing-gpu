#pragma once

#include <directx12-wrapper/extensions/imgui.hpp>
#include <directx12-wrapper/swap_chain.hpp>

#include "../../runtime/render/render_device.hpp"

namespace raytracing::runtime {

	struct runtime_service;
	struct runtime_frame;
	
}

namespace raytracing::extensions::imgui {

	using runtime::render::render_device;
	using runtime::runtime_service;
	using runtime::runtime_frame;
	
	class imgui_renderer final {
	public:
		imgui_renderer() = default;

		~imgui_renderer() = default;

		void resolve(
			const wrapper::directx12::swap_chain& swap_chain,
			const render_device& device, uint32 font_size);

		void update(const runtime_service& service, const runtime_frame& frame);

		void render(const runtime_service& service, const runtime_frame& frame);
		
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
