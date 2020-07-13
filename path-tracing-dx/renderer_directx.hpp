#pragma once

#include "../path-tracing-core/renderer_backend.hpp"

#include "wrapper/command_list.hpp"
#include "wrapper/swap_chain.hpp"

namespace path_tracing::dx {

	using namespace wrapper;
	
	class renderer_directx final : public core::renderer_backend {
	public:
		explicit renderer_directx(void* handle, int width, int height);

		~renderer_directx();

		void render(const std::shared_ptr<core::camera>& camera) override;

		void build(const std::shared_ptr<core::scene>& scene) override;

		void resize(int new_width, int new_height) override;

		void release() override;
	private:
		void render_imgui() const;
	private:
		std::shared_ptr<command_allocator> mCommandAllocator;
		std::shared_ptr<command_queue> mCommandQueue;

		std::shared_ptr<graphics_command_list> mCommandList;

		std::shared_ptr<descriptor_heap> mRenderTargetViewHeap;
		std::shared_ptr<descriptor_heap> mImGuiDescriptorHeap;
		
		std::shared_ptr<swap_chain> mSwapChain;
		
		std::shared_ptr<device> mDevice;
	};
	
}
