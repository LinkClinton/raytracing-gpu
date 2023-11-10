#pragma once

#include "../../runtime/resources/components/texture.hpp"

namespace raytracing::extensions::textures
{

	runtime::resources::components::cpu_texture load_exr_texture(const std::string& filename);

}
