#pragma once

#include "interfaces/string_property.hpp"

#include "materials/material.hpp"
#include "emitters/emitter.hpp"
#include "math/matrix4x4.hpp"
#include "shapes/shape.hpp"
#include "media/medium.hpp"

#include <memory>

namespace metascene {

	using namespace materials;
	using namespace emitters;
	using namespace shapes;
	using namespace media;
	using namespace math;

	struct entity final : interfaces::string_property {
		std::shared_ptr<metascene::media::media> media;
		std::shared_ptr<material> material;
		std::shared_ptr<emitter> emitter;
		std::shared_ptr<shape> shape;
		
		matrix4x4 transform = matrix4x4(1);

		entity() = default;

		std::string to_string() const noexcept override;
	};
	
}
