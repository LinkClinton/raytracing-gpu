#include "image_stb.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <stb_image_write.h>

void path_tracing::extensions::images::write_png_image_to_file(const std::string& filename, const image_info<byte>& image)
{
	stbi_write_png(filename.c_str(), static_cast<int>(image.size_x),
		static_cast<int>(image.size_y), 4, image.values.data(), 0);
}
