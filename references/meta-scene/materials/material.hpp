#pragma once

#include "../interfaces/string_property.hpp"
#include "../spectrums/color_spectrum.hpp"
#include "../utilities.hpp"

namespace metascene {

	namespace materials {

		using material_property =
			std::tuple<
			std::shared_ptr<spectrums::spectrum>,
			std::shared_ptr<spectrums::spectrum>,
			std::shared_ptr<spectrums::spectrum>>;
		
		enum class type : uint32 {
			unknown = 0, diffuse = 1,
			plastic = 2, glass = 3,
			metal = 4, uber = 5,
			mirror = 6, substrate = 7,
			translucent = 8, mixture = 9,
			subsurface = 10
		};
		
		struct material : interfaces::string_property {
			type type = type::unknown;

			material() = default;

			material(const materials::type& type);
		};

		material_property read_material_property_from_name(const std::string& name);
	}
}
