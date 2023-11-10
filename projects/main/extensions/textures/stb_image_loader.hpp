#pragma once

#include "../../runtime/resources/components/texture.hpp"

namespace raytracing::extensions::textures
{
	runtime::resources::components::cpu_texture load_sdr_texture(const std::string& filename);

	runtime::resources::components::cpu_texture load_hdr_texture(const std::string& filename);
}