#include "texture_loader.hpp"

#include "../spdlog/spdlog.hpp"

#include "stb_image_loader.hpp"
#include "tiny_exr_loader.hpp"

#include <filesystem>

raytracing::runtime::resources::components::cpu_texture raytracing::extensions::textures::load_texture(
	const std::string& filename)
{
	const auto filepath = std::filesystem::path(filename);

	if (filepath.extension() == ".exr")
	{
		return load_exr_texture(filepath.string());
	}

	if (filepath.extension() == ".hdr")
	{
		return load_hdr_texture(filepath.string());
	}

	if (filepath.extension() == ".png" || filepath.extension() == ".jpg" ||
		filepath.extension() == ".bmp" || filepath.extension() == ".jpeg")
	{
		return load_sdr_texture(filepath.string());
	}

	spdlog::error("load texture({0}) failed, unsupported format({1})", filepath.string(), filepath.extension().string());

	return {};
}
