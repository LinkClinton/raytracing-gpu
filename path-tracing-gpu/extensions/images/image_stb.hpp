#pragma once

#include "image_info.hpp"

namespace path_tracing::extensions::images {

	void write_png_image_to_file(const std::string& filename, const image_info<byte>& image);
	
}
