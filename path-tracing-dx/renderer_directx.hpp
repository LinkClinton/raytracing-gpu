#pragma once

#include "../path-tracing-core/renderer_backend.hpp"

#include "utilities/scene_pipeline.hpp"
#include "wrapper/command_list.hpp"
#include "wrapper/swap_chain.hpp"

namespace path_tracing::dx {

	using namespace utilities;
	using namespace wrapper;
	
	class renderer_directx final : public core::renderer_backend {
	public:
		explicit renderer_directx(void* handle, int width, int height);

		~renderer_directx();

		void render(const std::shared_ptr<core::camera>& camera) override;

		void build(const std::shared_ptr<core::scene>& scene, const render_config& config) override;

		void resize(int new_width, int new_height) override;

		void release() override;
	private:
		void render_scene() const;
		
		void render_imgui() const;
	private:
		std::shared_ptr<command_allocator> mCommandAllocator;
		std::shared_ptr<command_queue> mCommandQueue;

		std::shared_ptr<graphics_command_list> mImGuiCommandList;
		std::shared_ptr<graphics_command_list> mSceneCommandList;

		std::shared_ptr<descriptor_heap> mRenderTargetViewHeap;
		std::shared_ptr<descriptor_heap> mImGuiDescriptorHeap;
		
		std::shared_ptr<swap_chain> mSwapChain;

		std::shared_ptr<texture2d> mRenderTargetHDR;
		std::shared_ptr<texture2d> mRenderTargetSDR;
		
		std::shared_ptr<device> mDevice;

		std::shared_ptr<resource_cache> mResourceCache;
		std::shared_ptr<resource_scene> mResourceScene;
		std::shared_ptr<scene_pipeline> mScenePipeline;

		scene_info mSceneInfo;
		size_t mCurrentSample = 0;
	};
	
}
