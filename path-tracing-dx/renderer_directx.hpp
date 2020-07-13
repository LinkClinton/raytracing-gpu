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
		command_allocator mCommandAllocator;
		command_queue mCommandQueue;

		graphics_command_list mCommandList;

		descriptor_heap mImGuiDescriptorHeap;
		
		swap_chain mSwapChain;
		
		device mDevice;
	};
	
}
