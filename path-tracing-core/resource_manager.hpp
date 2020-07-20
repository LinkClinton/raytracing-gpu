#pragma once

#include "meta-scene/textures/image_texture.hpp"
#include "meta-scene/shapes/mesh.hpp"

#include "materials/material.hpp"
#include "textures/texture.hpp"
#include "emitters/emitter.hpp"
#include "scenes/entity.hpp"
#include "shapes/shape.hpp"
#include "shapes/mesh.hpp"

#include <unordered_map>
#include <memory>

namespace path_tracing::core {

	using namespace materials;
	using namespace textures;
	using namespace emitters;
	using namespace scenes;
	using namespace shapes;
	
	struct resource_manager final {
		static inline std::unordered_map<std::string, std::shared_ptr<mesh>> meshes_index;
		static inline std::unordered_map<std::string, std::shared_ptr<image>> images_index;
		
		static inline std::vector<std::shared_ptr<material>> materials;
		static inline std::vector<std::shared_ptr<texture>> textures;
		static inline std::vector<std::shared_ptr<emitter>> emitters;
		static inline std::vector<std::shared_ptr<entity>> entities;
		static inline std::vector<std::shared_ptr<shape>> shapes;
		static inline std::vector<std::shared_ptr<image>> images;

		static std::shared_ptr<mesh> read_ply_mesh(const std::shared_ptr<metascene::shapes::mesh>& mesh);

		static std::shared_ptr<image> read_spectrum_image(const std::shared_ptr<metascene::textures::image_texture>& texture);

		static std::shared_ptr<image> read_real_image(const std::shared_ptr<metascene::textures::image_texture>& texture);

		static std::shared_ptr<image> read_environment_map(const std::string& filename, bool gamma);
	};
	
}
