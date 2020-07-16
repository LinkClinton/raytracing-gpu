#pragma once

#include "../../path-tracing-core/scenes/entity.hpp"

#include "../wrapper/raytracing_pipeline.hpp"
#include "../utilities/resource_scene.hpp"

namespace path_tracing::dx::utilities {

	class scene_pipeline final : public noncopyable {
	public:
		explicit scene_pipeline(const std::shared_ptr<device>& device);

		~scene_pipeline() = default;

		void set_resource_cache(const std::shared_ptr<resource_cache>& cache);
		
		void set_resource_scene(const std::shared_ptr<resource_scene>& scene);

		void set_entities(const std::vector<std::shared_ptr<entity>>& entities);

		void set_max_depth(size_t depth);

		void execute(const std::shared_ptr<command_queue>& queue);

		void render(const std::shared_ptr<graphics_command_list>& command_list) const;
	private:
		std::shared_ptr<command_allocator> mCommandAllocator;
		std::shared_ptr<graphics_command_list> mCommandList;
		
		std::shared_ptr<raytracing_pipeline> mPipeline;

		std::shared_ptr<resource_cache> mResourceCache;
		std::shared_ptr<resource_scene> mResourceScene;

		std::shared_ptr<device> mDevice;
		
		std::vector<std::shared_ptr<entity>> mEntities;

		size_t mMaxDepth = 5;
	};
	
}
