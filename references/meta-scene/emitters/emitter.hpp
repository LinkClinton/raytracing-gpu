#pragma once

#include "../interfaces/string_property.hpp"
#include "../utilities.hpp"

namespace metascene {

	namespace emitters {

		enum class type : uint32 {
			unknown = 0, surface = 1, environment = 2, directional = 3
		};
		
		struct emitter : interfaces::string_property {
			type type = type::unknown;

			emitter() = default;

			emitter(const emitters::type& type);
		};
		
	}
}
