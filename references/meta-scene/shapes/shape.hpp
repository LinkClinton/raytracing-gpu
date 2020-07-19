#pragma once

#include "../interfaces/string_property.hpp"
#include "../utilities.hpp"

namespace metascene {

	namespace shapes {

		enum class type : uint32 {
			unknown = 0, mesh = 1, sphere = 2, triangles = 3, disk = 4, curve = 5
		};

		struct shape : interfaces::string_property {
			bool reverse_orientation = false;

			type type = type::unknown;

			shape() = default;

			shape(const shapes::type& type);
		};
	}
}
