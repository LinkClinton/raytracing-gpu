#include "resource_manager.hpp"

#include "importers/image_importer.hpp"
#include "importers/ply_importer.hpp"

namespace path_tracing::core {

	std::shared_ptr<mesh> resource_manager::read_ply_mesh(const std::shared_ptr<metascene::shapes::mesh>& mesh)
	{
		const auto index = mesh->to_string();

		if (meshes_index.find(index) != meshes_index.end()) return meshes_index.at(index);

		const auto triangles = importers::load_ply_mesh(mesh->filename);
		const auto shape = std::make_shared<shapes::mesh>(
			triangles->positions, triangles->normals, triangles->uvs,
			triangles->indices, mesh->reverse_orientation);

		shapes.push_back(shape);
		meshes_index.insert({ index, shape });

		return shape;
	}

	std::shared_ptr<image> resource_manager::read_spectrum_image(const std::shared_ptr<metascene::textures::image_texture>& texture)
	{
		const auto index = texture->to_string();

		if (images_index.find(index) != images_index.end()) return images_index.at(index);

		const auto image = importers::import_texture2d<vector3>(texture->filename, texture->gamma);

		images.push_back(image);
		images_index.insert({ index, image });

		return image;
	}

	std::shared_ptr<image> resource_manager::read_real_image(const std::shared_ptr<metascene::textures::image_texture>& texture)
	{
		const auto index = texture->to_string();

		if (images_index.find(index) != images_index.end()) return images_index.at(index);

		const auto image = importers::read_texture2d<real>(texture->filename, texture->gamma);

		images.push_back(image);
		images_index.insert({ index, image });

		return image;
	}

	std::shared_ptr<image> resource_manager::read_environment_map(const std::string& filename, bool gamma)
	{
		const auto index = filename;

		if (images_index.find(index) != images_index.end()) return images_index.at(index);

		const auto image = importers::import_environment_map(filename, gamma);

		images.push_back(image);
		images_index.insert({ index, image });

		return image;
	}
}
