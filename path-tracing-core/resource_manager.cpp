#include "resource_manager.hpp"

#include "importers/image_importer.hpp"
#include "importers/ply_importer.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <stb_image_write.h>

namespace path_tracing::core {

	inline real gamma_correct(real value)
	{
		if (value <= 0.0031308f) return 12.92f * value;

		return 1.055f * pow(value, 1.f / 2.4f) - 0.055f;
	}
	
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

	void resource_manager::write_image(const std::string& filename, const std::vector<real>& image, size_t width, size_t height)
	{
		auto sdr_image = std::vector<byte>(image.size());

		for (size_t index = 0; index < image.size(); index++) {
			sdr_image[index] = static_cast<byte>(glm::clamp(
				255 * gamma_correct(image[index]) + 0.5f,
				static_cast<real>(0),
				static_cast<real>(255)
			));
		}

		for (size_t index = 3; index < image.size(); index += 4)
			sdr_image[index] = 255;

		stbi_write_png((filename + ".png").c_str(), static_cast<int>(width), static_cast<int>(height), 4, sdr_image.data(), 0);
	}


}
