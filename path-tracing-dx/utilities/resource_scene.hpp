#pragma once

#include "resource_cache.hpp"

#include "../wrapper/raytracing_acceleration.hpp"
#include "../wrapper/descriptor_heap.hpp"
#include "../wrapper/root_signature.hpp"

namespace path_tracing::dx::utilities {

	using namespace core;

	struct scene_info final {
		matrix4x4 raster_to_camera = matrix4x4(1);
		matrix4x4 camera_to_world = matrix4x4(1);

		real camera_focus = 0;
		real camera_lens = 0;
		
		uint32 sample_index = 0;
		uint32 emitters = 0;
		
		matrix4x4 unused0 = matrix4x4(1);
		vector4 unused1 = vector4(1);
		vector4 unused2 = vector4(1);
		vector4 unused3 = vector4(1);

		scene_info() = default;
	};
	
	class resource_scene final : public noncopyable {
	public:
		explicit resource_scene(const std::shared_ptr<device>& device);

		~resource_scene() = default;

		void set_render_target(const std::shared_ptr<texture2d>& render_target);

		void set_entities(const std::vector<std::shared_ptr<entity>>& entities,
			const std::shared_ptr<resource_cache>& cache);
		
		void set_scene_info(const scene_info& info);

		void execute(const std::shared_ptr<command_queue>& queue);

		void render(const std::shared_ptr<graphics_command_list>& command_list) const;

		std::shared_ptr<texture2d> render_target() const noexcept;
		
		std::shared_ptr<root_signature> signature() const noexcept;
	private:
		std::shared_ptr<descriptor_table> mDescriptorTable;
		std::shared_ptr<descriptor_heap> mDescriptorHeap;
		std::shared_ptr<root_signature> mRootSignature;

		std::shared_ptr<raytracing_acceleration> mAcceleration;
		
		std::shared_ptr<graphics_command_list> mCommandList;
		std::shared_ptr<command_allocator> mCommandAllocator;

		std::shared_ptr<texture2d> mRenderTarget;
		
		std::shared_ptr<buffer> mSceneInfo;
		std::shared_ptr<buffer> mMaterials;
		std::shared_ptr<buffer> mEmitters;
		
		std::shared_ptr<device> mDevice;

		std::vector<raytracing_instance> mInstancesData;
		std::vector<material_gpu_buffer> mMaterialsData;
		std::vector<emitter_gpu_buffer> mEmittersData;
		
		scene_info mSceneInfoData;
	};
	
}
