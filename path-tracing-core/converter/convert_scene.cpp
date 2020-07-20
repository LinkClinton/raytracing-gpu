#include "convert_scene.hpp"

#include "convert_material.hpp"
#include "convert_emitter.hpp"
#include "convert_shape.hpp"

#include "../resource_manager.hpp"

namespace path_tracing::core::converter {

	std::shared_ptr<entity> create_entity(const std::shared_ptr<metascene::entity>& entity, real radius)
	{
		const auto instance = std::make_shared<scenes::entity>(
			create_material(entity->material), create_emitter(entity->emitter),
			create_shape(entity->shape), entity->transform);

		resource_manager::entities.push_back(instance);

		return instance;
	}

	std::shared_ptr<scene> create_scene(const std::shared_ptr<metascene::scene>& meta_scene)
	{
		const auto scene = std::make_shared<scenes::scene>();

		for (const auto& entity : meta_scene->entities)
			scene->add_entity(create_entity(entity, 0));

		return scene;
	}

	
}
