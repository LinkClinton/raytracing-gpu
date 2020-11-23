#pragma once

#include "image_info.hpp"

namespace wrapper::directx12 {

	class texture2d;
	
}

namespace path_tracing::runtime::render {

	class render_device;
	
}

namespace path_tracing::extensions::images {

	void write_image_to_png(const std::string& filename, const image_info<byte>& image);

	void write_image_to_png(const std::string& filename,
		const wrapper::directx12::texture2d& texture2d,	const runtime::render::render_device& device);
	
}
