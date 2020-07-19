#include "convert_material.hpp"
#include "convert_texture.hpp"

#include "meta-scene/materials/diffuse_material.hpp"

#include "../materials/diffuse_material.hpp"

namespace path_tracing::core::converter {

	std::shared_ptr<material> create_diffuse_material(const std::shared_ptr<metascene::materials::diffuse_material>& material)
	{
		return std::make_shared<diffuse_material>(create_spectrum_texture(material->reflectance));
	}
	
	std::shared_ptr<material> create_material(const std::shared_ptr<metascene::materials::material>& material)
	{
		if (material == nullptr) return nullptr;

		if (material->type == metascene::materials::type::diffuse)
			return create_diffuse_material(std::static_pointer_cast<metascene::materials::diffuse_material>(material));

		return std::make_shared<diffuse_material>(vector3(1));
	}
}
