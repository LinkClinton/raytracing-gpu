#include "convert_material.hpp"
#include "convert_texture.hpp"

#include "meta-scene/materials/substrate_material.hpp"
#include "meta-scene/materials/diffuse_material.hpp"
#include "meta-scene/materials/mirror_material.hpp"

#include "../materials/diffuse_material.hpp"
#include "../materials/mirror_material.hpp"
#include "../resource_manager.hpp"

#pragma optimize("", off)

namespace path_tracing::core::converter {

	std::shared_ptr<material> create_substrate_material(const std::shared_ptr<metascene::materials::substrate_material>& material)
	{
		// todo
		const auto diffuse = create_spectrum_texture(material->diffuse);
		const auto instance = std::make_shared<diffuse_material>(diffuse);

		resource_manager::materials.push_back(instance);

		return instance;
	}
	
	std::shared_ptr<material> create_diffuse_material(const std::shared_ptr<metascene::materials::diffuse_material>& material)
	{
		const auto diffuse = create_spectrum_texture(material->reflectance);
		const auto instance = std::make_shared<diffuse_material>(diffuse);

		resource_manager::materials.push_back(instance);

		return instance;
	}

	std::shared_ptr<material> create_mirror_material(const std::shared_ptr<metascene::materials::mirror_material>& material)
	{
		const auto reflectance = create_spectrum_texture(material->reflectance);
		const auto instance = std::make_shared<mirror_material>(reflectance);

		resource_manager::materials.push_back(instance);

		return instance;
	}

	std::shared_ptr<material> create_material(const std::shared_ptr<metascene::materials::material>& material)
	{
		if (material == nullptr) return nullptr;

		if (material->type == metascene::materials::type::substrate)
			return create_substrate_material(std::static_pointer_cast<metascene::materials::substrate_material>(material));
		
		if (material->type == metascene::materials::type::diffuse)
			return create_diffuse_material(std::static_pointer_cast<metascene::materials::diffuse_material>(material));

		if (material->type == metascene::materials::type::mirror)
			return create_mirror_material(std::static_pointer_cast<metascene::materials::mirror_material>(material));
		
		return nullptr;
	}
}
