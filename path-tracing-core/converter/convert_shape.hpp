#pragma once

#include "meta-scene/shapes/shape.hpp"

#include "../shapes/shape.hpp"

using namespace path_tracing::core::shapes;

namespace path_tracing::core::converter {

	std::shared_ptr<shape> create_shape(const std::shared_ptr<metascene::shapes::shape>& shape);
	
}
