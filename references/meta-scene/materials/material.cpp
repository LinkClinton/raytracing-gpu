#include "material.hpp"

#include "unordered_map"

metascene::materials::material::material(const materials::type& type) : type(type)
{
}

namespace metascene::materials {

	using material_property_origin_data =
		std::tuple<spectrums::color_spectrum, spectrums::color_spectrum, spectrums::color_spectrum>;
	
	std::unordered_map<std::string, material_property_origin_data> material_properties = {
		{
			"Skin1",
			{
				spectrums::color_spectrum(real(0.740), real(0.880), real(1.010)),
				spectrums::color_spectrum(real(0.032), real(0.170), real(0.480)),
				spectrums::color_spectrum(real(0.440), real(0.220), real(0.130))
			}
		},
		{
			"Skin2",
			{
				spectrums::color_spectrum(real(1.090), real(1.590), real(1.790)),
				spectrums::color_spectrum(real(0.013), real(0.070), real(0.145)),
				spectrums::color_spectrum(real(0.630), real(0.440), real(0.340))
			}
		},
		{
			"Skimmilk",
			{
				spectrums::color_spectrum(real(0.700), real(1.220), real(1.900)),
				spectrums::color_spectrum(real(0.0014), real(0.0025), real(0.0142)),
				spectrums::color_spectrum(real(0.81), real(0.81), real(0.69))
			}
		}
	};
	
}

metascene::materials::material_property metascene::materials::read_material_property_from_name(const std::string& name)
{
	const auto [sigma_s, sigma_a, diffuse] = material_properties[name];

	return {
		std::make_shared<spectrums::color_spectrum>(sigma_s.red, sigma_s.green, sigma_s.blue),
		std::make_shared<spectrums::color_spectrum>(sigma_a.red, sigma_a.green, sigma_a.blue),
		std::make_shared<spectrums::color_spectrum>(diffuse.red, diffuse.green, diffuse.blue)
	};
}
