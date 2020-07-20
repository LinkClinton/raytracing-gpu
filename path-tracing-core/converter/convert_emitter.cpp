#include "convert_emitter.hpp"

#include "meta-scene/emitters/environment_emitter.hpp"
#include "meta-scene/emitters/surface_emitter.hpp"

#include "../emitters/environment_emitter.hpp"
#include "../emitters/surface_emitter.hpp"
#include "../resource_manager.hpp"

#include "convert_spectrum.hpp"

namespace path_tracing::core::converter {

	std::shared_ptr<emitter> create_surface_emitter(const std::shared_ptr<metascene::emitters::surface_emitter>& emitter)
	{
		const auto instance = std::make_shared<surface_emitter>(read_spectrum(emitter->radiance));

		resource_manager::emitters.push_back(instance);

		return instance;
	}

	std::shared_ptr<emitter> create_environment_emitter(const std::shared_ptr<metascene::emitters::environment_emitter>& emitter)
	{
		if (emitter->environment_map.empty()) {
			const auto instance = std::make_shared<environment_emitter>(read_spectrum(emitter->intensity));

			resource_manager::emitters.push_back(instance);

			return instance;
		}
		
		const auto texture = std::make_shared<textures::texture>(
			resource_manager::read_environment_map(emitter->environment_map, emitter->gamma), vector3(1));
		
		const auto instance = std::make_shared<environment_emitter>(texture, read_spectrum(emitter->intensity));

		resource_manager::textures.push_back(texture);
		resource_manager::emitters.push_back(instance);

		return instance;
	}

	std::shared_ptr<emitter> create_emitter(const std::shared_ptr<metascene::emitters::emitter>& emitter)
	{
		if (emitter == nullptr) return nullptr;

		if (emitter->type == metascene::emitters::type::surface)
			return create_surface_emitter(std::static_pointer_cast<metascene::emitters::surface_emitter>(emitter));

		if (emitter->type == metascene::emitters::type::environment)
			return create_environment_emitter(std::static_pointer_cast<metascene::emitters::environment_emitter>(emitter));
		
		return nullptr;
	}
}
