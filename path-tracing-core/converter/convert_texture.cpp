#include "convert_texture.hpp"
#include "convert_spectrum.hpp"

#include "meta-scene/textures/constant_texture.hpp"
#include "meta-scene/textures/image_texture.hpp"
#include "meta-scene/textures/scale_texture.hpp"
#include "meta-scene/logs.hpp"

#include "../resource_manager.hpp"

namespace path_tracing::core::converter {

	vector3 create_constant_spectrum_texture(const std::shared_ptr<metascene::textures::texture>& texture)
	{
		const auto instance = std::static_pointer_cast<metascene::textures::constant_texture>(texture);

		if (instance->value_type == metascene::textures::value_type::real)
			return vector3(instance->real);

		return vector3(read_spectrum(instance->spectrum));
	}

	real create_constant_real_texture(const std::shared_ptr<metascene::textures::texture>& texture)
	{
		const auto instance = std::static_pointer_cast<metascene::textures::constant_texture>(texture);

		assert(instance->value_type == metascene::textures::value_type::real);

		return instance->real;
	}

	std::shared_ptr<texture> create_scale_spectrum_texture(const std::shared_ptr<metascene::textures::scale_texture>& texture)
	{
		assert(texture->base->type == metascene::textures::type::image && texture->scale->type == metascene::textures::type::constant);

		const auto instance = std::make_shared<textures::texture>(
			resource_manager::read_spectrum_image(std::static_pointer_cast<metascene::image_texture>(texture->base)),
			create_constant_spectrum_texture(texture->scale));

		resource_manager::textures.push_back(instance);

		return instance;
	}

	std::shared_ptr<texture> create_image_spectrum_texture(const std::shared_ptr<metascene::textures::image_texture>& texture)
	{
		const auto instance = std::make_shared<textures::texture>(
			resource_manager::read_spectrum_image(texture), vector3(1));

		resource_manager::textures.push_back(instance);

		return instance;
	}
	
	std::shared_ptr<texture> create_image_spectrum_texture(const std::shared_ptr<metascene::textures::texture>& texture)
	{
		if (texture->type == metascene::textures::type::scale)
			return create_scale_spectrum_texture(std::static_pointer_cast<metascene::scale_texture>(texture));

		if (texture->type == metascene::textures::type::image)
			return create_image_spectrum_texture(std::static_pointer_cast<metascene::image_texture>(texture));

		metascene::logs::error("unknown texture.");
		
		return nullptr;
	}

	std::shared_ptr<texture> create_scale_real_texture(const std::shared_ptr<metascene::textures::scale_texture>& texture)
	{
		assert(texture->base->type == metascene::textures::type::image && texture->scale->type == metascene::textures::type::constant);

		const auto instance = std::make_shared<textures::texture>(
			resource_manager::read_real_image(std::static_pointer_cast<metascene::image_texture>(texture->base)),
			create_constant_spectrum_texture(texture->scale));

		resource_manager::textures.push_back(instance);

		return instance;
	}

	std::shared_ptr<texture> create_image_real_texture(const std::shared_ptr<metascene::textures::image_texture>& texture)
	{
		const auto instance = std::make_shared<textures::texture>(resource_manager::read_real_image(texture), vector3(1));

		resource_manager::textures.push_back(instance);

		return instance;
	}
	
	std::shared_ptr<texture> create_image_real_texture(const std::shared_ptr<metascene::textures::texture>& texture)
	{
		if (texture->type == metascene::textures::type::scale)
			return create_scale_real_texture(std::static_pointer_cast<metascene::scale_texture>(texture));

		if (texture->type == metascene::textures::type::image)
			return create_image_real_texture(std::static_pointer_cast<metascene::image_texture>(texture));

		metascene::logs::error("unknown texture.");
		
		return nullptr;
	}

	std::shared_ptr<texture> create_spectrum_texture(const std::shared_ptr<metascene::textures::texture>& texture)
	{
		if (texture->type == metascene::textures::type::image)
			return create_image_spectrum_texture(texture);

		const auto result = std::make_shared<textures::texture>(nullptr, create_constant_spectrum_texture(texture));

		resource_manager::textures.push_back(result);

		return result;
	}

	std::shared_ptr<texture> create_real_texture(const std::shared_ptr<metascene::textures::texture>& texture)
	{
		if (texture->type == metascene::textures::type::image)
			return create_image_real_texture(texture);

		const auto result = std::make_shared<textures::texture>(nullptr, vector3(create_constant_real_texture(texture)));

		resource_manager::textures.push_back(result);

		return result;
	}


}
