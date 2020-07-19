#include "resource_manager.hpp"

#include "importers/ply_importer.hpp"

namespace path_tracing::core {

	std::shared_ptr<mesh> resource_manager::read_ply_mesh(const std::shared_ptr<metascene::shapes::mesh>& mesh)
	{
		const auto index = mesh->to_string();

		if (meshes.find(index) != meshes.end()) return meshes.at(index);

		const auto triangles = importers::load_ply_mesh(mesh->filename);

		return meshes[index] = std::make_shared<shapes::mesh>(
			triangles->positions, triangles->normals,
			triangles->uvs, triangles->indices, mesh->reverse_orientation);
	}
}
