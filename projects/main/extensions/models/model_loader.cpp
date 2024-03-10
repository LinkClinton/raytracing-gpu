#include "model_loader.hpp"

#include "../spdlog/spdlog.hpp"

#include "tiny_ply_loader.hpp"

#include <filesystem>

raytracing::runtime::resources::components::mesh raytracing::extensions::models::load_mesh(
	const std::string& filename)
{
	const auto filepath = std::filesystem::path(filename);

	if (filepath.extension() == ".ply")
	{
		return load_ply_mesh(filepath.string());
	}

	spdlog::error("load mesh({0}) failed, unsupported format(1).", filepath.string(), filepath.extension().string());

	return {};
}
