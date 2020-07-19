#include "convert_emitter.hpp"

#include "meta-scene/emitters/surface_emitter.hpp"

#include "../emitters/surface_emitter.hpp"

#include "convert_spectrum.hpp"

namespace path_tracing::core::converter {

	std::shared_ptr<emitter> create_surface_emitter(const std::shared_ptr<metascene::emitters::surface_emitter>& emitter)
	{
		return std::make_shared<surface_emitter>(read_spectrum(emitter->radiance));
	}

	std::shared_ptr<emitter> create_emitter(const std::shared_ptr<metascene::emitters::emitter>& emitter)
	{
		if (emitter == nullptr) return nullptr;

		if (emitter->type == metascene::emitters::type::surface)
			return create_surface_emitter(std::static_pointer_cast<metascene::emitters::surface_emitter>(emitter));

		return nullptr;
	}
}
