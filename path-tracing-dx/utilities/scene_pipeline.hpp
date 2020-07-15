#pragma once

#include "../../path-tracing-core/scenes/entity.hpp"

#include "../wrapper/raytracing_pipeline.hpp"
#include "../utilities/resource_scene.hpp"

namespace path_tracing::dx::utilities {

	class scene_pipeline final : public noncopyable {
	public:
		scene_pipeline() = default;

		~scene_pipeline() = default;

		void set_resource_scene(const std::shared_ptr<resource_scene>& resource);

		void set_entities(const std::vector<std::shared_ptr<entity>>& entities);

		void set_max_depth(size_t depth);

		void execute(const std::shared_ptr<device>& device);
	private:
		std::shared_ptr<raytracing_pipeline> mPipeline;

		std::shared_ptr<resource_scene> mResourceScene;

		std::vector<std::shared_ptr<entity>> mEntities;

		size_t mMaxDepth = 5;
	};
	
}
