#include "convert_material.hpp"
#include "convert_texture.hpp"

#include "meta-scene/materials/substrate_material.hpp"
#include "meta-scene/materials/diffuse_material.hpp"
#include "meta-scene/materials/mirror_material.hpp"
#include "meta-scene/materials/metal_material.hpp"
#include "meta-scene/materials/uber_material.hpp"
#include "meta-scene/logs.hpp"

#include "../materials/substrate_material.hpp"
#include "../materials/diffuse_material.hpp"
#include "../materials/mirror_material.hpp"
#include "../materials/metal_material.hpp"
#include "../resource_manager.hpp"

#pragma optimize("", off)

namespace path_tracing::core::converter {

	std::shared_ptr<material> create_substrate_material(const std::shared_ptr<metascene::materials::substrate_material>& material)
	{
		const auto instance = std::make_shared<substrate_material>(
			create_spectrum_texture(material->specular),
			create_spectrum_texture(material->diffuse),
			create_real_texture(material->roughness_u),
			create_real_texture(material->roughness_v),
			material->remapped_roughness_to_alpha);

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

	std::shared_ptr<material> create_metal_material(const std::shared_ptr<metascene::materials::metal_material>& material)
	{
		const auto instance = std::make_shared<metal_material>(
			create_spectrum_texture(material->eta),
			create_spectrum_texture(material->k),
			create_real_texture(material->roughness_u),
			create_real_texture(material->roughness_v),
			material->remapped_roughness_to_alpha);

		resource_manager::materials.push_back(instance);

		return instance;
	}
	
	std::shared_ptr<material> create_uber_material(const std::shared_ptr<metascene::materials::material>& material)
	{
		return nullptr;
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

		if (material->type == metascene::materials::type::metal)
			return create_metal_material(std::static_pointer_cast<metascene::materials::metal_material>(material));
		
		metascene::logs::error("unknown material.");
		
		return nullptr;
	}
}
